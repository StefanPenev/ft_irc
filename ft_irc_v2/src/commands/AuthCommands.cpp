/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AuthCommands.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stefan <stefan@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 15:40:55 by stefan            #+#    #+#             */
/*   Updated: 2025/06/14 23:59:11 by stefan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"
#include "ReplyBuilder.hpp"
#include "ReplyCode.hpp"
#include "Server.hpp"
#include <sstream>
#include <cctype>
#include <iostream>

// Handle PASS
void CommandHandler::handlePass(int fd, const std::vector<std::string>& args) {
    User* user = _server.getUserByFd(fd);
    if (!user) return;

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

// Handle NICK
void CommandHandler::handleNick(int fd, const std::vector<std::string>& args) {
    User* user = _server.getUserByFd(fd);
    if (!user) return;

    if (args.empty()) {
        user->getSendBuffer() += ReplyBuilder::build(ERR_NONICKNAMEGIVEN, *user);
        return;
    }

    std::string newNick = args[0];

    if (!isValidNickname(newNick)) {
        user->getSendBuffer() += ReplyBuilder::build(ERR_ERRONEUSNICKNAME, *user, newNick);
        return;
    }

    if (_server.getUserByNick(newNick)) {
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

// Handle USER
void CommandHandler::handleUser(int fd, const std::vector<std::string>& args) {
    User* user = _server.getUserByFd(fd);
    if (!user) return;

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

// Utility functions
/**
 * Checks whether the given nickname is valid according to IRC rules.
 * Returns true if the nickname is valid, false otherwise.
 */
bool CommandHandler::isValidNickname(const std::string& nick) const {
    if (nick.empty())
        return false;

    // Cannot start with digit or invalid char
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

/**
 * Attempts to complete user registration.
 * If the user has sent the correct password, and both nickname and username are set,
 * the user is marked as REGISTERED and a welcome message is added to their send buffer.
 */
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
