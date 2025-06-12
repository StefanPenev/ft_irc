/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MessagingCommands.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anilchen <anilchen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 15:41:13 by stefan            #+#    #+#             */
/*   Updated: 2025/06/12 15:49:17 by anilchen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "User.hpp"
#include "Channel.hpp"
#include "ReplyBuilder.hpp"
#include "ReplyCode.hpp"
#include "Server.hpp"
#include <string>
#include <vector>
#include <map>
#include <sstream>

void CommandHandler::handleMessage(int fd, const std::vector<std::string>& args, bool isPrivmsg) {
    User* sender = _server.getUserByFd(fd);
    if (!sender || sender->getState() != User::REGISTERED) {
        if (isPrivmsg)
            sender->getSendBuffer() += ReplyBuilder::build(ERR_NOTREGISTERED, *sender);
        return;
    }

    if (args.size() < 2) {
        if (isPrivmsg)
            sender->getSendBuffer() += ReplyBuilder::build(ERR_NEEDMOREPARAMS, *sender, args.empty() ? "PRIVMSG/NOTICE" : args[0]);
        return;
    }

    std::string target = args[0];
    std::string message;
    for (size_t i = 1; i < args.size(); ++i) {
        if (i == 1 && args[i].size() && args[i][0] == ':')
            message += args[i].substr(1);
        else {
            if (!message.empty()) message += " ";
            message += args[i];
        }
    }

    if (target[0] == '#') {
        Channel* channel = _server.getChannelByName(target);
        if (!channel) {
            if (isPrivmsg)
                sender->getSendBuffer() += ReplyBuilder::build(ERR_NOSUCHCHANNEL, *sender, target);
            return;
        }
        if (!channel->isMember(sender)) {
            if (isPrivmsg)
                sender->getSendBuffer() += ReplyBuilder::build(ERR_CANNOTSENDTOCHAN, *sender, target, "Cannot send to channel");
            return;
        }

        std::ostringstream oss;
        oss << ":" << sender->getPrefix() << " " << (isPrivmsg ? "PRIVMSG" : "NOTICE") << " " << target << " :" << message << "\r\n";
        channel->broadcast(oss.str(), sender, &_server);
    } else {
        User* recipient = _server.getUserByNick(target);
        if (!recipient) {
            if (isPrivmsg)
                sender->getSendBuffer() += ReplyBuilder::build(ERR_NOSUCHNICK, *sender, target);
            return;
        }

        std::ostringstream oss;
        oss << ":" << sender->getPrefix() << " " << (isPrivmsg ? "PRIVMSG" : "NOTICE") << " " << recipient->getNickname() << " :" << message << "\r\n";
        recipient->getSendBuffer() += oss.str();
        _server.flushSendBuffer(recipient->getFd());
        //new added
        if (recipient != sender) 
        {
	        sender->getSendBuffer() += oss.str();
	        _server.flushSendBuffer(sender->getFd());
        }
    }
}

void CommandHandler::handlePrivmsg(int fd, const std::vector<std::string>& args) {
    handleMessage(fd, args, true);
}

void CommandHandler::handleNotice(int fd, const std::vector<std::string>& args) {
    handleMessage(fd, args, false);
}
