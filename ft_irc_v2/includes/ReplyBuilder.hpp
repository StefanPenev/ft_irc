/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ReplyBuilder.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anilchen <anilchen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 13:55:06 by stefan            #+#    #+#             */
/*   Updated: 2025/06/02 14:04:50 by anilchen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REPLY_BUILDER_HPP
#define REPLY_BUILDER_HPP

#include <string>
#include <map>
#include "ReplyCode.hpp"
#include "User.hpp"

class ReplyBuilder {
public:
    static void init(const std::string& serverName);
    static std::string build(int code, const User& user, const std::string& arg = "", const std::string& msg = "");

private:
    static std::string _serverName;
    static std::map<int, std::string> _templates;
};

#endif
