/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stefan <stefan@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 11:54:43 by anilchen          #+#    #+#             */
/*   Updated: 2025/05/31 00:45:43 by stefan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ReplyBuilder.hpp"
#include "Server.hpp"
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <string>

// hexchat

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
	serv.run();
	return (0);
}
