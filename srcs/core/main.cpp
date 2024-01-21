/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maldavid <kbz_8.dev@akel-engine.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 09:27:04 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/21 09:30:55 by maldavid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <logs.hpp>

int main(int ac, char** av)
{
	if(ac != 3)
	{
		irc::logs::report(irc::log_message, "usage './ircserv <port> <password>', try again dumbass");
		return 0;
	}
	(void)av;
	return 0;
}
