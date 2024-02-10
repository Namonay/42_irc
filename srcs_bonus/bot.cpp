/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/30 01:54:56 by vvaas             #+#    #+#             */
/*   Updated: 2024/02/10 18:03:17 by vvaas            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include <Bot.hpp>
#include <logs.hpp>
#include <string>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cstdlib>
#include <signal.h>
#include <cerrno>
#include <ctime>

bool active = true;

Bot::Bot() :  begin(clock()), _fd(-1), _channel_created(false), _logged(false)
{}

Bot::~Bot()
{
	if(_fd >= 0)
		close(_fd);
}

void signalsHandler(int foo)
{
	(void)foo;
	std::cout << "\b\b  \b\b" << std::flush;
	irc::logs::report(irc::log_message, "Shutting down...");
	active = false;
}

bool Bot::init(const std::string &ip, const std::string &port, const std::string &password)
{
	if(ip.empty() || port.empty() || password.empty())
	{
		irc::logs::report(irc::log_error, "An argument is empty");
		return false;
	}

	char* end;
	int portval = std::strtol(port.c_str(), &end, 10);
	if(errno == ERANGE || *end != 0 || portval < 0 || portval > 0xFFFF)
	{
		irc::logs::report(irc::log_error, "invalid port");
		return false;
	}
	_connect_commands.push_back("PASS " + password + "\r\n");
	_connect_commands.push_back("NICK greg\r\n");
	_connect_commands.push_back("USER greg_Bot 0 * :Botrealname\r\n");
	_connect_commands.push_back("JOIN #greg\r\n");
	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(_fd == -1)
	{
		irc::logs::report(irc::log_error, "FD error");
		return false;
	}
	_serv_addr.sin_family = AF_INET;
    _serv_addr.sin_port = htons(portval);
	_serv_addr.sin_addr.s_addr = inet_addr(ip.c_str());
	if(connect(_fd, (struct sockaddr*)&_serv_addr, sizeof(_serv_addr)) < 0)
	{
        irc::logs::report(irc::log_error, "connect error");
		return false;
	}
	if(fcntl(_fd, F_SETFL, O_NONBLOCK) < 0)
	{
        irc::logs::report(irc::log_error, "fcntl error");
		return false;
	}
	signal(SIGINT, signalsHandler);
	signal(SIGQUIT, signalsHandler);
	return true;
}

void Bot::send_message(const std::string &content)
{
	if(send(_fd, content.c_str(), content.length(), 0) < 0)
	{
		irc::logs::report(irc::log_error, "send error");
		active = false;
	}
}

void Bot::handle_response(std::string buffer)
{
	if(!_logged && buffer == ":YipIRC 001 greg :Welcome to YipIRC 😀, your nickname is : greg\r\n")
	{
		_logged = true;
		irc::logs::report(irc::log_message, "Logged in succesfully");
	}
	else if(!_logged)
		return;
	if(!_channel_created && buffer == ":greg JOIN :#greg\r\n")
	{
		_channel_created = true;
		irc::logs::report(irc::log_message, "Created the channel succesfully");
	}
	else if(!_channel_created)
		return;
	if(buffer.find("KICK #greg greg :") != std::string::npos || buffer.find("explose") != std::string::npos || buffer.find(":YipIRC KILL greg :") != std::string::npos)
	{
		irc::logs::report(irc::log_message, "Exiting server");
		send_message("QUIT :Salut mon pote\r\n");
		active = false;
	}
	if(buffer.find("quoi") != std::string::npos)
		send_message("PRIVMSG #greg :feur\r\n");
}

void Bot::connect_to_server()
{
	char buffer[1024];
	for(std::vector<std::string>::iterator it = _connect_commands.begin(); it != _connect_commands.end(); ++it)
	{
		if(send(_fd, (*it).c_str(), (*it).size(), 0) < 0)
		{
			 irc::logs::report(irc::log_error, "send error");
			 return;
		}
		if(recv(_fd, buffer, 1024, 0) > 0)
			handle_response(buffer);
	}
	while(active)
	{
		if (((clock() -  begin) * 1000 / CLOCKS_PER_SEC) > 1000 && !_logged)
		{
			active = false;
			irc::logs::report(irc::log_error, "Couldn't log into server");
		}
		if(recv(_fd, buffer, 1024, 0) > 0)
			handle_response(buffer);
		std::memset(buffer, 0, sizeof(buffer));
	}
}
