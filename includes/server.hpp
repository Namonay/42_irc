/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 09:12:28 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/25 18:44:01 by vvaas            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef __SERVER_IRC__
#define __SERVER_IRC__

#include <string>
#include <vector>
#include <irc.hpp>
#include <unstd/shared_ptr.hpp>
#include <config.hpp>

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

			~Server();

		private:
			void initSocket();
			void initSocketData();
			void initSignal();

			bool handleMessage(unstd::SharedPtr<class Client> client);

			void handleInput();

			// ugly as f*ck
			void handleNick(unstd::SharedPtr<class Client> client, const class Message& msg);
			void handleUser(unstd::SharedPtr<class Client> client, const class Message& msg);
			void handlePass(unstd::SharedPtr<class Client> client, const class Message& msg);
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
			const std::string _password;
			const std::string _ip;
			int _port;
			int _main_socket;
			bool _active;
	};
}

#endif
