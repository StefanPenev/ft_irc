/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stefan <stefan@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 11:28:46 by anilchen          #+#    #+#             */
/*   Updated: 2025/06/01 21:06:17 by stefan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include "Channel.hpp"
# include "CommandHandler.hpp"
# include "PollManager.hpp"
# include "User.hpp"
# include <map>

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

  public:
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
