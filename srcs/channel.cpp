/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 10:36:21 by maldavid          #+#    #+#             */
/*   Updated: 2024/02/05 16:26:14 by maldavid         ###   ########.fr       */
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
		if(mode)
		{
			for(it = _clients.begin(); it != _clients.end(); ++it)
			{
				
				if(const_cast<unstd::SharedPtr<irc::Client>&>(*it)->getNickName() == clientname)
				{
					if (isOp(*it))
						return ;
					showModesModify(client, mode, 'o', clientname);
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
			if(const_cast<unstd::SharedPtr<irc::Client>&>(*it)->getNickName() == clientname)
			{
				showModesModify(client, mode, 'o', clientname);
				_operators.erase(it);
				break;
			}
		}
		if(it == _operators.end())
			client->sendCode(ERR_USERNOTINCHANNEL, "User not in channel");
	}

	bool Channel::removeClient(unstd::SharedPtr<Client> client, const std::string& reason, bool quit)
	{
		if(!_clients.erase(client))
			return (false);
		if(isOp(client))
			_operators.erase(client);
		for(client_it it = _clients.begin(); it != _clients.end(); ++it)
		{
			if(!quit)
				const_cast<unstd::SharedPtr<irc::Client>&>(*it)->sendMsg(client->getNickName(), "PART", _name + ' ' + reason);
			else
				const_cast<unstd::SharedPtr<irc::Client>&>(*it)->sendMsg(client->getNickName(), "QUIT", reason);
		}
		if(!quit)
			client->sendMsg(client->getNickName(), "PART", _name + ' ' + reason);
		return true;
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

	void Channel::showModesModify(unstd::SharedPtr<Client> client, bool modevalue, char flag, std::string op) const
	{
		std::string modes;
		std::string out = "MODE ";
		

		modes += (modevalue) ? '+' : '-';
		modes += flag;
		if (flag == 'k')
			modes += " " + _password;
		if (flag == 'l')
			modes += " " + unstd::toString(_channel_size);;
		if (flag == 'o')
			modes += " " + op;
		out += getName();
		for(client_it it = _clients.begin(); it != _clients.end(); ++it)
			const_cast<unstd::SharedPtr<irc::Client>&>(*it)->sendMsg(client->getNickName(), out, modes);
	}

	void Channel::showModes(unstd::SharedPtr<Client> client)
	{
		std::string modes = _name + " +";

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
		if (modes.size() == _name.size() + 2)
			return ;
		client->sendCode(RPL_CHANNELMODEIS, modes);
	}

	void Channel::changeMode(unstd::SharedPtr<class Client> client, const Message& msg)
	{
		bool modevalue = (msg.getTokens()[2][0] != '-');
		std::string flags = msg.getTokens()[2];
		unsigned long arg_nb = 3;
		int arg_index = 2;

		if (flags.find_first_not_of("itkol+-") != std::string::npos || flags.find_last_of("+-") != 0)
		{
			client->sendCode(ERR_UNKNOWNMODE, "MODE : Unknown mode");
			return ;
		}
		for (std::string::iterator it = flags.begin() + 1; it != flags.end(); ++it)
		{
			if (std::distance(flags.begin(), it) != static_cast<long>(flags.find_first_of(*it)))
				it = flags.erase(it) - 1;
		}
		if (flags.find('o') != std::string::npos)
			arg_nb++;
		if (flags.find('k') != std::string::npos && modevalue)
			arg_nb++;
		if (flags.find('l') != std::string::npos && modevalue)
			arg_nb++;
		if (msg.getTokens().size() != arg_nb)
		{
			client->sendCode(ERR_NEEDMOREPARAMS, "MODE : Need more params");
			return ;
		}

		for (std::string::iterator it = flags.begin() + 1; it != flags.end(); ++it)
		{
			switch(*it)
			{
				case 'i':
					if (_invite_only == modevalue)
						break;
					_invite_only = modevalue;
					showModesModify(client, modevalue, 'i');
					break ;
				case 't':
					if (_topic_op_restrict == modevalue)
						break;
					_topic_op_restrict = modevalue;
					showModesModify(client, modevalue, 't');
					break ;
				case 'k':
					if(modevalue)
					{
						if (msg.getArgs()[arg_index] == _password)
						{
							arg_index++;
							break;
						}
						logs::report(log_message, "%s password set as %s", _name.c_str(), msg.getArgs()[arg_index].c_str());
						_password = msg.getArgs()[arg_index++];
						showModesModify(client, modevalue, 'k');
					}
					else
					{
						if (_password.empty())
							return ;
						_password.clear();
						logs::report(log_message, "password removed on %s", _name.c_str());
						showModesModify(client, modevalue, 'k');
					}
					break ;
				case 'o':
					modOperator(client, msg.getArgs()[arg_index++], modevalue);
					break ;
				case 'l':
					if (!modevalue && _channel_size == -1)
						return ;
					if(!modevalue)
					{
						_channel_size = -1;
						showModesModify(client, modevalue, 'l');
						break;
					}
					if(modevalue)
					{
						char* end;
						long tmp = std::strtol(msg.getArgs()[arg_index++].c_str(), &end, 10);
						if(errno == ERANGE || *end != 0 || tmp < 0)
						{
							client->sendCode(ERR_UNKNOWNMODE, "MODE : Invalid channel size");
							logs::report(log_error, "invalid channel size");
						}
						if (tmp == _channel_size)
							return ;
						else
						{
							if (tmp == _channel_size)
								break;
							_channel_size = tmp;
							showModesModify(client, modevalue, 'l');
						}
					}
					break;
			}
		}
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
		if(isOp(target))
			_operators.erase(target);
		_clients.erase(target);
		return true;
	}

	Channel::~Channel() {}
}
