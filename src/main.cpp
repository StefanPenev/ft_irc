/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anilchen <anilchen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 11:54:43 by anilchen          #+#    #+#             */
/*   Updated: 2025/05/30 16:10:47 by anilchen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ReplyBuilder.hpp"
#include "Server.hpp"
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <string>

// hexchat

int	main(int argc, char const **argv)
{
	int	port;

	if (argc != 3)
	{
		std::cerr << "[USAGE]: ./ircserv <port> <password>\n";
		return (1);
	}
	for (int i = 0; argv[1][i] != '\0'; i++)
	{
		if (!std::isdigit(argv[1][i]))
		{
			std::cerr << "[ERROR]: Port must contain only digits\n";
			return (1);
		}
	}
	port = atoi(argv[1]);
	if (port < 1024 || port > 65535)
	{
		std::cerr << "[ERROR]: Invalid port number. Please specify a number between 1024 and 65535.\n";
		return (1);
	}
	ReplyBuilder::init("ircserv");
	Server serv(port, argv[2]);
	serv.run();
	return (0);
}

// Tests for CommandHandler
// #include <iostream>
// #include <vector>
// #include <string>
// #include <map>
// #include "Server.hpp"
// #include "CommandHandler.hpp"
// #include "ReplyBuilder.hpp"
// #include "User.hpp"

// int main() {
//     std::map<int, User*> users;
//     ReplyBuilder::init("ircserv");
//     Server fakeServer(6667, "pass");

//     CommandHandler handler(fakeServer, "pass");

//     int fd1 = 1;
//     User* user1 = new User(fd1);
//     users[fd1] = user1;

//     int fd2 = 2;
//     User* user2 = new User(fd2);
//     users[fd2] = user2;

//     handler.handleCommand(fd2, "PASS pass");
//     handler.handleCommand(fd2, "NICK othernick");
//     handler.handleCommand(fd2, "USER testuser2 0 * :Real Name 2");
//     user2->getSendBuffer().clear(); // Clear any welcome messages etc.

//     std::vector<std::string> testCommands;

//     // Correct tests
//     testCommands.push_back("PASS pass");
//     testCommands.push_back("NICK testnick");
//     testCommands.push_back("USER testuser 0 * :Real Name");
//     testCommands.push_back("JOIN #testchannel");
//     testCommands.push_back("MODE #testchannel +i");
//     testCommands.push_back("MODE #testchannel -i");
//     testCommands.push_back("MODE #testchannel +t");
//     testCommands.push_back("MODE #testchannel +k secret123");
//     testCommands.push_back("MODE #testchannel -k");
//     testCommands.push_back("MODE #testchannel +l 10");
//     testCommands.push_back("MODE #testchannel -l");
//     testCommands.push_back("PRIVMSG #testchannel :Hello, world!");
//     testCommands.push_back("NOTICE #testchannel :Notice message");
//     testCommands.push_back("PART #testchannel");
//     testCommands.push_back("INVITE othernick #testchannel");
//     testCommands.push_back("KICK #testchannel othernick :bye!");
//     testCommands.push_back("TOPIC #testchannel :Welcome to the test channel");
//     testCommands.push_back("TOPIC #testchannel");

//     // Invalid commands
//     testCommands.push_back("");
//     testCommands.push_back("PASS wrongpass");
//     testCommands.push_back("NICK");
//     testCommands.push_back("USER");
//     testCommands.push_back("JOIN");
//     testCommands.push_back("MODE #testchannel +z");
//     testCommands.push_back("MODE");
//     testCommands.push_back("PRIVMSG");
//     testCommands.push_back("KICK #testchannel");
//     testCommands.push_back("INVITE");
//     testCommands.push_back("UNKNOWNCMD param1 param2");
//     testCommands.push_back("MODE #testchannel +k");
//     testCommands.push_back("MODE #testchannel +l");
//     testCommands.push_back("TOPIC");
//     testCommands.push_back("TOPIC #nonexistent");
//     testCommands.push_back("TOPIC #testchannel :");
//     testCommands.push_back("TOPIC #testchannel :New topic without privileges");

//     for (size_t i = 0; i < testCommands.size(); ++i) {
//         std::cout << "\n> Sending: " << (testCommands[i].empty() ? "<empty>" : testCommands[i]) << std::endl;

//         handler.handleCommand(fd1, testCommands[i]);
//         std::string& sendBuffer = user1->getSendBuffer();
//         if (sendBuffer.empty()) {
//             std::cout << "(No response from server)" << std::endl;
//         } else {
//             std::cout << "Server response:\n" << sendBuffer << std::endl;
//             sendBuffer.clear();
//         }
//     }

//     delete user1;
//     delete user2;

//     return (0);
// }