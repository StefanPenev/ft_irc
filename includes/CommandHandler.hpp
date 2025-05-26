/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stefan <stefan@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/23 15:45:28 by stefan            #+#    #+#             */
/*   Updated: 2025/05/25 20:01:10 by stefan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_HANDLER
# define COMMAND_HANDLER

#include "User.hpp"
#include "Channel.hpp"
#include "CommandParser.hpp"
#include <string>
#include <map>
#include <vector>

class CommandHandler {
public:
    CommandHandler(const std::string& serverPassword, std::map<int, User*>& users);
    void handleCommand(int clientFd, const std::string& line);
    User* findUserByNick(const std::string& nick);

private:
    typedef void (CommandHandler::*HandlerFn)(int, const std::vector<std::string>&);
    std::map<std::string, HandlerFn> _handlers;
    std::string _serverPassword;
    std::map<int, User*>& _users;    
    std::map<std::string, Channel*> _channels;
    
    bool isValidNickname(const std::string& nick) const;
    bool isNicknameTaken(const std::string& nick) const;
    void tryRegisterUser(User* user);

    void handlePass(int fd, const std::vector<std::string>& args);
    void handleNick(int fd, const std::vector<std::string>& args);
    void handleUser(int fd, const std::vector<std::string>& args);
    void handleJoin(int fd, const std::vector<std::string>& args);
    void handlePart(int fd, const std::vector<std::string>& args);
    void handleMode(int fd, const std::vector<std::string>& args);
    //void handleTopic(int fd, const std::vector<std::string>& args);
    // void handleInvite(int fd, const std::vector<std::string>& args);
    // void handleKick(int fd, const std::vector<std::string>& args);
    // void handlePrivmsg(int fd, const std::vector<std::string>& args);
};

#endif