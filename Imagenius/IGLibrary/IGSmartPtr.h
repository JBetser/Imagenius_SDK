#pragma once
#include "IGRefCount.h"
#include <string>
#include <functional>           // for std::less

using namespace std;

namespace IGLibrary
{

enum memory_order
{
    memory_order_relaxed = 0,
    memory_order_acquire = 1,
    memory_order_release = 2,
    memory_order_acq_rel = 3, // acquire | release
    memory_order_seq_cst = 7  // acq_rel | 4
};

namespace IGDetail
{

class IGSharedCount
{
private:
    IGRefCount * pi_;

public:
    IGSharedCount();
	~IGSharedCount();
    IGSharedCount(IGSharedCount const & r);
	IGSharedCount & operator= (IGSharedCount const & r);

    template<class Y> explicit IGSharedCount( Y * p ): pi_( 0 )
    {
        pi_ = new IGRefCountSingleImpl<Y>( p );

        if( pi_ == 0 )
        {
            checked_delete( p );
        }
    }
/*
    template<class P, class D> IGSharedCount( P p, D d ): pi_(0)
    {
        pi_ = new IGRefCountDoubleImpl<P, D>(p, d);

        if(pi_ == 0)
        {
            d(p); // delete p
        }
    }

    template<class P, class D, class A> IGSharedCount( P p, D d, A a ): pi_( 0 )
    {
        typedef IGRefCountTripleImpl<P, D, A> impl_type;
        typedef typename A::template rebind< impl_type >::other A2;
        A2 a2( a );
        pi_ = a2.allocate( 1, static_cast< impl_type* >( 0 ) );
        if( pi_ != 0 )
        {
            new( static_cast< void* >( pi_ ) ) impl_type( p, d, a );
        }
        else
        {
            d( p );
        }
	}*/

    void swap(IGSharedCount & r);
    long use_count() const;
    bool unique() const;
    bool empty() const;
	void * get_deleter( type_info const & ti ) const;

    friend inline bool operator==(IGSharedCount const & a, IGSharedCount const & b)
    {
        return a.pi_ == b.pi_;
    }

    friend inline bool operator<(IGSharedCount const & a, IGSharedCount const & b)
    {
        return std::less<IGRefCount *>()( a.pi_, b.pi_ );
    }    
};

struct static_cast_tag {};
struct const_cast_tag {};
struct dynamic_cast_tag {};
struct polymorphic_cast_tag {};

template<class T> struct shared_ptr_traits
{
    typedef T & reference;
};

template<> struct shared_ptr_traits<void>
{
    typedef void reference;
};

template<> struct shared_ptr_traits<void const>
{
    typedef void reference;
};

template<> struct shared_ptr_traits<void volatile>
{
    typedef void reference;
};

template<> struct shared_ptr_traits<void const volatile>
{
    typedef void reference;
};

} // namespace IGDetail


//
//  IGSmartPtr
//
//  An enhanced relative of scoped_ptr with reference counted copy semantics.
//  The object pointed to is deleted when the last IGSmartPtr pointing to it
//  is destroyed or reset.
//
template<class T> class IGSmartPtr
{
private:
	typedef IGSmartPtr<T> this_type;

public:
    typedef T element_type;
    typedef T value_type;
    typedef T * pointer;
    typedef typename IGDetail::shared_ptr_traits<T>::reference reference;

    IGSmartPtr(): px(0), pn() // never throws in 1.30+
    {
    }

    template<class Y>
    explicit IGSmartPtr( Y * p ): px( p ), pn( p ) // Y must be complete
    {
    }

    // IGSmartPtr will release p by calling d(p)
    //
    template<class Y, class D> IGSmartPtr(Y * p, D d): px(p), pn(p, d)
    {
    }

    // As above, but with allocator. A's copy constructor shall not throw.
    template<class Y, class D, class A> IGSmartPtr( Y * p, D d, A a ): px( p ), pn( p, d, a )
    {
    }

    template<class Y>
    IGSmartPtr( IGSmartPtr<Y> const & r ) : px( r.px ), pn( r.pn ) // never throws
    {
    }

    // aliasing
    template< class Y >
    IGSmartPtr( IGSmartPtr<Y> const & r, T * p ): px( p ), pn( r.pn ) // never throws
    {
    }

    template<class Y>
    IGSmartPtr(IGSmartPtr<Y> const & r, IGDetail::static_cast_tag): px(static_cast<element_type *>(r.px)), pn(r.pn)
    {
    }

    template<class Y>
    IGSmartPtr(IGSmartPtr<Y> const & r, IGDetail::const_cast_tag): px(const_cast<element_type *>(r.px)), pn(r.pn)
    {
    }

    template<class Y>
    IGSmartPtr(IGSmartPtr<Y> const & r, IGDetail::dynamic_cast_tag): px(dynamic_cast<element_type *>(r.px)), pn(r.pn)
    {
        if(px == 0) // need to allocate new counter -- the cast failed
        {
            pn = IGDetail::IGSharedCount();
        }
    }

    template<class Y>
    IGSmartPtr(IGSmartPtr<Y> const & r, IGDetail::polymorphic_cast_tag): px(dynamic_cast<element_type *>(r.px)), pn(r.pn)
    {
        if(px == 0)
        {
            _ASSERTE (0 && L"Bad Cast");
        }
    }

    template<class Y>
    IGSmartPtr & operator=(IGSmartPtr<Y> const & r) // never throws
    {
        px = r.px;
        pn = r.pn; // IGSharedCount::op= doesn't throw
        return *this;
    }

    void reset() // never throws in 1.30+
    {
        this_type().swap(*this);
    }

    template<class Y> void reset(Y * p) // Y must be complete
    {
        _ASSERT(p == 0 || p != px); // catch self-reset errors
        this_type(p).swap(*this);
    }

    template<class Y, class D> void reset( Y * p, D d )
    {
        this_type( p, d ).swap( *this );
    }

    template<class Y, class D, class A> void reset( Y * p, D d, A a )
    {
        this_type( p, d, a ).swap( *this );
    }

    template<class Y> void reset( IGSmartPtr<Y> const & r, T * p )
    {
        this_type( r, p ).swap( *this );
    }

    reference operator* () const // never throws
    {
        _ASSERT(px != 0);
        return *px;
    }

	operator T*() const
	{
		return px;
	}

	template<class Y>
	IGSmartPtr<Y> & operator = (Y * p)
	{
		_ASSERT(p == 0 || p != px); // catch self-reset errors
        this_type(p).swap(*this);
		return (*this);
	}

    T * operator-> () const // never throws
    {
        _ASSERTE(px != 0 && L"Null Pointer");
        return px;
    }

    T * get() const // never throws
    {
        return px;
    }

    // operator! is redundant, but some compilers need it
    bool operator! () const // never throws
    {
        return px == 0;
    }

    bool unique() const // never throws
    {
        return pn.unique();
    }

    long use_count() const // never throws
    {
        return pn.use_count();
    }

    void swap(IGSmartPtr<T> & other) // never throws
    {
        std::swap(px, other.px);
        pn.swap(other.pn);
    }

    template<class Y> bool _internal_less(IGSmartPtr<Y> const & rhs) const
    {
        return pn < rhs.pn;
    }

    void * _internal_get_deleter( type_info const & ti ) const
    {
        return pn.get_deleter( ti );
    }

    bool _internal_equiv( IGSmartPtr const & r ) const
    {
        return px == r.px && pn == r.pn;
    }

    T * px;                     // contained pointer
    IGDetail::IGSharedCount pn;    // reference counter

};  // IGSmartPtr

template<class T, class U> inline bool operator==(IGSmartPtr<T> const & a, IGSmartPtr<U> const & b)
{
    return a.get() == b.get();
}

template<class T, class U> inline bool operator!=(IGSmartPtr<T> const & a, IGSmartPtr<U> const & b)
{
    return a.get() != b.get();
}

template<class T, class U> inline bool operator<(IGSmartPtr<T> const & a, IGSmartPtr<U> const & b)
{
    return a._internal_less(b);
}

template<class T> inline void swap(IGSmartPtr<T> & a, IGSmartPtr<T> & b)
{
    a.swap(b);
}

template<class T, class U> IGSmartPtr<T> static_pointer_cast(IGSmartPtr<U> const & r)
{
    return IGSmartPtr<T>(r, IGDetail::static_cast_tag());
}

template<class T, class U> IGSmartPtr<T> const_pointer_cast(IGSmartPtr<U> const & r)
{
    return IGSmartPtr<T>(r, IGDetail::const_cast_tag());
}

template<class T, class U> IGSmartPtr<T> dynamic_pointer_cast(IGSmartPtr<U> const & r)
{
    return IGSmartPtr<T>(r, IGDetail::dynamic_cast_tag());
}

// shared_*_cast names are deprecated. Use *_pointer_cast instead.

template<class T, class U> IGSmartPtr<T> shared_static_cast(IGSmartPtr<U> const & r)
{
    return IGSmartPtr<T>(r, IGDetail::static_cast_tag());
}

template<class T, class U> IGSmartPtr<T> shared_dynamic_cast(IGSmartPtr<U> const & r)
{
    return IGSmartPtr<T>(r, IGDetail::dynamic_cast_tag());
}

template<class T, class U> IGSmartPtr<T> shared_polymorphic_cast(IGSmartPtr<U> const & r)
{
    return IGSmartPtr<T>(r, IGDetail::polymorphic_cast_tag());
}

template<class T, class U> IGSmartPtr<T> shared_polymorphic_downcast(IGSmartPtr<U> const & r)
{
    _ASSERTE(dynamic_cast<T *>(r.get()) == r.get() && L"Bad Cast");
    return shared_static_cast<T>(r);
}

// get_pointer() enables mem_fn to recognize IGSmartPtr

template<class T> inline T * get_pointer(IGSmartPtr<T> const & p)
{
    return p.get();
}

// operator<<
template<class E, class T, class Y> std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, IGSmartPtr<Y> const & p)
{
    os << p.get();
    return os;
}

// get_deleter
template<class D, class T> D * get_deleter(IGSmartPtr<T> const & p)
{
    return static_cast<D *>(p._internal_get_deleter(typeid(D)));
}

}