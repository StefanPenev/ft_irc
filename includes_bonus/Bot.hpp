/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anilchen <anilchen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 16:15:31 by anilchen          #+#    #+#             */
/*   Updated: 2025/06/13 15:14:37 by anilchen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BOT_HPP
# define BOT_HPP

# include "User_bonus.hpp"
# include <arpa/inet.h>
# include <fstream>
# include <iostream>
# include <netdb.h>
# include <netinet/in.h>
# include <sstream>
# include <string>
# include <sys/socket.h>
# include <unistd.h>

# define IRC_SERVER "127.0.0.1"
# define WEATHER_API_HOST "api.openweathermap.org"
# define BUFFER_SIZE 4096

class Bot : public User
{
  public:
	Bot(int fd, const std::string &name);
	~Bot();
	std::string handleMessage(const std::string &msg);
	int connectToHost(const char *host, int port);

  private:
	std::string httpGet(const std::string &host, const std::string &path);
	std::string loadApiKey(const std::string &filename);
	std::string extractField(const std::string &json, const std::string &key);
	std::string parseWeatherSummary(const std::string &json);
};

#endif
