/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ReplyBuilder.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anilchen <anilchen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 13:55:01 by stefan            #+#    #+#             */
/*   Updated: 2025/06/02 14:06:35 by anilchen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ReplyBuilder.hpp"
#include <sstream>

std::string ReplyBuilder::_serverName = "ircserv";
std::map<int, std::string> ReplyBuilder::_templates;

void ReplyBuilder::init(const std::string &serverName)
{
	_serverName = serverName;
	_templates[RPL_WELCOME] = ":<server> 001 <nick> :Welcome to the Internet Relay Network <nick>";
	_templates[ERR_NEEDMOREPARAMS] = ":<server> 461 <nick> <arg> :Not enough parameters";
	_templates[ERR_ALREADYREGISTRED] = ":<server> 462 <nick> :You may not reregister";
	_templates[ERR_PASSWDMISMATCH] = ":<server> 464 <nick> :Password incorrect";
	_templates[ERR_NONICKNAMEGIVEN] = ":<server> 431 <nick> :No nickname given";
	_templates[ERR_ERRONEUSNICKNAME] = ":<server> 432 <nick> <arg> :Erroneous nickname";
	_templates[ERR_NICKNAMEINUSE] = ":<server> 433 <nick> <arg> :Nickname is already in use";
	_templates[ERR_NOTREGISTERED] = ":<server> 451 <nick> :You have not registered";
	_templates[ERR_NOTONCHANNEL] = ":<server> 442 <nick> <arg> :You're not on that channel";
	_templates[ERR_CHANOPRIVSNEEDED] = ":<server> 482 <nick> <arg> :You're not channel operator";
	_templates[ERR_UNKNOWNMODE] = ":<server> 472 <nick> <arg> :is unknown mode char";
	_templates[RPL_CHANNELMODEIS] = ":<server> 324 <nick> <arg> <msg>";
	_templates[ERR_INVITEONLYCHAN] = ":<server> 473 <nick> <arg> :Cannot join channel (+i)";
	_templates[ERR_BADCHANNELKEY] = ":<server> 475 <nick> <arg> :Cannot join channel (+k)";
	_templates[ERR_CHANNELISFULL] = ":<server> 471 <nick> <arg> :Cannot join channel (+l)";
	_templates[ERR_USERNOTINCHANNEL] = ":<server> 441 <nick> <arg> :They aren't on that channel";
	_templates[ERR_NOSUCHCHANNEL] = ":<server> 403 <nick> <arg> :No such channel";
	_templates[RPL_NOTOPIC] = ":<server> 331 <nick> <arg> :No topic is set";
	_templates[RPL_TOPIC] = ":<server> 332 <nick> <arg> :<msg>";
	_templates[ERR_UNKNOWNCOMMAND] = ":<server> 421 <nick> <arg> :Unknown command";
	_templates[RPL_INVITING] = ":<server> 341 <nick> <arg> :Inviting";
	_templates[RPL_AWAY] = ":<server> 301 <nick> <arg> :<msg>";
	_templates[ERR_USERONCHANNEL] = ":<server> 443 <nick> <arg> :is already on channel";
	_templates[ERR_NOSUCHNICK] = ":<server> 401 <nick> <arg> :No such nick/channel";
	_templates[ERR_CANNOTSENDTOCHAN] = ":<server> 404 <nick> <arg> :<msg>";
}

std::string ReplyBuilder::build(int code, const User &user,
	const std::string &arg, const std::string &msg)
{
	size_t	pos;

	std::string result = _templates[code];
	std::string nick = user.getNickname().empty() ? "*" : user.getNickname();
	while ((pos = result.find("<server>")) != std::string::npos)
		result.replace(pos, 8, _serverName);
	while ((pos = result.find("<nick>")) != std::string::npos)
		result.replace(pos, 6, nick);
	while ((pos = result.find("<arg>")) != std::string::npos)
		result.replace(pos, 5, arg);
	while ((pos = result.find("<msg>")) != std::string::npos)
		result.replace(pos, 5, msg);
	return (result + "\r\n");
}
