/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anilchen <anilchen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/25 10:29:52 by stefan            #+#    #+#             */
/*   Updated: 2025/06/10 15:39:02 by anilchen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel_bonus.hpp"
#include "Server_bonus.hpp"
#include "User_bonus.hpp"
#include <sstream>
#include <cstdlib>

Channel::Channel(const std::string& name)
    : _name(name),
      _inviteOnly(false),
      _topicLocked(false),
      _key(""),
      _hasKey(false),
      _userLimit(0) {}

void Channel::setInviteOnly(bool on) {
    _inviteOnly = on;
}

bool Channel::isInviteOnly() const {
    return _inviteOnly;
}

void Channel::setTopicLocked(bool on) {
    _topicLocked = on;
}

bool Channel::isTopicLocked() const {
    return _topicLocked;
}

void Channel::setKey(const std::string& key) {
    if (key.empty())
    {
        _key.clear();
        _hasKey = false;
    }
    else
    {
        _key = key;
        _hasKey = true;
    }
}

void Channel::clearKey() {
    _key.clear();
    _hasKey = false;
}

bool Channel::hasKey() const {
    return _hasKey;
}

std::string Channel::getKey() const {
    return _key;
}

void Channel::setUserLimit(int limit) {
    _userLimit = (limit > 0) ? limit : 0;
}

void Channel::clearUserLimit() {
    _userLimit = 0;
}

int Channel::getUserLimit() const {
    return _userLimit;
}

void Channel::setOperatorStatus(User* user, bool on) {
    if (!user) return;
    if (on)
        _operators[user->getFd()] = true;
    else
        _operators.erase(user->getFd());
}

bool Channel::isOperator(User* user) const {
    if (!user) return false;
    std::map<int, bool>::const_iterator it = _operators.find(user->getFd());
    return it != _operators.end();
}

void Channel::addUser(User* user) {
    _members.insert(user);
    _invited.erase(user);
    if (_members.size() == 1)
        setOperatorStatus(user, true);
}

void Channel::removeUser(User* user) {
    _members.erase(user);
    _operators.erase(user->getFd());
}

bool Channel::isMember(User* user) const {
    return _members.find(user) != _members.end();
}

int Channel::getUserCount() const {
    return _members.size();
}

void Channel::inviteUser(User* user) {
    if (user)
        _invited.insert(user);
}

bool Channel::isInvited(User* user) const {
    return user && (_invited.find(user) != _invited.end());
}

void Channel::removeInvite(User* user) {
    _invited.erase(user);
}

std::string Channel::getModeString() const {
    std::ostringstream oss;
    oss << "+";
    if (_inviteOnly)  oss << "i";
    if (_topicLocked) oss << "t";
    if (_hasKey)      oss << "k";
    if (_userLimit > 0) oss << "l";
    return oss.str();
}

const std::string& Channel::getName() const {
    return _name;
}

void Channel::setTopic(const std::string& topic) {
    _topic = topic;
}

const std::string& Channel::getTopic() const {
    return _topic;
}

bool Channel::hasTopic() const {
    return !_topic.empty();
}

const std::set<User*>& Channel::getUsers() const {
    return _members;
}

void Channel::broadcast(const std::string& message, User* except, Server* server) {
    for (std::set<User*>::iterator it = _members.begin(); it != _members.end(); ++it) {
        if (*it != except) {
            (*it)->getSendBuffer() += message;
            if (server)
                server->flushSendBuffer((*it)->getFd());
        }
    }
}
