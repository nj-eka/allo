#pragma once

#include <cassert>
#include <bitset>
#include <limits>
#include <cstdint>
#include <memory>
#include <string>
#include <format>

#include "utils/logging.hpp"

namespace alloc{

constexpr std::size_t OPTIMAL_BPOOL_ELEMENTS_COUNT = std::numeric_limits<unsigned long long>::digits; // = 64 on most platforms

enum class placement_policy {
    last, // - designed just to be quickest
    first // - neither this nor that
    // best // - least fragmentation but slowest; todo: implement
};

template <typename T> // size N independent class 
struct bpool_base {
    // virtual ~bpool_base() = default;
    virtual ~bpool_base(){
        TRACE(__PRETTY_FUNCTION__);   
    }

    virtual T* allocate(size_t n) = 0;
    virtual void deallocate(T* ptr, size_t n) = 0;

    virtual bool contains(T *p, size_t n) noexcept = 0;

    virtual size_t total_count() noexcept = 0;
    virtual size_t free_count() noexcept = 0;
    virtual bool is_free(size_t n) noexcept = 0;

    virtual void set(placement_policy pp) noexcept = 0;

    virtual std::string repr() noexcept = 0;

    virtual void reset() noexcept = 0;    
};

template<typename T, size_t N = OPTIMAL_BPOOL_ELEMENTS_COUNT> // N is part of type cuz std::bitset<N> or use boost::bitset with dynamic size ...
struct bpool final: bpool_base<T> {
    using value_t = T;
    constexpr static auto value_size = sizeof(value_t);
    constexpr static auto value_alignment = alignof(value_t);
    using value_storage_t = std::aligned_storage_t<value_size, value_alignment>;
private:
    value_storage_t _pool[N]; // T-aligned
    std::bitset<N> _free{};
    placement_policy _policy;
public:
    // ~bpool() noexcept override = default;
    ~bpool() noexcept override { 
        TRACE(__PRETTY_FUNCTION__);
    }
    bpool(placement_policy pp = placement_policy::last): _policy(pp) {
        TRACE(__PRETTY_FUNCTION__);
        _free.set(); 
    }
    bpool(const bpool&) = delete;
    bpool &operator=(const bpool&) = delete;

    T* allocate(size_t n) override;
    void deallocate(T* ptr, size_t n) override;

    bool contains(T *p, size_t n) noexcept override {
        TRACE(__PRETTY_FUNCTION__);
        T* from = reinterpret_cast<T*>(const_cast<value_storage_t*>(&_pool[0]));
        // std::cout << " contains " << p << " - " << (p + n) << " from " << from  << " to " << from + N << " = " << (bool)((from <= p) && ((p + n) <= (from + N))) << std::endl; 
        return (from <= p) && ((p + n) <= (from + N));
    }

    void set(placement_policy pp) noexcept override { _policy = pp; }

    size_t total_count() noexcept override { return N; } // if it's not overrided than static constexpr ...
    size_t free_count() noexcept override { return _free.count(); } // can be constexpr cuz template<std::size_t _Nb> constexpr std::size_t std::bitset<_Nb>::count() const noexcept
    bool is_free(size_t n) noexcept override {
        TRACE(__PRETTY_FUNCTION__);
        [[maybe_unused]] auto [pos, mask] = _find_placement(n);
        return (pos != N);
    }

    std::string repr() noexcept override { return _free.to_string('*', '_'); }

    void reset() noexcept  override { _free.reset(); }    
private:
    size_t _get_index_from_address(T *p) noexcept {
        TRACE(__PRETTY_FUNCTION__);
        return contains(p, 1) ? p - reinterpret_cast<T*>(&_pool) : N;
    }
    std::pair<size_t, std::bitset<N>> _find_placement(size_t n) const noexcept{
        // if constexpr(P == placement_policy::first) - in case of P is part of this type
        switch (_policy) {
            case  placement_policy::first:
                return _find_placement_first(n);
            case  placement_policy::last:
                return _find_placement_last(n);
            default:
                /// throw not implemented
                return _find_placement_first(n);
        }
    }
    std::pair<size_t, std::bitset<N>> _find_placement_first(size_t n) const noexcept;
    std::pair<size_t, std::bitset<N>> _find_placement_last(size_t n) const noexcept;
};

// flow of implementation details into interface cuz template... todo: try module )
namespace _details {

template <size_t N>
std::bitset<N> get_mask(size_t from, size_t to);

template <size_t N>
size_t countl(std::bitset<N> bs, bool value = true, size_t l_from = 0, size_t max_count = N);

template <size_t N>
size_t countr(std::bitset<N> bs, bool value = true, size_t r_from = 0, size_t max_count = N);

}

template<typename T, size_t N>
T* bpool<T, N>::allocate(size_t n) {
    TRACE(__PRETTY_FUNCTION__);
    if (n == 0 || n > free_count()) 
        return nullptr;
    // todo: add check of max available placement size (if it makes sense... not only for best placament policy)
    if (auto [pos, mask] = _find_placement(n); pos != N){   
        _free &= mask.flip();
        return std::assume_aligned<value_alignment, T>(reinterpret_cast<T*>(&_pool[pos]));
    } else {
        // return static_cast<T *>(::operator new(value_size));
        return nullptr;
    }
}

template<typename T, size_t N>
void bpool<T, N>::deallocate(T* p, size_t n) {
    TRACE(__PRETTY_FUNCTION__);
    if (auto i = _get_index_from_address(p); i != N){
        _free |= _details::get_mask<N>(i, i + n);
    } else {
        // ::operator delete(p);
    }
}

template<typename T, size_t N>
std::pair<size_t, std::bitset<N>> bpool<T, N>::_find_placement_first(size_t n) const noexcept
{
    TRACE(__PRETTY_FUNCTION__);
    for (auto pos = _free._Find_first(); pos <= N - n;){
        if (_details::countr(_free, true, pos, n) >= n)
            return std::make_pair(pos, _details::get_mask<N>(pos, pos + n));
        pos = _free._Find_next(pos);
        // for "big" n
        // if (auto mask = get_mask<size>(pos, pos + n); (_free & mask).count() == n)
        //     return std::make_pair(pos, mask);
    }
    return std::make_pair(N, std::bitset<N>());
}

template<typename T, size_t N>
std::pair<size_t, std::bitset<N>> bpool<T, N>::_find_placement_last(size_t n) const noexcept
{
    TRACE(__PRETTY_FUNCTION__);
    // #include <bit> std::countl_one - only if T is an unsigned integer type...
    if (size_t i = _details::countl(_free, true, 0); i >= n){   
        return std::make_pair(N - i, _details::get_mask<N>(N - i, N - i + n));
    } else {
        return std::make_pair(N, std::bitset<N>());
    }
}

template <size_t N>
std::bitset<N> _details::get_mask(size_t from, size_t to) { 
  std::bitset<N> low{}, up{};
  low.set(); up.set();
  return (low <<= from) & (up <<= to).flip();
}

template <size_t N>
size_t _details::countl(std::bitset<N> bs, bool value, size_t l_from, size_t max_count) {
    // for this task, fast optimization is possible
    // asm ("bsrl %1, %0" 
    //     : "=r" (position) 
    //     : "r" (number));    
    for(size_t i = 0; i < max_count; ++i)
        if (bs[N - 1 - l_from - i] != value)
            return i;
    return max_count;
}

template <size_t N>
size_t _details::countr(std::bitset<N> bs, bool value, size_t r_from, size_t max_count) {
    // for this task, fast optimization is possible
    // asm ("bsrl %1, %0" 
    //     : "=r" (position) 
    //     : "r" (number));
    for(size_t i = 0; i < max_count; ++i)
        if (bs[r_from + i] != value)
            return i;
    return max_count;
}

}