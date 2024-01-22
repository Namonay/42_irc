/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 10:33:17 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/22 01:29:55 by vvaas            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef __CLIENT__
#define __CLIENT__
#include <irc.hpp>
#include <string>
#include <sstream>
namespace irc
{
	class Client
	{
		public:
			Client(int fd, sockaddr_in sock, int id);
			inline int get_fd(void) { return (_fd); }
			inline void set_fd(int new_fd) { _fd = new_fd; }
			inline sockaddr_in  get_sockaddr(void) { return (_s_data); }
			inline std::string get_string_id(void) { std::stringstream out; out << _id; return (out.str()); }
			~Client();

		private:
			sockaddr_in _s_data;
			int _fd;
			int _id;
	};
}

#endif
