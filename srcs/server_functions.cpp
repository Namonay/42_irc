/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_functions.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 17:31:06 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/25 02:43:54 by vvaas            ###   ########.fr       */
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

		for(std::string::const_iterator it = msg.getTokens()[1].begin(); it != msg.getTokens()[1].end(); ++it)
		{
			if((it == msg.getTokens()[1].begin() || *(it - 1) == ',') && std::strchr("&#", *it) == NULL)
			{
				logs::report(log_error, "PART, invalid channel name '%s'", msg.getTokens()[1].c_str());
				return;
			}
		}

		channel_it chit;
		for(chit = _channels.begin(); chit != _channels.end(); ++chit)
		{
			if(msg.getTokens()[1] == chit->getName())
				break;
		}
		if(chit == _channels.end())
		{
			client->sendCode(ERR_NOSUCHCHANNEL, "NO such channel");
			return;
		}
		if(!chit->removeClient(client))
		{
			client->sendCode(ERR_NOTONCHANNEL, "Not on channel");
			return;
		}
		client->printUserHeader();
		std::cout << "leaving channel, " << msg.getTokens()[1] << std::endl;
		if(chit->getNumberOfClients() == 0)
			logs::report(log_message, "channel '%s' has been destroyed", chit->getName().c_str());
	}

	void Server::handleJoin(unstd::SharedPtr<class Client> client, const Message& msg)
	{
		if(msg.getTokens().size() < 2)
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
		if(msg.getTokens().size() < 2)
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
				it->handleMessage(complete_msg, client);
				break;
			}
		}
	}

	void Server::handleNotice(unstd::SharedPtr<class Client> client, const Message& msg)
	{
		if(msg.getTokens().size() < 2)
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
		if (msg.getTokens().size() != 3 || (msg.getTokens().size() != 4 && !std::strchr("koi", msg.getTokens()[2][1])))
			return ;
		logs::report(log_message, "Mode parsing ok");
		channel_it it;
		for (it = _channels.begin(); it != _channels.end() && it->getName() != msg.getTokens()[1]; ++it);
		if (it == _channels.end())
			client->sendCode(ERR_NOSUCHCHANNEL, "No such channel");
		if (!it->isOp(client))
			client->sendCode(ERR_CHANOPRIVSNEEDED, "You need operator privileges to execute this command");
		if (msg.getTokens()[2][0] != '-' && msg.getTokens()[2][0] != '+')
			return ;
		it->changeMode(client, msg);
	}
}
