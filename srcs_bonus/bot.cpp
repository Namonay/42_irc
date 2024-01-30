/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/30 01:54:56 by vvaas             #+#    #+#             */
/*   Updated: 2024/01/30 17:26:11 by vvaas            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include <bot.hpp>
#include <logs.hpp>
#include <string>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

bot::bot() : _channel_created(false), _logged(false)
{}

bot::~bot() {}

void bot::init()
{
	_connect_commands.push_back("PASS " PASSWORD "\r\n");
	_connect_commands.push_back("NICK greg\r\n");
	_connect_commands.push_back("USER greg_bot 0 * :botrealname\r\n");
	_connect_commands.push_back("JOIN #greg\r\n");
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
	std::cout << buffer << std::flush;
	if (!_logged && buffer == ":yipirc 001 greg :Welcome to yipirc :), greg\r\n")
	{
		_logged = true;
		irc::logs::report(irc::log_message, "Logged in succesfully");
	}
	else if (!_logged)
		return ;
	if (!_channel_created && buffer == ":yipirc 353 greg @ #greg :@greg\r\n")
	{
		_channel_created = true;
		irc::logs::report(irc::log_message, "Created the channel succesfully");
	}
	else if (!_channel_created)
		return ;
}

void bot::connect_to_server()
{
	char buffer[1024];
	for (std::vector<std::string>::iterator it = _connect_commands.begin(); it != _connect_commands.end(); ++it)
	{
		if (send(_fd, (*it).c_str(), (*it).size(), 0) < 0)
			 irc::logs::report(irc::log_fatal_error, "send error");
		if (recv(_fd, buffer, 1024, 0) > 0)
			handle_response(buffer);
	} 
	while (true)
	{
		if (recv(_fd, buffer, 1024, 0) > 0)
			handle_response(buffer);
		std::memset(buffer, 0, sizeof(buffer));
	}
}