/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_bonus.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anilchen <anilchen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 11:28:46 by anilchen          #+#    #+#             */
/*   Updated: 2025/06/10 15:37:12 by anilchen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Channel_bonus.hpp"
# include "CommandHandler_bonus.hpp"
# include "PollManager_bonus.hpp"
# include "User_bonus.hpp"
# include <map>
# include <string>

class Server
{
  private:
	int _serverSocket;
	int _port;
	std::string _password;
	PollManager _pollManager;

	int setupSocket();
	struct addrinfo *resolveAddress(const std::string &portStr);
	int createSocket(struct addrinfo *res);
	void enableAddressReuse(int sockfd);
	void bindSocket(int sockfd, struct addrinfo *res);
	void startListening(int sockfd);
	void handleNewConnection();
	void handleClientMessage(int clientFd);
	std::map<int, User *> _users;
	std::map<std::string, Channel *> _channels;
	CommandHandler *_commandHandler;
	void CreateBot();

  public:
	static bool running;

	Server();
	Server(int port, const std::string &password);
	~Server();
	void run();

	User *getUserByFd(int fd);
	void removeUserByFd(int fd);
	Channel *getChannelByName(const std::string &name);
	void flushSendBuffer(int fd);
	void addUser(int fd, User *user);
	void addChannel(const std::string &name, Channel *channel);
	User *getUserByNick(const std::string &nick);
	Channel *getOrCreateChannel(const std::string &name, User *user);
};

#endif
