/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 09:31:17 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/21 17:31:15 by vvaas            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include <server.hpp>
#include <client.hpp>
#include <channel.hpp>
#include <logs.hpp>
#include <cstring>

/** Commands to handle
 * NICK
 * USER
 * QUIT
 * PART
 * JOIN
 * PRIVMSG
 * NOTICE
 * KICK
 * MOTD
 * TOPIC
 * PING
 * MODE
 */

namespace irc
{
	Server::Server(int port, const std::string& password) : _s_len(sizeof(_s_data)), _password(password), _port(port), _active(true)
	{
		std::memset(&_s_data, 0, sizeof(sockaddr));
		init_socket();
		wait();
	}
	void Server::init_socket(void)
	{
		_s_data.sin_family = AF_INET;
		_s_data.sin_addr.s_addr = INADDR_ANY;
		_s_data.sin_port = htons(_port);
		_main_socket = socket(AF_INET, SOCK_STREAM, 0); // AF_INET == IPv4, SOCK_STREAM == TCP
		if (_main_socket < 0)
			irc::logs::report(irc::log_fatal_error, "socket error");
		std::cout << "Socket creating succesful" << std::endl;
		if (bind(_main_socket, (struct sockaddr *)&_s_data, sizeof(_s_data)) != 0)
			irc::logs::report(irc::log_fatal_error, "bind error");
		std::cout << "bind succesful, starting listen loop" << std::endl;
		if (listen(_main_socket, MAX_USERS) != 0)
			irc::logs::report(irc::log_fatal_error, "listen error");
		std::cout << "listen queue created succesful" << std::endl;
	}

	void Server::wait(void)
	{
		while (_active)
		{
			FD_ZERO(&_fd_set);
			FD_SET(_main_socket, &_fd_set);
		}
	}
	Server::~Server()
	{
		if (_main_socket > 0)
			close(_main_socket);
	}
	
}
