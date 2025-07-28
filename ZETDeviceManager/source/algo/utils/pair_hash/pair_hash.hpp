#pragma once

#include <utility>

namespace zetlab {

namespace utils {

template <class T> inline void hash_combine(size_t& seed, T const& v)
{
    seed ^= ::std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

struct pair_hash
{
    template <class T1, class T2>
    size_t operator()(const ::std::pair<T1, T2>& p) const
    {
        size_t seed = 0;
        hash_combine(seed, p.first);
        hash_combine(seed, p.second);
        return seed;
    }
};

};

};
