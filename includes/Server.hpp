/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stefan <stefan@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 11:28:46 by anilchen          #+#    #+#             */
/*   Updated: 2025/05/26 08:59:50 by stefan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>

class Server
{
  private:
	int _serverSocket;
	std::string _password;
	int _port;

	int setupSocket();
	struct addrinfo *resolveAddress(const std::string &portStr);
	int createSocket(struct addrinfo *res);
	void enableAddressReuse(int sockfd);
	void bindSocket(int sockfd, struct addrinfo *res);
	void startListening(int sockfd);

  public:
	Server();
	Server(int port, const std::string &password);
	~Server();
	void run();
};

#endif
