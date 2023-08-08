#pragma once

#include <iostream>
#include <string>
#include <tuple>

inline size_t _memory = 0;
inline size_t _alloc = 0;


// https://habr.com/ru/articles/490640/
// void* operator new(std::size_t size);
// void operator delete(void* ptr);
// void* operator new[](std::size_t size);
// void operator delete[](void* ptr);
// void* operator new(std::size_t size, void* ptr);
// void* operator new[](std::size_t size, void* ptr);
// void* operator new(std::size_t size, const std::nothrow_t& nth);
// void* operator new[](std::size_t size, const std::nothrow_t& nth);

// -std=c++17
// void* operator new (std::size_t size, std::align_val_t al);
// void* operator new[](std::size_t size, std::align_val_t al);

// overload
// void* operator new(std::size_t size, /* доп. параметры */);
// void* operator new[](std::size_t size, /* доп. параметры */);
// void operator delete(void* p, /* доп. параметры */);
// void operator delete[](void* p, /* доп. параметры */);

inline void *operator new(size_t s) noexcept(false) {
  _memory += s;
  ++_alloc;
  return malloc(s);
}

inline void operator delete(void *p) throw() {
  --_alloc;
  free(p);
}

// предупреждение: программа должна также определять «void operator delete(void*, std::size_t)» [-Wsized-deallocation]
inline void operator delete(void *p, std::size_t) noexcept
{
    ::operator delete(p);
}


inline auto memuse() { return std::make_pair(_memory, _alloc); }

inline auto memstat(std::ostream &os) { os << _memory << " : " << _alloc; }

inline auto memstatln(std::ostream &os, size_t indent) {
  os << std::string(" ", indent);
  memstat(os);
  os << std::endl;
}