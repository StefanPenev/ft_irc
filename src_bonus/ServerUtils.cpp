/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerUtils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anilchen <anilchen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 01:12:14 by stefan            #+#    #+#             */
/*   Updated: 2025/06/10 15:39:33 by anilchen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server_bonus.hpp"
#include "Channel_bonus.hpp"
#include "User_bonus.hpp"
#include <iostream>
#include <sys/socket.h>
#include <cstdio>
#include <unistd.h>

User* Server::getUserByFd(int fd) {
    std::map<int, User*>::iterator it = _users.find(fd);
    if (it != _users.end()) {
        return it->second;
    }
    return NULL;
}

void Server::removeUserByFd(int fd) {
	std::map<int, User*>::iterator it = _users.find(fd);
	if (it == _users.end())
		return;

	User* user = it->second;
	const std::set<std::string>& channels = user->getChannels();
	for (std::set<std::string>::const_iterator ch = channels.begin(); ch != channels.end(); ++ch) {
		Channel* channel = getChannelByName(*ch);
		if (channel) {
			channel->removeUser(user);
		}
	}
	_pollManager.removeFd(fd);
	close(fd);
	delete user;
	_users.erase(it);
}

Channel* Server::getChannelByName(const std::string& name) {
    std::map<std::string, Channel*>::iterator it = _channels.find(name);
    if (it != _channels.end()) {
        return it->second;
    }
    return NULL;
}

void Server::addUser(int fd, User* user) {
    if (user) {
        _users[fd] = user;
    }
}

void Server::addChannel(const std::string& name, Channel* channel) {
    if (channel) {
        _channels[name] = channel;
    }
}

User* Server::getUserByNick(const std::string& nick) {
    for (std::map<int, User*>::iterator it = _users.begin(); it != _users.end(); ++it) {
        if (it->second->getNickname() == nick) {
            return it->second;
        }
    }
    return 0;
}

void Server::flushSendBuffer(int fd) {
    User* user = getUserByFd(fd);
    if (!user) {
        std::cerr << "[ERROR] flushSendBuffer: User for FD " << fd << " not found" << std::endl;
        return;
    }

    std::string& buffer = user->getSendBuffer();
    if (buffer.empty()) {
        return;
    }

    ssize_t bytesSent = send(fd, buffer.c_str(), buffer.size(), 0);
    if (bytesSent < 0) {
        perror("[ERROR] send");
        return;
    }
    buffer.erase(0, bytesSent);
}

Channel* Server::getOrCreateChannel(const std::string& name, User* user) {
    Channel* channel = getChannelByName(name);
    if (!channel) {
        channel = new Channel(name);
        _channels[name] = channel;

        channel->addUser(user);
        channel->setOperatorStatus(user, true);
        user->joinChannel(name);
    }
    return channel;
}
