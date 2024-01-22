/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 09:31:17 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/22 01:42:10 by vvaas            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include <server.hpp>
#include <client.hpp>
#include <channel.hpp>
#include <logs.hpp>
#include <cstring>
#include <errno.h>
#include <stack>
#include <fcntl.h>
#include <ansi.hpp>

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

	void Server::init_socket_data(void)
	{
		_s_data.sin_family = AF_INET;
		_s_data.sin_addr.s_addr = INADDR_ANY;
		_s_data.sin_port = htons(_port);
		_main_socket = socket(AF_INET, SOCK_STREAM, 0); // AF_INET == IPv4, SOCK_STREAM == TCP
		if (_main_socket < 0)
			logs::report(log_fatal_error, "socket error");
		logs::report(log_message, "socket succesfully started");
	}

	void Server::init_socket(void)
	{
		int opt = 0;
	
		init_socket_data();
		if (setsockopt(_main_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) // SOL_SOCKET : modify socket only, SO_REUSEADDR : Reusable after program ends
			logs::report(log_fatal_error, "setsockopt() error (tout a pete)");
		if (bind(_main_socket, (struct sockaddr *)&_s_data, sizeof(_s_data)) != 0) // Bind _main_socket to localhost
			logs::report(log_fatal_error, "bind error");
		logs::report(log_message, "bind successful, starting listen loop");
		if (listen(_main_socket, MAX_USERS) != 0) // init the listen with MAX_USERS
			logs::report(log_fatal_error, "listen error");
		logs::report(log_message, "listen queue created successfully");
		logs::report(log_message, "Server is running");
	}

	void Server::handle_input(void)
	{
		char buffer[1024] = {0};

		for (std::vector<Client>::iterator it = _client.begin(); it != _client.end(); it++)
		{
			if (!FD_ISSET(it->get_fd(), &_fd_set))
				continue ;
			while (recv(it->get_fd(), buffer, 1024, 0) > 0) // read() but for socket fd
			{
				std::cout << AnsiColor::cyan << "[User " << it->get_string_id() << "] : " << AnsiColor::reset << buffer << std::flush;
				memset(buffer, 0, sizeof(buffer)); // clear the buffer to avoid trash remaining
			}
			if (recv(it->get_fd(), buffer, 1024, 0) == 0) // recv return 0 if an user disconnect
			{
				std::cout << AnsiColor::cyan << "[User " << it->get_string_id() << "] : " << AnsiColor::reset << "Disconnected" << std::endl;
				it = _client.erase(it) - 1; // magic bitch
			}
		}
	}

	void Server::wait(void)
	{
		int tmp;
		int fd = 0;
		int i = 0;
		socklen_t len = sizeof(sockaddr_in);

		while (_active)
		{
			FD_ZERO(&_fd_set);
			FD_SET(_main_socket, &_fd_set);
			for (std::vector<Client>::iterator it = _client.begin(); it != _client.end(); it++)
				FD_SET(it->get_fd(), &_fd_set);
			tmp = select(MAX_USERS, &_fd_set, NULL, NULL, NULL); // SELECT blocks till a connection or message is received, and let only those in _fd_set
			if (tmp < 0)
				logs::report(log_fatal_error, "select fd error");
			if (FD_ISSET(_main_socket, &_fd_set)) // if it's a new connection
			{
				sockaddr_in cli_sock;
				fd = accept(_main_socket, (sockaddr *)&cli_sock, &len); // adds the new connection
				if (fd < 0)
					logs::report(log_fatal_error, "accept() error");
				if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
					logs::report(log_fatal_error, "fcntl() error");
				_client.push_back(Client(fd, cli_sock, i++)); // put the client into the vector used in handle_input
				std::cout << AnsiColor::cyan << "[User " << _client.back().get_string_id() << "] : " << AnsiColor::reset << "Connected" << std::endl;
			}
			handle_input();
		}
	}

	Server::~Server()
	{
		if (_main_socket > 0)
			close(_main_socket);
	}
	
}
