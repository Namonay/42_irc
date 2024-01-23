/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shared_ptr.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maldavid <kbz_8.dev@akel-engine.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 10:09:02 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/23 17:35:24 by maldavid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __UNSTD_SHARED_PTR__
#define __UNSTD_SHARED_PTR__

#include <unstd/bits/ref_counter.hpp>
#include <cstddef>
#include <map>

namespace unstd
{
	template <typename T>
	class SharedPtr
	{
		template <typename WT>
		friend class WeakPtr;

		public:
			explicit SharedPtr(T* ptr = NULL);
			SharedPtr(const SharedPtr& rhs);
			SharedPtr& operator=(const SharedPtr& rhs);
			operator bool() const;
			bool operator==(const SharedPtr& rhs);
			bool operator==(T* ptr);
			T& operator*();
			T* operator->();
			T* get() const;
			void swap(SharedPtr& rhs);
			void reset(T* ptr = NULL);
			~SharedPtr();

		private:
			void safeRelease();

		private:
			T* _ptr;
			bits::RefCount* _ref;
	};
}

#include <unstd/shared_ptr.ipp>

#endif
