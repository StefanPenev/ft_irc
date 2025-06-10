/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ReplyCode.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stefan <stefan@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 13:54:02 by stefan            #+#    #+#             */
/*   Updated: 2025/06/09 12:54:10 by stefan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REPLY_CODE_HPP
#define REPLY_CODE_HPP

enum ReplyCode {
    RPL_WELCOME = 1,
    RPL_CHANNELMODEIS = 324,
    RPL_NOTOPIC = 331,
    RPL_TOPIC = 332,
    RPL_INVITING = 341,
    RPL_AWAY = 301,

    ERR_NONICKNAMEGIVEN = 431,
    ERR_ERRONEUSNICKNAME = 432,
    ERR_NICKNAMEINUSE = 433,

    ERR_NOTREGISTERED = 451,
    ERR_NEEDMOREPARAMS = 461,
    ERR_ALREADYREGISTRED = 462,
    ERR_PASSWDMISMATCH = 464,

    ERR_NOSUCHCHANNEL = 403,
    ERR_NOSUCHNICK = 401,
    ERR_NOTONCHANNEL = 442,
    ERR_USERONCHANNEL = 443,
    ERR_USERNOTINCHANNEL = 441,
    ERR_UNKNOWNMODE = 472,
    ERR_CHANOPRIVSNEEDED = 482,
    ERR_CANNOTSENDTOCHAN = 404,
    ERR_BADCHANMASK = 476,

    ERR_INVITEONLYCHAN = 473,
    ERR_BADCHANNELKEY = 475,
    ERR_CHANNELISFULL = 471,

    ERR_UNKNOWNCOMMAND = 421
};

#endif
