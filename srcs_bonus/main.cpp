/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/29 20:36:23 by vvaas             #+#    #+#             */
/*   Updated: 2024/02/07 16:34:12 by maldavid         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include <Bot.hpp>
#include <logs.hpp>

int main(int argc, char **argv)
{
	if(argc != 4)
	{
		irc::logs::report(irc::log_error, "./greg <ip address> <port> <password>");
		return 1;
	}
	Bot greg;

	if(!greg.init(argv[1], argv[2], argv[3]))
		return 1;
	greg.connect_to_server();
	return 0;
}
