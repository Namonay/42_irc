/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maldavid <kbz_8.dev@akel-engine.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 19:13:26 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/20 19:14:55 by maldavid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __UNSTD_STRING__
#define __UNSTD_STRING__

#include <sstream>

namespace unstd
{
	template <typename T>
	std::string toString(T n)
	{
		std::ostringstream ss;
		ss << n;
		return ss.str();
	}
}

#endif
