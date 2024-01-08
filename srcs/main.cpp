/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/24 10:40:01 by maldavid          #+#    #+#             */
/*   Updated: 2023/11/29 16:43:45 by vvaas            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// t'es mauvais

#include "../includes/irc.hpp"

unsigned long get_port(char *port)
{
	if (!port)
		return (0);
	if (std::atoi(port) < 1)
		std::terminate();
	return (htons(std::atoi(port)));
}

void init_socket(sockaddr_in &s_data, int &s_socket, char *port)
{
	s_data.sin_family = AF_INET;
	s_data.sin_addr.s_addr = INADDR_ANY;
	s_data.sin_port = get_port(port);
	s_socket = socket(PF_INET, SOCK_STREAM, 0);
	if (s_socket < 0)
	{
		std::cerr << "Fatal Error : socket error" << std::endl;
		std::terminate();
	}
	std::cout << "Socket creating succesful" << std::endl;
	if (bind(s_socket, (struct sockaddr *)&s_data, sizeof(sockaddr)) != 0)
	{
		std::cerr << "Fatal Error : bind error" << std::endl;
		std::terminate();
	}
	std::cout << "bind succesful, starting listen loop" << std::endl;
	if (listen(s_socket, 20) != 0)
	{
		std::cerr << "Fatal Error : listen error" << std::endl;
		std::terminate();
	}
	std::cout << "listen queue created succesful" << std::endl;
}

void write_info(sockaddr_in s_data, int s_socket)
{
	std::cout << "My port is : " << ntohl(s_data.sin_port) << std::endl;
	std::cout << "I created a socket, n`" << s_socket << std::endl;
}
int main(int argc, char** argv)
{
	int s_socket;
	struct sockaddr_in s_data;

	if (argc != 3)
	{
		std::cerr << "Fatal Error : invalid arguments :" << std::endl << "USAGE : ./ircserv <port> <password>";
		return (1);
	}
	init_socket(s_data, s_socket, argv[1]);
	write_info(s_data, s_socket);
	while (1)
	{
		struct sockaddr_in c_addr;
        socklen_t length = sizeof(c_addr);
		if (accept(s_socket, (sockaddr *)&c_addr, &length) < 0)
		{
			std::cerr << "Connection error ?" << std::endl;
			std::terminate();
		}
	}
	close(s_socket);
	return 0;
}
