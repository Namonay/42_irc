/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   weak_ptr.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maldavid <kbz_8.dev@akel-engine.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 11:17:07 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/20 13:16:14 by maldavid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __UNSTD_WEAK_PTR__
#define __UNSTD_WEAK_PTR__

#include <unstd/bits/ref_counter.h>
#include <unstd/shared_ptr.h>

namespace unstd
{
	template <typename T>
	class WeakPtr
	{
		public:
			WeakPtr();
			WeakPtr(const WeakPtr& rhs);
			WeakPtr(const SharedPtr<T>& shared);

			WeakPtr& operator=(const WeakPtr& rhs);
			WeakPtr& operator=(const SharedPtr<T>& shared);

			void reset();
			void swap(WeakPtr& rhs);
			bool expired() const;

			SharedPtr<T> lock() const;

			~WeakPtr();

		private:
			void safeRelease();

		private:
			bits::RefCount* _ref;
			T* _ptr;
	};
}

#include <unstd/weak_ptr.h>

#endif
