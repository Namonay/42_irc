/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 19:13:26 by maldavid          #+#    #+#             */
/*   Updated: 2024/02/06 12:35:38 by vvaas            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef __UNSTD_STRING__
#define __UNSTD_STRING__

#include <sstream>
#include <string>
#include <vector>

namespace unstd
{
	template <typename T>
	std::string toString(T n)
	{
		std::ostringstream ss;
		ss << n;
		return ss.str();
	}

	std::vector<std::string> split(const std::string& s, char delim);
}

#endif
