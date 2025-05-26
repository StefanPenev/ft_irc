/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stefan <stefan@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/25 10:28:55 by stefan            #+#    #+#             */
/*   Updated: 2025/05/25 19:51:27 by stefan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <map>
#include <set>

class User;

class Channel {
private:
    std::string _name;
    std::string _topic;
    bool _inviteOnly;         // +i
    bool _topicLocked;        // +t
    std::string _key;         // +k
    bool _hasKey;
    int _userLimit;           // +l

    std::map<int, bool> _operators;
    std::set<User*> _members;
    std::set<User*> _invited;

public:
    Channel(const std::string& name);
    
    void setInviteOnly(bool on);
    bool isInviteOnly() const;

    void setTopicLocked(bool on);
    bool isTopicLocked() const;

    void setKey(const std::string& key);
    void clearKey();
    bool hasKey() const;
    std::string getKey() const;

    void setUserLimit(int limit);
    void clearUserLimit();
    int getUserLimit() const;

    void setOperatorStatus(User* user, bool on);
    bool isOperator(User* user) const;

    void addUser(User* user);
    void removeUser(User* user);
    bool isMember(User* user) const;

    int getUserCount() const;

    void inviteUser(User* user);
    bool isInvited(User* user) const;
    void removeInvite(User* user);

    std::string getModeString() const;
    const std::string& getName() const;

    void setTopic(const std::string& topic);
    const std::string& getTopic() const;
    bool hasTopic() const;

    void broadcast(const std::string& message, User* except = NULL);
};

#endif
