/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stefan <stefan@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/14 21:54:53 by stefan            #+#    #+#             */
/*   Updated: 2025/06/14 21:55:22 by stefan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WeatherBot.hpp"
#include <cstdlib>

int main()
{
    WeatherBot bot("127.0.0.1",                 // IRC_SERVER
                   8080,                        // IRC_PORT
                   "pass",                      // IRC_PASSWORD
                   "weatherbot",                // IRC_NICK
                   "weatherbot",                // IRC_USER
                   "#weather",                  // IRC_CHANNEL
                   "api.openweathermap.org");   // WEATHER_API_HOST

    return bot.run();
}