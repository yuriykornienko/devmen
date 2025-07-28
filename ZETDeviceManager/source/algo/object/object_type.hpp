#pragma once
#ifndef object_type_hpp__
#define object_type_hpp__

namespace zetlab {

namespace object_type {

enum values {

    undefined = 0x0,
    device,
    channel,

    undefined_7xxx,
    _7xxx_controller,
    _7xxx_is,

	sd,
};

}

// strange postfix operator
inline object_type::values operator++( object_type::values& ls, int )
{
    if (object_type::undefined == ls || object_type::undefined_7xxx == ls)
        return ls;

    return (object_type::values)(ls - 1);
}

}

#endif // object_type_hpp__
