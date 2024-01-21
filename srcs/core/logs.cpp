/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logs.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maldavid <kbz_8.dev@akel-engine.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 09:17:47 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/21 09:30:13 by maldavid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <logs.hpp>
#include <ansi.hpp>
#include <cstdarg>
#include <cstdlib>
#include <cstdio>

#define BUFFER_SIZE 4096

namespace irc
{
	namespace logs
	{
		void report(LogType type, std::string message, ...)
		{
			char buffer[BUFFER_SIZE];

			va_list al;
			va_start(al, message);
			vsnprintf(buffer, BUFFER_SIZE, message.c_str(), al);
			va_end(al);

			switch(type)
			{
				case log_message: std::cout << AnsiColor::blue << "[IRC serv] Message : " << AnsiColor::reset << buffer << std::endl; break;
				case log_warning: std::cout << AnsiColor::magenta << "[IRC serv] Warning : " << AnsiColor::reset << buffer << std::endl; break;
				case log_error: std::cerr << AnsiColor::red << "[IRC serv] Error : " << AnsiColor::reset << buffer << std::endl; break;
				case log_fatal_error:
					std::cerr << AnsiColor::red << "[IRC serv] Fatal Error : " << AnsiColor::reset << buffer << std::endl;
					std::exit(EXIT_FAILURE);
				break;
			}
		}
	}
}
