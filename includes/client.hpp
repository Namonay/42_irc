/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 10:33:17 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/30 17:06:21 by vvaas            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef __CLIENT__
#define __CLIENT__

#include <irc.hpp>
#include <string>
#include <set>
#include <errorscode.hpp>
#include <logs.hpp>

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
			inline void register_user() { _registered = true; }
			inline void welcome() { if (!isLogged() || isWelcomed() || _nickname.empty()) return ; _welcomed = true; sendCode(RPL_WELCOME, "Welcome to yipirc :), " + _nickname); }
			inline void requireDisconnect() { _disconnect_required = true; }

			inline bool isLogged() const { return _logged; }
			inline bool isRegistered() const { return _registered; }
			inline bool isWelcomed() const { return _welcomed; }
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
			void sendCodeInChannel(const std::string& code, const class Channel &chan, const std::string& msg);
			void kill(const std::string& reason);
			inline void inviteToChannel(const std::string& channel) { _invites.insert(channel); }
			inline bool hasBeenInvitedTo(const std::string& channel) { return _invites.erase(channel); }

			~Client();

		private:
			std::set<std::string> _invites;
			std::string _nickname;
			std::string _username;
			std::string _realname;
			std::string _msg_in_flight;
			sockaddr_in _s_data;
			int _fd;
			int _id;
			bool _welcomed;
			bool _logged;
			bool _registered;
			bool _disconnect_required;
	};
}

#endif
