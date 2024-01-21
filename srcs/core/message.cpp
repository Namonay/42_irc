/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maldavid <kbz_8.dev@akel-engine.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 11:38:34 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/21 11:40:56 by maldavid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <message.hpp>
#include <client.hpp>

namespace irc
{
	Message::Message(unstd::SharedPtr<class Client> client, const std::string& msg) : _raw_msg(msg), _client(client)
	{

	}
	
	Message::~Message() {}
}
