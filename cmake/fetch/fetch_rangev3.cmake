include(FetchContent)
FetchContent_Declare(
    range-v3 # Range library for C++14/17/20
    GIT_REPOSITORY "https://github.com/ericniebler/range-v3" 
    GIT_TAG 0.19.0
)
FetchContent_MakeAvailable(range-v3)

# conan version: https://github.com/ericniebler/range-v3