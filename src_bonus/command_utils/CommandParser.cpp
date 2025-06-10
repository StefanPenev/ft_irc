/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anilchen <anilchen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/23 16:12:02 by stefan            #+#    #+#             */
/*   Updated: 2025/06/10 15:37:47 by anilchen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandParser_bonus.hpp"
#include <cctype>
#include <sstream>

std::string CommandParser::trim(const std::string& str) {
    size_t start = 0;
    while (start < str.size() && std::isspace(str[start]))
        ++start;

    size_t end = str.size();
    while (end > start && std::isspace(str[end - 1]))
        --end;

    return str.substr(start, end - start);
}

bool CommandParser::parse(const std::string& line, std::string& command, std::vector<std::string>& args) {
    command.clear();
    args.clear();

    std::string trimmed = trim(line);
    if (trimmed.empty())
        return false;

    std::istringstream iss(trimmed);
    std::string token;

    if (!(iss >> token))
        return false;

    command = token;

    while (iss >> token) {
        if (token[0] == ':') {
            std::string trailing = token.substr(1);
            std::string rest;
            std::getline(iss, rest);
            if (!rest.empty()) {
                trailing += " " + trim(rest);
            }
            args.push_back(trailing);
            break;
        } else {
            args.push_back(token);
        }
    }
    return true;
}
