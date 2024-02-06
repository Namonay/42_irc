/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ref_counter.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 12:54:09 by maldavid          #+#    #+#             */
/*   Updated: 2024/02/06 12:35:38 by vvaas            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef __UNSTD_BITS_REFERENCE_COUNTER__
#define __UNSTD_BITS_REFERENCE_COUNTER__

namespace unstd
{
	namespace bits
	{
		struct RefCount
		{
			int shared;
			int weak;

			RefCount(int s, int w) : shared(s), weak(w) {}
		};
	}
}

#endif
