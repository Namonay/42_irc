/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 10:36:21 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/30 17:39:44 by vvaas            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include <channel.hpp>
#include <logs.hpp>
#include <cerrno>
#include <errorscode.hpp>
#include <cstdlib>
#include <unstd/string.hpp>

namespace irc
{
	typedef std::set<unstd::SharedPtr<Client> >::iterator client_it;
	typedef std::set<unstd::SharedPtr<Client> >::const_iterator client_const_it;

	Channel::Channel(const std::string& name) : _name(name), _channel_size(-1), _invite_only(false), _topic_op_restrict(false) {}

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
		sendWho(client);
	}

	void Channel::modOperator(unstd::SharedPtr<class Client> client, const std::string& clientname, bool mode)
	{
		client_it it;
		logs::report(log_message, "mode operator, mode = %d", mode);
		if(mode)
		{
			for(it = _clients.begin(); it != _clients.end(); ++it)
			{
				if(const_cast<unstd::SharedPtr<irc::Client>&>(*it)->getNickName() == clientname)
				{
					_operators.insert(const_cast<unstd::SharedPtr<irc::Client>&>(*it));
					break;
				}
			}
			if(it == _clients.end())
				client->sendCode(ERR_USERNOTINCHANNEL, "User not in channel");
			return;
		}

		for(it = _operators.begin(); it != _operators.end(); ++it)
		{
			logs::report(log_message, "nickname %s, clientname : %s", const_cast<unstd::SharedPtr<irc::Client>&>(*it)->getNickName().c_str(), clientname.c_str());
			if(const_cast<unstd::SharedPtr<irc::Client>&>(*it)->getNickName() == clientname)
			{
				logs::report(log_message, "found %s to erase", const_cast<unstd::SharedPtr<irc::Client>&>(*it)->getNickName().c_str());
				_operators.erase(it);
				break;
			}
		}
		if(it == _operators.end())
			client->sendCode(ERR_USERNOTINCHANNEL, "User not in channel");
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
		std::string clientlist(":yipirc " RPL_NAMREPLY " " + client->getNickName() + " @ " + getName() + " :");
		
		for(client_it it = _clients.begin(); it != _clients.end(); ++it)
		{
			if(isOp(const_cast<unstd::SharedPtr<irc::Client>&>(*it)))
				clientlist += '@';
			clientlist += const_cast<unstd::SharedPtr<irc::Client>&>(*it)->getNickName() + ' ';
		}
		logs::report(log_message, "%s", clientlist.c_str());
		client->sendModular("%s\r\n", clientlist.c_str());
		clientlist = ":yipirc " RPL_NAMREPLY " " + client->getNickName() + " @ " + getName() + " :";
		client->sendModular(":yipirc %s %s %s : End of names list\r\n", RPL_ENDOFNAMES, client->getNickName().c_str(), getName().c_str());
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

	void Channel::showModesModify(unstd::SharedPtr<Client> client, const Message& msg) const
	{
		std::vector<std::string> mode = msg.getArgs();
		std::string modes;
		std::string out = "MODE ";
		
		for(std::vector<std::string>::iterator it = mode.begin() + 1; it != mode.end(); ++it)
		{
			modes += *it;
			modes += " ";
		}
		out += getName();
		for(client_it it = _clients.begin(); it != _clients.end(); ++it)
			const_cast<unstd::SharedPtr<irc::Client>&>(*it)->sendMsg(client->getNickName(), out, modes);
	}

	void Channel::showModes(unstd::SharedPtr<Client> client)
	{
		std::string modes = " +";

		if(_invite_only)
			modes += 'i';
		if(_topic_op_restrict)
			modes += 't';
		if(_password.size() > 0)
			modes += 'k';
		if(_channel_size != -1)
			modes += 'l';
		if(modes.size() <= 2)
			return ;
		client->sendCode(RPL_CHANNELMODEIS, modes);
	}

	void Channel::changeMode(unstd::SharedPtr<class Client> client, const Message& msg)
	{
		bool modevalue = (msg.getTokens()[2][0] != '-');
		logs::report(log_message, "tokensize : %d, mode : %c, modevalue %d", msg.getTokens().size(), msg.getTokens()[2][1], modevalue);
		switch(msg.getTokens()[2][1])
		{
			case 'i': _invite_only = modevalue; break;
			case 't': _topic_op_restrict = modevalue; break;
			case 'k':
			{
				if(modevalue && msg.getTokens().size() == 4)
				{
					logs::report(log_message, "%s password set as %s", _name.c_str(), msg.getArgs()[2].c_str());
					_password = msg.getArgs()[2];
				}
				else if(msg.getTokens().size() < 4)
				{
					_password.clear();
					logs::report(log_message, "password removed on %s", _name.c_str());
				}
				break;
			}
			case 'o':
			{
				if(isOp(client))
					modOperator(client, msg.getTokens()[3], modevalue);
				else if(!isOp(client))
					client->sendCode(ERR_CHANOPRIVSNEEDED, "You need to be operator to execute this command");
				break;
			}
			case 'l':
			{
				if(msg.getTokens().size() < 3 && modevalue)
					return;
				if(!modevalue)
				{
					_channel_size = -1;
					break;
				}
				if(modevalue)
				{
					char* end;
					long tmp = std::strtol(msg.getArgs()[2].c_str(), &end, 10);
					if(errno == ERANGE || *end != 0 || tmp < 0)
						logs::report(log_error, "invalid channel size");
					else
						_channel_size = tmp;
				}
				break;
			}

			default : client->sendCode(":yipirc " ERR_UNKNOWNMODE " @", getName().c_str(), "Unknown mode"); return;
		}
		logs::report(log_message, "%s MODES : i:%d t:%d k:%s l:%d", getName().c_str(), _invite_only, _topic_op_restrict, _password.c_str(), _channel_size);
		showModesModify(client, msg);
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
		relayTopic(client, true);
	}

	void Channel::relayTopic(unstd::SharedPtr<Client> client, bool broadcast)
	{
		if(!hasClient(client))
			return;
		if(_topic.empty())
		{
			if(!broadcast)
				return client->sendCode(RPL_NOTOPIC, client->getNickName() + " " + _name, "no topic is set");
			for(client_it it = _clients.begin(); it != _clients.end(); ++ it)
				const_cast<unstd::SharedPtr<irc::Client>&>(*it)->sendCode(RPL_NOTOPIC, client->getNickName() + " " + _name, "no topic is set");
		}
		if(!broadcast)
			return client->sendCode(RPL_TOPIC, client->getNickName() + " " + _name, _topic);
		for(client_it it = _clients.begin(); it != _clients.end(); ++ it)
				const_cast<unstd::SharedPtr<irc::Client>&>(*it)->sendCode(RPL_TOPIC, client->getNickName() + " " + _name, _topic);
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
			op->sendCodeInChannel(ERR_USERNOTINCHANNEL, *this, "they aren't on that channel");
			return false;
		}
		if(!isOp(op))
		{
			op->sendCodeInChannel(ERR_CHANOPRIVSNEEDED, *this, "you're not channel operator");
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
