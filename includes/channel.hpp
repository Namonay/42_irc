/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maldavid <kbz_8.dev@akel-engine.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 10:34:25 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/22 20:05:52 by maldavid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CHANNEL__
#define __CHANNEL__

#include <string>
#include <set>
#include <unstd/shared_ptr.hpp>
#include <client.hpp>

namespace irc
{
	class Channel
	{
		public:
			Channel(const std::string& name);

			inline const std::string& getName() const { return _name; }
			inline const std::string& getPassword() const { return _password; }

			inline void addClient(unstd::SharedPtr<Client> client) { _clients.insert(client); }
			inline bool removeClient(unstd::SharedPtr<Client> client) { return _clients.erase(client); }
			inline std::size_t getNumberOfClients() const { return _clients.size(); }

			inline void addOperator(unstd::SharedPtr<Client> op) { _operators.insert(op); }
			inline bool removeOperator(unstd::SharedPtr<Client> op) { return _operators.erase(op); }

			inline bool isInviteOnly() const { return _invite_only; }

			void setTopic(unstd::SharedPtr<Client> client, const std::string& new_topic);

			~Channel();

		private:
			std::set<unstd::SharedPtr<Client> > _clients;
			std::set<unstd::SharedPtr<Client> > _operators;
			const std::string _name;
			std::string _password;
			std::string _topic;
			bool _invite_only;
			bool _topic_op_restrict;
	};
}

#endif
