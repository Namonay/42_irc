/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_functions.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 17:31:06 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/24 12:52:32 by maldavid         ###   ########.fr       */
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

namespace irc
{
	void Server::handleNick(unstd::SharedPtr<class Client> client, const Message& msg)
	{
		if(msg.getTokens().size() != 2 && msg.getTokens().size() != 3)
		{
			logs::report(log_error, "NICK, invalid command '%s'", msg.getRawMsg().c_str());
			return;
		}
		const std::string& nickname = msg.getTokens()[1];
		for(std::vector<unstd::SharedPtr<Client> >::iterator it = _client.begin(); it != _client.end(); ++it)
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

		if (client->getLogged())
			return ;
		if (msg.getTokens()[1] == _password)
		{
			client->setLogged();
			client->sendCode(RPL_WELCOME, "Welcome :)\n");
		}
		else
		{
			client->sendCode(ERR_PASSWDMISMATCH);
			client->setDisconnect();
		}
	}
	void Server::handleQuit(unstd::SharedPtr<class Client> client, const Message& msg)
	{
		(void)msg;
		for(std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it)
			it->removeClient(client);
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

		for(std::string::const_iterator it = msg.getTokens()[1].begin(); it != msg.getTokens()[1].end(); ++it)
		{
			if((it == msg.getTokens()[1].begin() || *(it - 1) == ',') && std::strchr("&#", *it) == NULL)
			{
				logs::report(log_error, "PART, invalid channel name '%s'", msg.getTokens()[1].c_str());
				return;
			}
		}

		std::vector<Channel>::iterator chit;
		for(chit = _channels.begin(); chit != _channels.end(); ++chit)
		{
			if(msg.getTokens()[1] == chit->getName())
				break;
		}
		if(chit == _channels.end())
		{
			logs::report(log_error, "PART, channel not found '%s'", msg.getTokens()[1].c_str());
			return;
		}
		if(!chit->removeClient(client))
		{
			logs::report(log_error, "PART, client was not in channel '%s'", msg.getTokens()[1].c_str());
			return;
		}
		client->printUserHeader();
		std::cout << "leaving channel, " << msg.getTokens()[1] << std::endl;
		if(chit->getNumberOfClients() == 0)
			logs::report(log_message, "channel '%s' has been destroyed", chit->getName().c_str());
	}

	void Server::handleJoin(unstd::SharedPtr<class Client> client, const Message& msg)
	{
		if(msg.getTokens().size() < 2 && msg.getTokens().size() > 3)
		{
			logs::report(log_error, "JOIN, invalid command '%s'", msg.getRawMsg().c_str());
			return;
		}
		if(msg.getTokens()[1][0] != '#' && msg.getTokens()[1][0] != '&')
		{
			logs::report(log_error, "JOIN, invalid channel name '%s'", msg.getTokens()[1].c_str());
			return;
		}

		std::vector<Channel>::iterator it;
		for(it = _channels.begin(); it != _channels.end(); ++it)
		{
			if(msg.getTokens()[1] == it->getName())
				break;
		}
		if(it == _channels.end())
		{
			_channels.push_back(Channel(msg.getTokens()[1]));
			_channels.back().addClient(client);
			logs::report(log_message, "channel '%s' has been created", msg.getTokens()[1].c_str());
		}
		else
			it->addClient(client, true);
		client->printUserHeader();
		std::cout << "joining new channel, " << msg.getTokens()[1] << std::endl;
	}

	void Server::handlePrivMsg(unstd::SharedPtr<class Client> client, const Message& msg)
	{
		if(msg.getTokens().size() < 2)
		{
			logs::report(log_error, "PRIVMSG, invalid command '%s'", msg.getRawMsg().c_str());
			return;
		}
		std::vector<Channel>::iterator it;
		for(it = _channels.begin(); it != _channels.end(); ++it)
		{
			if(msg.getTokens()[1] == it->getName())
			{
				std::string complete_msg;
				for(std::vector<std::string>::const_iterator tit = msg.getTokens().begin(); tit != msg.getTokens().end(); ++it)
				{
					complete_msg.append(*tit);
					complete_msg.append(" ");
				}
				it->handleMessage(complete_msg, client);
				break;
			}
		}
	}

	void Server::handleNotice(unstd::SharedPtr<class Client> client, const Message& msg)
	{
		(void)client;
		(void)msg;
	}

	void Server::handleKick(unstd::SharedPtr<class Client> client, const Message& msg)
	{
		(void)client;
		(void)msg;
	}

	void Server::handleMotD(unstd::SharedPtr<class Client> client, const Message& msg)
	{
		(void)client;
		(void)msg;
	}

	void Server::handleTopic(unstd::SharedPtr<class Client> client, const Message& msg)
	{
		(void)client;
		if(msg.getTokens().size() == 1)
		{
			logs::report(log_error, "TOPIC, invalid command '%s'", msg.getRawMsg().c_str());
			return;
		}
		if(msg.getTokens()[1][0] != '#' && msg.getTokens()[1][0] != '&')
		{
			logs::report(log_error, "TOPIC, invalid channel name '%s'", msg.getTokens()[1].c_str());
			return;
		}
	}

	void Server::handlePing(unstd::SharedPtr<class Client> client, const Message& msg)
	{
		(void)client;
		(void)msg;
	}

	void Server::handleMode(unstd::SharedPtr<class Client> client, const Message& msg)
	{
		(void)client;
		(void)msg;
	}
}
