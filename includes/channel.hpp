/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maldavid <kbz_8.dev@akel-engine.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 10:34:25 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/22 16:06:15 by maldavid         ###   ########.fr       */
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
			inline void addClient(unstd::SharedPtr<Client> client) { _clients.insert(client); }
			inline bool removeClient(unstd::SharedPtr<Client> client) { return _clients.erase(client); }
			inline std::size_t getNumberOfClients() const { return _clients.size(); }

			~Channel();

		private:
			std::set<unstd::SharedPtr<Client> > _clients;
			const std::string _name;
	};
}

#endif
