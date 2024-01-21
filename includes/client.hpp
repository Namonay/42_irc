/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 10:33:17 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/21 16:32:05 by vvaas            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef __CLIENT__
#define __CLIENT__

namespace irc
{
	class Client
	{
		public:
			Client(int fd);

			~Client();

		private:
			int _fd;
	};
}

#endif
