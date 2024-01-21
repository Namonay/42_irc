/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ansi.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maldavid <kbz_8.dev@akel-engine.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 19:08:31 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/20 19:18:41 by maldavid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ansi.hpp>
#include <unstd/string.hpp>

std::ostream& operator<<(std::ostream& os, const AnsiColor::Codes code)
{
	return os << "\033[1;" << unstd::toString(static_cast<int>(code)) << "m";
}
