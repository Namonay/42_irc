/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 11:35:21 by maldavid          #+#    #+#             */
/*   Updated: 2024/02/06 12:35:38 by vvaas            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef __MESSAGE__
#define __MESSAGE__

#include <vector>
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
			inline const std::vector<std::string>& getArgs() const { return _args; }
			inline const std::string& getReason() const { return _reason; }
			inline const std::vector<std::string>& getTokens() const { return _tokens; }

			~Message();

		private:
			std::vector<std::string> _tokens;
			std::vector<std::string> _args;
			const std::string _raw_msg;
			std::string _command;
			std::string _reason;
			unstd::SharedPtr<class Client> _client;
	};
}

#endif
