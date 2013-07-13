#include "stdafx.h"
#include "IGSmartPtr.h"
#include <string>

using namespace std;
using namespace IGLibrary;
using namespace IGLibrary::IGDetail;

IGSharedCount::IGSharedCount() : pi_(0)
{
}

IGSharedCount::~IGSharedCount() // nothrow
{
    if( pi_ != 0 ) pi_->release();
}

IGSharedCount::IGSharedCount(IGSharedCount const & r): pi_(r.pi_) // nothrow
{
    if( pi_ != 0 ) pi_->add_ref_copy();
}

IGSharedCount & IGSharedCount::operator =(IGSharedCount const & r) // nothrow
{
    IGRefCount * tmp = r.pi_;
    if( tmp != pi_ )
    {
        if( tmp != 0 ) tmp->add_ref_copy();
        if( pi_ != 0 ) pi_->release();
        pi_ = tmp;
    }
    return *this;
}

void IGSharedCount::swap(IGSharedCount & r) // nothrow
{
    IGRefCount * tmp = r.pi_;
    r.pi_ = pi_;
    pi_ = tmp;
}

long IGSharedCount::use_count() const // nothrow
{
    return pi_ != 0? pi_->use_count(): 0;
}

bool IGSharedCount::unique() const // nothrow
{
    return use_count() == 1;
}

bool IGSharedCount::empty() const // nothrow
{
    return pi_ == 0;
}

void * IGSharedCount::get_deleter( type_info const & ti ) const
{
    return pi_? pi_->get_deleter( ti ): 0;
}