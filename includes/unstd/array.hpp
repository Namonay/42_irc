/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   array.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 09:31:30 by maldavid          #+#    #+#             */
/*   Updated: 2024/02/06 12:36:19 by vvaas            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef __UNSTD_ARRAY__
#define __UNSTD_ARRAY__

#include <cstddef>
#include <cassert>

namespace unstd
{
	template <typename T, std::size_t N>
	class TArray
	{
		public:
			TArray() {}
			TArray(const TArray<T, N>& rhs)
			{
				for(std::size_t i = 0; i < N; i++)
					_data[i] = rhs._data[i];
			}

			inline TArray& operator=(const TArray<T, N>& rhs)
			{
				for(std::size_t i = 0; i < N; i++)
					_data[i] = rhs._data[i];
				return *this;
			}

			inline bool empty() const
			{
				return N == 0;
			}

			inline std::size_t size() const
			{
				return N;
			}

			inline std::size_t max_size() const
			{
				return N;
			}

			inline T* data()
			{
				return _data;
			}

			inline T& at(std::size_t pos)
			{
				assert(pos < N && "TArray assertion failed : index out of range");
				return _data[pos];
			}

			inline T& operator[](std::size_t pos)
			{
				return _data[pos];
			}

			~TArray() {}

		private:
			T _data[N];
	};
}

#endif
