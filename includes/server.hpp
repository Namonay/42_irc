/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 09:12:28 by maldavid          #+#    #+#             */
/*   Updated: 2024/02/06 12:35:48 by vvaas            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef __SERVER_IRC__
#define __SERVER_IRC__

#include <string>
#include <vector>
#include <irc.hpp>
#include <unstd/shared_ptr.hpp>
#include <config.hpp>
#include <channel.hpp>

namespace irc
{
	class Server
	{
		public:
			Server(int port, const std::string& password);

			inline void closeMainSocket()
			{
				if(_main_socket > NULL_SOCKET)
					close(_main_socket);
				_main_socket = NULL_SOCKET;
				_active = false;
			}

			void wait();

			inline const std::string &getRunDate(void) const { return _run_date; };
			inline size_t getClientCount(void) const { return _client.size(); };
			inline size_t getChannelCount(void) const { return _channels.size(); };

			~Server();

		private:
			void initSocket();
			void initSocketData();
			void initSignal();

			bool handleMessage(unstd::SharedPtr<class Client> client);

			void handleInput();

			// ugly as f*ck
			void handleNick(unstd::SharedPtr<class Client> client, const class Message& msg, const class Server& server);
			void handleUser(unstd::SharedPtr<class Client> client, const class Message& msg, const class Server& server);
			void handlePass(unstd::SharedPtr<class Client> client, const class Message& msg, const class Server& server);
			void handleQuit(unstd::SharedPtr<class Client> client, const class Message& msg);
			void handlePart(unstd::SharedPtr<class Client> client, const class Message& msg);
			void handleJoin(unstd::SharedPtr<class Client> client, const class Message& msg);
			void handleInvite(unstd::SharedPtr<class Client> client, const class Message& msg);
			void handlePrivMsg(unstd::SharedPtr<class Client> client, const class Message& msg);
			void handleNotice(unstd::SharedPtr<class Client> client, const class Message& msg);
			void handleWho(unstd::SharedPtr<class Client> client, const class Message& msg);
			void handleKick(unstd::SharedPtr<class Client> client, const class Message& msg);
			void handleTopic(unstd::SharedPtr<class Client> client, const class Message& msg);
			void handlePing(unstd::SharedPtr<class Client> client, const class Message& msg);
			void handleMode(unstd::SharedPtr<class Client> client, const class Message& msg);
			void handleRussian(unstd::SharedPtr<class Client> client);

			class Channel* getChannelByName(const std::string& name);
			unstd::SharedPtr<class Client> getClientByName(const std::string& name);

			bool isUserKnown(const std::string& user) const;
			bool isChannelKnown(const std::string& channel) const;

		private:
			std::vector<class Channel> _channels;
			std::vector<unstd::SharedPtr<class Client> > _client;
			sockaddr_in _s_data;
			socklen_t _s_len;
			fd_set _fd_set;
			std::string _run_date;
			const std::string _password;
			const std::string _ip;
			int _port;
			int _main_socket;
			bool _active;
	};
}

#endif
