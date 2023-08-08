//-----------------------------------------------------------------------------
//
// based on Pablo Halpern slist realization in "Allocators: the Good Parts" CppCon 2017
// + MIPT masters course on C++
// 
//----------------------------------------------------------------------------
#pragma once

#include <algorithm>
#include <iterator>
#include <utility>
#include <memory>

#include <boost/type_index.hpp>
#include "utils/logging.hpp"

using std::addressof;
using std::allocator_traits;
using std::equal;
using std::forward;
using std::forward_iterator_tag;
using std::move;
using std::ptrdiff_t;
using std::swap;

namespace cont {
namespace slist_details {

template <typename Tp> 
struct node;

template <typename Tp> 
struct node_base {
  node<Tp> *next_;

  node_base() : next_(nullptr) { 
    TRACE(__PRETTY_FUNCTION__); 
  }
  node_base(node_base &&rhs) : next_(rhs.next_) { 
    TRACE(__PRETTY_FUNCTION__);
    rhs.next_ = nullptr; 
  }
  node_base &operator=(node_base &&rhs) {
    TRACE(__PRETTY_FUNCTION__);
    if (&rhs == this)
      return *this;
    next_ = rhs.next_;
    rhs.next_ = nullptr;
    return *this;
  }
  node_base(const node_base &) = delete;
  node_base &operator=(const node_base &) = delete;
};

template <typename Tp> struct node : node_base<Tp> {
  union {
    Tp value_;
  };
};

template <typename Tp> struct const_iterator {
  using value_type = Tp;
  using pointer = Tp const *;
  using reference = Tp const &;
  using difference_type = ptrdiff_t;
  using iterator_category = forward_iterator_tag;

  reference operator*() const {
    TRACE(__PRETTY_FUNCTION__);
    DEBUG("return ")
    return prev_->next_->value_; 
  }
  pointer operator->() const {
    TRACE(__PRETTY_FUNCTION__);
    return addressof(prev_->next_->value_); 
  }

  const_iterator &operator++() {
    prev_ = prev_->next_;
    return *this;
  }
  const_iterator operator++(int) {
    const_iterator tmp(*this);
    ++*this;
    return tmp;
  }

  bool operator==(const_iterator other) const { return prev_ == other.prev_; }
  bool operator!=(const_iterator other) const { return !operator==(other); }

  node_base<Tp> *prev_;

  explicit const_iterator(const node_base<Tp> *prev)
      : prev_(const_cast<node_base<Tp> *>(prev)) {
    TRACE(__PRETTY_FUNCTION__);
  }
};

template <typename Tp> struct iterator : public const_iterator<Tp> {
  using Base = const_iterator<Tp>;

  using pointer = Tp *;
  using reference = Tp &;

  reference operator*() const { return this->prev_->next_->value_; }
  pointer operator->() const { return addressof(this->prev_->next_->value_); }

  iterator &operator++() {
    Base::operator++();
    return *this;
  }
  iterator operator++(int) {
    iterator tmp(*this);
    ++*this;
    return tmp;
  }

  explicit iterator(node_base<Tp> *prev) : const_iterator<Tp>(prev) {
    TRACE(__PRETTY_FUNCTION__);    
  }
};

} // namespace slist_details

template <typename T, typename Allocator = std::allocator<std::remove_cv_t<T>>>
struct slist {
  using value_type = T;
  using reference = value_type &;
  using const_reference = value_type const &;
  using difference_type = ptrdiff_t;
  using size_type = size_t;
  using allocator_type = Allocator;
  using iterator = slist_details::iterator<value_type>;
  using const_iterator = slist_details::const_iterator<value_type>;
  using alloc_traits = allocator_traits<allocator_type>;
private:
  using node_base = slist_details::node_base<value_type>;
  using node = slist_details::node<value_type>;
  using node_allocator_type = typename alloc_traits::template rebind_alloc<node>; // depracated in std20 -> rebind_alloc
  using node_alloc_traits = allocator_traits<node_allocator_type>;

  node_base head_;
  node_base *ptail_;
  size_t size_;
  node_allocator_type node_alloc_;

public:
  slist(node_allocator_type&& a = {}) : head_(), ptail_(&head_), size_(0), node_alloc_(std::forward<node_allocator_type>(a)) {
    TRACE("call [%s] with a as [%s]", __PRETTY_FUNCTION__, boost::typeindex::type_id_runtime(a));
  }
  slist(allocator_type&& a) : slist(node_alloc_type(std::forward<allocator_type>(a))) {
    TRACE("call [%s] with a as [%s]", __PRETTY_FUNCTION__, boost::typeindex::type_id_runtime(a));  
  }
// todo: try to apply ALL standard requirements...  
// https://en.cppreference.com/w/cpp/named_req/AllocatorAwareContainer
// Copy constructors of AllocatorAwareContainers obtain their instances of the allocator 
// by calling std::allocator_traits<allocator_type>::select_on_container_copy_construction 
// on the allocator of the container being copied.
// https://en.cppreference.com/w/cpp/memory/allocator_traits/select_on_container_copy_construction
  slist(const slist &other): slist(other.get_allocator()) {
    TRACE("call [%s] with other as [%s]", __PRETTY_FUNCTION__, boost::typeindex::type_id_runtime(other));    
    operator=(other);
  }
  slist(const slist &other, allocator_type a): slist(a) {
    TRACE("call [%s] with other as [%s]", __PRETTY_FUNCTION__, boost::typeindex::type_id_runtime(other));      
    operator=(other);
  }
// Move constructors obtain their instances of allocators by move-constructing from the allocator belonging to the old container.
  slist(slist &&other) noexcept : slist(other.get_allocator()) {
    TRACE("call [%s] with other as [%s]", __PRETTY_FUNCTION__, boost::typeindex::type_id_runtime(other));   
    head_ = move(other.head_);
    ptail_ = move(other.ptail_);
  }
  slist(slist &&other, allocator_type a): slist(a) {
    TRACE("call [%s] with other as [%s]", __PRETTY_FUNCTION__, boost::typeindex::type_id_runtime(other));    
    operator=(move(other));
  }
  slist(size_t size): slist(){
    TRACE(__PRETTY_FUNCTION__);       
    while(size--)
      push_front(T{});
  }
  ~slist() {
    TRACE(__PRETTY_FUNCTION__);
    erase(begin(), end()); 
  }

  slist &operator=(const slist &other);
  slist &operator=(slist &&other);
// Swap will replace the allocator only if std::allocator_traits<allocator_type>::propagate_on_container_swap::value is true. 
// Specifically, it will exchange the allocator instances through an unqualified call to the non-member function swap, see Swappable.  
  void swap(slist &other) noexcept;

  size_t size() const noexcept { return size_; }
  bool empty() const noexcept { return 0 == size_; }

  iterator begin() { return iterator(&head_); }
  iterator end() { return iterator(ptail_); }
  const_iterator begin() const { return const_iterator(&head_); }
  const_iterator end() const { return const_iterator(ptail_); }
  const_iterator cbegin() const { return const_iterator(&head_); }
  const_iterator cend() const { return const_iterator(ptail_); }

  T &front() { return head_.next_->value_; }
  T const &front() const { return head_.next_->value_; }

  template <typename... Args> iterator emplace(iterator i, Args &&... args);
  template <typename... Args> void emplace_front(Args &&... args) {
    emplace(begin(), forward<Args>(args)...);
  }
  template <typename... Args> void emplace_back(Args &&... args) {
    emplace(end(), forward<Args>(args)...);
  }

  iterator insert(iterator i, const T &v) { return emplace(i, v); }
  void push_front(const T &v) { emplace(begin(), v); }
  void push_back(const T &v) { emplace(end(), v); }

  // Note: erasing elements invalidates iterators to the node following the element being erased.
  iterator erase(iterator b, iterator e);
  iterator erase(iterator i) {
    iterator e = i;
    return erase(i, ++e);
  }
  void pop_front() { erase(begin()); }

  // The accessor get_allocator() obtains a copy of the allocator that was used to construct the container or installed by the most recent allocator replacement operation.
  allocator_type get_allocator() && {
    WARNING(__PRETTY_FUNCTION__);      
    return allocator_type{}; 
  }
  node_allocator_type& get_node_allocator() & { 
    WARNING(__PRETTY_FUNCTION__);
    return node_alloc_; 
  }

};
template <typename T, typename A, typename TT, typename AA>
bool operator==(const slist<T, A> &a, const slist<TT, AA> &b) {
  TRACE("call [%s] with a as [%s] and b as [%s]", __PRETTY_FUNCTION__, boost::typeindex::type_id_runtime(a), boost::typeindex::type_id_runtime(b));  
  return (a.size() == b.size()) && equal(a.begin(), a.end(), b.begin());
}

template <typename T, typename A, typename TT, typename AA>
bool operator!=(const slist<T, A> &a, const slist<TT, AA> &b) {
  return !(a == b);
}

template <typename T, typename A> 
slist<T, A> &slist<T, A>::operator=(const slist &other) {
  if (&other == this)
    return *this;
  erase(begin(), end());
  for (const T &v : other)
    push_back(v);
  return *this;
}

template <typename T, typename A> 
slist<T, A> &slist<T, A>::operator=(slist &&other) {
  if (&other == this)
    return *this;
  if (node_alloc_ == other.node_alloc_) {
    erase(begin(), end());
    swap(other);
  } else
    operator=(other); // Copy assign
  return *this;
}

template <typename T, typename A> 
void slist<T, A>::swap(slist &other) noexcept {
  assert(node_alloc_ == other.node_alloc_);
  node_base *new_tail = other.empty() ? &head_ : other.ptail_;
  node_base *new_other_tail = empty() ? &other.head_ : ptail_;
  swap(head_.next_, other.head_.next_);
  swap(size_, other.size_);
  ptail_ = new_tail;
  other.ptail_ = new_other_tail;
}

template <typename T, typename A>
template <typename... Args>
typename slist<T, A>::iterator slist<T, A>::emplace(iterator it, Args &&... args) {
  TRACE(__PRETTY_FUNCTION__);    
  // node *new_node = node_alloc_.allocate(); // static_cast<node *>(...)
  node *new_node = node_alloc_traits::allocate(node_alloc_, 1);
  try {
/* 
  Allocator-aware containers always call std::allocator_traits<A>::construct(m, p, args) 
  to construct an object of type T at p using args, with m == get_allocator(). 
  The default construct in std::allocator calls ::new((void*)p) T(args) (until C++20) 
  std::allocator has no construct member and std::construct_at(p, args) is called when constructing elements (since C++20), 
  but specialized allocators may choose a different definition
*/
    // node_alloc_traits::construct(node_alloc_, addressof(new_node->value_), forward<Args>(args)...);
    std::construct_at(addressof(new_node->value_), forward<Args>(args)...);
  } catch (...) {
    // Recover resources if exception on constructor call.
    // node_alloc_.deallocate(new_node);
    node_alloc_traits::deallocate(node_alloc_, new_node, 1);
    // node_alloc_.deallocate(new_node, 1);  
    throw;
  }
  new_node->next_ = it.prev_->next_;
  it.prev_->next_ = new_node;
  if (it.prev_ == ptail_)
    ptail_ = new_node; // Added at end
  ++size_;
  return it;
}

template <typename T, typename A>
typename slist<T, A>::iterator slist<T, A>::erase(iterator b, iterator e) {
  TRACE(__PRETTY_FUNCTION__);  
  node *erase_next = b.prev_->next_;
  node *erase_past = e.prev_->next_; // one past last erasure
  if (nullptr == erase_past)
    ptail_ = b.prev_;          // Erasing at tail
  b.prev_->next_ = erase_past; // splice out sublist
  while (erase_next != erase_past) {
    node *old_node = reinterpret_cast<node*>( erase_next );
    erase_next = erase_next->next_;
    --size_;
    // node_alloc_traits::destroy(node_alloc_, addressof(old_node->value_));
		if constexpr (not std::is_fundamental_v<T>)
		{
      addressof(old_node->value_)->~T();
		}    
    node_alloc_traits::deallocate(node_alloc_, old_node, 1);
    // node_alloc_.deallocate(old_node, 1);    
  }
  return b;
}

} // namespace cont
