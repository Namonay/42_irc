/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 10:36:21 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/25 02:33:59 by vvaas            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include <channel.hpp>
#include <logs.hpp>
#include <errorscode.hpp>
#include <iostream>
#include <cstdlib>
namespace irc
{
	typedef std::set<unstd::SharedPtr<Client> >::iterator client_it;

	Channel::Channel(const std::string& name) : _name(name), _channel_size(-1) {}

	void Channel::addClient(unstd::SharedPtr<Client> client, bool op)
	{
		if(!_clients.insert(client).second)
		{
			client->sendCode(ERR_USERONCHANNEL, "You are already in the channel");
			return;
		}
		if(op)
			_operators.insert(client);
		for(client_it it = _clients.begin(); it != _clients.end(); ++it)
			const_cast<unstd::SharedPtr<irc::Client>&>(*it)->sendMsg(client->getNickName(), "JOIN", _name);
	}
	bool Channel::removeClient(unstd::SharedPtr<Client> client)
	{
		if (!_clients.erase(client))
			return (false);
		for (client_it it = _clients.begin(); it != _clients.end(); ++it)
			const_cast<unstd::SharedPtr<irc::Client>&>(*it)->sendMsg(client->getNickName(), "PART", _name);
		client->sendMsg(client->getNickName(), "PART", _name);
		return (true);
	}

	void Channel::handleMessage(const std::string& msg, unstd::SharedPtr<Client> client, bool notice) const
	{
		const std::string sender = client ? client->getNickName() : "";
		for(client_it it = _clients.begin(); it != _clients.end(); ++it)
		{
			if(client == *it)
				continue;
			const_cast<unstd::SharedPtr<irc::Client>&>(*it)->sendMsg(sender, (notice ? "NOTICE " : "PRIVMSG ") + _name, msg);
		}
	}

	void Channel::changeMode(unstd::SharedPtr<class Client> client, const Message& msg)
	{
		bool modevalue = (msg.getTokens()[2][0] != '-');
		// attention on est sur un truc solidement MERDIQUE, a toucher a tes propres risques (je suis proche du nervous breakdown) gl hf :)
		switch (msg.getTokens()[2][1])
		{
			case 'i':
				_invite_only = modevalue;
				break;
			case 't':
				_topic_op_restrict = modevalue;
				break;
			case 'k':
			logs::report(log_message, "%s password set as %s", getName().c_str(), msg.getTokens()[3].c_str());
				_password = msg.getTokens()[3];
				break;
			case 'o':
				if (isOp(client))
					modevalue = !modevalue; // todo sa me clc :(
				else
					client->sendCode(ERR_CHANOPRIVSNEEDED, "You need to be operator to execute this command");
				break;
			case 'l':
				if (static_cast<int>(getNumberOfClients()) > std::atoi(msg.getTokens()[3].c_str()))
					return ;
				if (modevalue)
					_channel_size = std::atoi(msg.getTokens()[3].c_str());
				if (!modevalue)
					_channel_size = -1;
		}
	}

	void Channel::setTopic(unstd::SharedPtr<Client> client, const std::string& new_topic)
	{
		if(_topic_op_restrict && !isOp(client))
		{
			client->sendCode(ERR_CHANOPRIVSNEEDED, "You need operator privileges");
			return ;
		}
		_topic = new_topic;
	}

	bool Channel::isOp(unstd::SharedPtr<Client> client)
	{
		for (client_it it = _clients.begin(); it != _clients.end(); ++it)
		{
			if (const_cast<unstd::SharedPtr<irc::Client>&>(*it)->getNickName() == client->getNickName())
				return (true);
		}
		return (false);
	}
	Channel::~Channel() {}
}
