/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stefan <stefan@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/23 16:12:05 by stefan            #+#    #+#             */
/*   Updated: 2025/05/25 15:55:08 by stefan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMANDPARSER_HPP
#define COMMANDPARSER_HPP

#include <string>
#include <vector>

class CommandParser {
public:
    static bool parse(const std::string& line, std::string& command, std::vector<std::string>& args);

private:
    static std::string trim(const std::string& str);
};

#endif
