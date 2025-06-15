/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PollManager.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anilchen <anilchen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 13:12:22 by anilchen          #+#    #+#             */
/*   Updated: 2025/06/02 13:42:34 by anilchen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PollManager.hpp"
#include <iostream>
#include <poll.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <cerrno>

PollManager::PollManager() : _pollFds()
{
}

PollManager::~PollManager()
{
	for (size_t i = 0; i < _pollFds.size(); i++)
	{
		close(_pollFds[i].fd);
	}
}

void PollManager::addFd(int fd)
{
	pollfd	pfd;

	pfd.fd = fd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	_pollFds.push_back(pfd);
}

void PollManager::removeFd(int fd)
{
	for (size_t i = 0; i < _pollFds.size(); i++)
	{
		if (_pollFds[i].fd == fd)
		{
			_pollFds.erase(_pollFds.begin() + i);
			break ;
		}
	}
}

int PollManager::wait(void)
{
	int	numReady;

	numReady = poll(_pollFds.data(), _pollFds.size(), -1);
	if (numReady == -1)
	{
		if (errno == EINTR)
			return -1;
		throw std::runtime_error("poll() failed");
	}
	return (numReady);
	// int poll (struct pollfd *__fds, nfds_t __nfds, int __timeout)
	// _pollFds.data() - pointer to first pollfd in vector
	// _pollFds.size() - number of fds to poll
	// -1 - wait indefinitely
	// On success, poll() returns a nonnegative value which is the  number  of
	// elements in the pollfds whose revents fields have been set to a nonzero
	// value (indicating an event or an error).  A return value of zero  indi‐
	// cates that the system call timed out before any file descriptors became
	// read.
	// On error, -1 is returned,
}

std::vector<struct pollfd> &PollManager::getFds()
{
	return (this->_pollFds);
}
