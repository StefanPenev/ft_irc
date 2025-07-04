/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stefan <stefan@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 12:15:14 by anilchen          #+#    #+#             */
/*   Updated: 2025/06/14 23:54:14 by stefan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "CommandHandler.hpp"
#include "PollManager.hpp"
#include "ReplyBuilder.hpp"
#include "Server.hpp"
#include "User.hpp"
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <poll.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

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
		perror("[ERROR]: socket()");
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
		perror("[ERROR]: bind()");
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
		perror("[ERROR]: listen()");
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
	_pollManager.addFd(sockfd);
	freeaddrinfo(res);
	// res->ai_addr and res->ai_addrlen received from getaddrinfo()
	return (sockfd);
}

void Server::handleNewConnection()
{
	int						clientid;
	struct sockaddr_storage	addr;
	socklen_t				addrlen;
	User					*newUser;

	addrlen = sizeof(addr);
	clientid = accept(this->_serverSocket, (struct sockaddr *)&addr, &addrlen);
	if (clientid == -1)
	{
		perror("[ERROR] accept()");
		return ;
	}
	// int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
	// sockfd is the listening socket descriptor. Simple enough.
	// --------------
	// addr will usually be a pointer to a local sockaddr structure.
	// This is where information about incoming connections goes (you can use it to determine
	// which host is calling you and from which port).
	// --------------
	// addrlen is a local integer variable that must contain the size of the
	// struct sockaddr_storage before its address is passed to accept().
	// accept() will not write more bytes to addr than specified. If it writes fewer than specified,
	// it will change the value of addrlen.
	std::cout << "[SERVER] New connection accepted. FD = " << clientid << std::endl;
	if (_users.find(clientid) == _users.end())
	{
		newUser = new User(clientid);
		_users[clientid] = newUser;
	}
	else
	{
		newUser = _users[clientid];
	}
	newUser->SetRecvBuffer("");
	_pollManager.addFd(clientid);
}

void Server::handleClientMessage(int clientFd)
{
    int bytesRead;
    std::string buf(1024, '\0');
    bytesRead = recv(clientFd, &buf[0], 1024, 0);

    if (bytesRead <= 0)
    {
        std::cerr << "[WARNING] Client " << clientFd
                  << " disconnected or recv failed. bytesRead=" << bytesRead << std::endl;
        removeUserByFd(clientFd);
        return;
    }

    std::string &buffer = _users[clientFd]->getRecvBuffer();
    buffer.append(buf, 0, bytesRead);

    size_t pos;
    while ((pos = buffer.find("\r\n")) != std::string::npos)
    {
        std::string line = buffer.substr(0, pos);
        buffer.erase(0, pos + 2);

        try {
            _commandHandler->handleCommand(clientFd, line);
        } catch (const std::exception &ex) {
            std::cerr << "[ERROR] Exception in handleCommand: " << ex.what() << std::endl;
        } catch (...) {
            std::cerr << "[ERROR] Unknown error in handleCommand" << std::endl;
        }

        if (getUserByFd(clientFd))
        {
            flushSendBuffer(clientFd);
        }
        else
        {
            break;
        }
    }
}

void Server::run()
{
	int	numReady;
	int	handled;

	// std::map<int, std::string> recvBuffers;
	std::cout << "[SERVER] Running and listening on port " << _port << std::endl;
	std::cout << "[SERVER] Waiting for new client connection..." << std::endl;
	while (true)
	{
		handled = 0;
		numReady = _pollManager.wait();
		if (numReady == -1)
		{
			throw std::runtime_error("poll() failed");
		}
		if (numReady == 0)
		{
			continue ;
		}
		std::vector<pollfd> &fds = _pollManager.getFds();
		for (size_t i = 0; i < fds.size() && handled < numReady; i++)
		{
			if (fds[i].fd == _serverSocket && fds[i].revents & POLLIN)
			{
				handleNewConnection();
				handled++;
			}
			if (fds[i].fd != _serverSocket && fds[i].revents & POLLIN)
			{
				handleClientMessage(fds[i].fd);
				handled++;
			}
		}
	}
}

Server::Server(int port, const std::string &password) : _port(port),
	_password(password), _pollManager()
{
	this->_serverSocket = setupSocket();
	this->_commandHandler = new CommandHandler(*this, this->_password);
}

Server::~Server()
{
    for (std::map<std::string, Channel*>::iterator it = _channels.begin();
         it != _channels.end(); 
         ++it)
    {
        delete it->second;
    }

    for (std::map<int, User*>::iterator uit = _users.begin();
         uit != _users.end();
         ++uit)
    {
        delete uit->second;
    }
    delete this->_commandHandler;
    std::cout << "DEBUG: Server is destroyed" << "\n";
}
