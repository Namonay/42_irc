/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 10:36:21 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/23 17:23:24 by vvaas            ###   ########.fr       */
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
		for (std::set<unstd::SharedPtr<Client> >::iterator it = _clients.begin(); it != _clients.end(); ++it)
		{
			if (const_cast<unstd::SharedPtr<irc::Client>&>(*it)->getNickName() == client->getNickName())
			{
				logs::report(log_message, "%s is already is channel for", client->getNickName().c_str());
				client->sendCode(ERR_USERONCHANNEL, "You are already in the channel");
				return ;
			}
		}
		logs::report(log_message, "i tried to insert %s, success : %d", client->getNickName().c_str(), _clients.insert(client).second);
		for(std::set<unstd::SharedPtr<Client> >::iterator it = _clients.begin(); it != _clients.end(); ++it)
		{
			const_cast<unstd::SharedPtr<irc::Client>&>(*it)->sendMsg(client->getNickName(), "JOIN", _name);
			logs::report(log_message, "%s has been sent a JOIN message", const_cast<unstd::SharedPtr<irc::Client>&>(*it)->getNickName().c_str());
		}
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
		if(_topic_op_restrict && _operators.find(client) == _operators.end())
		{
			// send error code to user
		}
		_topic = new_topic;
	}

	Channel::~Channel() {}
}
