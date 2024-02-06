/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ansi.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 19:05:23 by maldavid          #+#    #+#             */
/*   Updated: 2024/02/06 12:35:38 by vvaas            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef __ANSI__
#define __ANSI__

#include <iostream>

namespace AnsiColor
{
	enum Codes
	{
		red = 31,
		green = 32,
		blue = 34,
		def = 0,
		black = 30,
		yellow = 33,
		magenta = 35,
		cyan = 36,
		white = 37,
		bg_red = 41,
		bg_green = 42,
		bg_blue = 44,
		bg_def = 0,
		bg_black = 40,
		bg_yellow = 43,
		bg_magenta = 45,
		bg_cyan = 46,
		bg_white = 47,
		reset = 0,
		bold = 1,
		underline = 4,
		inverse = 7,
		bold_off = 21,
		underline_off = 24,
		inverse_off = 27
	};
}

std::ostream& operator<<(std::ostream& os, const AnsiColor::Codes code);

#endif
