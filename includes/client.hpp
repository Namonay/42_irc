/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 10:33:17 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/26 00:58:20 by vvaas            ###   ########.fr       */
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

			inline void login() { _logged = true; }
			inline void requireDisconnect() { _disconnect_required = true; }

			inline bool isLogged() const { return _logged; }
			inline bool disconnectRequired() const { return _disconnect_required; }

			inline const std::string& getNickName() const { return _nickname; }
			inline const std::string& getUserName() const { return _username; }
			inline const std::string& getRealName() const { return _realname; }

			void printUserHeader() const;

			inline void setFd(int new_fd) { _fd = new_fd; }

			void sendPlainText(const std::string& str);
			void sendCode(const std::string& code, const std::string& msg);
			void sendCode(const std::string& code, const std::string& msg0, const std::string& msg1);
			void sendMsg(const std::string& sender, const std::string& cmd, const std::string& trailing);
			void sendModular(std::string message, ...);

			~Client();

		private:
			std::string _nickname;
			std::string _username;
			std::string _realname;
			std::string _msg_in_flight;
			sockaddr_in _s_data;
			int _fd;
			int _id;
			bool _logged;
			bool _disconnect_required;
	};
}

#endif
