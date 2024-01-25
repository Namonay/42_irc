/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 10:35:52 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/25 21:22:34 by maldavid         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include <client.hpp>
#include <ansi.hpp>
#include <iostream>
#include <logs.hpp>
#include <irc.hpp>
#include <config.hpp>
#include <cstdarg>
#include <cstring>
#include <cstdlib>
#include <cstdio>

namespace irc
{
	Client::Client(int fd, sockaddr_in sock, int id) : _s_data(sock), _fd(fd), _id(id), _logged(false) {}

	void Client::sendCode(const std::string& code, const std::string& msg)
	{
		const std::string command = code + " :" + msg + "\r\n";
#ifdef DEBUG
		logs::report(log_message, "sending '%s'", command.c_str());
#endif
		if(send(_fd, command.c_str(), command.size(), 0) != static_cast<ssize_t>(command.length()))
			logs::report(log_error, "server failed to send a code to '%s' (:sadge:)", _username.c_str());
	}

	void Client::sendPlainText(const std::string& str)
	{
#ifdef DEBUG
		logs::report(log_message,"sending '%s'", str.c_str());
#endif
		if(send(_fd, str.c_str(), str.length(), 0) != static_cast<ssize_t>(str.length()))
			logs::report(log_error, "server failed to send a message to '%s' (:sadge:)", _username.c_str());
	}

	void Client::sendMsg(const std::string& sender, const std::string& cmd, const std::string& trailing)
	{
		const std::string out = ':' + sender + ' ' + cmd + " :" + trailing + "\r\n";
#ifdef DEBUG
		logs::report(log_message,"sending '%s'", out.c_str());
#endif
		if(send(_fd, out.c_str(), out.length(), 0) != static_cast<ssize_t>(out.length()))
			logs::report(log_error, "server failed to send a message from '%s' to '%s' (:sadge:)", sender.c_str(), _username.c_str());
	}

	void Client::sendModular(std::string message, ...)
		{
			char buffer[LOGS_BUFFER_SIZE];

			va_list al;
			va_start(al, message);
			vsnprintf(buffer, LOGS_BUFFER_SIZE, message.c_str(), al);
			va_end(al);

			logs::report(log_message,"<- S %s", buffer);
			if (send(_fd, buffer, std::strlen(buffer), 0) < static_cast<ssize_t>(std::strlen(buffer)))
				logs::report(log_error, "server failed to send a message to '%s'", _nickname.c_str());
		}
	void Client::printUserHeader() const
	{
		std::cout << AnsiColor::green << "[User " << _id;
		if(!_realname.empty())
			std::cout << " {realname " << _realname << '}';
		if(!_username.empty())
			std::cout << " {username " << _username << "}";
		if(!_nickname.empty())
			std::cout << " {nickname " << _nickname << "}";
		std::cout << "] : " << AnsiColor::reset;
	}

	std::string Client::getNextMsg()
	{
		std::size_t finder = _msg_in_flight.find("\r\n");
		if(finder == std::string::npos)
			logs::report(log_fatal_error, "client %d [getNextMsg()] : cannot get any other message, panic !", _id);
		std::string msg = _msg_in_flight.substr(0, finder);
		_msg_in_flight = _msg_in_flight.substr(finder + 2);
		return msg;
	}

	Client::~Client() {}
}
