/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler_bonus.hpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anilchen <anilchen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/23 15:45:28 by stefan            #+#    #+#             */
/*   Updated: 2025/06/10 15:36:18 by anilchen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_HANDLER
# define COMMAND_HANDLER

//Forward declaration - it avaids circular dependencies
class Server;

#include "User_bonus.hpp"
#include "Channel_bonus.hpp"
#include "CommandParser_bonus.hpp"
#include <string>
#include <map>
#include <vector>

class CommandHandler {
public:
    CommandHandler(Server& server, const std::string& serverPassword);
    void handleCommand(int clientFd, const std::string& line);

private:
    typedef void (CommandHandler::*HandlerFn)(int, const std::vector<std::string>&);
    std::map<std::string, HandlerFn> _handlers;
    std::string _serverPassword;
    Server& _server;
    
    bool isValidNickname(const std::string& nick) const;
    bool isNicknameTaken(const std::string& nick) const;
    void tryRegisterUser(User* user);

    void handlePass(int fd, const std::vector<std::string>& args);
    void handleNick(int fd, const std::vector<std::string>& args);
    void handleUser(int fd, const std::vector<std::string>& args);
    void handleJoin(int fd, const std::vector<std::string>& args);
    void handlePart(int fd, const std::vector<std::string>& args);
    void handleMode(int fd, const std::vector<std::string>& args);
    void handleTopic(int fd, const std::vector<std::string>& args);
    void handleInvite(int fd, const std::vector<std::string>& args);
    void handleKick(int fd, const std::vector<std::string>& args);
    void handlePrivmsg(int fd, const std::vector<std::string>& args);
    void handleNotice(int fd, const std::vector<std::string>& args);
    void handleMessage(int fd, const std::vector<std::string>& args, bool isPrivmsg);
    void handlePing(int fd, const std::vector<std::string>& args);
    void handlePong(int fd, const std::vector<std::string>& args);
    void handleQuit(int fd, const std::vector<std::string>& args);
    void handleNames(int fd, const std::vector<std::string>& args);
};

#endif