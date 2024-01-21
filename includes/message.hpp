/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maldavid <kbz_8.dev@akel-engine.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 11:35:21 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/21 11:39:56 by maldavid         ###   ########.fr       */
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

			~Message();

		private:
			const std::string _raw_msg;
			std::string _command;
			unstd::SharedPtr<class Client> _client;
	};
}

#endif
