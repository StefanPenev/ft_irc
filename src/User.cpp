/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anilchen <anilchen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 23:51:59 by stefan            #+#    #+#             */
/*   Updated: 2025/05/30 15:36:39 by anilchen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "User.hpp"

User::User(int fd)
    : _fd(fd), _isOperator(false), _state(CONNECTED) {}

User::~User() {}

int User::getFd() const { return _fd; }
const std::string& User::getNickname() const { return _nickname; }
const std::string& User::getUsername() const { return _username; }
const std::string& User::getRealname() const { return _realname; }
const std::string& User::getHostname() const { return _hostname; }
User::State User::getState() const { return _state; }
bool User::isOperator() const { return _isOperator; }
const std::set<std::string>& User::getChannels() const { return _channels; }
std::string& User::getRecvBuffer() { return _recvBuffer; }
std::string& User::getSendBuffer() { return _sendBuffer; }

void User::setNickname(const std::string& nick) { _nickname = nick; }
void User::setUsername(const std::string& user) { _username = user; }
void User::setRealname(const std::string& real) { _realname = real; }
void User::setHostname(const std::string& host) { _hostname = host; }
void User::setState(State state) { _state = state; }
void User::setOperator(bool value) { _isOperator = value; }

void User::joinChannel(const std::string& name) {
    _channels.insert(name);
}

void User::leaveChannel(const std::string& name) {
    _channels.erase(name);
}

bool User::isInChannel(const std::string& name) const {
    return _channels.find(name) != _channels.end();
}

void User::setChannelOperator(const std::string& channelName, bool on) {
    // Only set if user is in channel
    if (!isInChannel(channelName))
        return;
    if (on)
        _channelOps[channelName] = true;
    else
        _channelOps.erase(channelName);
}

bool User::isChannelOperator(const std::string& channelName) const {
    if (!isInChannel(channelName))
        return false;
    std::map<std::string, bool>::const_iterator it = _channelOps.find(channelName);
    return (it != _channelOps.end());
}

std::string User::getPrefix() const {
    return _nickname + "!" + _username + "@" + _hostname;
}


void User::SetRecvBuffer(const std::string &msg) //anna
{
    this->_recvBuffer = msg;
}