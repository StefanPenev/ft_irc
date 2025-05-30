/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PollManager.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anilchen <anilchen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 13:00:19 by anilchen          #+#    #+#             */
/*   Updated: 2025/05/30 15:07:18 by anilchen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POLLMANAGER_HPP
# define POLLMANAGER_HPP

# include <vector>

// struct pollfd
// {
// 	int fd;
// 	short events;
// 	short revents;
// };

class PollManager
{
  private:
	std::vector<struct pollfd> _pollFds;

  public:
	PollManager();
	~PollManager();

	void addFd(int fd);
	void removeFd(int fd);
	int wait();
	std::vector<struct pollfd> &getFds();
};

#endif
