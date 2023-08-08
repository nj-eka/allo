include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git # https://google.github.io/googletest/
    GIT_TAG        v1.13.0
)
FetchContent_MakeAvailable(googletest)