/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logs.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maldavid <kbz_8.dev@akel-engine.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 09:17:47 by maldavid          #+#    #+#             */
/*   Updated: 2024/02/07 16:53:53 by maldavid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <logs.hpp>
#include <ansi.hpp>
#include <cstdarg>
#include <cstdio>
#include <config.hpp>

namespace irc
{
	namespace logs
	{
		void report(LogType type, std::string message, ...)
		{
			char buffer[LOGS_BUFFER_SIZE];

			va_list al;
			va_start(al, message);
			vsnprintf(buffer, LOGS_BUFFER_SIZE, message.c_str(), al);
			va_end(al);

			switch(type)
			{
				case log_message: std::cout << AnsiColor::blue << "[IRC serv] Message : " << AnsiColor::reset << buffer << std::endl; break;
				case log_warning: std::cout << AnsiColor::magenta << "[IRC serv] Warning : " << AnsiColor::reset << buffer << std::endl; break;
				case log_error: std::cerr << AnsiColor::red << "[IRC serv] Error : " << AnsiColor::reset << buffer << std::endl; break;

				default: break;
			}
		}
	}
}
