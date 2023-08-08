#include <iostream>
#include <cassert>
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


int main(int argc, char const *argv[]) {
  try {
    auto [desc, vm] = args::parse(argc, argv);
    if (args::info_requested(vm, desc)){
      return EAGAIN;
    }
    Logger::init(vm);
    TRACE("init logs")

    /////////////////////////// hw3 ///////////////////////////////
    {
      INFO("Start:...")
      auto map1 = std::map<int, int, std::less<int>>();
      map1[0] = 1;
      for (int i = 1; i <= 9; ++i)
        map1[i] = map1[i-1] * i;

      for (auto const& [key, val] : map1){
        // assert(map1[key] == val);
        std::cout << key << " " << val << std::endl;
      }

      auto map2 = std::map<int, int, std::less<int>, alloc::bpool_alloc<std::pair<const int, int>, 10>>();
      map2[0] = 1;
      for (int i = 1; i <= 9; ++i){
        map2[i] = map2[i-1] * i;
        DEBUG("%d!=%d\n", i, map2[i]);
      }

      for (auto const& [key, val] : map2)
        // assert(map1[key] == val);
        std::cout << key << " " << val << std::endl;
      INFO("part1 - done")
      // assert(map1 == map2); // todo: comparing allocators is tricky task...
    }

    {
      cont::slist<int> sl1(10);
      std::iota(sl1.begin(), sl1.end(), 0);
      std::copy(sl1.begin(), sl1.end(), std::ostream_iterator<int>(std::cout, " "));
      std::cout << std::endl;

      cont::slist<int, alloc::bpool_alloc<int, 10>> sl2(10);
      std::iota(sl2.begin(), sl2.end(), 0);
      std::copy(sl2.begin(), sl2.end(), std::ostream_iterator<int>(std::cout, " "));
      std::cout << std::endl;
      
      assert(sl1 == sl2); // comparing custom list container is done
      INFO("part2 - done")
    }
    INFO("The END")
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