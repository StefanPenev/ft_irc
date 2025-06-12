/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anilchen <anilchen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 16:16:26 by anilchen          #+#    #+#             */
/*   Updated: 2025/06/12 16:22:49 by anilchen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Bot.hpp"
#include <iostream>
#include <string>

Bot::Bot(int fd, const std::string &name) : User(fd, name)
{
}

std::string Bot::handleMessage(const std::string &msg)
{
	std::cout << "[BOT]: Received message: " << msg << "\n";
	for (size_t i = 0; i != msg.size(); i++)
	{
		tolower(msg[i]);
	}
	if (msg.find("hello") != std::string::npos
		|| msg.find("hi") != std::string::npos)
	{
		return ("Hi! I'm a ft_irc-bot!\n");
	}
	else if (msg.find("help") != std::string::npos)
	{
		return "Available commands: hello, help, about\n";
	}
	else if (msg.find("about") != std::string::npos)
	{
		return "I'm a bot created for the ft_irc project at 42. My creators`s names is Anna and Stefan.\n";
	}
	return ("Sorry, I don't understand.\n");
}