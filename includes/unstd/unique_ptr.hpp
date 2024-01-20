/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unique_ptr.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maldavid <kbz_8.dev@akel-engine.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 09:40:56 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/20 19:12:51 by maldavid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __UNSTD_UNIQUE_PTR__
#define __UNSTD_UNIQUE_PTR__

#include <cstddef>

namespace unstd
{
	template <typename T>
	class UniquePtr
	{
		public:
			UniquePtr();
			UniquePtr(UniquePtr& ptr);
			explicit UniquePtr(T* ptr);

			inline T* get();

			inline void swap(const UniquePtr<T>& ptr);

			inline void reset(T* ptr = NULL);
			inline T* release();

			inline operator bool() const;

			inline UniquePtr& operator=(T* ptr);

			inline T* operator->() const;
			inline T& operator*() const;

			~UniquePtr();

		private:
			UniquePtr(const UniquePtr& ptr);
			inline UniquePtr& operator=(const UniquePtr& ptr);

		private:
			T* _ptr;
	};
}

#include <unstd/unique_ptr.ipp>

#endif
