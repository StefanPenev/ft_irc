/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anilchen <anilchen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 16:16:26 by anilchen          #+#    #+#             */
/*   Updated: 2025/06/10 16:57:02 by anilchen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Bot.hpp"
#include <iostream>

Bot::Bot(int fd, const std::string &name) : User(fd, name)
{
}

void Bot::handleMessage(const std::string &msg)
{
	std::cout << "[BOT]: Received message: " << msg << std::endl;
	for (int i = 0; i != msg.size(); i++)
	{
		tolower(msg[i]);
	}
	if (msg.find("hello") != std::string::npos
		|| msg.find("hi") != std::string::npos)
	{
		_sendBuffer = "Hi! I'm a ft_irc-bot!\n";
	}
}