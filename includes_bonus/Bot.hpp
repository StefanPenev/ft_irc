/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anilchen <anilchen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 16:15:31 by anilchen          #+#    #+#             */
/*   Updated: 2025/06/12 16:16:11 by anilchen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BOT_HPP
# define BOT_HPP

#define BOT_FD -2

# include "User_bonus.hpp"
#include <string>

class Bot : public User
{
  public:
	Bot(int fd, const std::string &name);
	~Bot();
	std::string handleMessage(const std::string &msg);
};

#endif
