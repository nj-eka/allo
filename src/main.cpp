#include <iostream>
#include <cassert>
#include <cstdint>
#include <type_traits>
#include <map>
#include <utility>
#include <numeric>
#include <algorithm>
#include <iterator>

#include "args/options.hpp"
#include "utils/logs_init.hpp"
#include "utils/logging.hpp"
#include "utils/trace.hpp"

#include "alloc/bpool_alloc.hpp"
#include "cont/slist.hpp"

namespace {

using uint_t = std::uint_fast64_t;

template <uint_t N>
struct fact : std::integral_constant<uint_t, N * fact<N - 1>()>{};
template <>
struct fact<0> : std::integral_constant<uint_t, 1>{};

template <typename T, typename Cmp, typename Alloc, T... ints>
void fill_map(std::map<T, T, Cmp, Alloc> &m, std::integer_sequence<T, ints...>)
{
    ((m[ints] = fact<ints>()), ...); // fact<ints>() == fact<ints>{} == fact<ints>::value
}

}

int main(int argc, char const *argv[]) {
  try {
    auto [desc, vm] = args::parse(argc, argv);
    if (args::info_requested(vm, desc)){
      return EAGAIN;
    }
    Logger::init(vm);
    TRACE("init logs");

    /////////////////////////// hw3 ///////////////////////////////
    {
      INFO("Start:...");
      auto mp1 = std::map<uint_t, uint_t, std::less<uint_t>>();
      fill_map(mp1, std::make_integer_sequence<uint_t, 10>{});
      for (auto const& [key, val] : mp1)
        std::cout << key << " " << val << std::endl;

      auto mp2 = std::map<uint_t, uint_t, std::less<uint_t>, alloc::bpool_alloc<std::pair<const uint_t, uint_t>, 10>>();
      fill_map(mp2, std::make_integer_sequence<uint_t, 10>{});
      for (auto const& [key, val] : mp2)
        std::cout << key << " " << val << std::endl;
      INFO("part1 - done");
      // assert(map1 == map2); // todo: comparing allocators is some tricky task...
    }

    {
      cont::slist<uint_t> sl1(10);
      std::iota(sl1.begin(), sl1.end(), 0);
      std::copy(sl1.begin(), sl1.end(), std::ostream_iterator<uint_t>(std::cout, " "));
      std::cout << std::endl;

      cont::slist<uint_t, alloc::bpool_alloc<uint_t, 10>> sl2(10);
      std::iota(sl2.begin(), sl2.end(), 0);
      std::copy(sl2.begin(), sl2.end(), std::ostream_iterator<uint_t>(std::cout, " "));
      std::cout << std::endl;
      
      assert(sl1 == sl2); // comparing custom list container is done
      INFO("part2 - done");
    }
    INFO("The END");
  } catch (const args::opt::error &e) {
    ERROR("Invalid command args: %s (see --help)\n", e.what());
    return EINVAL;
  } catch (const std::system_error &e) {
    ERROR("System error [%s] code msg (%s)\n", e.what(), e.code().message());
    return e.code().value();
  } catch (const std::exception& e) {
    const boost::stacktrace::stacktrace* st = boost::get_error_info<traced>(e);
    if (st){
      ERROR("What: %s\nStack trace: %s\n", e.what(), *st);
    } else {
      ERROR("What: %s\n", e.what());
    }
    return ENOEXEC;
  }
  return EXIT_SUCCESS;
}