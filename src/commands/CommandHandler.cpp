/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stefan <stefan@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/23 15:45:25 by stefan            #+#    #+#             */
/*   Updated: 2025/05/25 20:01:24 by stefan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"
#include "ReplyBuilder.hpp"
#include "ReplyCode.hpp"
#include "User.hpp"
#include <sstream>
#include <cctype>
#include <iostream>

CommandHandler::CommandHandler(const std::string& serverPassword, std::map<int, User*>& users)
: _serverPassword(serverPassword), _users(users) {
    _handlers["PASS"] = &CommandHandler::handlePass;
    _handlers["NICK"] = &CommandHandler::handleNick;
    _handlers["USER"] = &CommandHandler::handleUser;
    _handlers["JOIN"] = &CommandHandler::handleJoin;
    _handlers["PART"] = &CommandHandler::handlePart;
    _handlers["MODE"] = &CommandHandler::handleMode;
    //_handlers["TOPIC"] = &CommandHandler::handleTopic;
    // _handlers["INVITE"] = &CommandHandler::handleInvite;
    // _handlers["KICK"] = &CommandHandler::handleKick;
    // _handlers["PRIVMSG"] = &CommandHandler::handlePrivmsg;
}

void CommandHandler::handleCommand(int fd, const std::string& line) {
    std::string cmd;
    std::vector<std::string> args;

    if (!CommandParser::parse(line, cmd, args)) {
        return;
    }

    for (size_t i = 0; i < cmd.size(); ++i) {
        cmd[i] = static_cast<char>(std::toupper(cmd[i]));
    }

    std::map<std::string, HandlerFn>::iterator it = _handlers.find(cmd);
    if (it != _handlers.end()) {
        (this->*(it->second))(fd, args);
    } else {
        std::cout << "Unknown command: " << cmd << std::endl;
    }
}

User* CommandHandler::findUserByNick(const std::string& nick) {
    for (std::map<int, User*>::iterator it = _users.begin(); it != _users.end(); ++it) {
        if (it->second && it->second->getNickname() == nick) {
            return it->second;
        }
    }
    return NULL; 
}