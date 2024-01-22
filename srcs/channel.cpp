/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maldavid <kbz_8.dev@akel-engine.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 10:36:21 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/22 14:29:55 by maldavid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <channel.hpp>

namespace irc
{
	Channel::Channel(const std::string& name) : _name(name)
	{

	}

	Channel::~Channel() {}
}
