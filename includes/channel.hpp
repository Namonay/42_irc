/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 10:34:25 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/23 17:35:49 by maldavid         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef __CHANNEL__
#define __CHANNEL__

#include <string>
#include <set>
#include <unstd/shared_ptr.hpp>
#include <client.hpp>

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

			void addClient(unstd::SharedPtr<Client> client);
			bool removeClient(unstd::SharedPtr<Client> client);

			inline std::size_t getNumberOfClients() const { return _clients.size(); }

			inline void addOperator(unstd::SharedPtr<Client> op) { _operators.insert(op); }
			inline bool removeOperator(unstd::SharedPtr<Client> op) { return _operators.erase(op); }

			void handleMessage(const std::string& msg, unstd::SharedPtr<Client> client) const;

			inline bool isInviteOnly() const { return _invite_only; }

			void setTopic(unstd::SharedPtr<Client> client, const std::string& new_topic);

			~Channel();

		private:
			std::set<unstd::SharedPtr<Client>, ClientCmp> _clients;
			std::set<unstd::SharedPtr<Client>, ClientCmp> _operators;
			const std::string _name;
			std::string _password;
			std::string _topic;
			bool _invite_only;
			bool _topic_op_restrict;
	};
}

#endif
