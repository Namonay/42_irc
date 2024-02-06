/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_functions.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 17:31:06 by maldavid          #+#    #+#             */
/*   Updated: 2024/02/06 12:56:32 by vvaas            ###   ########.fr       */
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
#include <cstdlib>
#include <ctime>

namespace irc
{
	typedef std::vector<unstd::SharedPtr<Client> >::iterator client_it;
	typedef std::vector<unstd::SharedPtr<Client> >::const_iterator client_const_it;

	typedef std::vector<Channel>::iterator channel_it;
	typedef std::vector<Channel>::const_iterator channel_const_it;

	void Server::handleNick(unstd::SharedPtr<class Client> client, const Message& msg, const Server& server)
	{
		if(msg.getTokens().size() < 2)
		{
			client->sendCode(ERR_NONICKNAMEGIVEN, " NICK : No nickname given");
			return;
		}
		if(msg.getTokens().size() >= 3)
			return;
		const std::string& nickname = msg.getTokens()[1];
		for(client_it it = _client.begin(); it != _client.end(); ++it)
		{
			if((*it)->getNickName() == nickname)
			{
				if(client->getNickName().empty())
				{
					client->sendCode(ERR_NICKCOLLISION, " NICK : Nickname is used");
					client->kill(" NICK : Nickname already used");
					client->requireDisconnect();
					return;
				}
				client->sendCode(ERR_NICKCOLLISION, "NICK : Nickname is used");
				return;
			}
		}
		std::string oldNick = (client->getNickName().size() > 0) ? client->getNickName() : msg.getTokens()[1]; // get nickname before /nick execution inside server (oldNick)
		client->printUserHeader();
		client->setNewNickName(msg.getTokens()[1]);
		for(client_it it = _client.begin(); it != _client.end(); ++it)
			(*it)->sendMsg(oldNick, "NICK", msg.getTokens()[1]);
		client->welcome(server);
		client->printUserHeader();
		std::cout << "new nickname, " << client->getNickName() << std::endl;
	}

	void Server::handleUser(unstd::SharedPtr<class Client> client, const Message& msg, const Server& server)
	{
		if(msg.getTokens().size() < 5)
		{
			client->sendCode(ERR_NEEDMOREPARAMS, "USER : Need more parameters");
			return;
		}
		if(client->isRegistered())
		{
			client->sendCode(ERR_ALREADYREGISTRED, "USER : You are already registered");
			return;
		}
		if(msg.getTokens()[4][0] != ':')
			return;
		client->printUserHeader();
		client->setNewUserName(msg.getTokens()[1]);
		std::cout << "new username, " << client->getUserName() << std::endl;

		client->printUserHeader();

		std::string realname;
		for(std::vector<std::string>::const_iterator it = msg.getTokens().begin() + 4; it != msg.getTokens().end(); ++it)
		{
			realname.append(*it);
			realname.append(" ");
		}
		realname.erase(realname.begin());
		realname.erase(realname.end() - 1);
		client->setNewRealName(realname);
		std::cout << "new realname, " << client->getRealName() << std::endl;
		client->register_user();
		client->welcome(server);
	}

	void Server::handlePass(unstd::SharedPtr<class Client> client, const Message& msg, const Server& server)
	{
		if(client->isLogged())
		{
			client->sendCode(ERR_ALREADYREGISTRED, "PASS : You are already registered");
			return;
		}
		if(msg.getTokens()[1] == _password)
		{
			client->login();
			client->welcome(server);
		}
		else
		{
			client->sendCode(ERR_PASSWDMISMATCH, " PASS : Invalid password");
			client->requireDisconnect();
		}
	}

	void Server::handleQuit(unstd::SharedPtr<class Client> client, const Message& msg)
	{
		for(channel_it it = _channels.begin(); it != _channels.end(); ++it)
			it->removeClient(client, (msg.getReason().empty() ? "Leaving" : msg.getReason()), true);
		client->printUserHeader();
		std::cout << "quit" << std::endl;
	}

	void Server::handlePart(unstd::SharedPtr<class Client> client, const Message& msg)
	{
		if(msg.getTokens().size() < 2 && msg.getTokens().size() > 3)
		{
			client->sendCode(ERR_NEEDMOREPARAMS, "PART : Parameters amount invalid");
			return;
		}
		if(msg.getTokens()[1][0] != '#' && msg.getTokens()[1][0] != '&')
			return;
		if(!isChannelKnown(msg.getArgs()[0]))
		{
			client->sendCode(ERR_NOSUCHCHANNEL, msg.getArgs()[0] + " no such channel");
			return;
		}
		Channel* channel = getChannelByName(msg.getArgs()[0]);
		if(channel == NULL)
			logs::report(log_fatal_error, "(KICK), cannot get channel '%s' by name; panic !", channel->getName().c_str());
		if(!channel->removeClient(client, (msg.getReason().empty() ? "" : msg.getReason())))
		{
			client->sendCode(ERR_NOTONCHANNEL, "PART : Not on channel");
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
			client->sendCode(ERR_NEEDMOREPARAMS, "JOIN : Need more params");
			return;
		}
		if(msg.getTokens()[1][0] != '#' && msg.getTokens()[1][0] != '&')
		{
			client->sendCode(ERR_NOSUCHCHANNEL, ": " + msg.getTokens()[1], "No such channel");
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
			return;
		}
		if((msg.getTokens().size() == 3 && msg.getTokens()[2] != it->getPassword()) || (msg.getTokens().size() == 2 && it->getPassword().size() > 0))
			client->sendCode(ERR_BADCHANNELKEY, "JOIN : Invalid password");
		else if(it->getChannelSize() != -1 && it->getChannelSize() <= static_cast<int>(it->getNumberOfClients()))
			client->sendCode(ERR_CHANNELISFULL, "JOIN : Channel is full");
		else if(it->isInviteOnly() && !client->hasBeenInvitedTo(it->getName()))
			client->sendCode(ERR_INVITEONLYCHAN, it->getName());
		else if(it->getPassword().size() == 0)
			it->addClient(client);
		else if(msg.getTokens().size() == 3 && it->getPassword().size() > 0 && msg.getTokens()[2] == it->getPassword())
			it->addClient(client);
	}

	void Server::handlePrivMsg(unstd::SharedPtr<class Client> client, const Message& msg)
	{
		if(msg.getArgs().empty())
		{
			client->sendCode(ERR_NORECIPIENT, "PRIVMSG : No recipient given");
			return;
		}
		if(msg.getTokens().size() < 3)
		{
			client->sendCode(ERR_NOTEXTTOSEND, "PRIVMSG : No text to send");
			return;
		}
		if(msg.getReason().empty())
		{
			client->sendCode(ERR_NOTEXTTOSEND, "PRIVMSG : No text to send");
			return;
		}
		if(msg.getTokens()[1][0] != '&' && msg.getTokens()[1][0] != '#')
		{
			for(client_it itc = _client.begin(); itc != _client.end(); ++itc)
			{
				if((*itc)->getNickName() == msg.getTokens()[1] && (*itc)->getNickName() != client->getNickName())
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
					if(complete_msg.empty())
						client->sendCode(ERR_NOTEXTTOSEND, "PRIVMSG : No text to send");
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
				if(!it->hasClient(client))
				{
					client->sendCode(ERR_CANNOTSENDTOCHAN, "PRIVMSG : You are not in the channel");
					return;
				}
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
			return;
		if(msg.getTokens()[1][0] != '&' && msg.getTokens()[1][0] != '#')
		{
			for(client_it itc = _client.begin(); itc != _client.end(); ++itc)
			{
				if((*itc)->getNickName() == msg.getTokens()[1] && (*itc)->getNickName() != client->getNickName())
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
				it->handleMessage(complete_msg, client, true);
				break;
			}
		}
	}

	void Server::handleInvite(unstd::SharedPtr<class Client> client, const Message& msg)
	{
		if(msg.getArgs().empty() || msg.getArgs().size() != 2)
		{
			client->sendCode(ERR_NEEDMOREPARAMS, "INVITE : Invalid parameters");
			return;
		}

		if(!isUserKnown(msg.getArgs()[0]))
		{
			client->sendCode(ERR_NOSUCHNICK, const_cast<std::string&>(msg.getArgs()[0]) + " no such nick");
			return;
		}
		if(!isChannelKnown(msg.getArgs()[1]))
		{
			client->sendCode(ERR_NOSUCHCHANNEL, const_cast<std::string&>(msg.getArgs()[1]) + " no such channel");
			return;
		}

		Channel* channel_target = getChannelByName(msg.getArgs()[1]);
		if(channel_target == NULL)
			logs::report(log_fatal_error, "(INVITE), cannot get channel '%s' by name; panic !", msg.getArgs()[1].c_str());

		unstd::SharedPtr<Client> client_target = getClientByName(msg.getArgs()[0]);
		if(client_target.get() == NULL)
			logs::report(log_fatal_error, "(INVITE), cannot get client '%s' by name; panic !", msg.getArgs()[0].c_str());

		if(!channel_target->hasClient(client))
		{
			logs::report(log_fatal_error, "(INVITE), cannot get channel '%s' by name; panic !", msg.getArgs()[1].c_str());
			return;
		}

		if(channel_target->hasClient(client_target->getNickName()))
		{
			client->sendCode(ERR_USERONCHANNEL, msg.getArgs()[0] + ' ' + msg.getArgs()[1], "is already on channel");
			return;
		}

		if(channel_target->isInviteOnly() && !channel_target->isOp(client))
		{
			client->sendCode(ERR_CHANOPRIVSNEEDED, msg.getArgs()[1], "INVITE : you're not channel operator");
			return;
		}

		client_target->inviteToChannel(channel_target->getName());
		client_target->sendMsg(client->getNickName(), "INVITE " + msg.getArgs()[0] + ' ' + msg.getArgs()[1], "");
		client->sendCode(RPL_INVITING, msg.getArgs()[1] + ' ' + msg.getArgs()[0], "");
	}

	void Server::handleKick(unstd::SharedPtr<class Client> client, const Message& msg)
	{
		if(msg.getArgs().empty())
		{
			client->sendCode(ERR_NONICKNAMEGIVEN, "KICK : No nickname given");
			return;
		}
		if (msg.getArgs().size() < 2)
		{
			client->sendCode(ERR_NEEDMOREPARAMS, "KICK : Not enough parameters");
			return ;
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
				client->sendCode(ERR_NOSUCHNICK, const_cast<std::string&>(*user) + " : no such nick");
				continue;
			}
			if(!isChannelKnown(*channel))
			{
				client->sendCode(ERR_NOSUCHCHANNEL, const_cast<std::string&>(*channel) + " : no such channel");
				continue;
			}

			Channel* channel_target = getChannelByName(*channel);
			if(channel_target == NULL)
				logs::report(log_fatal_error, "(KICK), cannot get channel '%s' by name; panic !", channel->c_str());
			unstd::SharedPtr<Client> client_target = getClientByName(*user);
			if(client_target.get() == NULL)
				logs::report(log_fatal_error, "(KICK), cannot get client '%s' by name; panic !", user->c_str());

			if(!channel_target->kick(client, client_target, msg.getReason()))
				continue;
			client->printUserHeader();
			std::cout << "kicked " << *user << " from " << *channel << std::endl;
			if(channel_target->getNumberOfClients() == 0)
			{
				channel_it it;
				for(it = _channels.begin(); it < _channels.end(); ++it)
				{
					if(it->getName() == msg.getArgs()[0])
						break;
				}
				_channels.erase(it);
				logs::report(log_message, "channel '%s' has been destroyed", channel_target->getName().c_str());
			}
		}
	}

	void Server::handleWho(unstd::SharedPtr<class Client> client, const class Message& msg	)
	{
		irc::Channel *chan;

		if(msg.getTokens().size() != 2)
			return;
		if((chan = getChannelByName(msg.getTokens()[1])) == NULL)
		{
			client->sendCode(ERR_NOSUCHCHANNEL, "WHO : No such channel");
			return;
		}
		chan->sendWho(client);
	}

	void Server::handleTopic(unstd::SharedPtr<class Client> client, const Message& msg)
	{
		if(msg.getArgs().empty())
		{
			client->sendCode(ERR_NEEDMOREPARAMS, "TOPIC : Need more parameters");
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
			return;
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
		irc::Channel *chan;
		if(msg.getTokens().size() < 2)
		{
			client->sendCode(ERR_NEEDMOREPARAMS, "MODE : Need more parameters");
			return;
		}
		if(msg.getTokens().size() == 2 && (msg.getTokens()[1][0] == '#' || msg.getTokens()[1][0] == '&'))
		{
			chan = getChannelByName(msg.getTokens()[1]);
			if(chan == NULL)
				client->sendCode(ERR_NOSUCHCHANNEL, "MODE : No such channel");
			else
				chan->showModes(client);
			return;
		}
		chan = getChannelByName(msg.getTokens()[1]);
		if(chan == NULL)
		{
			client->sendCode(ERR_NOSUCHCHANNEL, "MODE : No such channel");
			return;
		}
		if(!chan->isOp(client))
		{
			client->sendCodeInChannel(ERR_CHANOPRIVSNEEDED, *chan, "MODE : You need operator privileges to execute this command");
			return;
		}
		if(msg.getTokens()[2][0] != '-' && msg.getTokens()[2][0] != '+')
		{
			client->sendCode(ERR_UNKNOWNMODE, "MODE : Invalid flags (missing +/-)");
			return;
		}
		chan->changeMode(client, msg);
	}
	void Server::handleRussian(unstd::SharedPtr<class Client> client)
	{
		srand(time(NULL));

		if(rand() % 6 == 0)
		{
			client->kill("Pew Pew");
			usleep(100);
			client->requireDisconnect();
		}
		else
			client->sendModular("%s", "Click..");
	}
}

