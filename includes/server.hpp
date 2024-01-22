/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 09:12:28 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/22 17:21:23 by vvaas            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef __SERVER_IRC__
#define __SERVER_IRC__

#include <string>
#include <vector>
#include <irc.hpp>
#include <unstd/shared_ptr.hpp>
#include <csignal>

#define MAX_USERS 20

namespace irc
{
	class Server
	{
		public:
			Server(int port, const std::string& password);

			inline void closeMainSocket() { if (_main_socket > 0) close(_main_socket); _main_socket = 0; _active = false; }
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
			void handleQuit(unstd::SharedPtr<class Client> client, const class Message& msg);
			void handlePart(unstd::SharedPtr<class Client> client, const class Message& msg);
			void handleJoin(unstd::SharedPtr<class Client> client, const class Message& msg);
			void handlePrivMsg(unstd::SharedPtr<class Client> client, const class Message& msg);
			void handleNotice(unstd::SharedPtr<class Client> client, const class Message& msg);
			void handleKick(unstd::SharedPtr<class Client> client, const class Message& msg);
			void handleMotD(unstd::SharedPtr<class Client> client, const class Message& msg);
			void handleTopic(unstd::SharedPtr<class Client> client, const class Message& msg);
			void handlePing(unstd::SharedPtr<class Client> client, const class Message& msg);
			void handleMode(unstd::SharedPtr<class Client> client, const class Message& msg);

		private:
			std::vector<class Channel> _channels;
			std::vector<unstd::SharedPtr<class Client> > _client;
			sockaddr_in _s_data;
			socklen_t _s_len;
			fd_set _fd_set;
			const std::string _password;
			const std::string _ip;
			const int _port;
			int _main_socket;
			bool _active;
	};
}

#endif
