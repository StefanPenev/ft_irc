/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anilchen <anilchen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 16:15:31 by anilchen          #+#    #+#             */
/*   Updated: 2025/06/10 16:47:36 by anilchen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BOT_HPP
# define BOT_HPP

# include "User_bonus.hpp"

class Bot : public User
{
  public:
	Bot(int fd, const std::string &name);
	~Bot();
	void handleMessage(const std::string &msg);
};

#endif
