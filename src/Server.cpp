/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stefan <stefan@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 12:15:14 by anilchen          #+#    #+#             */
/*   Updated: 2025/05/30 01:11:51 by stefan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "CommandHandler.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

//new
#include "User.hpp"
#include "Channel.hpp"
/////

struct addrinfo *Server::resolveAddress(const std::string &portStr)
{
	int	status;

	struct addrinfo hints, *res;
	// hints is local stack variable
	// res will be dinamically allocated by getaddrinfo
	std::memset(&hints, 0, sizeof(hints)); // assign all hints to 0
	hints.ai_family = AF_INET;             // reassign to IPv4
	hints.ai_socktype = SOCK_STREAM;       // reassign to TCP
	hints.ai_flags = AI_PASSIVE;
	// reassigning for bind() with INADDR_ANY
	status = getaddrinfo(NULL, portStr.c_str(), &hints, &res);
	// int getaddrinfo(const char *node, const char *service,
	// const struct addrinfo *hints,  struct addrinfo **res);
	// node = NULL → any address (INADDR_ANY)
	// service = port number as string (PortString.c_str() to convert string PortString to const char *)
	// hints = settings
	// res = output
	if (status != 0)
	{
		std::cerr << "getaddrinfo error\n";
		exit(1);
	}
	return (res);
}

int Server::createSocket(struct addrinfo *res)
{
	int	sockfd;

	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	// int socket(int domain, int type, int protocol);
	// create a socket based on what getaddrinfo returned
	if (sockfd == -1)
	{
		perror("socket");
		freeaddrinfo(res);
		exit(1);
	}
	return (sockfd);
}

void Server::enableAddressReuse(int sockfd)
{
	int	opt;

	opt = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	// Hey, if the port is already in use, let me reuse it anyway
	// 	- crash protection
	// int setsockopt(int sockfd, int level, int optname,
	// const void *optval, socklen_t optlen);
	// level - SOL_SOCKET means we want to set the socket level option,
	// 	not IP protocol, TCP, etc.
	// optname SO_REUSEADDR
	//	- Allow reuse of address (IP:PORT) even if it is still busy
	// optval (1 = turn it on, 0 - turn it off)
}

void Server::bindSocket(int sockfd, struct addrinfo *res)
{
	if (bind(sockfd, res->ai_addr, res->ai_addrlen) != 0)
	{
		perror("bind");
		freeaddrinfo(res);
		close(sockfd);
		exit(1);
	}
	// res->ai_addr and res->ai_addrlen received from getaddrinfo()
}

void Server::startListening(int sockfd)
{
	if (listen(sockfd, SOMAXCONN) != 0)
	{
		perror("listen");
		close(sockfd);
		exit(1);
	}
	// int listen(int sockfd, int backlog);
	// SOMAXCONN — system maximum connection queue (usually 128)
	// listen()  marks  the  socket referred to by sockfd as a passive socket,
	// that is, as a socket that will be used to  accept  incoming  connection
	// requests using accept(2).
}

int Server::setupSocket()
{
	struct addrinfo	*res;
	int				sockfd;

	std::ostringstream oss;
	oss << this->_port;
	std::string portStr = oss.str();
	res = resolveAddress(portStr);
	// res will be dynamically allocated by getaddrinfo
	sockfd = createSocket(res);
	// create a socket based on what getaddrinfo returned
	enableAddressReuse(sockfd);
	// if the port is already in use, let me reuse it anyway - crash protection
	bindSocket(sockfd, res);
	// When a socket is created with socket(),
	// it exists in a name space (address family) but has no address assigned to it. bind() assigns the address specified by addr to the socket referred to by the file descriptor sockfd
	startListening(sockfd);
	// listen() marks the socket referred to by sockfd as a passive socket,
	// that is,
	// as a socket that will be used to accept incoming connection requests using accept(2).
	freeaddrinfo(res);
	// res->ai_addr and res->ai_addrlen received from getaddrinfo()
	return (sockfd);
}

void Server::run()
{
	//new
	struct sockaddr_storage addr;
	socklen_t addrlen = sizeof(addr);
	int clientid;
	int bytesRead;
	
	std::map<int, std::string> recvBuffers;
	
	std::cout << "[SERVER] Running and listening on port " << _port << std::endl;
	
	while (true) {
		std::cout << "[SERVER] Waiting for new client connection..." << std::endl;
		clientid = accept(this->_serverSocket, (struct sockaddr*)&addr, &addrlen);
		if (clientid == -1) {
			perror("[ERROR] accept()");
			continue;
		}
	
		std::cout << "[SERVER] New connection accepted. FD = " << clientid << std::endl;
	
		//CHAINING POINT: User object is created as soon as a new client connects
		if (_users.find(clientid) == _users.end()) {
			User* newUser = new User(clientid);
			_users[clientid] = newUser;
		}
	
		std::string buf(1024, '\0');
		bytesRead = recv(clientid, &buf[0], 1024, 0);
		if (bytesRead <= 0) {
			std::cerr << "[WARNING] Client " << clientid << " disconnected or recv failed. bytesRead=" << bytesRead << std::endl;
			close(clientid);
			delete _users[clientid];
			_users.erase(clientid);
			recvBuffers.erase(clientid);
			continue;
		}
	
		std::string& buffer = recvBuffers[clientid];
		buffer.append(buf, 0, bytesRead);
	
		size_t pos;
		//CHAINING POINT: Check for complete command line (IRC commands end with \r\n)
		while ((pos = buffer.find("\r\n")) != std::string::npos) {
			std::string line = buffer.substr(0, pos);
			buffer.erase(0, pos + 2);
	
			//CHAINING POINT: Call to Team B's command processing logic
			try {
				_commandHandler->handleCommand(clientid, line);
			} catch (const std::exception& ex) {
				std::cerr << "[ERROR] Exception in handleCommand: " << ex.what() << std::endl;
			} catch (...) {
				std::cerr << "[ERROR] Unknown error in handleCommand" << std::endl;
			}
	
			flushSendBuffer(clientid);
		}
	}	
	///////////////////////

	// struct sockaddr	addr;
	// socklen_t		addrlen;
	// int				clientid;
	// int				bytesRead;

	// //std::string buf(1024, '\0');
	// addrlen = sizeof(addr);
	// std::cout << "Server is running and listening on port " << _port << "\n";
	// clientid = accept(this->_serverSocket, &addr, &addrlen);
	// if (clientid == -1)
	// {
	// 	perror("accept");
	// 	return ;
	// }
	// //   int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
	// // sockfd is the listening socket descriptor. Simple enough.
	// // --------------
	// // addr will usually be a pointer to a local sockaddr structure.
	// // This is where information about incoming connections goes (you can use it to determine
	// // which host is calling you and from which port).
	// // --------------
	// // addrlen is a local integer variable that must contain the size of the
	// // struct sockaddr_storage before its address is passed to accept(). accept()
	// // will not write more bytes to addr than specified. If it writes fewer than specified,
	// // it will change the value of addrlen.
	// while (true)
	// {
    //     std::string buf(1024, '\0');
	// 	bytesRead = recv(clientid, &buf[0], 1024, 0);
	// 	// int recv(int sockfd, void *buf, int len, int flags);
	// 	// sockfd is the socket descriptor to read from,
	// 	// buf is the buffer to read from,
	// 	// len is the maximum length of the buffer,
	// 	// flags can again be set to 0 (see the recv() man page.)
	// 	if (bytesRead == -1)
	// 		break ;
	// 	std::cout << "Client says: " << buf.substr(0, bytesRead) << "\n";
    //     std::string reply = "KILL ALL HUMANS!!!\n";
	// 	send(clientid, reply.c_str(), reply.size(), 0);
	// 	// int send(int sockfd, const void *msg, int len, int flags);
	// 	// sockfd is the socket descriptor where you want to send the data
	// 	//(probably returned by socket() or received by accept().)
	// 	// msg is a pointer to the data to send.
	// 	// len is the length of that data in bytes.
	// 	// flags is simply set to 0 . (See the man page for the send()
	// 	// call for some information on flags.)
    //     buf.clear();
	// }
    // close(clientid);
}

Server::Server(int port, const std::string &password)
{
	this->_port = port;
	this->_password = password;
	this->_serverSocket = setupSocket();

	//new
	this->_commandHandler = new CommandHandler(*this, this->_password);
}

Server::~Server()
{	
	//new
	delete this->_commandHandler;
	std::cout << "DEBUG: Server is destroyed" << "\n";
}
