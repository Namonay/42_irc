/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maldavid <kbz_8.dev@akel-engine.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 09:31:17 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/21 10:38:12 by maldavid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <server.hpp>
#include <client.hpp>
#include <channel.hpp>

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
