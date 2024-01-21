/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maldavid <kbz_8.dev@akel-engine.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 09:12:28 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/21 10:37:58 by maldavid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __SERVER_IRC__
#define __SERVER_IRC__

#include <string>
#include <vector>

namespace irc
{
	class Server
	{
		public:
			Server(int port, const std::string& password);

			~Server();

		private:
			std::vector<class Channel> _channels;
			const std::string _password;
			const int _port;
			int _socket;
	};
}

#endif
