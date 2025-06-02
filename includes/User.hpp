/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stefan <stefan@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 23:52:02 by stefan            #+#    #+#             */
/*   Updated: 2025/06/02 20:59:51 by stefan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <set>
#include <vector>
#include <map>

class User {
public:
    enum State {
        CONNECTED,
        PASSWORD_SENT,
        REGISTERED
    };

    User(int fd);
    ~User();

    // Getters
    int getFd() const;
    const std::string& getNickname() const;
    const std::string& getUsername() const;
    const std::string& getRealname() const;
    const std::string& getHostname() const;
    State getState() const;
    bool isOperator() const;
    const std::set<std::string>& getChannels() const;
    std::string& getRecvBuffer();
    std::string& getSendBuffer();
    void setChannelOperator(const std::string& channelName, bool on);
    bool isChannelOperator(const std::string& channelName) const;

    // Setters
    void setNickname(const std::string& nick);
    void setUsername(const std::string& user);
    void setRealname(const std::string& real);
    void setHostname(const std::string& host);
    void setState(State state);
    void setOperator(bool value);
    void SetRecvBuffer(const std::string& msg); //anna

    // Channel management
    void joinChannel(const std::string& name);
    void leaveChannel(const std::string& name);
    bool isInChannel(const std::string& name) const;

    void markForDisconnect();
    bool isMarkedForDisconnect() const;

    std::string getPrefix() const;

private:
    int _fd;
    std::string _nickname;
    std::string _username;
    std::string _realname;
    std::string _hostname;
    bool _isOperator;
    State _state;
    bool _markedForDisconnect;
    std::set<std::string> _channels;

    std::string _recvBuffer;
    std::string _sendBuffer;
    std::map<std::string, bool> _channelOps;
    //std::map<int, std::string> recvBuffers; //anna
    
};

#endif
