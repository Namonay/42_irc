/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 10:33:17 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/22 13:03:37 by maldavid         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef __CLIENT__
#define __CLIENT__

#include <irc.hpp>
#include <string>

namespace irc
{
	class Client
	{
		public:
			Client(int fd, sockaddr_in sock, int id);

			inline void newMsgInFlight(const std::string& msg) { _msg_in_flight = msg; }

			inline int getFD() const { return _fd; }
			inline int getID() const { return _id; }
			inline sockaddr_in getSockAddr() const { return _s_data; }
			inline const std::string& getMsgInFlight() const { return _msg_in_flight; }

			std::string getNextMsg();

			inline void setNewNickName(const std::string& name) { _nickname = name; }
			inline void setNewUserName(const std::string& name) { _username = name; }
			inline void setNewRealName(const std::string& name) { _realname = name; }

			inline const std::string& getNickName() const { return _nickname; }
			inline const std::string& getUserName() const { return _username; }
			inline const std::string& getRealName() const { return _realname; }

			void printUserHeader() const;

			inline void setFd(int new_fd) { _fd = new_fd; }

			~Client();

		private:
			std::string _nickname;
			std::string _username;
			std::string _realname;
			std::string _msg_in_flight;
			sockaddr_in _s_data;
			int _fd;
			int _id;
	};
}

#endif
