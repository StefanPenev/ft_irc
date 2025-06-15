/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerCommands.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stefan <stefan@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 15:40:13 by stefan            #+#    #+#             */
/*   Updated: 2025/06/02 21:26:33 by stefan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"
#include "ReplyBuilder.hpp"
#include "Server.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include <vector>
#include <string>
#include <unistd.h>
#include <sstream>
#include <cstdlib>

void CommandHandler::handlePing(int fd, const std::vector<std::string>& args) {
    User* user = _server.getUserByFd(fd);
    if (!user)
        return;
    if (args.empty()) {
        user->getSendBuffer() += ReplyBuilder::build(ERR_NEEDMOREPARAMS, *user, "PING");
        return;
    }
    std::string token = args[0];
    user->getSendBuffer() += "PONG :" + token + "\r\n";
}

void CommandHandler::handlePong(int fd, const std::vector<std::string>& args) {
    (void)args;
    User* user = _server.getUserByFd(fd);
    if (!user)
        return;
    // Optionally, update last-pong timestamp
}

void CommandHandler::handleQuit(int fd, const std::vector<std::string>& args) {
    User* user = _server.getUserByFd(fd);
    if (!user)
        return;

    std::string reason = "Client Quit";
    if (!args.empty()) {
        reason = args[0];
        if (!reason.empty() && reason[0] == ':')
            reason.erase(0, 1);
    }

    std::string quitMsg = ":" + user->getPrefix() + " QUIT :" + reason + "\r\n";

    const std::set<std::string>& channelNames = user->getChannels();
    std::vector<std::string> channelsToPart(channelNames.begin(), channelNames.end());

    for (size_t i = 0; i < channelsToPart.size(); ++i) {
        Channel* chan = _server.getChannelByName(channelsToPart[i]);
        if (chan) {
            chan->broadcast(quitMsg, user, &_server);
            chan->removeUser(user);
        }
    }

    user->markForDisconnect();
}

void CommandHandler::handleNames(int fd, const std::vector<std::string>& args) {
    User* user = _server.getUserByFd(fd);
    if (!user)
        return;

    // If no channel is specified, list all visible channels (optional, here we require a channel)
    if (args.empty() || args[0].empty()) {
        user->getSendBuffer() += ":ircserv 366 " + user->getNickname() + " * :End of /NAMES list\r\n";
        _server.flushSendBuffer(fd);
        return;
    }

    std::string chanName = args[0];
    Channel* channel = _server.getChannelByName(chanName);
    if (!channel) {
        user->getSendBuffer() += ":ircserv 366 " + user->getNickname() + " " + chanName + " :End of /NAMES list\r\n";
        _server.flushSendBuffer(fd);
        return;
    }

    // Build the names list with @ for ops
    std::string namesList;
    const std::set<User*>& members = channel->getUsers();
    for (std::set<User*>::const_iterator it = members.begin(); it != members.end(); ++it) {
        if (!namesList.empty())
            namesList += " ";
        if ((*it)->isChannelOperator(chanName))
            namesList += "@";
        namesList += (*it)->getNickname();
    }

    // Send RPL_NAMREPLY (353)
    user->getSendBuffer() +=
        ":ircserv 353 " + user->getNickname() + " = " + chanName + " :" + namesList + "\r\n";
    // Send RPL_ENDOFNAMES (366)
    user->getSendBuffer() +=
        ":ircserv 366 " + user->getNickname() + " " + chanName + " :End of /NAMES list\r\n";
    _server.flushSendBuffer(fd);
}