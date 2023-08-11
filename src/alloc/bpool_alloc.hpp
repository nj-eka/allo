#pragma once

#include <cassert>
#include <cstddef>
#include <forward_list>
#include <functional>
#include <utility>
#include <array>
#include <new>
#include <stdexcept>

#include "utils/logging.hpp"
#include "utils/trace.hpp"
#include "bpool.hpp"

namespace alloc{

template <class T, size_t N = OPTIMAL_BPOOL_SEGMENT_ELEMENTS_COUNT>
struct bpool_alloc {
  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;  
  // using propagate_on_container_copy_assignment = false_type;
  // using propagate_on_container_move_assignment = false_type;
  // using propagate_on_container_swap            = false_type;
  // using is_always_equal                        = true_type;
private:
  placement_policy _initial_placement_policy;
  std::forward_list<std::unique_ptr<alloc::bpool_base<T>>> _bpools;
  size_t _bpools_size = 0; // not to use distance(_bpools.begin(), _bpools.end())
  void _extend_bpool(); // todo: add support for n depended extention
public:
  bpool_alloc(placement_policy ipp = placement_policy::first) noexcept : _initial_placement_policy(ipp) {
    TRACE(__PRETTY_FUNCTION__);
  }
  bpool_alloc(const bpool_alloc &) = delete; // ? deep copy
  bpool_alloc(bpool_alloc && other) noexcept {
    TRACE(__PRETTY_FUNCTION__);
    std::swap(_bpools, other._bpools);
    std::swap(_initial_placement_policy, other._initial_placement_policy);
    std::swap(_bpools_size, other._bpools_size);
  }
  bpool_alloc &operator=(const bpool_alloc &) = delete; // ? deep copy

  template <class U>
  bpool_alloc(bpool_alloc<U, N> const&) noexcept {
    TRACE(__PRETTY_FUNCTION__);
  }

  template <class Up> struct rebind { 
    using other = bpool_alloc<Up, N>;
  };

  T *allocate(size_t n = 1) 
  {
    TRACE(__PRETTY_FUNCTION__);
    if (n >  N * (1 << (_bpools_size + 1)) ) throw std::bad_alloc();
    for(auto& bp: _bpools)
        if (auto ptr = bp->allocate(n); ptr != nullptr)
          return ptr;
    _extend_bpool(); // todo: + n
    if (auto ptr = _bpools.front()->allocate(n); ptr != nullptr)
          return ptr;
    throw std::bad_alloc();
  }

  void deallocate(T *ptr, std::size_t n = 1) {
    TRACE(__PRETTY_FUNCTION__);
    for(auto& bp : _bpools)
        if (bp->contains(ptr, n)){
          bp->deallocate(ptr, n);
          return;
        }
    throw_with_trace(std::out_of_range(std::format("{}", __PRETTY_FUNCTION__)));
  }    

  template<typename U, typename ...Args>
  void construct(U *p, Args &&...args) 
  {
    TRACE(__PRETTY_FUNCTION__);
    new(p) U(std::forward<Args>(args)...);
  }

  void destroy(T *p) 
  {
    TRACE(__PRETTY_FUNCTION__);
    p->~T();
  }

  // void shrink(); todo: add clean up

  size_t total_count() const noexcept {
    size_t i = 0;
    for(const auto& bp: _bpools){
      i += bp->total_count();
    }
    return i;
  }

  size_t free_count() const noexcept {
    size_t i = 0;
    for(const auto& bp: _bpools){
      i += bp->free_count();
    }
    return i;
  }

  size_t get_bpools_size() const noexcept {
    return _bpools_size;
  }

  std::string repr() const noexcept {
    std::stringstream ss;
    for(const auto& bp: _bpools){
      ss << bp->repr() << "\n";
    }
    return ss.str();
  }

};

template <class T, size_t N>
void bpool_alloc<T, N>::_extend_bpool(){
  TRACE(__PRETTY_FUNCTION__);
  // todo: switch to use next multiple of OPTIMAL_BPOOL_SEGMENT_ELEMENTS_COUNT from N if benchmarks are positive. 
  // constexpr size_t M = ((N + OPTIMAL_BPOOL_SEGMENT_ELEMENTS_COUNT - 1) / OPTIMAL_BPOOL_SEGMENT_ELEMENTS_COUNT) * OPTIMAL_BPOOL_SEGMENT_ELEMENTS_COUNT;
  switch (_bpools_size){
    // static extentions
    case 0:
      // _bpools.push_front(std::unique_ptr<bpool_base<T>>(new bpool<T, N>(_initial_placement_policy)));
      _bpools.push_front(std::make_unique<bpool<T, N>>(_initial_placement_policy));
      break;
    case 1:
      _bpools.push_front(std::make_unique<bpool<T, 2*N>>(_initial_placement_policy));
      break;
    case 2:
      _bpools.push_front(std::make_unique<bpool<T, 4*N>>(_initial_placement_policy));
      break;
    case 3:
      _bpools.push_front(std::make_unique<bpool<T, 8*N>>(_initial_placement_policy));
      break;
    case 4:
      _bpools.push_front(std::make_unique<bpool<T, 16*N>>(_initial_placement_policy));
      break;
    // ...
    default:
      // todo: add support for dynamic extention of n depended 
      throw std::bad_alloc();
  }
  ++_bpools_size;
}

} // namespace alloc
