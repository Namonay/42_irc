/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 09:27:04 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/30 17:35:22 by maldavid         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include <logs.hpp>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <server.hpp>
#include <signal.h>
#include <iostream>

static irc::Server* serv_ptr = NULL;

void signalsHandler(int foo)
{
	(void)foo;
	if (!serv_ptr)
		return ;
	serv_ptr->closeMainSocket();
	std::cout << "\b\b  \b\b" << std::flush;
	irc::logs::report(irc::log_message, "Shutting down...");
}

int main(int ac, char** av)
{
	if (ac != 3)
	{
		irc::logs::report(irc::log_message, "usage './ircserv <port> <password>', try again dumbass");
		return 0;
	}
	if (av[1] == NULL || av[2] == NULL)
		irc::logs::report(irc::log_fatal_error, "invalid argv, argv[1] or argv[2] is NULL (wtf)");

	char* end;
	int port = std::strtol(av[1], &end, 10);
	if (errno == ERANGE || *end != 0 || port < 0 || port > 0xFFFF || std::strlen(av[1]) == 0)
		irc::logs::report(irc::log_fatal_error, "invalid port");

	irc::Server serv(port, av[2]);
	serv_ptr = &serv;
	signal(SIGINT, signalsHandler);
	signal(SIGQUIT, signalsHandler);
	serv.wait();
	serv.closeMainSocket();
	irc::logs::report(irc::log_message, "Server has been closed");
	return 0;
}
