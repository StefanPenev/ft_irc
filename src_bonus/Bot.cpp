/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anilchen <anilchen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 16:16:26 by anilchen          #+#    #+#             */
/*   Updated: 2025/06/13 19:42:07 by anilchen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Bot.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <sstream>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdlib>

// Constructor: initializes Bot with given fd and nickname,
//	calls User constructor
Bot::Bot(int fd, const std::string &name) : User(fd, name)
{
}

// Destructor
Bot::~Bot()
{
}

// Connects to a given host and port, returns a connected socket fd or
// -1 on failure
int Bot::connectToHost(const char *host, int port)
{
	struct hostent		*server;
	int					sockfd;
	struct sockaddr_in	serv_addr;

	server = gethostbyname(host); // DNS resolution
	if (!server)
		return (-1);
	sockfd = socket(AF_INET, SOCK_STREAM, 0); // Create TCP socket
	if (sockfd < 0)
		return (-1);
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port); // Convert port to network byte order
	memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		close(sockfd);
		return (-1); // Failed to connect
	}
	return (sockfd); // Success, return socket
}

// Loads API key from a config file formatted as "API_KEY=..."
std::string Bot::loadApiKey(const std::string &filename)
{
	std::ifstream file(filename.c_str());
	std::string line;
	while (std::getline(file, line))
	{
		if (line.find("API_KEY=") == 0)
			return (line.substr(8)); // Extract value after "API_KEY="
	}
	return (""); // Not found or failed to open file
}

// Sends HTTP GET request to host/path and returns full raw response
std::string Bot::httpGet(const std::string &host, const std::string &path)
{
	int		sockfd;
	char	buffer[BUFFER_SIZE];
	int		n;

	sockfd = connectToHost(host.c_str(), 80); // Connect to HTTP port
	if (sockfd < 0)
		return ("");
	// Build HTTP request
	std::ostringstream request;
	request << "GET " << path << " HTTP/1.1\r\n";
	request << "Host: " << host << "\r\n";
	request << "Connection: close\r\n\r\n";
	std::string reqStr = request.str();
	send(sockfd, reqStr.c_str(), reqStr.length(), 0); // Send request
	// Read response
	std::string response;
	while ((n = recv(sockfd, buffer, BUFFER_SIZE - 1, 0)) > 0)
	{
		buffer[n] = '\0';
		response += buffer;
	}
	close(sockfd); // Close socket after reading
	return (response);
}

// Extracts value from JSON-like string by finding key and parsing value manually
std::string Bot::extractField(const std::string &json, const std::string &key)
{
	size_t	pos;

	pos = json.find(key);
	if (pos == std::string::npos)
		return ("");
	pos += key.length();
	while (pos < json.length() && (json[pos] == ' ' || json[pos] == ':'
			|| json[pos] == '"'))
		++pos;
	std::string value;
	while (pos < json.length() && (isalnum(json[pos]) || json[pos] == '.'
			|| json[pos] == '-' || json[pos] == ' '))
	{
		if (json[pos] == '"')
			break ;
		value += json[pos++];
	}
	return (value);
}

// Combines extracted weather fields into human-readable summary string
std::string Bot::parseWeatherSummary(const std::string &json)
{
	std::string description = extractField(json, "description");
	std::string temp = extractField(json, "\"temp\":");
	std::string feels = extractField(json, "\"feels_like\":");
	std::string humidity = extractField(json, "\"humidity\":");
	std::string wind = extractField(json, "\"speed\":");
	std::ostringstream result;
	result << "Weather: " << description;
	if (!temp.empty())
		result << ", Temp: " << temp << "\u00B0C";
	if (!feels.empty())
		result << ", Feels Like: " << feels << "\u00B0C";
	if (!humidity.empty())
		result << ", Humidity: " << humidity << "%";
	if (!wind.empty())
		result << ", Wind: " << wind << " m/s";
	return (result.str());
}

// Handles incoming user message and returns appropriate bot response
std::string Bot::handleMessage(const std::string &msg)
{
	std::cout << "[BOT] Received message: " << msg << "\n";
	// Convert input to lowercase for case-insensitive matching
	std::string lowercase = msg;
	for (size_t i = 0; i < lowercase.size(); i++)
		lowercase[i] = tolower(lowercase[i]);
	// Built-in text commands
	if (lowercase.find("hello") != std::string::npos
		|| lowercase.find("hi") != std::string::npos)
		return ("Hi! I'm a ft_irc-bot!");
	if (lowercase.find("help") != std::string::npos)
		return ("Available commands: hello, help, about, !weather <city>, roll");
	if (lowercase.find("about") != std::string::npos)
		return ("I'm a bot created for the ft_irc project at 42 by Stefan Penev and Ana Ilchenko.");
	if (lowercase.find("roll") != std::string::npos)
	{
		int roll = (std::rand() % 6) + 1;
		std::ostringstream oss;
		oss << "You rolled: " << roll;
		std::string res = oss.str();
		return (res);
	}
	// Weather command with city parameter
	if (lowercase.find("!weather ") == 0)
	{
		std::string city = lowercase.substr(9);
		std::string apiKey = loadApiKey("./src_bonus/config.txt");
		if (apiKey.empty())
			return ("API key not found.");
		std::string path = "/data/2.5/weather?q=" + city + "&appid=" + apiKey
			+ "&units=metric";
		std::string response = httpGet(WEATHER_API_HOST, path);
		if (response.empty())
			return ("Failed to fetch weather data.");
		return ("Weather in " + city + ": " + parseWeatherSummary(response));
	}
	// Fallback for unknown input
	return ("Sorry, I don't understand.");
}
