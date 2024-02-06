/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/25 15:19:30 by maldavid          #+#    #+#             */
/*   Updated: 2024/02/06 12:36:19 by vvaas            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include <unstd/string.hpp>

namespace unstd
{
	std::vector<std::string> split(const std::string& s, char delim)
	{
		std::vector<std::string> elems;
		std::string token;
		for (std::string::const_iterator it = s.begin(); it != s.end();)
		{
			if (*it == delim)
			{
				elems.push_back(token);
				token.clear();
				while (*it == delim && it != s.end())
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
