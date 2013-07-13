#include "StdAfx.h"
#include "IGRefCount.h"

using namespace IGLibrary;

// the class reference counted

IGRefCount::IGRefCount() : use_count_( 1 )
						 , weak_count_( 1 )
{
}						    

IGRefCount::~IGRefCount()
{
	_ASSERTE (weak_count_==0 && L"Ref count reaches -1");
}

void IGRefCount::destroy() // nothrow
{
    delete this;
}

void IGRefCount::add_ref_copy()
{
    ::InterlockedIncrement (&use_count_);
}

bool IGRefCount::add_ref_lock() // true on success
{
    for( ;; )
    {
        long tmp = static_cast< long const volatile& > (use_count_);
        if( tmp == 0 ) return false;

		if( ::InterlockedCompareExchange (&use_count_, tmp + 1, tmp) == tmp )
			return true;
    }
}

void IGRefCount::release() // nothrow
{
    if (::InterlockedDecrement (&use_count_) == 0 )
    {
        dispose();
        weak_release();
    }
}

void IGRefCount::weak_add_ref() // nothrow
{
    ::InterlockedIncrement (&weak_count_);
}

void IGRefCount::weak_release() // nothrow
{
    if( ::InterlockedDecrement (&weak_count_) == 0 )
    {
        destroy();
    }
}

long IGRefCount::use_count() const // nothrow
{
    return static_cast<long const volatile &> (use_count_);
}