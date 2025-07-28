#pragma once
#ifndef crawler_interface_hpp__
#define crawler_interface_hpp__

namespace zetlab {

interface crawler_interface
{
    virtual const long update() = 0;
};

}

#endif // crawler_interface_hpp__
