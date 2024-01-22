/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 09:27:04 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/22 17:23:14 by vvaas            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include <logs.hpp>
#include <cstdlib>
#include <cctype>
#include <cstring>
#include <cerrno>
#include <server.hpp>
#include <signal.h>

static unstd::SharedPtr<irc::Server> server_ref;

void signalsHandler(int foo)
{
	(void)foo;
	if (!server_ref)
		return;
	server_ref->closeMainSocket();
}

int main(int ac, char** av)
{
	if(ac != 3)
	{
		irc::logs::report(irc::log_message, "usage './ircserv <port> <password>', try again dumbass");
		return 0;
	}
	if(av[1] == NULL || av[2] == NULL)
		irc::logs::report(irc::log_fatal_error, "invalid argv, argv[1] or argv[2] is NULL (wtf)");

	char* end;
	int port = std::strtol(av[1], &end, 10);
	if(errno == ERANGE || *end != 0 || port < 0 || port > 0xFFFF || std::strlen(av[1]) == 0)
		irc::logs::report(irc::log_fatal_error, "invalid port");

	unstd::SharedPtr<irc::Server> serv(new irc::Server(port, av[2]));
	server_ref = serv;
	signal(SIGINT, signalsHandler);
	signal(SIGQUIT, signalsHandler);
	serv->wait();
	serv->closeMainSocket();

	return 0;
}
