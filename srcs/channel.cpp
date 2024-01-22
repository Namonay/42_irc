/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maldavid <kbz_8.dev@akel-engine.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 10:36:21 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/22 20:06:36 by maldavid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <channel.hpp>

namespace irc
{
	Channel::Channel(const std::string& name) : _name(name)
	{

	}

	void Channel::setTopic(unstd::SharedPtr<Client> client, const std::string& new_topic)
	{
		if(_topic_op_restrict && _operators.find(client) == _operators.end())
		{
			// send error code to user
		}
		_topic = new_topic;
	}

	Channel::~Channel() {}
}
