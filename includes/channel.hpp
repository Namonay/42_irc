/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maldavid <kbz_8.dev@akel-engine.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 10:34:25 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/22 14:41:25 by maldavid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CHANNEL__
#define __CHANNEL__

#include <string>

namespace irc
{
	class Channel
	{
		public:
			Channel(const std::string& name);

			inline const std::string& getName() const { return _name; }

			~Channel();

		private:
			const std::string _name;
	};
}

#endif
