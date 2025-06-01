/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerCommands.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stefan <stefan@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 15:40:13 by stefan            #+#    #+#             */
/*   Updated: 2025/06/01 20:33:08 by stefan           ###   ########.fr       */
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
    const std::set<std::string>& userChannelNames = user->getChannels();
    std::vector<std::string> channelsToPart(userChannelNames.begin(), userChannelNames.end());
    for (size_t i = 0; i < channelsToPart.size(); ++i) {
        Channel* chan = _server.getChannelByName(channelsToPart[i]);
        if (chan) {
            chan->broadcast(quitMsg, user, &_server);
            chan->removeUser(user);
        }
    }

    _server.removeUserByFd(fd);
}