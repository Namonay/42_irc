/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_functions.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 17:31:06 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/29 15:13:34 by maldavid         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include <server.hpp>
#include <client.hpp>
#include <channel.hpp>
#include <logs.hpp>
#include <cstring>
#include <fcntl.h>
#include <ansi.hpp>
#include <config.hpp>
#include <message.hpp>
#include <errorscode.hpp>
#include <irc.hpp>
#include <cstring>
#include <unstd/string.hpp>

namespace irc
{
	typedef std::vector<unstd::SharedPtr<Client> >::iterator client_it;
	typedef std::vector<unstd::SharedPtr<Client> >::const_iterator client_const_it;

	typedef std::vector<Channel>::iterator channel_it;
	typedef std::vector<Channel>::const_iterator channel_const_it;

	void Server::handleNick(unstd::SharedPtr<class Client> client, const Message& msg)
	{
		if(msg.getTokens().size() != 2 && msg.getTokens().size() != 3)
		{
			logs::report(log_error, "NICK, invalid command '%s'", msg.getRawMsg().c_str());
			return;
		}
		const std::string& nickname = msg.getTokens()[1];
		for(client_it it = _client.begin(); it != _client.end(); ++it)
		{
			if((*it)->getNickName() == nickname)
			{
				client->sendCode(ERR_NICKCOLLISION, "Nickname is used");
				return;
			}
		}
		std::string oldNick = (client->getNickName().size() > 0) ? client->getNickName() : msg.getTokens()[1]; // get nickname before /nick execution inside server (oldNick)
		client->printUserHeader();
		client->setNewNickName(msg.getTokens()[1]);
		client->sendMsg(oldNick, "NICK", msg.getTokens()[1]);
		if (client->isLogged())
			client->sendCode(RPL_WELCOME, "Welcome to yipiirc :)\n");
		std::cout << "new nickname, " << client->getNickName() << std::endl;
	}

	void Server::handleUser(unstd::SharedPtr<class Client> client, const Message& msg)
	{
		if(msg.getTokens().size() != 5)
		{
			logs::report(log_error, "USER, invalid command '%s'", msg.getRawMsg().c_str());
			return;
		}
		client->printUserHeader();
		client->setNewUserName(msg.getTokens()[1]);
		std::cout << "new username, " << client->getUserName() << std::endl;

		client->printUserHeader();
		client->setNewRealName(msg.getTokens()[4]);
		std::cout << "new realname, " << client->getRealName() << std::endl;
	}

	void Server::handlePass(unstd::SharedPtr<class Client> client, const Message& msg)
	{
		if(client->isLogged())
			return;
		if(msg.getTokens()[1] == _password)
		{
			client->login();
			if (client->getNickName().size() != 0)
				client->sendCode(RPL_WELCOME, "Welcome to yipiirc :)\n");
		}
		else
		{
			client->sendCode(ERR_PASSWDMISMATCH);
			client->requireDisconnect();
		}
	}

	void Server::handleQuit(unstd::SharedPtr<class Client> client, const Message& msg)
	{
		(void)msg;
		for(channel_it it = _channels.begin(); it != _channels.end(); ++it)
		{
			it->removeClient(client);
		}
		client->printUserHeader();
		std::cout << "quit" << std::endl;
	}

	void Server::handlePart(unstd::SharedPtr<class Client> client, const Message& msg)
	{
		if(msg.getTokens().size() < 2 && msg.getTokens().size() > 3)
		{
			logs::report(log_error, "PART, invalid command '%s'", msg.getRawMsg().c_str());
			return;
		}
		if(msg.getTokens()[1][0] != '#' && msg.getTokens()[1][0] != '&')
		{
			logs::report(log_error, "PART, invalid channel name '%s'", msg.getTokens()[1].c_str());
			return;
		}
		if(!isChannelKnown(msg.getArgs()[0]))
		{
			client->sendCode(ERR_NOSUCHCHANNEL, msg.getArgs()[0] + " no such channel");
			return;
		}
		Channel* channel = getChannelByName(msg.getArgs()[0]);
		if(channel == NULL)
			logs::report(log_fatal_error, "(KICK), cannot get channel '%s' by name; panic !", channel->getName().c_str());
		if(!channel->removeClient(client))
		{
			client->sendCode(ERR_NOTONCHANNEL, "Not on channel");
			return;
		}
		client->printUserHeader();
		std::cout << "leaving channel, " << msg.getArgs()[0] << std::endl;
		if(channel->getNumberOfClients() == 0)
		{
			channel_it it;
			for(it = _channels.begin(); it < _channels.end(); ++it)
			{
				if(it->getName() == msg.getArgs()[0])
					break;
			}
			_channels.erase(it);
			logs::report(log_message, "channel '%s' has been destroyed", channel->getName().c_str());
		}
	}

	void Server::handleJoin(unstd::SharedPtr<class Client> client, const Message& msg)
	{
		if(msg.getArgs().empty())
		{
			client->sendCode(ERR_NEEDMOREPARAMS, "Need more params");
			logs::report(log_error, "JOIN, invalid command '%s'", msg.getRawMsg().c_str());
			return;
		}
		if(msg.getTokens()[1][0] != '#' && msg.getTokens()[1][0] != '&')
		{
			logs::report(log_error, "JOIN, invalid channel name '%s'", msg.getTokens()[1].c_str());
			return;
		}

		channel_it it;
		for(it = _channels.begin(); it != _channels.end(); ++it)
		{
			if(msg.getTokens()[1] == it->getName())
				break;
		}
		if(it == _channels.end())
		{
			_channels.push_back(Channel(msg.getTokens()[1]));
			_channels.back().addClient(client, true);
			logs::report(log_message, "channel '%s' has been created", msg.getTokens()[1].c_str());
			return ;
		}
		if (msg.getTokens().size() == 3 && msg.getTokens()[2] != it->getPassword())
			client->sendCode(ERR_BADCHANNELKEY, "Invalid password");
		else if (it->getChannelSize() != -1 && it->getChannelSize() >= static_cast<int>(it->getNumberOfClients()))
			client->sendCode(ERR_CHANNELISFULL, "Channel is full");
		else if (it->getPassword().size() == 0)
			it->addClient(client);
		else if (it->getPassword().size() > 0 && msg.getTokens()[2] == it->getPassword())
			it->addClient(client);
	
		// client->printUserHeader();
		// std::cout << "joining new channel, " << msg.getTokens()[1] << std::endl;
	}

	void Server::handlePrivMsg(unstd::SharedPtr<class Client> client, const Message& msg)
	{
		if(msg.getArgs().empty())
		{
			client->sendCode(ERR_NORECIPIENT, "No recipient given\n");
			return;
		}
		if(msg.getTokens().size() < 3)
		{
			client->sendCode(ERR_NOTEXTTOSEND, "No text to send\n");
			return;
		}
		if (msg.getTokens()[1][0] != '&' && msg.getTokens()[1][0] != '#')
		{
			for (client_it itc = _client.begin(); itc != _client.end(); ++itc)
			{
				if ((*itc)->getNickName() == msg.getTokens()[1] && (*itc)->getNickName() != client->getNickName())
				{
					std::string complete_msg;
					if(msg.getTokens().size() > 2)
					{
						for(std::vector<std::string>::const_iterator tit = msg.getTokens().begin() + 2; tit < msg.getTokens().end(); ++tit)
						{
							complete_msg.append(*tit);
							complete_msg.append(" ");
						}
						complete_msg.erase(complete_msg.begin());
					}
					(*itc)->sendMsg(client->getNickName(), "PRIVMSG " + (*itc)->getNickName(), complete_msg);
					break;
				}
			}
			return;
		}
		for(channel_it it = _channels.begin(); it != _channels.end(); ++it)
		{
			if(msg.getTokens()[1] == it->getName())
			{
				std::string complete_msg;
				if(msg.getTokens().size() > 2)
				{
					for(std::vector<std::string>::const_iterator tit = msg.getTokens().begin() + 2; tit < msg.getTokens().end(); ++tit)
					{
						complete_msg.append(*tit);
						complete_msg.append(" ");
					}
					complete_msg.erase(complete_msg.begin());
				}
				it->handleMessage(complete_msg, client);
				break;
			}
		}
	}

	void Server::handleNotice(unstd::SharedPtr<class Client> client, const Message& msg)
	{
		if(msg.getArgs().empty())
		{
			logs::report(log_error, "NOTICE, invalid command '%s'", msg.getRawMsg().c_str());
			return;
		}
		if (msg.getTokens()[1][0] != '&' && msg.getTokens()[1][0] != '#')
		{
			for (client_it itc = _client.begin(); itc != _client.end(); ++itc)
			{
				if ((*itc)->getNickName() == msg.getTokens()[1] && (*itc)->getNickName() != client->getNickName())
				{
					std::string complete_msg;
					if(msg.getTokens().size() > 2)
					{
						for(std::vector<std::string>::const_iterator tit = msg.getTokens().begin() + 2; tit < msg.getTokens().end(); ++tit)
						{
							complete_msg.append(*tit);
							complete_msg.append(" ");
						}
						complete_msg.erase(complete_msg.begin());
					}
					(*itc)->sendMsg(client->getNickName(), "NOTICE " + (*itc)->getNickName(), complete_msg);
					break;
				}
			}
			return ;
		}
		for(channel_it it = _channels.begin(); it != _channels.end(); ++it)
		{
			if(msg.getTokens()[1] == it->getName())
			{
				std::string complete_msg;
				if(msg.getTokens().size() > 2)
				{
					for(std::vector<std::string>::const_iterator tit = msg.getTokens().begin() + 2; tit < msg.getTokens().end(); ++tit)
					{
						complete_msg.append(*tit);
						complete_msg.append(" ");
					}
					complete_msg.erase(complete_msg.begin());
				}
				it->handleMessage(complete_msg, client, true);
				break;
			}
		}
	}

	void Server::handleInvite(unstd::SharedPtr<class Client> client, const Message& msg)
	{
		(void)client;
		(void)msg;
	}

	void Server::handleKick(unstd::SharedPtr<class Client> client, const Message& msg)
	{
		if(msg.getArgs().empty())
		{
			logs::report(log_error, "KICK, invalid command '%s'", msg.getRawMsg().c_str());
			return;
		}

		typedef std::vector<std::string>::iterator name_it;
		std::vector<std::string> channels = unstd::split(msg.getArgs()[0], ',');
		std::vector<std::string> users = unstd::split(msg.getArgs()[1], ',');

		if(users.size() != channels.size())
		{
			client->sendCode(ERR_NEEDMOREPARAMS, "KICK : Not enough parameters");
			return;
		}

		for(name_it user = users.begin(), channel = channels.begin(); user < users.end(); ++user, ++channel)
		{
			if(!isUserKnown(*user))
			{
				client->sendCode(ERR_NOSUCHNICK, const_cast<std::string&>(*user) + " no such nick");
				continue;
			}
			if(!isChannelKnown(*channel))
			{
				client->sendCode(ERR_NOSUCHCHANNEL, const_cast<std::string&>(*channel) + " no such channel");
				continue;
			}

			Channel* channel_target = getChannelByName(*channel);
			if(channel_target == NULL)
				logs::report(log_fatal_error, "(KICK), cannot get channel '%s' by name; panic !", channel->c_str());
			unstd::SharedPtr<Client> client_target = getClientByName(*user);
			if(client_target.get() == NULL)
				logs::report(log_fatal_error, "(KICK), cannot get client '%s' by name; panic !", user->c_str());

			if(!channel_target->kick(client, client_target, msg.getReason()))
			{
				logs::report(log_error, "could not kick %s because why not", user->c_str());
				continue;
			}
			client->printUserHeader();
			std::cout << "kicked " << *user << " from " << *channel << std::endl;
		}
	}

	void Server::handleWho(unstd::SharedPtr<class Client> client, const class Message& msg)
	{
		irc::Channel *chan;

		logs::report(log_message, "tokensize ok : %d", msg.getTokens().size());
		if(msg.getTokens().size() != 2)
			return;
		if((chan = getChannelByName(msg.getTokens()[1])) == NULL)
			client->sendCode(ERR_NOSUCHCHANNEL, "No such channel");
		chan->sendWho(client);
	}

	void Server::handleTopic(unstd::SharedPtr<class Client> client, const Message& msg)
	{
		if(msg.getArgs().empty())
		{
			logs::report(log_error, "TOPIC, invalid command '%s'", msg.getRawMsg().c_str());
			return;
		}
		if(!isChannelKnown(msg.getArgs()[0]))
		{
			client->sendCode(ERR_NOSUCHCHANNEL, msg.getArgs()[0] + " no such channel");
			return;
		}
		Channel* channel = getChannelByName(msg.getArgs()[0]);
		if(channel == NULL)
			logs::report(log_fatal_error, "(TOPIC), cannot get channel '%s' by name; panic !", channel->getName().c_str());
		if(!channel->hasClient(client))
		{
			client->sendCode(ERR_NOTONCHANNEL, msg.getArgs()[0] + " you're not on that channel");
			return;
		}
		if(!msg.getReason().empty())
		{
			channel->setTopic(client, msg.getReason());
			client->printUserHeader();
			std::cout << "changed topic to " << msg.getReason() << std::endl;
		}
		else
			channel->relayTopic(client);
	}

	void Server::handlePing(unstd::SharedPtr<class Client> client, const Message& msg)
	{
		if(msg.getTokens().size() == 1)
		{
			logs::report(log_error, "PING, invalid command '%s'", msg.getRawMsg().c_str());
			return;
		}
		std::string out = "PONG";
		for(std::vector<std::string>::const_iterator it = msg.getTokens().begin() + 1; it < msg.getTokens().end(); ++it)
			out += ' ' + *it;
		out += "\r\n";
		client->sendPlainText(out);
		client->printUserHeader();
		std::cout << "pong" << std::endl;
	}

	void Server::handleMode(unstd::SharedPtr<class Client> client, const Message& msg)
	{
		logs::report(log_message, "Mode requested");
		irc::Channel *chan;
		if(msg.getTokens().size() < 2)
			return ;
		if(msg.getTokens().size() == 2 && (msg.getTokens()[1][0] == '#' || msg.getTokens()[1][0] == '&'))
		{
			chan = getChannelByName(msg.getTokens()[1]);
			if(chan == NULL)
				client->sendCode(ERR_NOSUCHCHANNEL, "No such channel");
			else
				chan->showModes(client);
			return ;
		}
		logs::report(log_message, "Mode parsing ok");
		channel_it it;
		for(it = _channels.begin(); it != _channels.end() && it->getName() != msg.getTokens()[1]; ++it);
		if(it == _channels.end())
			client->sendCode(ERR_NOSUCHCHANNEL, "No such channel");
		if(getChannelByName(msg.getTokens()[1]) && !it->isOp(client))
			client->sendCode(ERR_CHANOPRIVSNEEDED, "You need operator privileges to execute this command");
		if(msg.getTokens()[2][0] != '-' && msg.getTokens()[2][0] != '+')
			return ;
		it->changeMode(client, msg);
	}
}
