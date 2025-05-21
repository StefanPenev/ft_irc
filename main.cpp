/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anilchen <anilchen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 11:54:43 by anilchen          #+#    #+#             */
/*   Updated: 2025/05/21 13:40:10 by anilchen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <string>

//hexchat

int	main(int argc, char const **argv)
{
	int	port;

	if (argc != 3)
	{
		std::cerr << "Usage: ./ircserv <port> <password>\n";
		return (1);
	}
	for (int i = 0; argv[1][i] != '\0'; i++)
	{
		if (!std::isdigit(argv[1][i]))
		{
			std::cerr << "Error: Port must contain only digits\n";
			return (1);
		}
	}
	port = atoi(argv[1]);
	Server serv(port, argv[2]);
	serv.run();
	return (0);
}
