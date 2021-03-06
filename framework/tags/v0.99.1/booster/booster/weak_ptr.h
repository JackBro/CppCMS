#ifndef BOOSTER_SMART_PTR_WEAK_PTR_HPP_INCLUDED
#define BOOSTER_SMART_PTR_WEAK_PTR_HPP_INCLUDED

//
//  weak_ptr.hpp
//
//  Copyright (c) 2001, 2002, 2003 Peter Dimov
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/smart_ptr/weak_ptr.htm for documentation.
//

#include <memory> // boost.TR1 include order fix
#include <booster/smart_ptr/shared_count.h>
#include <booster/shared_ptr.h>

#ifdef BOOSTER_MSVC  // moved here to work around VC++ compiler crash
# pragma warning(push)
# pragma warning(disable:4284) // odd return type for operator->
#endif

namespace booster
{

template<class T> class weak_ptr
{
private:

    // Borland 5.5.1 specific workarounds
    typedef weak_ptr<T> this_type;

public:

    typedef T element_type;

    weak_ptr(): px(0), pn() // never throws in 1.30+
    {
    }

//  generated copy constructor, assignment, destructor are fine


//
//  The "obvious" converting constructor implementation:
//
//  template<class Y>
//  weak_ptr(weak_ptr<Y> const & r): px(r.px), pn(r.pn) // never throws
//  {
//  }
//
//  has a serious problem.
//
//  r.px may already have been invalidated. The px(r.px)
//  conversion may require access to *r.px (virtual inheritance).
//
//  It is not possible to avoid spurious access violations since
//  in multithreaded programs r.px may be invalidated at any point.
//

    template<class Y>
    weak_ptr( weak_ptr<Y> const & r, typename detail::sp_enable_if_convertible<Y,T>::type = detail::sp_empty() )
    : px(r.lock().get()), pn(r.pn) // never throws
    {
    }


    template<class Y>
    weak_ptr( shared_ptr<Y> const & r, typename detail::sp_enable_if_convertible<Y,T>::type = detail::sp_empty() )
    : px( r.px ), pn( r.pn ) // never throws
    {
    }

    template<class Y>
    weak_ptr & operator=(weak_ptr<Y> const & r) // never throws
    {
        px = r.lock().get();
        pn = r.pn;
        return *this;
    }

    template<class Y>
    weak_ptr & operator=(shared_ptr<Y> const & r) // never throws
    {
        px = r.px;
        pn = r.pn;
        return *this;
    }

    shared_ptr<T> lock() const // never throws
    {
        return shared_ptr<element_type>( *this, booster::detail::sp_nothrow_tag() );
    }

    long use_count() const // never throws
    {
        return pn.use_count();
    }

    bool expired() const // never throws
    {
        return pn.use_count() == 0;
    }

    bool _empty() const // extension, not in std::weak_ptr
    {
        return pn.empty();
    }

    void reset() // never throws in 1.30+
    {
        this_type().swap(*this);
    }

    void swap(this_type & other) // never throws
    {
        std::swap(px, other.px);
        pn.swap(other.pn);
    }

    void _internal_assign(T * px2, booster::detail::shared_count const & pn2)
    {
        px = px2;
        pn = pn2;
    }

    template<class Y> bool _internal_less(weak_ptr<Y> const & rhs) const
    {
        return pn < rhs.pn;
    }

// Tasteless as this may seem, making all members public allows member templates
// to work in the absence of member template friends. (Matthew Langston)

private:

    template<class Y> friend class weak_ptr;
    template<class Y> friend class shared_ptr;

    T * px;                       // contained pointer
    booster::detail::weak_count pn; // reference counter

};  // weak_ptr

template<class T, class U> inline bool operator<(weak_ptr<T> const & a, weak_ptr<U> const & b)
{
    return a._internal_less(b);
}

template<class T> void swap(weak_ptr<T> & a, weak_ptr<T> & b)
{
    a.swap(b);
}

} // namespace boost

#ifdef BOOSTER_MSVC
# pragma warning(pop)
#endif    

#endif  // #ifndef BOOST_SMART_PTR_WEAK_PTR_HPP_INCLUDED
