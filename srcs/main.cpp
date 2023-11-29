/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/24 10:40:01 by maldavid          #+#    #+#             */
/*   Updated: 2023/11/29 14:03:14 by vvaas            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/irc.hpp"

unsigned long get_port(char *port)
{
	if (!port)
		return (0);
	return (std::atoi(port));
}
int main(int argc, char** argv)
{
	int s_socket;
	struct sockaddr_in s_data;

	s_data.sin_family = AF_INET;
	s_data.sin_addr.s_addr = INADDR_ANY;
	s_data.sin_port = get_port(argv[1]);
	if (argc != 3)
		return (1);
	(void)argv;

	return 0;
}
