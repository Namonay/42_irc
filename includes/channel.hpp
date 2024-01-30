/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 10:34:25 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/30 20:54:21 by maldavid         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef __CHANNEL__
#define __CHANNEL__

#include <string>
#include <set>
#include <unstd/shared_ptr.hpp>
#include <client.hpp>
#include <message.hpp>

namespace irc
{
	struct ClientCmp
	{
		bool operator()(const unstd::SharedPtr<Client>& lhs, const unstd::SharedPtr<Client>& rhs) const
		{
			return lhs.get() < rhs.get();
		}
	};

	class Channel
	{
		public:
			Channel(const std::string& name);

			inline const std::string& getName() const { return _name; }
			inline const std::string& getPassword() const { return _password; }

			void addClient(unstd::SharedPtr<Client> client, bool op = false);
			bool removeClient(unstd::SharedPtr<Client> client, const std::string& reason, bool quit = false);

			inline std::size_t getNumberOfClients() const { return _clients.size(); }
			inline int getChannelSize() const { return _channel_size; }
			void modOperator(unstd::SharedPtr<class Client> client, const std::string &clientname, bool mode);
			inline bool removeOperator(unstd::SharedPtr<Client> op) { return _operators.erase(op); }
			void changeMode(unstd::SharedPtr<class Client> client, const Message& msg);
			bool isOp(unstd::SharedPtr<Client> client) const;
			void handleMessage(const std::string& msg, unstd::SharedPtr<Client> client, bool notice = false) const;
			void sendWho(unstd::SharedPtr<Client> client);
			inline bool hasClient(unstd::SharedPtr<Client> client) const { return _clients.find(client) != _clients.end(); }
			bool hasClient(std::string client) const;

			inline bool isInviteOnly() const { return _invite_only; }

			void setTopic(unstd::SharedPtr<Client> client, const std::string& new_topic);
			void relayTopic(unstd::SharedPtr<Client> client, bool broadcast = false);
			void showModesModify(unstd::SharedPtr<Client> client, const Message& msg) const;
			void showModes(unstd::SharedPtr<Client> client);
			bool kick(unstd::SharedPtr<Client> op, unstd::SharedPtr<Client> target, const std::string& reason);

			~Channel();

		private:
			std::set<unstd::SharedPtr<Client>, ClientCmp> _clients;
			std::set<unstd::SharedPtr<Client>, ClientCmp> _operators;
			std::string _name;
			std::string _password;
			std::string _topic;
			int  _channel_size;
			bool _invite_only;
			bool _topic_op_restrict;
	};
}

#endif
