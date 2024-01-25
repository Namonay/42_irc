/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maldavid <kbz_8.dev@akel-engine.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/25 15:19:30 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/25 15:20:20 by maldavid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unstd/string.hpp>

namespace unstd
{
	std::vector<std::string> split(const std::string& s, char delim)
	{
		std::vector<std::string> elems;
		std::string token;
		for(std::string::const_iterator it = s.begin(); it != s.end();)
		{
			if(*it == delim)
			{
				elems.push_back(token);
				token.clear();
				while(*it == delim && it != s.end())
					it++;
			}
			else
			{
				token.push_back(*it);
				it++;
			}
		}
		elems.push_back(token);
		return elems;
	}
}
