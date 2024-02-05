/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 11:38:34 by maldavid          #+#    #+#             */
/*   Updated: 2024/02/05 16:23:31 by maldavid         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

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
			for(std::vector<std::string>::iterator vit = elems.begin(); vit != elems.end();)
			{
				bool isempty = true;
				for(std::string::const_iterator it = vit->begin(); it != vit->end(); ++it)
				{
					if(!std::isspace(*it))
					{
						isempty = false;
						break;
					}
				}
				if(isempty)
					vit = elems.erase(vit);
				else
					vit++;
			}
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
				if(it != tokens.end() - 1)
					_reason.push_back(' ');
			}
		}
	}

	Message::~Message() {}
}
