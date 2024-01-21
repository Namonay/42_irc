/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maldavid <kbz_8.dev@akel-engine.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 11:35:21 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/21 12:04:43 by maldavid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __MESSAGE__
#define __MESSAGE__

#include <string>
#include <unstd/shared_ptr.hpp>

namespace irc
{
	class Message
	{
		public:
			Message(unstd::SharedPtr<class Client> client, const std::string& msg);

			inline unstd::SharedPtr<class Client> getClient() const { return _client; }
			inline const std::string& getRawMsg() const { return _raw_msg; }
			inline const std::string& getCmd() const { return _command; }

			~Message();

		private:
			const std::string _raw_msg;
			std::string _command;
			unstd::SharedPtr<class Client> _client;
	};
}

#endif
