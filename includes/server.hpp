/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 09:12:28 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/21 17:18:17 by vvaas            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef __SERVER_IRC__
#define __SERVER_IRC__

#include <string>
#include <vector>
#include <irc.hpp>
#include <unstd/shared_ptr.hpp>

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
			void wait(void);

		private:
			sockaddr_in _s_data;
			socklen_t	_s_len;
			fd_set		_fd_set;

			std::vector<class Channel> _channels;
			std::vector<unstd::SharedPtr<class Client> > _client;
			const std::string _password;
			const std::string _ip;
			const int _port;
			int _main_socket;
			bool _active;
	};
}

#endif
