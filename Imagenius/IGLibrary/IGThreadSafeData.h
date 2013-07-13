#pragma once

#include <windows.h>

namespace IGLibrary
{

template<class T>
class IGThreadSafeData
{
public:
	IGThreadSafeData()
	{
		::InitializeCriticalSection (&m_criticalSection);
	}
	IGThreadSafeData (T initValue) 
	{
		m_data = initValue;
		::InitializeCriticalSection (&m_criticalSection);
	}
	~IGThreadSafeData() 
	{
		::DeleteCriticalSection (&m_criticalSection);
	}

	IGThreadSafeData<T>& operator = (const T& data)
	{
		SetData (data);
		return *this;
	}

	IGThreadSafeData<T>& operator ++ () // prefix increment declared only
	{
		::EnterCriticalSection (&m_criticalSection);
		m_data++;
		::LeaveCriticalSection (&m_criticalSection);
		return *this;
	}

	IGThreadSafeData<T>& operator -- ()	// prefix decrement declared only
	{
		::EnterCriticalSection (&m_criticalSection);
		m_data--;
		::LeaveCriticalSection (&m_criticalSection);
		return *this;
	}

	IGThreadSafeData<T>& operator += (const T& data)
	{
		::EnterCriticalSection (&m_criticalSection);
		m_data += data;
		::LeaveCriticalSection (&m_criticalSection);
		return *this;
	}

	T& operator -= (const T& data)
	{
		::EnterCriticalSection (&m_criticalSection);
		m_data -= data;
		::LeaveCriticalSection (&m_criticalSection);
		return *this;
	}
	
	operator T() const
	{
		return GetData();
	}

	inline T& Lock ()
	{
		::EnterCriticalSection (&m_criticalSection);
		return m_data;
	}

	inline void UnLock ()
	{
		::LeaveCriticalSection (&m_criticalSection);
	}

protected:
	inline T GetData() const
	{
		T data;
		::EnterCriticalSection (&m_criticalSection);
		data = m_data;
		::LeaveCriticalSection (&m_criticalSection);
		return data;
	}

	inline void SetData (const T& data)
	{
		::EnterCriticalSection (&m_criticalSection);
		m_data = data;
		::LeaveCriticalSection (&m_criticalSection);
	}

private:
	IGThreadSafeData(const IGThreadSafeData&);	// declared but not implemented
	IGThreadSafeData operator = (const IGThreadSafeData&);	// declared but not implemented

	mutable CRITICAL_SECTION	m_criticalSection;
	T					m_data;
};

} // namespace IGLibrary