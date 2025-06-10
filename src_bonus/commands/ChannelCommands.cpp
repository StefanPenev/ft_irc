/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChannelCommands.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anilchen <anilchen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 15:40:30 by stefan            #+#    #+#             */
/*   Updated: 2025/06/10 15:38:14 by anilchen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler_bonus.hpp"
#include "ReplyBuilder_bonus.hpp"
#include "ReplyCode_bonus.hpp"
#include "Channel_bonus.hpp"
#include "Server_bonus.hpp"
#include <sstream>
#include <cctype>
#include <cstdlib>
#include <iostream>

//Handle Kick
void CommandHandler::handleKick(int fd, const std::vector<std::string>& args) {
    User* sender = _server.getUserByFd(fd);

    if (args.size() < 2) {
        sender->getSendBuffer() += ReplyBuilder::build(ERR_NEEDMOREPARAMS, *sender, "KICK");
        return;
    }

    std::string chanName = args[0];
    std::string targetNick = args[1];
    std::string reason = (args.size() > 2) ? args[2] : sender->getNickname();

    Channel* channel = _server.getChannelByName(chanName);
    if (!channel) {
        sender->getSendBuffer() += ReplyBuilder::build(ERR_NOSUCHCHANNEL, *sender, chanName);
        return;
    }

    if (!channel->isMember(sender)) {
        sender->getSendBuffer() += ReplyBuilder::build(ERR_NOTONCHANNEL, *sender, chanName);
        return;
    }

    if (!channel->isOperator(sender)) {
        sender->getSendBuffer() += ReplyBuilder::build(ERR_CHANOPRIVSNEEDED, *sender, chanName);
        return;
    }

    User* target = _server.getUserByNick(targetNick);
    if (!target || !channel->isMember(target)) {
        sender->getSendBuffer() += ReplyBuilder::build(ERR_USERNOTINCHANNEL, *sender, targetNick, chanName);
        return;
    }

    // Broadcast KICK message to all channel users (including target)
    std::string kickMsg = ":" + sender->getNickname() + " KICK " + chanName + " " + targetNick + " :" + reason + "\r\n";
    channel->broadcast(kickMsg, NULL, &_server);

    // Remove the user from the channel
    channel->removeUser(target);
    target->leaveChannel(chanName);
}

//Handle Invite
void CommandHandler::handleInvite(int fd, const std::vector<std::string>& args) {
    User* user = _server.getUserByFd(fd);
    if (!user)
        return;

    // Check for enough parameters
    if (args.size() < 2) {
        user->getSendBuffer() += ReplyBuilder::build(ERR_NEEDMOREPARAMS, *user, "INVITE");
        return;
    }

    const std::string& targetNick = args[0];
    const std::string& channelName = args[1];

    Channel* channel = _server.getChannelByName(channelName);
    if (!channel) {
        user->getSendBuffer() += ReplyBuilder::build(ERR_NOSUCHCHANNEL, *user, channelName);
        return;
    }

    // Check if inviter is a member of the channel
    if (!channel->isMember(user)) {
        user->getSendBuffer() += ReplyBuilder::build(ERR_NOTONCHANNEL, *user, channelName);
        return;
    }

    // Check invite privileges if channel is invite-only (+i)
    if (channel->isInviteOnly() && !channel->isOperator(user)) {
        user->getSendBuffer() += ReplyBuilder::build(ERR_CHANOPRIVSNEEDED, *user, channelName);
        return;
    }

    User* targetUser = _server.getUserByNick(targetNick);
    if (!targetUser) {
        user->getSendBuffer() += ReplyBuilder::build(ERR_NOSUCHNICK, *user, targetNick);
        return;
    }

    // Check if target user is already in the channel
    if (channel->isMember(targetUser)) {
        user->getSendBuffer() += ReplyBuilder::build(ERR_USERONCHANNEL, *user, targetNick, channelName);
        return;
    }

    // Add to invite list if channel tracks it
    channel->inviteUser(targetUser);

    // Send RPL_INVITING to inviter
    user->getSendBuffer() += ReplyBuilder::build(RPL_INVITING, *user, targetNick, channelName);
    
    // Notify invited user
    std::string inviteMsg = ":" + user->getPrefix() + " INVITE " + targetNick + " :" + channelName + "\r\n";
    targetUser->getSendBuffer() += inviteMsg;
    _server.flushSendBuffer(targetUser->getFd());
}


//Handle Topic
void CommandHandler::handleTopic(int fd, const std::vector<std::string>& args) {
    User* user = _server.getUserByFd(fd);
    if (!user)
        return;

    // Only one parameter is required for a topic query
    if (args.size() < 1) {
        user->getSendBuffer() += ReplyBuilder::build(ERR_NEEDMOREPARAMS, *user, "TOPIC");
        return;
    }

    const std::string& chanName = args[0];
    Channel* channel = _server.getChannelByName(chanName);

    if (!channel) {
        user->getSendBuffer() += ReplyBuilder::build(ERR_NOSUCHCHANNEL, *user, chanName);
        return;
    }

    // Check if user is in the channel
    if (!channel->isMember(user)) {
        user->getSendBuffer() += ReplyBuilder::build(ERR_NOTONCHANNEL, *user, chanName);
        return;
    }

    // --- Topic query ---
    if (args.size() == 1) {
        if (channel->hasTopic()) {
            user->getSendBuffer() += ReplyBuilder::build(RPL_TOPIC, *user, chanName, channel->getTopic());
        } else {
            user->getSendBuffer() += ReplyBuilder::build(RPL_NOTOPIC, *user, chanName);
        }
        return;
    }

    // --- Topic setting ---
    if (channel->isTopicLocked() && !channel->isOperator(user)) {
        user->getSendBuffer() += ReplyBuilder::build(ERR_CHANOPRIVSNEEDED, *user, chanName);
        return;
    }

    std::string topic;
    for (size_t i = 1; i < args.size(); ++i) {
        if (!topic.empty())
            topic += " ";
        topic += args[i];
    }
    if (!topic.empty() && topic[0] == ':')
        topic = topic.substr(1);

    channel->setTopic(topic);
    std::string msg = ":" + user->getPrefix() + " TOPIC " + chanName + " :" + topic + "\r\n";
    channel->broadcast(msg, NULL, &_server);
}

//Handle Mode
void CommandHandler::handleMode(int fd, const std::vector<std::string>& args) {
    User* user = _server.getUserByFd(fd);
    if (user->getState() != User::REGISTERED) {
        user->getSendBuffer() += ReplyBuilder::build(ERR_NOTREGISTERED, *user);
        return;
    }

    if (args.empty() || args[0].empty()) {
        user->getSendBuffer() += ReplyBuilder::build(ERR_NEEDMOREPARAMS, *user, "MODE");
        return;
    }

    std::string target = args[0];

    if (target[0] != '#' && target[0] != '&') {
        user->getSendBuffer() += ReplyBuilder::build(ERR_UNKNOWNMODE, *user, target, "Unknown or unsupported MODE target");
        return;
    }

    Channel* channel = _server.getChannelByName(target);
    if (!channel) {
    user->getSendBuffer() += ReplyBuilder::build(ERR_NOSUCHCHANNEL, *user, target);
    return;
    }

    if (!user->isInChannel(target)) {
        user->getSendBuffer() += ReplyBuilder::build(ERR_NOTONCHANNEL, *user, target);
        return;
    }

    if (args.size() == 1) {
        user->getSendBuffer() += ReplyBuilder::build(RPL_CHANNELMODEIS, *user, target, channel->getModeString());
        return;
    }

    if (!user->isChannelOperator(target)) {
        user->getSendBuffer() += ReplyBuilder::build(ERR_CHANOPRIVSNEEDED, *user, target);
        return;
    }

    std::string modeStr = args[1];
    bool adding = true;
    size_t paramIndex = 2;
    std::string replyModes;
    std::vector<std::string> replyParams;

    for (size_t i = 0; i < modeStr.size(); ++i) {
        char mode = modeStr[i];
        if (mode == '+') {
            adding = true;
            replyModes += "+";
        } else if (mode == '-') {
            adding = false;
            replyModes += "-";
        } else {
            switch (mode) {
                case 'i':
                    channel->setInviteOnly(adding);
                    replyModes += "i";
                    break;

                case 't':
                    channel->setTopicLocked(adding);
                    replyModes += "t";
                    break;

                case 'k':
                    if (adding) {
                        if (paramIndex >= args.size()) {
                            user->getSendBuffer() += ReplyBuilder::build(ERR_NEEDMOREPARAMS, *user, "MODE");
                            return;
                        }
                        std::string key = args[paramIndex++];
                        channel->setKey(key);
                        replyModes += "k";
                        replyParams.push_back(key);
                    } else {
                        channel->setKey("");
                        replyModes += "k";
                    }
                    break;

                case 'l':
                    if (adding) {
                        if (paramIndex >= args.size()) {
                            user->getSendBuffer() += ReplyBuilder::build(ERR_NEEDMOREPARAMS, *user, "MODE");
                            return;
                        }
                        int limit = std::atoi(args[paramIndex++].c_str());
                        if (limit <= 0) limit = 0;
                        channel->setUserLimit(limit);
                        replyModes += "l";
                        std::ostringstream oss;
                        oss << limit;
                        replyParams.push_back(oss.str());
                    } else {
                        channel->setUserLimit(0);
                        replyModes += "l";
                    }
                    break;

                case 'o':
                    if (paramIndex >= args.size()) {
                        user->getSendBuffer() += ReplyBuilder::build(ERR_NEEDMOREPARAMS, *user, "MODE");
                        return;
                    }
                    {
                        std::string nick = args[paramIndex++];
                        User* targetUser = _server.getUserByNick(nick);
                        if (!targetUser || !targetUser->isInChannel(target)) {
                            user->getSendBuffer() += ReplyBuilder::build(ERR_USERNOTINCHANNEL, *user, nick, target);
                            continue;
                        }
                        channel->setOperatorStatus(targetUser, adding);
                        targetUser->setChannelOperator(target, adding);
                        replyModes += "o";
                        replyParams.push_back(nick);
                    }
                    break;

                default:
                    user->getSendBuffer() += ReplyBuilder::build(ERR_UNKNOWNMODE, *user, std::string(1, mode), "is unknown mode char");
                    break;
            }
        }
    }

    if (replyModes.length() > 1) {
        std::string reply = ":" + user->getNickname() + " MODE " + target + " " + replyModes;
        for (size_t i = 0; i < replyParams.size(); ++i) {
            reply += " " + replyParams[i];
        }
        reply += "\r\n";
        channel->broadcast(reply, NULL, &_server);
    }
}

//Handle Part
void CommandHandler::handlePart(int fd, const std::vector<std::string>& args) {
    User* user = _server.getUserByFd(fd);

    // Must be REGISTERED
    if (user->getState() != User::REGISTERED) {
        user->getSendBuffer() += ReplyBuilder::build(ERR_NOTREGISTERED, *user);
        return;
    }

    // Must have at least one channel name
    if (args.empty() || args[0].empty()) {
        user->getSendBuffer() += ReplyBuilder::build(ERR_NEEDMOREPARAMS, *user, "PART");
        return;
    }

    // Split channel list (e.g. PART #chan1,#chan2) 
    std::string channelList = args[0];
    std::vector<std::string> channels;
    {
        std::stringstream ss(channelList);
        std::string item;
        while (std::getline(ss, item, ',')) {
            if (!item.empty()) {
                channels.push_back(item);
            }
        }
    }

    std::string partMessage;
    if (args.size() > 1) {
        partMessage = args[1];
        if (!partMessage.empty() && partMessage[0] == ':') {
            partMessage.erase(0, 1);
        }
        for (size_t i = 2; i < args.size(); ++i) {
            partMessage += " " + args[i];
        }
    }

    // For each channel, remove user if present and notify with PART
    for (size_t i = 0; i < channels.size(); ++i) {
        const std::string &chanName = channels[i];

        // If the user is not in that channel, reply with ERR_NOTONCHANNEL (442)
        if (!user->isInChannel(chanName)) {
            user->getSendBuffer() += ReplyBuilder::build(ERR_NOTONCHANNEL, *user, chanName);
            continue;
        }
        // Remove user from channel
        user->leaveChannel(chanName);

        // Send PART message
        user->getSendBuffer() += ":" + user->getNickname() + " PART " + chanName;
        if (!partMessage.empty()) {
            user->getSendBuffer() += " :" + partMessage;
        }
        user->getSendBuffer() += "\r\n";
    }
}

//Handle Join
void CommandHandler::handleJoin(int fd, const std::vector<std::string>& args) {
    User* user = _server.getUserByFd(fd);

    if (user->getState() != User::REGISTERED) {
        user->getSendBuffer() += ReplyBuilder::build(ERR_NOTREGISTERED, *user);
        return;
    }

    if (args.empty() || args[0].empty()) {
        user->getSendBuffer() += ReplyBuilder::build(ERR_NEEDMOREPARAMS, *user, "JOIN");
        return;
    }

    // Split channel names
    std::string channelList = args[0];
    std::stringstream ss(channelList);
    std::string chanName;
    std::vector<std::string> keys;

    // Parse keys if present
    if (args.size() > 1) {
        std::stringstream keyStream(args[1]);
        std::string key;
        while (std::getline(keyStream, key, ',')) {
            keys.push_back(key);
        }
    }

    size_t chanIndex = 0;
    while (std::getline(ss, chanName, ',')) {
        if (chanName.empty()) continue;

        if (chanName[0] != '#' && chanName[0] != '&') {
            user->getSendBuffer() += ReplyBuilder::build(ERR_BADCHANMASK, *user, chanName, "");
            ++chanIndex;
            continue;
        }
        
        // Check if user is already in the channel
        if (user->isInChannel(chanName)) {
            ++chanIndex;
            continue;
        }

        Channel* channel = _server.getOrCreateChannel(chanName, user);

        // Enforce invite-only mode (+i)
        if (channel->isInviteOnly() && !channel->isInvited(user)) {
            user->getSendBuffer() += ReplyBuilder::build(ERR_INVITEONLYCHAN, *user, chanName);
            ++chanIndex;
            continue;
        }

        // Enforce key (+k)
        if (channel->hasKey()) {
            std::string providedKey;
            if (chanIndex < keys.size()) {
                providedKey = keys[chanIndex];
            }

            if (providedKey != channel->getKey()) {
                user->getSendBuffer() += ReplyBuilder::build(ERR_BADCHANNELKEY, *user, chanName);
                ++chanIndex;
                continue;
            }
        }

        // Enforce user limit (+l)
        if (channel->getUserLimit() > 0 && channel->getUserCount() >= channel->getUserLimit()) {
            user->getSendBuffer() += ReplyBuilder::build(ERR_CHANNELISFULL, *user, chanName);
            ++chanIndex;
            continue;
        }

        // Passed all checks — join
        channel->addUser(user);
        user->joinChannel(chanName);
        if (channel->getUserCount() == 1)
            user->setChannelOperator(chanName, true);
        channel->removeInvite(user);

        std::string joinMsg = ":" + user->getPrefix() + " JOIN :" + chanName + "\r\n";
        user->getSendBuffer() += joinMsg;
        channel->broadcast(joinMsg, user, &_server);
        
        // Build the NAMES list
        std::string namesList;
        const std::set<User*>& members = channel->getUsers();
        for (std::set<User*>::const_iterator it = members.begin(); it != members.end(); ++it) {
            if (!namesList.empty())
                namesList += " ";
            if ((*it)->isChannelOperator(chanName))
                namesList += "@";
            namesList += (*it)->getNickname();
        }
        
        user->getSendBuffer() +=
            ":ircserv 353 " + user->getNickname() + " = " + chanName + " :" + namesList + "\r\n";


        user->getSendBuffer() +=
            ":ircserv 366 " + user->getNickname() + " " + chanName + " :End of /NAMES list\r\n";

        _server.flushSendBuffer(fd);

        ++chanIndex;
    }
}
