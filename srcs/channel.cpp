/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 10:36:21 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/25 21:21:12 by maldavid         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include <channel.hpp>
#include <logs.hpp>
#include <errorscode.hpp>
#include <cstdlib>
#include <unstd/string.hpp>

namespace irc
{
	typedef std::set<unstd::SharedPtr<Client> >::iterator client_it;
	typedef std::set<unstd::SharedPtr<Client> >::const_iterator client_const_it;

	Channel::Channel(const std::string& name) : _name(name), _channel_size(-1), _topic_op_restrict(false) {}

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

	void Channel::ModOperator(unstd::SharedPtr<class Client> client, const std::string &clientname, bool mode)
	{
		client_it it;
		for (it = _clients.begin(); it != _clients.end(); ++it)
		{
			if (const_cast<unstd::SharedPtr<irc::Client>&>(*it)->getNickName() == clientname)
				_operators.insert(const_cast<unstd::SharedPtr<irc::Client>&>(*it));
		}
		if (it == _clients.end())
			client->sendCode(ERR_USERNOTINCHANNEL, "User not in channel");
		else
		{
			std::string out;
			if (mode)
				out = clientname + ": +o";
			else
				out = clientname + ": -o";
			for (it = _clients.begin(); it != _clients.end(); ++it)
				const_cast<unstd::SharedPtr<irc::Client>&>(*it)->sendMsg(client->getNickName(), "MODE", out);
		}
	}

	bool Channel::removeClient(unstd::SharedPtr<Client> client)
	{
		if (!_clients.erase(client))
			return (false);
		if (isOp(client))
			_operators.erase(client);
		for(client_it it = _clients.begin(); it != _clients.end(); ++it)
			const_cast<unstd::SharedPtr<irc::Client>&>(*it)->sendMsg(client->getNickName(), "PART", _name);
		client->sendMsg(client->getNickName(), "PART", _name);
		return (true);
	}

	void Channel::sendWho(unstd::SharedPtr<Client> client)
	{
		std::string clientlist(RPL_NAMREPLY " " + client->getNickName() + " = " + getName() + ": ");
		
		for (client_it it = _clients.begin(); it != _clients.end(); ++it)
		{
			clientlist += const_cast<unstd::SharedPtr<irc::Client>&>(*it)->getNickName() + ' ';
		}
		client->sendModular("%s", clientlist.c_str());
		client->sendModular("%s %s %s : End of names list", RPL_ENDOFNAMES, client->getNickName().c_str(), getName().c_str());
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

	void Channel::showModes(void) const
	{
		std::string modes = "+";

		if (_invite_only)
			modes += 'i';
		if (_topic_op_restrict)
			modes += 't';
		if (_password.size() > 0)
			modes += 'k';
		if (_channel_size != -1)
		{
			modes += 'l';
			modes += " " + unstd::toString(_channel_size);
		}
		for(client_it it = _clients.begin(); it != _clients.end(); ++it)
			const_cast<unstd::SharedPtr<irc::Client>&>(*it)->sendCode(RPL_CHANNELMODEIS, modes);
	}

	void Channel::changeMode(unstd::SharedPtr<class Client> client, const Message& msg)
	{
		bool modevalue = (msg.getTokens()[2][0] != '-');
		// attention on est sur un truc solidement MERDIQUE, a toucher a tes propres risques (je suis proche du nervous breakdown) gl hf :)
		logs::report(log_message, "tokensize : %d, mode : %c, modevalue %d", msg.getTokens().size(), msg.getTokens()[2][1], modevalue);
		switch (msg.getTokens()[2][1])
		{
			case 'i':
				_invite_only = modevalue;
				break;
			case 't':
				_topic_op_restrict = modevalue;
				break;
			case 'k':
				if (modevalue && msg.getTokens().size() == 4)
				{
					logs::report(log_message, "%s password set as %s", getName().c_str(), msg.getTokens()[3].c_str());
					_password = msg.getTokens()[3];
				}
				else if (msg.getTokens().size() < 4)
				{
					_password = "";
					logs::report(log_message, "password removed on %s", getName().c_str());
				}
				break;
			case 'o':
				if (isOp(client) && modevalue)
					ModOperator(client, msg.getTokens()[3], modevalue);
				else
					client->sendCode(ERR_CHANOPRIVSNEEDED, "You need to be operator to execute this command");
				break;
			case 'l':
				if (msg.getTokens().size() < 3 && modevalue)
					return ;
				if (static_cast<int>(getNumberOfClients()) > std::atoi(msg.getTokens()[3].c_str()))
					return ;
				if (modevalue)
					_channel_size = std::atoi(msg.getTokens()[3].c_str());
				if (!modevalue)
					_channel_size = -1;
		}
		logs::report(log_message, "%s MODES : i:%d t:%d k:%s l:%d", getName().c_str(), _invite_only, _topic_op_restrict, _password.c_str(), _channel_size);
		showModes();
	}

	bool Channel::hasClient(std::string client) const
	{
		for(client_const_it it = _clients.begin(); it != _clients.end(); ++it)
		{
			if(const_cast<unstd::SharedPtr<irc::Client>&>(*it)->getNickName() == client)
				return true;
		}
		return false;
	}

	void Channel::setTopic(unstd::SharedPtr<Client> client, const std::string& new_topic)
	{
		if(_topic_op_restrict && !isOp(client))
		{
			client->sendCode(ERR_CHANOPRIVSNEEDED, "You need operator privileges");
			return;
		}
		_topic = new_topic;
		relayTopic(client);
	}

	void Channel::relayTopic(unstd::SharedPtr<Client> client)
	{
		if(!hasClient(client))
			return;
		if(_topic.empty())
			return client->sendCode(RPL_NOTOPIC " " + _name, " no topic is set");
		return client->sendCode(RPL_TOPIC " " + _name, _topic);
	}

	bool Channel::isOp(unstd::SharedPtr<Client> client) const
	{
		for(client_const_it it = _operators.begin(); it != _operators.end(); ++it)
		{
			if(const_cast<unstd::SharedPtr<irc::Client>&>(*it)->getNickName() == client->getNickName())
				return true;
		}
		return false;
	}

	bool Channel::kick(unstd::SharedPtr<Client> op, unstd::SharedPtr<Client> target, const std::string& reason)
	{
		if(!hasClient(op))
		{
			op->sendCode(ERR_NOTONCHANNEL, _name + " you're not on that channel");
			return false;
		}
		if(!hasClient(target))
		{
			op->sendCode(ERR_USERNOTINCHANNEL, const_cast<std::string&>(target->getNickName()) + ' ' + _name + "  they aren't on that channel");
			return false;
		}
		if(!isOp(op))
		{
			op->sendCode(ERR_CHANOPRIVSNEEDED, const_cast<std::string&>(_name) + " you're not channel operator");
			return false;
		}
		for(client_it it = _clients.begin(); it != _clients.end(); ++it)
			const_cast<unstd::SharedPtr<irc::Client>&>(*it)->sendMsg(op->getNickName(), "KICK " + _name + ' ' + target->getNickName(), reason);
		if (isOp(target))
			_operators.erase(target);
		_clients.erase(target);
		return true;
	}

	Channel::~Channel() {}
}
