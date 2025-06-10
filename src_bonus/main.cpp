/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anilchen <anilchen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 11:54:43 by anilchen          #+#    #+#             */
/*   Updated: 2025/06/10 15:39:06 by anilchen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ReplyBuilder_bonus.hpp"
#include "Server_bonus.hpp"
#include <cctype>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <string>

// hexchat

bool Server::running = true;

void	handleSigint(int sig)
{
	(void)sig;
	Server::running = false;
}

int	main(int argc, char const **argv)
{
	int	port;

	if (argc != 3)
	{
		std::cerr << "[USAGE]: ./ircserv <port> <password>\n";
		return (1);
	}
	for (int i = 0; argv[1][i] != '\0'; i++)
	{
		if (!std::isdigit(argv[1][i]))
		{
			std::cerr << "[ERROR]: Port must contain only digits\n";
			return (1);
		}
	}
	port = atoi(argv[1]);
	if (port < 1024 || port > 65535)
	{
		std::cerr << "[ERROR]: Invalid port number. Please specify a number between 1024 and 65535.\n";
		return (1);
	}
	ReplyBuilder::init("ircserv");
	Server serv(port, argv[2]);
	std::signal(SIGINT, handleSigint);
	try
	{
		serv.run();
	}
	catch (const std::exception &e)
	{
		std::cerr << "Exception caught: " << e.what() << '\n';
	}
	return (0);
}
