#pragma once

#include <string>

namespace IGLibrary
{

// this add a count management to its childs
/** This class adds reference count management.
@warning the destructor is protected by design.*/
class IGRefCount
{

public:
	IGRefCount();
	virtual ~IGRefCount();

	// dispose() is called when use_count_ drops to zero, to release
    // the resources managed by *this.
    virtual void dispose() = 0;

    // destroy() is called when weak_count_ drops to zero.
    virtual void destroy();
    virtual void * get_deleter( type_info const & ti ) = 0;
    void add_ref_copy();
    bool add_ref_lock();
    void release();
    void weak_add_ref();
    void weak_release();
    long use_count() const;

private:
	IGRefCount( IGRefCount const & );
    IGRefCount & operator= ( IGRefCount const & );

    long use_count_;        // #shared
    long weak_count_;       // #weak + (#shared != 0)    
};

template<class T> inline void checked_delete(T * x)
{
    // intentionally complex - simplification causes regressions
    typedef char type_must_be_complete[ sizeof(T)? 1: -1 ];
    (void) sizeof(type_must_be_complete);
    delete x;
}

template<class X> 
class IGRefCountSingleImpl : public IGRefCount
{
private:
    X * px_;

    IGRefCountSingleImpl( IGRefCountSingleImpl const & );
    IGRefCountSingleImpl & operator= ( IGRefCountSingleImpl const & );

    typedef IGRefCountSingleImpl<X> this_type;

public:
    explicit IGRefCountSingleImpl (X * px) : px_( px )
	{
	}

    virtual void dispose()
    {
        checked_delete( px_ );
    }

    virtual void * get_deleter (type_info const &)
    {
        return 0;
    }

    void * operator new( std::size_t )
    {
        return std::allocator<this_type>().allocate( 1, static_cast<this_type *>(0) );
    }

    void operator delete( void * p )
    {
        std::allocator<this_type>().deallocate( static_cast<this_type *>(p), 1 );
    }
};

}
