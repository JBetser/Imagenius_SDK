#include "stdafx.h"
#include "IGObserver.h"
#include <iostream>
#include <algorithm>

using namespace std;
using namespace std::tr1;
using namespace IGLibrary;

IGObserverHandle::IGObserverHandle (UnderlyingFunction underlying) : m_underlying (new UnderlyingFunction (underlying)) 
{
} 

void IGObserverHandle::operator() (T* data) const 
{
	(*m_underlying)(data);
} 
	
bool IGObserverHandle::operator== (IGObserverHandle<T> const& other) const
{
	return (other.m_underlying == m_underlying);
}
	
bool IGBaseDelegate::operator==(IGBaseDelegate const& other)
{
	return false;
}

IGDelegate::IGDelegate (T* observer, IGObserverHandle<T> handle) : m_observer(observer)
													 , m_handle(handle)
{
}
	
bool IGDelegate::operator== (IGBaseDelegate const& other)
{
	IGBaseDelegate const * otherPtr = &other;
	IGDelegate<T> const * otherDT = dynamic_cast<IGDelegate<T> const *>(otherPtr); 
	return ((otherDT) &&
			(otherDT->m_observer == m_observer) &&
			(otherDT->m_handle == m_handle));
}
	
void IGDelegate::operator() () const
{
	m_handle(m_observer);
}

void IGEvent::add(T* observer, IGObserverHandle<T> handle)
{
	m_observers.push_back (shared_ptr<IGBaseDelegate> (new IGDelegate<T>(observer, handle)));
}
	
void IGEvent::remove (T* observer, IGObserverHandle<T> handle)
{
	// I should be able to come up with a bind2nd(equals(dereference(_1))) kind of thing, but I can't figure it out now
	Observers::iterator it = find_if(	m_observers.begin(),
										m_observers.end(),
										Compare(IGDelegate<T>(observer, handle)));
	if (it != m_observers.end())
	{
		m_observers.erase(it);
	}
}

void IGEvent::operator()() const
{
	for (Observers::const_iterator it = m_observers.begin();
									it != m_observers.end(); 
									++it)
	{ 
		(*(*it))();
	}
}
	
IGEvent::Compare::Compare(IGBaseDelegate const& other) : m_other(other)
{
}
		
bool IGEvent::Compare::operator() (shared_ptr<IGBaseDelegate> const& other) const
{
	return (*other) == m_other;
}