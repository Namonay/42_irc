/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/30 01:54:36 by vvaas             #+#    #+#             */
/*   Updated: 2024/02/07 16:57:53 by maldavid         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include <irc.hpp>
#include <vector>
#include <string>

class Bot
{
	public:
		Bot();
		
		bool init(const std::string &ip, const std::string &port, const std::string &password);
		void connect_to_server(void);
		void handle_response(std::string buffer);
		void send_message(const std::string &content);

		~Bot();

	private :
		std::vector<std::string> _connect_commands;
		struct sockaddr_in _serv_addr;
		int _fd;
		bool _channel_created;
		bool _logged;
};
