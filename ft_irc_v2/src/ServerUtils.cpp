/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerUtils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stefan <stefan@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 01:12:14 by stefan            #+#    #+#             */
/*   Updated: 2025/06/15 00:02:03 by stefan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Channel.hpp"
#include "User.hpp"
#include <iostream>
#include <sys/socket.h>
#include <cstdio>
#include <unistd.h>

/**
 * Retrieves a User object by its associated file descriptor.
 * Returns a pointer to the User if found, NULL otherwise.
 */
User* Server::getUserByFd(int fd) {
    std::map<int, User*>::iterator it = _users.find(fd);
    if (it != _users.end()) {
        return it->second;
    }
    return NULL;
}

/**
 * Safely removes a user from the server by their file descriptor.
 * - Removes the user from all joined channels.
 * - Closes the user's socket.
 * - Deletes the User object and erases it from the user map.
 */
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

/**
 * Retrieves a Channel object by its name.
 * Returns a pointer to the Channel if found, NULL otherwise.
 */
Channel* Server::getChannelByName(const std::string& name) {
    std::map<std::string, Channel*>::iterator it = _channels.find(name);
    if (it != _channels.end()) {
        return it->second;
    }
    return NULL;
}

/**
 * Adds a new user to the server, associated with the given file descriptor.
 */
void Server::addUser(int fd, User* user) {
    if (user) {
        _users[fd] = user;
    }
}

/**
 * Adds a new channel to the server under the given name.
 */
void Server::addChannel(const std::string& name, Channel* channel) {
    if (channel) {
        _channels[name] = channel;
    }
}

/**
 * Retrieves a User object by their nickname.
 * Returns a pointer to the User if found, NULL otherwise.
 */
User* Server::getUserByNick(const std::string& nick) {
    for (std::map<int, User*>::iterator it = _users.begin(); it != _users.end(); ++it) {
        if (it->second->getNickname() == nick) {
            return it->second;
        }
    }
    return 0;
}

/**
 * Sends the contents of a user's send buffer over their socket.
 * On success, the sent portion is removed from the buffer.
 * If sending fails, an error is logged but no retry logic is applied.
 */
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

/**
 * Retrieves a Channel by name if it exists; otherwise creates it.
 * - Adds the given user to the channel.
 * - Grants operator status to the user.
 * - Adds the channel to the server's channel map.
 * Returns a pointer to the Channel.
 */
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
