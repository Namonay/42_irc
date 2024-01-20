/* **************************************************************************** */
/*                                                                              */
/*                                                         :::      ::::::::    */
/*    unique_ptr.tpp                                     :+:      :+:    :+:    */
/*                                                     +:+ +:+         +:+      */
/*    By: maldavid <kbz_8.dev@akel-engine.com>       +#+  +:+       +#+         */
/*                                                 +#+#+#+#+#+   +#+            */
/*    Created: 2024/01/20 09:53:45 by maldavid          #+#    #+#              */
/*    Updated: 2024/01/20 09:53:45 by maldavid         ###   ########.fr        */
/*                                                                              */
/* **************************************************************************** */

#include <cassert>

namespace unstd
{
	template <typename T>
	UniquePtr<T>::UniquePtr() : _ptr(NULL) {}
	
	template <typename T>
	UniquePtr<T>::UniquePtr(T* ptr) : _ptr(ptr) {}
	
	template <typename T>
	UniquePtr<T>::UniquePtr(UniquePtr<T>& rhs) : _ptr(rhs._ptr)
	{
		rhs._ptr = NULL;
	}

	template <typename T>
	T* UniquePtr<T>::get()
	{
		return _ptr;
	}

	template <typename T>
	void UniquePtr<T>::swap(const UniquePtr<T>& ptr)
	{
		T* temp = _ptr;
		_ptr = ptr._ptr;
		ptr._ptr = temp;
	}

	template <typename T>
	void UniquePtr<T>::reset(T* ptr)
	{
		if(_ptr != NULL)
			delete _ptr;
		_ptr = ptr;
	}

	template <typename T>
	T* UniquePtr<T>::release()
	{
		T* temp = _ptr;
		_ptr = NULL;
		return temp;
	}

	template <typename T>
	UniquePtr<T>::operator bool() const
	{
		return _ptr != NULL;
	}

	template <typename T>
	UniquePtr<T>& UniquePtr<T>::operator=(T* ptr)
	{
		assert(_ptr == NULL && "UniquePtr assertion failed : `operator=`, unique ptr already holds a pointer");
		reset(ptr);
	}

	template <typename T>
	T* UniquePtr<T>::operator->() const
	{
		return _ptr;
	}

	template <typename T>
	T& UniquePtr<T>::operator*() const
	{
		return *_ptr;
	}

	template <typename T>
	UniquePtr<T>::~UniquePtr()
	{
		if(_ptr != NULL)
			delete _ptr;
	}

	template <typename T>
	UniquePtr<T>::UniquePtr(const UniquePtr<T>& ptr)
	{
		(void)ptr;
	}

	template <typename T>
	UniquePtr<T>& UniquePtr<T>::operator=(const UniquePtr& ptr)
	{
		(void)ptr;
		return *this;
	}
}
