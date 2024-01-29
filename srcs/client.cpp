/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 10:35:52 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/30 00:29:52 by maldavid         ###   ########.fr       */
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
#include <vector>
#include <channel.hpp>

namespace irc
{
	Client::Client(int fd, sockaddr_in sock, int id) : _s_data(sock), _fd(fd), _id(id), _logged(false) {}

	void Client::sendCode(const std::string& code, const std::string& msg)
	{
		const std::string command = ":yipirc " + code + " " + getNickName() + " :"  + msg + "\r\n";
#ifdef DEBUG
		logs::report(log_message, "sending '%s'", command.c_str());
#endif
		if(send(_fd, command.c_str(), command.size(), 0) != static_cast<ssize_t>(command.length()))
			logs::report(log_error, "server failed to send a code to '%s' (:sadge:)", _username.c_str());
	}

	void Client::sendCode(const std::string& code, const std::string& msg0, const std::string& msg1)
	{
		const std::string command = ":yipirc " + code + " " + msg0 + " :"  + msg1 + "\r\n";
#ifdef DEBUG
		logs::report(log_message, "sending '%s'", command.c_str());
#endif
		if(send(_fd, command.c_str(), command.size(), 0) != static_cast<ssize_t>(command.length()))
			logs::report(log_error, "server failed to send a code to '%s' (:sadge:)", _username.c_str());
	}

	void Client::sendCodeInChannel(const std::string& code, const Channel &chan, const std::string& msg)
	{
		const std::string command = ":yipirc " + code + " " + getNickName() + " " + chan.getName() + " :"  + msg + "\r\n";
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
		std::string buffer;

		va_list al;
		va_list al_copy;

		va_start(al, message);
		__builtin_va_copy(al_copy, al);

		int len = vsnprintf(NULL, 0, message.c_str(), al);

		if(len > 0)
		{
			std::vector<char> tmp(len + 1);
			vsnprintf(&tmp[0], tmp.size(), message.c_str(), al_copy);
			buffer = std::string(&tmp[0], len);
			buffer += "\r";
		}
		va_end(al_copy);
		va_end(al);

#ifdef DEBUG
		logs::report(log_message,"sending '%s'", buffer.c_str());
#endif	
		if (send(_fd, buffer.c_str(), buffer.length(), 0) < static_cast<ssize_t>(buffer.length()))
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
		if(finder != std::string::npos)
		{
			std::string msg = _msg_in_flight.substr(0, finder);
			_msg_in_flight = _msg_in_flight.substr(finder + 2);
			return msg;
		}
		finder = _msg_in_flight.find("\n");
		if(finder != std::string::npos)
		{
			std::string msg = _msg_in_flight.substr(0, finder);
			_msg_in_flight = _msg_in_flight.substr(finder + 1);
			return msg;
		}
		logs::report(log_warning, "Incomplete data type");
		_msg_in_flight.clear();
		return (std::string());
	}

	void Client::kill(const std::string& reason)
	{
		sendMsg("yipirc", "KILL", reason);
	}

	Client::~Client() {}
}
