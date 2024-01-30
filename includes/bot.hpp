/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/30 01:54:36 by vvaas             #+#    #+#             */
/*   Updated: 2024/01/30 16:56:57 by vvaas            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#define IP "127.0.0.1"
#define PORT 6667
#define PASSWORD "gg"

#include <irc.hpp>
#include <vector>
#include <string>

class bot
{
	public:
		bot();
		void init(void);
		void connect_to_server(void);
		void handle_response(std::string buffer);
		~bot();
	private :
		struct sockaddr_in _serv_addr;
		int _fd;
		std::vector<std::string> _connect_commands;
};