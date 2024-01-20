/* **************************************************************************** */
/*                                                                              */
/*                                                         :::      ::::::::    */
/*    shared_ptr.ipp                                     :+:      :+:    :+:    */
/*                                                     +:+ +:+         +:+      */
/*    By: maldavid <kbz_8.dev@akel-engine.com>       +#+  +:+       +#+         */
/*                                                 +#+#+#+#+#+   +#+            */
/*    Created: 2024/01/20 10:17:18 by maldavid          #+#    #+#              */
/*    Updated: 2024/01/20 10:17:18 by maldavid         ###   ########.fr        */
/*                                                                              */
/* **************************************************************************** */

namespace unstd
{
	template <typename T>
	SharedPtr<T>::SharedPtr(T* ptr) : _ptr(ptr)
	{
		typename std::map<void*, bits::RefCount*>::iterator it = _refs.find(static_cast<void*>(ptr));
		if(it == _refs.end())
		{
			_ref = new bits::RefCount(1, 0);
			_refs[static_cast<void*>(ptr)] = _ref;
		}
		else
		{
			_ref = it->second;
			_ref->shared++;
		}
	}

	template <typename T>
	SharedPtr<T>::SharedPtr(const SharedPtr<T>& rhs) : _ptr(rhs._ptr), _ref(rhs._ref)
	{
		if(_ptr && _ref)
			++_ref->shared;
	}

	template <typename T>
	SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr<T>& rhs)
	{
		if(_ptr == rhs._ptr)
			return *this;

		safeRelease();
		_ptr = rhs._ptr;
		_ref = rhs._ref;
		_ref->shared++;
		return *this;
	}

	template <typename T>
	SharedPtr<T>::operator bool() const
	{
		return _ptr != NULL;
	}

	template <typename T>
	bool SharedPtr<T>::operator==(const SharedPtr<T>& rhs)
	{
		return _ptr == rhs._ptr;
	}

	template <typename T>
	bool SharedPtr<T>::operator==(T* ptr)
	{
		return _ptr == ptr;
	}

	template <typename T>
	T& SharedPtr<T>::operator*()
	{
		return *_ptr;
	}

	template <typename T>
	T* SharedPtr<T>::operator->()
	{
		return _ptr;
	}

	template <typename T>
	T* SharedPtr<T>::get()
	{
		return _ptr;
	}

	template <typename T>
	void SharedPtr<T>::swap(SharedPtr<T>& rhs)
	{
		T* tmptr = _ptr;
		bits::RefCount* tmpref = _ref;
		_ptr = rhs._ptr;
		_ref = rhs._ref;
		rhs._ptr = tmptr;
		rhs._ref = tmpref;
	}

	template <typename T>
	void SharedPtr<T>::reset(T* ptr)
	{
		safeRelease();
		_ptr = ptr;
		typename std::map<void*, bits::RefCount*>::iterator it = _refs.find(static_cast<void*>(ptr));
		if(it == _refs.end())
		{
			_ref = new bits::RefCount(1, 0);
			_refs[static_cast<void*>(ptr)] = _ref;
		}
		else
		{
			_ref = it->second;
			_ref->shared++;
		}
	}

	template <typename T>
	void SharedPtr<T>::safeRelease()
	{
		if(_ref == NULL)
			return;
		_ref->shared--;
		if(_ref->shared <= 0)
		{
			delete _ptr;
			_ptr = NULL;
			delete _ref;
			_ref = NULL;
		}
	}

	template <typename T>
	SharedPtr<T>::~SharedPtr()
	{
		safeRelease();
	}
}
