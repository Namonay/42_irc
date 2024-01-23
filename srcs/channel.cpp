/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 10:36:21 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/24 00:42:13 by vvaas            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include <channel.hpp>
#include <logs.hpp>
#include <iostream>
#include <errorscode.hpp>

namespace irc
{
	Channel::Channel(const std::string& name) : _name(name) {}

	void Channel::addClient(unstd::SharedPtr<Client> client)
	{
		if (!_clients.insert(client).second)
		{
			client->sendCode(ERR_USERONCHANNEL, "You are already in the channel");
			return ;
		}
		for(std::set<unstd::SharedPtr<Client> >::iterator it = _clients.begin(); it != _clients.end(); ++it)
			const_cast<unstd::SharedPtr<irc::Client>&>(*it)->sendMsg(client->getNickName(), "JOIN", _name);
	}
	void Channel::addClient(unstd::SharedPtr<Client> client, bool op)
	{
		if (!_clients.insert(client).second)
		{
			client->sendCode(ERR_USERONCHANNEL, "You are already in the channel");
			return ;
		}
		if (op)
			_operators.insert(client);
		for(std::set<unstd::SharedPtr<Client> >::iterator it = _clients.begin(); it != _clients.end(); ++it)
			const_cast<unstd::SharedPtr<irc::Client>&>(*it)->sendMsg(client->getNickName(), "JOIN", _name);
	}

	bool Channel::removeClient(unstd::SharedPtr<Client> client)
	{
		return _clients.erase(client);
	}

	void Channel::handleMessage(const std::string& msg, unstd::SharedPtr<Client> client) const
	{
		const std::string sender = client ? client->getNickName() : "";
		for(std::set<unstd::SharedPtr<Client> >::iterator it = _clients.begin(); it != _clients.end(); ++it)
		{
			if(client == *it)
				continue;
			const_cast<unstd::SharedPtr<irc::Client>&>(*it)->sendMsg(sender, "PRIVMSG " + _name, msg);
		}
	}

	void Channel::setTopic(unstd::SharedPtr<Client> client, const std::string& new_topic)
	{
		if(_topic_op_restrict && _operators.find(client) == _operators.end() )
		{
			client->sendCode(ERR_CHANOPRIVSNEEDED, "You need operator privileges");
			return ;
		}
		_topic = new_topic;
	}

	Channel::~Channel() {}
}
