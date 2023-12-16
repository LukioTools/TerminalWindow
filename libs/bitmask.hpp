#pragma once
#include <bitset>
#include <cmath>
#include <cstddef>
#include <ostream>


template<size_t ByteSize>
class BitMask
{
    public:
    static_assert(ByteSize > 0, "ByteSize must be greater than 0");

    unsigned char mask[ByteSize] = {0};

    inline constexpr size_t nbits() noexcept{
        return ByteSize * sizeof(unsigned char) * 8;
    }

    inline constexpr size_t nbytes() noexcept{
        return ByteSize * sizeof(unsigned char);
    }

    inline unsigned char& getByteRef(size_t i){
        return mask[static_cast<size_t>(std::floor(i/8))];
    }

    inline void set(size_t i, bool on){
        if(i >= nbits())
            return;
        
        unsigned char& m = getByteRef(i);
        if(on){
            m = m | (1 << i%8);
        }
        else{
            m = m & ~(1 << i%8);
        }
    }

    bool get(size_t i){
        if(i >= nbits())
            return false;

        unsigned char& m = getByteRef(i);
        return m & (1 << i%8);
    }


};
