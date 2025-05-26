/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AuthCommands.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stefan <stefan@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 15:40:55 by stefan            #+#    #+#             */
/*   Updated: 2025/05/25 16:45:36 by stefan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"
#include "ReplyBuilder.hpp"
#include "ReplyCode.hpp"
#include <sstream>
#include <cctype>
#include <iostream>

//Hanle Pass
void CommandHandler::handlePass(int fd, const std::vector<std::string>& args) {
    User* user = _users[fd];

    if (user->getState() == User::REGISTERED) {
        user->getSendBuffer() += ReplyBuilder::build(ERR_ALREADYREGISTRED, *user);
        return;
    }

    if (args.empty()) {
        user->getSendBuffer() += ReplyBuilder::build(ERR_NEEDMOREPARAMS, *user, "PASS");
        return;
    }

    if (args[0] != _serverPassword) {
        user->getSendBuffer() += ReplyBuilder::build(ERR_PASSWDMISMATCH, *user);
        return;
    }

    user->setState(User::PASSWORD_SENT);
}

//Handle Nick
void CommandHandler::handleNick(int fd, const std::vector<std::string>& args) {
    User* user = _users[fd];

    if (args.empty()) {
        user->getSendBuffer() += ReplyBuilder::build(ERR_NONICKNAMEGIVEN, *user);
        return;
    }

    std::string newNick = args[0];

    if (!isValidNickname(newNick)) {
        user->getSendBuffer() += ReplyBuilder::build(ERR_ERRONEUSNICKNAME, *user, newNick);
        return;
    }

    if (isNicknameTaken(newNick)) {
        user->getSendBuffer() += ReplyBuilder::build(ERR_NICKNAMEINUSE, *user, newNick);
        return;
    }

    std::string oldNick = user->getNickname();
    user->setNickname(newNick);

    if (!oldNick.empty()) {
        user->getSendBuffer() += ":" + oldNick + " NICK :" + newNick + "\r\n";
    }

    tryRegisterUser(user);
}

//Handle User
void CommandHandler::handleUser(int fd, const std::vector<std::string>& args) {
    User* user = _users[fd];

    if (user->getState() == User::REGISTERED) {
        user->getSendBuffer() += ReplyBuilder::build(ERR_ALREADYREGISTRED, *user);
        return;
    }

    if (user->getState() != User::PASSWORD_SENT) {
        user->getSendBuffer() += ReplyBuilder::build(ERR_NOTREGISTERED, *user);
        return;
    }

    if (args.size() < 4) {
        user->getSendBuffer() += ReplyBuilder::build(ERR_NEEDMOREPARAMS, *user, "USER");
        return;
    }
    
    user->setUsername(args[0]);

    std::string realname = args[3];
    if (realname[0] == ':')
        realname.erase(0, 1);

    for (size_t i = 4; i < args.size(); ++i) {
        realname += " " + args[i];
    }

    user->setRealname(realname);

    tryRegisterUser(user);
}

//Util functions
bool CommandHandler::isValidNickname(const std::string& nick) const {
    if (nick.empty())
        return false;

    // Cannot start with digit or #/&
    if (!isalpha(nick[0]) && nick[0] != '_' && nick[0] != '-' && nick[0] != '[')
        return false;

    for (size_t i = 0; i < nick.length(); ++i) {
        char c = nick[i];
        if (!(isalnum(c) || c == '-' || c == '_' || c == '[' || c == ']' ||
              c == '\\' || c == '`' || c == '^' || c == '{' || c == '}'))
            return false;
    }

    return true;
}

bool CommandHandler::isNicknameTaken(const std::string& nick) const {
    for (std::map<int, User*>::const_iterator it = _users.begin(); it != _users.end(); ++it) {
        if (it->second->getNickname() == nick)
            return true;
    }
    return false;
}

void CommandHandler::tryRegisterUser(User* user) {
    if (user->getState() == User::REGISTERED)
        return;

    if (user->getState() == User::PASSWORD_SENT &&
        !user->getNickname().empty() &&
        !user->getUsername().empty()) {

        user->setState(User::REGISTERED);
        user->getSendBuffer() += ReplyBuilder::build(RPL_WELCOME, *user);
    }
}