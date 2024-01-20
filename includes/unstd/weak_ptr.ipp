/* **************************************************************************** */
/*                                                                              */
/*                                                         :::      ::::::::    */
/*    weak_ptr.ipp                                       :+:      :+:    :+:    */
/*                                                     +:+ +:+         +:+      */
/*    By: maldavid <kbz_8.dev@akel-engine.com>       +#+  +:+       +#+         */
/*                                                 +#+#+#+#+#+   +#+            */
/*    Created: 2024/01/20 12:58:44 by maldavid          #+#    #+#              */
/*    Updated: 2024/01/20 12:58:44 by maldavid         ###   ########.fr        */
/*                                                                              */
/* **************************************************************************** */

namespace unstd
{
	template <typename T>
	WeakPtr<T>::WeakPtr() : _ref(NULL), _ptr(NULL) {}
	
	template <typename T>
	WeakPtr<T>::WeakPtr(const WeakPtr<T>& rhs) : _ref(rhs._ref), _ptr(rhs._ptr)
	{
		if(_ptr && _ref)
			_ref->weak++;
	}

	template <typename T>
	WeakPtr<T>::WeakPtr(const SharedPtr<T>& shared) : _ref(shared._ref), _ptr(shared._ptr)
	{
		if(_ptr && _ref)
			_ref->weak++;
	}

	template <typename T>
	WeakPtr<T>& WeakPtr<T>::operator=(const WeakPtr<T>& rhs)
	{
		if(_ptr == rhs._ptr)
			return *this;

		safeRelease();
		_ptr = rhs._ptr;
		_ref = rhs._ref;
		return *this;
	}

	template <typename T>
	WeakPtr<T>& WeakPtr<T>::operator=(const SharedPtr<T>& shared)
	{
		if(_ptr == shared._ptr)
			return *this;

		safeRelease();
		_ptr = shared._ptr;
		_ref = shared._ref;
		_ref->weak++;
		return *this;
	}

	template <typename T>
	void WeakPtr<T>::reset()
	{
		safeRelease();
		_ptr = NULL;
		_ref = NULL;
	}

	template <typename T>
	void WeakPtr<T>::swap(WeakPtr<T>& rhs)
	{
		T* tmptr = _ptr;
		bits::RefCount* tmpref = _ref;
		_ptr = rhs._ptr;
		_ref = rhs._ref;
		rhs._ptr = tmptr;
		rhs._ref = tmpref;
	}

	template <typename T>
	bool WeakPtr<T>::expired() const
	{
		return (_ptr && _ref && _ref->shared > 0);
	}

	template <typename T>
	SharedPtr<T> WeakPtr<T>::lock() const
	{
		return (expired() ? SharedPtr<T>() : SharedPtr<T>(_ptr));
	}

	template <typename T>
	void WeakPtr<T>::safeRelease()
	{
		if(_ref == NULL)
			return;
		_ref->weak--;
		if(_ref && _ref->shared <= 0)
		{
			delete _ptr;
			_ptr = NULL;
			delete _ref;
			_ref = NULL;
		}
	}

	template <typename T>
	WeakPtr<T>::~WeakPtr()
	{
		if(_ptr && _ref && _ref->shared > 0)
			safeRelease();
	}
}
