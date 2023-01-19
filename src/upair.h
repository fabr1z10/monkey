#pragma once


#include <utility>
#include <unordered_map>

template <typename T>
class UPair {
private:
    ::std::pair<T,T> p;
public:
    UPair() : p(0, 0) {}
    UPair(T a, T b) : p(::std::min(a,b),::std::max(a,b)) {
    }
    UPair(::std::pair<T,T> pair) : p(::std::min(pair.first,pair.second),::std::max(pair.first,pair.second)) {
    }
    friend bool operator==(UPair const& a, UPair const& b) {
        return a.p == b.p;
    }
    operator ::std::pair<T,T>() const {
        return p;
    }
    struct HashFunction
    {
        size_t operator()(const UPair<T>& point) const
        {
            size_t seed = 0;
            ::hash_combine(seed, point.p.first);
            ::hash_combine(seed, point.p.second);
            return seed;
        }
    };

};
