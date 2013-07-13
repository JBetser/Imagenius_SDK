#include "IGSmartPtr.h"
#include <list>
#include <functional> 

namespace IGLibrary
{

template <typename T>
class IGObserverHandle
{
public:
	typedef tr1::function<void (T*)> const UnderlyingFunction;
	IGObserverHandle (UnderlyingFunction underlying);

	void operator() (T* data) const;	
	bool operator== (IGObserverHandle<T> const& other) const;

private: 
	IGSmartPtr<UnderlyingFunction> const m_underlying;
};
	
class IGBaseDelegate
{
public: 
	virtual bool operator==(IGBaseDelegate const& other);	
	virtual void operator() () const = 0;
};

template <typename T>
class IGDelegate : public IGBaseDelegate
{
public:
	IGDelegate (T* observer, IGObserverHandle<T> handle);

	virtual bool operator== (IGBaseDelegate const& other);	
	virtual void operator() () const;

private:
	T* m_observer;	
	IGObserverHandle<T> m_handle;
};

class IGEvent
{
public: 
	template <typename T>
	void add(T* observer, IGObserverHandle<T> handle);

	template <typename T>
	void remove (T* observer, IGObserverHandle<T> handle);
	
	void operator()() const;

private:
	typedef list<shared_ptr<IGBaseDelegate>> Observers;
	Observers m_observers;
	
	class Compare
	{
	public:
		Compare(IGBaseDelegate const& other);
		
		bool operator();
	
	private:
		IGBaseDelegate const& m_other;
	};
};

}
