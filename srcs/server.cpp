/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 09:31:17 by maldavid          #+#    #+#             */
/*   Updated: 2024/02/07 16:52:35 by maldavid         ###   ########.fr       */
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
#include <ctime>

namespace irc
{
	typedef std::vector<unstd::SharedPtr<Client> >::iterator client_it;
	typedef std::vector<unstd::SharedPtr<Client> >::const_iterator client_const_it;

	typedef std::vector<Channel>::iterator channel_it;
	typedef std::vector<Channel>::const_iterator channel_const_it;

	Server::Server(int port, const std::string& password) : _s_len(sizeof(_s_data)), _password(password), _port(port), _main_socket(NULL_SOCKET), _active(true)
	{
		_init_failed = false;
		time_t ltime = time(NULL);
		struct tm tstruct = *localtime(&ltime);
		char buf[100];

		if(password.empty() || password.find_first_of(" \t\r\v") != std::string::npos)
		{
			logs::report(log_error, "Password is invalid !");
			return;
		}
		std::memset(&_s_data, 0, sizeof(sockaddr));
		initSocket();
		strftime(buf, 100, "%a %b %d %Y %T CET", &tstruct);
		_run_date = buf;
	}

	void Server::initSocketData()
	{
		_s_data.sin_family = AF_INET;
		_s_data.sin_addr.s_addr = INADDR_ANY;
		_s_data.sin_port = htons(_port);
		_main_socket = socket(AF_INET, SOCK_STREAM, 0); // AF_INET == IPv4, SOCK_STREAM == TCP
		if(_main_socket < 0)
		{
			logs::report(log_error, "socket error");
			_init_failed = true;
			return;
		}
		logs::report(log_message, "socket succesfully started");
	}

	void Server::initSocket()
	{
		int opt = 1;

		initSocketData();
		if(setsockopt(_main_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) // SOL_SOCKET : modify socket only, SO_REUSEADDR : Reusable after program ends
		{
			logs::report(log_error, "setsockopt() error (tout a pete)");
			_init_failed = true;
			return;
		}

		if(bind(_main_socket, reinterpret_cast<sockaddr*>(&_s_data), sizeof(_s_data)) != 0) // Bind _main_socket to localhost
		{
			logs::report(log_error, "bind error");
			_init_failed = true;
			return;
		}
		logs::report(log_message, "bind successful, starting listen loop");

		if(listen(_main_socket, MAX_USERS) != 0) // init the listen with MAX_USERS
		{
			logs::report(log_error, "listen error");
			_init_failed = true;
			return;
		}
		logs::report(log_message, "listen queue created successfully");

		logs::report(log_message, "server is up and running");
	}

	void Server::handleInput()
	{
		char buffer[INPUT_SIZE] = { 0 };
		ssize_t recv_size;
		for(std::vector<unstd::SharedPtr<Client> >::iterator it = _client.begin(); it != _client.end(); ++it)
		{
			if(!FD_ISSET((*it)->getFD(), &_fd_set))
				continue;
			while((recv_size = recv((*it)->getFD(), buffer, INPUT_SIZE, 0)) > 0) // read() but forsocket fd
			{
				(*it)->newMsgInFlight(buffer);
			#ifdef DEBUG
				logs::report(log_message,"processing '%s'", buffer);
			#endif
				while(handleMessage(*it));
				std::memset(buffer, 0, sizeof(buffer)); // clear the buffer to avoid trash remaining
			}
			if(recv_size == 0 || (*it)->disconnectRequired()) // recv return 0 ifan user disconnect
			{
				logs::report(log_message, "User %d disconnected", (*it)->getID());
				close((*it)->getFD());
				for(channel_it cit = _channels.begin(); cit != _channels.end();)
				{
					cit->removeClient(*it, "", true);
					if(cit->getNumberOfClients() == 0)
					{
						logs::report(log_message, "channel '%s' has been destroyed", cit->getName().c_str());
						cit = _channels.erase(cit);
					}
					else
						++cit;
				}
				it = _client.erase(it) - 1;
				
			}
		}
	}

	void Server::wait()
	{
		int tmp;
		int fd = 0;
		int i = 0;
		socklen_t len = sizeof(sockaddr_in);

		if(_main_socket == NULL_SOCKET)
			return;
		while(_active)
		{
			FD_ZERO(&_fd_set);
			FD_SET(_main_socket, &_fd_set);

			for(std::vector<unstd::SharedPtr<Client> >::iterator it = _client.begin(); it != _client.end(); ++it)
				FD_SET((*it)->getFD(), &_fd_set);

			tmp = select(MAX_USERS, &_fd_set, NULL, NULL, NULL); // SELECT blocks till a connection or message is received, and let only those in _fd_set
			if(tmp < 0 && _main_socket != NULL_SOCKET)
			{
				logs::report(log_error, "select fd error");
				return;
			}

			if(FD_ISSET(_main_socket, &_fd_set)) // ifit's a new connection
			{
				sockaddr_in cli_sock;
				fd = accept(_main_socket, reinterpret_cast<sockaddr*>(&cli_sock), &len); // adds the new connection
				if(fd < 0)
				{
					logs::report(log_error, "accept() error");
					return;
				}
				if(fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
				{
					logs::report(log_error, "fcntl() error");
					return;
				}

				unstd::SharedPtr<Client> new_client(new Client(fd, cli_sock, i++));
				_client.push_back(new_client); // put the client into the vector used in handle_input

				logs::report(log_message, "User %d connected", _client.back()->getID());
			}
			handleInput();
		}
	}

	bool Server::handleMessage(unstd::SharedPtr<Client> client)
	{
		if(client->getMsgInFlight().empty()) // ifthere are no commands just return
			return false;

		const Message msg(client, client->getNextMsg());
		if(msg.getCmd() == "NICK")
			handleNick(client, msg, *this);
		else if(msg.getCmd() == "USER")
			handleUser(client, msg, *this);
		else if(msg.getCmd() == "PASS")
			handlePass(client, msg, *this);
		else if(!client->isWelcomed())
			return true;
		else if(msg.getCmd() == "QUIT")
			handleQuit(client, msg);
		else if(msg.getCmd() == "WHO")
			handleWho(client, msg);
		else if(msg.getCmd() == "PART")
			handlePart(client, msg);
		else if(msg.getCmd() == "JOIN")
			handleJoin(client, msg);
		else if(msg.getCmd() == "INVITE")
			handleInvite(client, msg);
		else if(msg.getCmd() == "PRIVMSG")
			handlePrivMsg(client, msg);
		else if(msg.getCmd() == "NOTICE")
			handleNotice(client, msg);
		else if(msg.getCmd() == "KICK")
			handleKick(client, msg);
		else if(msg.getCmd() == "TOPIC")
			handleTopic(client, msg);
		else if(msg.getCmd() == "PING")
			handlePing(client, msg);
		else if(msg.getCmd() == "MODE")
			handleMode(client, msg);
		else if(msg.getCmd() == "imfeelinglucky")
			handleRussian(client);
		else
			client->sendCode(ERR_UNKNOWNCOMMAND, "No such command");
		return true;
	}

	bool Server::isUserKnown(const std::string& user) const
	{
		for(client_const_it it = _client.begin(); it < _client.end(); ++it)
		{
			if(const_cast<unstd::SharedPtr<Client>&>(*it)->getNickName() == user)
				return true;
		}
		return false;
	}

	bool Server::isChannelKnown(const std::string& channel) const
	{
		for(channel_const_it it = _channels.begin(); it < _channels.end(); ++it)
		{
			if(it->getName() == channel)
				return true;
		}
		return false;
	}

	Channel* Server::getChannelByName(const std::string& name)
	{
		for(channel_it it = _channels.begin(); it < _channels.end(); ++it)
		{
			if(it->getName() == name)
				return &*it;
		}
		return NULL;
	}

	unstd::SharedPtr<Client> Server::getClientByName(const std::string& name)
	{
		for(client_it it = _client.begin(); it < _client.end(); ++it)
		{
			if((*it)->getNickName() == name)
				return *it;
		}
		return unstd::SharedPtr<Client>(NULL);
	}

	Server::~Server()
	{
		for(client_it it = _client.begin(); it != _client.end(); ++it)
		{
			if((*it)->isWelcomed())
				(*it)->kill("Server shutting down");
		}
		closeMainSocket();
		for(int i = 3; i <= FD_MAX; ++i)
			close(i);
	}
}
