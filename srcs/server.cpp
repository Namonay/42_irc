/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maldavid <kbz_8.dev@akel-engine.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 09:31:17 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/21 12:13:51 by maldavid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <server.hpp>
#include <client.hpp>
#include <channel.hpp>

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
	Server::Server(int port, const std::string& password) : _password(password), _port(port)
	{
		(void)_port;
		(void)_password;
		(void)_socket;
		(void)_channels;
	}

	Server::~Server() {}
}
