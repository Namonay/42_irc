/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maldavid <kbz_8.dev@akel-engine.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 11:38:34 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/25 23:34:47 by maldavid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <message.hpp>
#include <client.hpp>
#include <vector>
#include <cctype>

namespace irc
{
	namespace details
	{
		void split(const std::string& s, std::vector<std::string>& elems)
		{
			std::string token;
			for(std::string::const_iterator it = s.begin(); it != s.end();)
			{
				if(std::isspace(*it))
				{
					elems.push_back(token);
					token.clear();
					while(std::isspace(*it) && it != s.end())
						it++;
				}
				else
				{
					token.push_back(*it);
					it++;
				}
			}
			elems.push_back(token);
		}

		std::vector<std::string> split(const std::string& s)
		{
			std::vector<std::string> elems;
			split(s, elems);
			return elems;
		}
	}

	Message::Message(unstd::SharedPtr<class Client> client, const std::string& msg) : _raw_msg(msg), _client(client)
	{
		std::vector<std::string> tokens = details::split(msg);
		if(tokens.empty())
			return;
		_command = tokens[0];
		_tokens = tokens;
		bool reason = false;
		for(std::vector<std::string>::iterator it = tokens.begin() + 1; it < tokens.end(); ++it)
		{
			if((*it)[0] == ':')
				reason = true;
			if(!reason)
				_args.push_back(*it);
			else
			{
				if((*it)[0] == ':')
					it->erase(it->begin());
				_reason.append(*it);
				_reason.push_back(' ');
			}
		}
	}

	Message::~Message() {}
}
