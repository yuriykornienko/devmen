#pragma once
#ifndef memento_hpp__
#define memento_hpp__

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
//#include "..\std\channel\channel.hpp"

namespace zetlab {

template< typename T >
class memento :
    public boost::noncopyable
{
public:
    typedef boost::shared_ptr< typename T::clipboard > snapshot;

public:
    memento( T* object ) :
        _object  (object),
        _snapshot(object->createMemento())
    {
    };

    void restore()
    {
       _object->restoreMemento( _snapshot );
    };

    void restore_(long mask)
    {
        _object->restoreMemento_(_snapshot, mask);
    };

    void restore_anyDevice(long modul, long mask)
    {
        _object->restoreMemento_anyDevice(modul , mask,  _snapshot);
    };

    long returnICP()
    {
        return  this->_snapshot->zetlab::std::channel::clipboard::icp;
    }

    auto getName()
    {
        return  this->_snapshot->zetlab::std::channel::clipboard::name;
    }

    long afteramplify()
    {
        return  this->_snapshot->zetlab::std::channel::clipboard::amplifygain;
    }
    

   // snapshot _snapshot;
    //T* _object;
private:
    T*       _object;
    snapshot _snapshot;
};

} // namespace zetlab

#endif // memento_hpp__;
