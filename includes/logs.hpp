/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logs.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 09:15:24 by maldavid          #+#    #+#             */
/*   Updated: 2024/02/06 12:35:38 by vvaas            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef __LOGS__
#define __LOGS__

#include <string>

namespace irc
{
	enum LogType
	{
		log_message = 0,
		log_warning,
		log_error,
		log_fatal_error
	};

	namespace logs
	{
		void report(LogType type, std::string message, ...); // message is not a const reference due to va_args
	}
}

#endif
