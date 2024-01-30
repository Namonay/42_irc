/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/30 01:54:56 by vvaas             #+#    #+#             */
/*   Updated: 2024/01/30 17:07:02 by vvaas            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include <bot.hpp>
#include <logs.hpp>
#include <string>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

bot::bot() {}

bot::~bot() {}

void bot::init()
{
	_connect_commands.push_back("PASS " PASSWORD "\r\n");
	_connect_commands.push_back("NICK greg\r\n");
	_connect_commands.push_back("USER greg_bot 0 * :botrealname\r\n");
	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd == -1)
		irc::logs::report(irc::log_fatal_error, "FD error");
	_serv_addr.sin_family = AF_INET;
    _serv_addr.sin_port = htons(PORT);
	_serv_addr.sin_addr.s_addr = inet_addr(IP);
	if (connect(_fd, (struct sockaddr*)&_serv_addr, sizeof(_serv_addr)) < 0)
        irc::logs::report(irc::log_fatal_error, "connect error");
	if(fcntl(_fd, F_SETFL, O_NONBLOCK) < 0)
		irc::logs::report(irc::log_fatal_error, "fcntl() error");
}

void bot::handle_response(std::string buffer)
{
	if (buffer == ":yipirc 001 greg :Welcome to yipirc :), greg\r\n")
		return ;
}

void bot::connect_to_server()
{
	char buffer[1024];
	for (std::vector<std::string>::iterator it = _connect_commands.begin(); it != _connect_commands.end(); ++it)
	{
		if (send(_fd, (*it).c_str(), (*it).size(), 0) < 0)
			 irc::logs::report(irc::log_fatal_error, "send error");
	} 
	while (true)
	{
		if (recv(_fd, buffer, 1024, 0) > 0)
			handle_response(buffer);
		std::memset(buffer, 0, sizeof(buffer));
	}
}