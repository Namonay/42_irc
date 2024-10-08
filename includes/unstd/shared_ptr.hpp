/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shared_ptr.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 10:09:02 by maldavid          #+#    #+#             */
/*   Updated: 2024/02/06 12:35:38 by vvaas            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef __UNSTD_SHARED_PTR__
#define __UNSTD_SHARED_PTR__

#include <unstd/bits/ref_counter.hpp>
#include <cstddef>

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
