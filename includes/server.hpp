/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 09:12:28 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/22 01:14:42 by vvaas            ###   ########.fr       */
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

			~Server();
			inline unsigned long get_port(void) { return (htons(_port)); }
		private:
			void init_socket(void);
			void init_socket_data(void);
			void init_signal(void);

			void wait(void);
			void add_client(int fd);

			void handle_input(void);
		private:
			sockaddr_in _s_data;
			socklen_t	_s_len;
			fd_set		_fd_set;

			std::vector<class Channel> _channels;
			std::vector<class Client> _client;
			const std::string _password;
			const std::string _ip;
			const int _port;
			int _main_socket;
			bool _active;
	};
}

#endif
