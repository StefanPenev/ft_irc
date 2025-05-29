/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stefan <stefan@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 11:28:46 by anilchen          #+#    #+#             */
/*   Updated: 2025/05/30 00:29:22 by stefan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>

//new
# include <map>
# include "User.hpp"
# include "Channel.hpp"
# include "CommandHandler.hpp"
//////

class Server
{
  private:
	int _serverSocket;
	std::string _password;
	int _port;

	int setupSocket();
	struct addrinfo *resolveAddress(const std::string &portStr);
	int createSocket(struct addrinfo *res);
	void enableAddressReuse(int sockfd);
	void bindSocket(int sockfd, struct addrinfo *res);
	void startListening(int sockfd);

	//new
	std::map<int, User*> _users;
    std::map<std::string, Channel*> _channels;
	CommandHandler* _commandHandler;
	/////
	
  public:
	Server();
	Server(int port, const std::string &password);
	~Server();
	void run();

	//new
	User* getUserByFd(int fd);
    Channel* getChannelByName(const std::string& name);
	void flushSendBuffer(int fd);
    void addUser(int fd, User* user);
    void addChannel(const std::string& name, Channel* channel);
	User* getUserByNick(const std::string& nick);
	Channel* getOrCreateChannel(const std::string& name, User* user);
	/////
};

#endif
