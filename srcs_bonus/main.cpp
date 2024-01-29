/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/29 20:36:23 by vvaas             #+#    #+#             */
/*   Updated: 2024/01/29 20:41:45 by vvaas            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include <irc.hpp>
#include <logs.hpp>

#define IP "127.0.0.1"
#define PORT 6667
int main()
{
	struct sockaddr_in serv_addr;
	int fd = socket(AF_LOCAL, SOCK_STREAM, 0);
	if (fd == -1)
		irc::logs::report(irc::log_fatal_error, "FD error");
	serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
	if (connect(fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
	{
        irc::logs::report(irc::log_fatal_error, "connect error");
        return -1;
    }
}