/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 09:31:17 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/22 17:00:42 by maldavid         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include <server.hpp>
#include <client.hpp>
#include <channel.hpp>
#include <logs.hpp>
#include <cstring>
#include <errno.h>
#include <stack>
#include <fcntl.h>
#include <ansi.hpp>
#include <config.hpp>
#include <message.hpp>
#include <algorithm>

namespace irc
{
	Server::Server(int port, const std::string& password) : _s_len(sizeof(_s_data)), _password(password), _port(port), _active(true)
	{
		std::memset(&_s_data, 0, sizeof(sockaddr));
		initSocket();
	}

	void Server::initSocketData()
	{
		_s_data.sin_family = AF_INET;
		_s_data.sin_addr.s_addr = INADDR_ANY;
		_s_data.sin_port = htons(_port);
		_main_socket = socket(AF_INET, SOCK_STREAM, 0); // AF_INET == IPv4, SOCK_STREAM == TCP
		if(_main_socket < 0)
			logs::report(log_fatal_error, "socket error");
		logs::report(log_message, "socket succesfully started");
	}

	void Server::initSocket()
	{
		int opt = 0;
	
		initSocketData();
		if(setsockopt(_main_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) // SOL_SOCKET : modify socket only, SO_REUSEADDR : Reusable after program ends
			logs::report(log_fatal_error, "setsockopt() error (tout a pete)");

		if(bind(_main_socket, reinterpret_cast<sockaddr*>(&_s_data), sizeof(_s_data)) != 0) // Bind _main_socket to localhost
			logs::report(log_fatal_error, "bind error");
		logs::report(log_message, "bind successful, starting listen loop");

		if(listen(_main_socket, MAX_USERS) != 0) // init the listen with MAX_USERS
			logs::report(log_fatal_error, "listen error");
		logs::report(log_message, "listen queue created successfully");

		logs::report(log_message, "server is up and running");
	}

	void Server::handleInput()
	{
		char buffer[INPUT_SIZE] = { 0 };

		for(std::vector<unstd::SharedPtr<Client> >::iterator it = _client.begin(); it != _client.end(); ++it)
		{
			if(!FD_ISSET((*it)->getFD(), &_fd_set))
				continue;

			while(recv((*it)->getFD(), buffer, INPUT_SIZE, 0) > 0) // read() but for socket fd
			{
				(*it)->newMsgInFlight(buffer);
				while(handleMessage(*it));
				std::memset(buffer, 0, sizeof(buffer)); // clear the buffer to avoid trash remaining
			}

			if(recv((*it)->getFD(), buffer, INPUT_SIZE, 0) == 0) // recv return 0 if an user disconnect
			{
				logs::report(log_message, "User %d disconnected", (*it)->getID());
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

		while(_active)
		{
			FD_ZERO(&_fd_set);
			FD_SET(_main_socket, &_fd_set);

			for(std::vector<unstd::SharedPtr<Client> >::iterator it = _client.begin(); it != _client.end(); ++it)
				FD_SET((*it)->getFD(), &_fd_set);

			tmp = select(MAX_USERS, &_fd_set, NULL, NULL, NULL); // SELECT blocks till a connection or message is received, and let only those in _fd_set
			if(tmp < 0)
				logs::report(log_fatal_error, "select fd error");

			if(FD_ISSET(_main_socket, &_fd_set)) // if it's a new connection
			{
				sockaddr_in cli_sock;
				fd = accept(_main_socket, (sockaddr *)&cli_sock, &len); // adds the new connection
				if(fd < 0)
					logs::report(log_fatal_error, "accept() error");
				if(fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
					logs::report(log_fatal_error, "fcntl() error");

				unstd::SharedPtr<Client> new_client(new Client(fd, cli_sock, i++));
				_client.push_back(new_client); // put the client into the vector used in handle_input

				logs::report(log_message, "User %d connected", _client.back()->getID());
			}

			handleInput();
		}
	}

	bool Server::handleMessage(unstd::SharedPtr<Client> client)
	{
		if(client->getMsgInFlight().empty()) // if there are no commands just return
			return false;

		const Message msg(client, client->getNextMsg());

		if(msg.getCmd() == "NICK")
			handleNick(client, msg);
		else if(msg.getCmd() == "USER")
			handleUser(client, msg);
		else if(msg.getCmd() == "QUIT")
			handleQuit(client, msg);
		else if(msg.getCmd() == "PART")
			handlePart(client, msg);
		else if(msg.getCmd() == "JOIN")
			handleJoin(client, msg);
		else if(msg.getCmd() == "PRIVMSG")
			handlePrivMsg(client, msg);
		else if(msg.getCmd() == "NOTICE")
			handleNotice(client, msg);
		else if(msg.getCmd() == "KICK")
			handleKick(client, msg);
		else if(msg.getCmd() == "MOTD")
			handleMotD(client, msg);
		else if(msg.getCmd() == "TOPIC")
			handleTopic(client, msg);
		else if(msg.getCmd() == "PING")
			handlePing(client, msg);
		else if(msg.getCmd() == "MODE")
			handleMode(client, msg);
		return true;
	}

	void Server::handleNick(unstd::SharedPtr<class Client> client, const Message& msg)
	{
		// TODO : handle nick collisions
		if(msg.getTokens().size() != 2 && msg.getTokens().size() != 3)
		{
			logs::report(log_error, "NICK, invalid command '%s'", msg.getRawMsg().c_str());
			return;
		}
		client->printUserHeader();
		client->setNewNickName(msg.getTokens()[1]);
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

		//client->printUserHeader();
		//client->setNewRealName(msg.getTokens()[1]);
		//std::cout << "new realname, " << client->getRealName() << std::endl;
	}

	void Server::handleQuit(unstd::SharedPtr<class Client> client, const Message& msg)
	{
		(void)msg;
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
			logs::report(log_message, "channel '%s' has beed destroyed", chit->getName().c_str());
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
			logs::report(log_message, "channel '%s' has beed created", msg.getTokens()[1].c_str());
			_channels.back().addClient(client);
		}
		else
			it->addClient(client);
		client->printUserHeader();
		std::cout << "joining new channel, " << msg.getTokens()[1] << std::endl;
	}

	void Server::handlePrivMsg(unstd::SharedPtr<class Client> client, const Message& msg)
	{
		(void)client;
		(void)msg;
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
		(void)msg;
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

	Server::~Server()
	{
		if (_main_socket > 0)
			close(_main_socket);
	}
}
