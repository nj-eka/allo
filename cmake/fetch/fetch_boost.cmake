# Download and Extract the Boost Library from GitHub
# use before include:
# set(BOOST_INCLUDE_LIBRARIES thread filesystem system)
set(BOOST_ENABLE_CMAKE ON)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(
    Boost::program_options 
    GIT_REPOSITORY https://github.com/boostorg/program_options.git
    GIT_TAG boost-1.82.0
    GIT_PROGRESS TRUE   
)
FetchContent_MakeAvailable(Boost::program_options)

# FetchContent_Declare(
#     Boost
#     GIT_REPOSITORY https://github.com/boostorg/boost.git
#     GIT_TAG boost-1.82.0
#     GIT_PROGRESS TRUE   
# )
# FetchContent_MakeAvailable(Boost)
# use after:
# add_executable(boost_test boost_test.cpp)
# target_link_libraries(boost_test PRIVATE Boost::program_options)

# cmake_minimum_required(VERSION 3.24)
# project(p)

# set(BOOST_INCLUDE_LIBRARIES thread filesystem system program_options)
# set(BOOST_ENABLE_CMAKE ON)

# include(FetchContent)
# FetchContent_Declare(
#   Boost
#   GIT_REPOSITORY https://github.com/boostorg/boost.git
#   GIT_TAG boost-1.80.0
# )
# FetchContent_MakeAvailable(Boost)

# add_executable(boost_test boost_test.cpp)
# target_link_libraries(boost_test PRIVATE Boost::filesystem
#                                          Boost::program_options)