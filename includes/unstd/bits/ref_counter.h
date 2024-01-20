/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ref_counter.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maldavid <kbz_8.dev@akel-engine.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 12:54:09 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/20 12:55:02 by maldavid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
