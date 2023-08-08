###################
###### CMake ######
###################
cmake_minimum_required(VERSION 3.26)
message("CMAKE_VERSION = ${CMAKE_VERSION}")

# set(RELEASE_VERSION)
# set(PACKAGE DEB)

# https://stackoverflow.com/questions/67571149/how-can-i-use-boost-log-with-cmake-find-package-error
cmake_policy(SET CMP0074 NEW)

#####################
###### modules ######
#####################

file(GLOB MODULE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/cmake/*/" LIST_DIRECTORIES true) # "${PROJECT_SOURCE_DIR}/cmake"
list(APPEND CMAKE_MODULE_PATH "${MODULE_DIRS}")

##### props ######
include(target_properties)

#####################
###### VERSION ######
#####################
include(git_version_detect)

####################
###### Project #####
####################
project(allo 
    VERSION ${GITVERSIONDETECT_VERSION_MAJOR}.${GITVERSIONDETECT_VERSION_MINOR}.${GITVERSIONDETECT_VERSION_PATCH}
    DESCRIPTION "course c++ developer - hw3"
    LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# message("CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
# message("CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")
# message("CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
# message("CMAKE_GENERATOR: ${CMAKE_GENERATOR}")

##################
###### VARS ######
##################
include(CMakePrintHelpers)
get_cmake_property(varNames VARIABLES)
cmake_print_variables(${varNames})

######################
###### SYS_INFO ######
######################
include(CMakePrintSystemInformation)
execute_process(COMMAND "${CMAKE_COMMAND}" "-E" "environment")

##########################
###### Dependencies ######
##########################

## BOOST
# add_definitions(-BOOST_LOG_DYN_LINK)
set(BOOST_ENABLE_CMAKE ON)
set(Boost_FIND_QUIETLY OFF)
set(Boost_USE_STATIC_LIBS ON)

# set(Boost_USE_MULTITHREADED ON)

# add_definitions(-DBOOST_LOG_DYN_LINK)
# set(Boost_USE_STATIC_RUNTIME OFF)
# include(fetch_boost) # https://github.com/boostorg/program_options.git
# set(CMAKE_FIND_DEBUG_MODE ON)
# find_package(Boost 1.82 REQUIRED COMPONENTS program_options)
find_package(Boost REQUIRED COMPONENTS program_options log_setup log)
# set(CMAKE_FIND_DEBUG_MODE OFF)

# # https://github.com/boostorg/log/issues/46?ysclid=ll2b2ty8ys162328322
# find_library(loglib libboost_log.a)
# target_link_libraries(Boost::log INTERFACE ${loglib})

if (Boost_FOUND)
    message("Boost VERSION: ${Boost_VERSION}")
    message("Boost INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message("Boost Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")
    message("Boost LIBRARIES: ${Boost_LIBRARIES}")

    include_directories(${Boost_INCLUDE_DIRS}) # -> set(TARGETS ... INCLUDE_DIRECTORIES ...)
    link_directories(${Boost_LIBRARY_DIRS})    
endif()

# (file)Boost.ProgramOptions + (compiler opton)libboost_program_options

## GTEST
# v0
# include(FetchContent)
# FetchContent_Declare(
#   googletest
#   URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
# )
# # For Windows: Prevent overriding the parent project's compiler/linker settings
# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# FetchContent_MakeAvailable(googletest)

# v1
# include(fetch_gtest)

# v2
find_package(GTest 1.13 REQUIRED)

## FMT
# include(fetch_fmt) - replaced by cxx23 standard

## RANGE
# include(fetch_rangev3) - replaced by cxx23 standard

##############################
######  Compile settings #####
##############################
# enable all warnings and treat them as errors
# list(APPEND CMAKE_CXX_FLAGS "-Wall ...")
# add_compile_options(-Wall -Wextra -Wpedantic -Wno-error=sized-deallocation -Wno-sized-deallocation -fPIC -fopenmp -DBOOST_LOG_DYN_LINK -lpthread -lboost_log -lboost_log_setup -pthread -ldl) # -Werror -std=c++23 -lstdc++_libbacktrace
add_compile_options(-Wall -Wextra -Wpedantic -Wno-error=sized-deallocation -Wno-sized-deallocation -pthread -lboost_log -lpthread -ldl) # -Werror -std=c++23 -lstdc++_libbacktrace

#######################
###### Clang-tidy #####
#######################
# # if clang-tidy is not already set, it is searched for here
# # If using a CMake version above 3.19 it is recommended to configure clang-tidy in a preset
# if(ENABLE_CLANG_TIDY)
#     if(NOT CMAKE_CXX_CLANG_TIDY)
#         find_program(CLANG_TIDY_EXECUTABLE NAMES clang-tidy)
#         if(CLANG_TIDY_EXECUTABLE)
#             list(APPEND CLANG_TIDY_COMMAND "${CLANG_TIDY_EXECUTABLE}"
#     "-checks=-*,readability-*,modernize-use-nullptr,-extra-arg=-std=c++2b"
#             )
#         endif()
#     else()
#         message(WARNING "Failed to find CLANG_TIDY.")        
#     endif()
# endif()

###########################
###### code_coverage ######
###########################
if(ENABLE_COVERAGE) # todo: maybe combined with BUILD_TESTING, DEBUG ...
    include(CodeCoverage)
endif()

######################################
###### Enable tests at top level #####
######################################
if(PROJECT_IS_TOP_LEVEL AND BUILD_TESTING)
    include(CTest)
    enable_testing()
endif()

######################
###### Libraries #####
######################
add_subdirectory(src/info) # static

########################
###### Application #####
########################
add_executable(allo)
target_sources(
    allo
    PRIVATE 
        "src/main.cpp"
)

target_compile_features(
    allo
    PRIVATE
        cxx_std_23 # to be sure
)

target_include_directories(
    allo
    PRIVATE
        ${Boost_INCLUDE_DIRS}
        src
        src/alloc
        src/cont
        src/utils
        src/info/include 
)

target_link_libraries(
    allo
        ${Boost_LIBRARIES} ${Boost_LOG_LIBRARY} ${Boost_LOG_SETUP_LIBRARY}
        Boost::program_options
        Boost::log_setup
        Boost::log
        # Boost::serialization
        # Boost::system
        # Boost::date_time     
        pthread
        dl    
        info_static    
)

# if (ENABLE_CLANG_TIDY)
#     set_target_properties(
#         allo
#         PROPERTIES
#             CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}"
#     )    
# endif()

print_target_properties(allo)

if(BUILD_TESTING)
    find_package(GTest REQUIRED)
    add_executable(
        alloc_cont_gtest
        "src/alloc/bpool_gtest.cpp"
        "src/alloc/bpool_alloc_gtest.cpp"
        "src/cont/slist_gtest.cpp"
    )
    set_target_properties(
        alloc_cont_gtest
        PROPERTIES
            CXX_STANDARD ${CMAKE_CXX_STANDARD}
            CXX_STANDARD_REQUIRED ${CMAKE_CXX_STANDARD_REQUIRED}
    )
    target_include_directories(
        alloc_cont_gtest
        PRIVATE
            ${GTEST_INCLUDE_DIRS}
            "${CMAKE_CURRENT_SOURCE_DIR}/src" 
            "${CMAKE_CURRENT_SOURCE_DIR}/src/alloc"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/cont"
    )
    target_link_libraries(
        alloc_cont_gtest
        PRIVATE
            ${GTEST_BOTH_LIBRARIES}
    )
    add_test(alloc_cont_gtest alloc_cont_gtest)
    if (MSVC)
        target_compile_options(
            alloc_cont_gtest
            PRIVATE  
                /W4
        )
    else()
        target_compile_options(
            alloc_cont_gtest
            PRIVATE
                -Wall -Wextra -pedantic -Werror
        )
    endif(MSVC) 

    if(ENABLE_COVERAGE)
        append_coverage_compiler_flags_to_target(allo)
    endif()    
endif(BUILD_TESTING)

install(TARGETS allo)

if (RELEASE_VERSION)
    set(CPACK_GENERATOR "${PACKAGE}")
    set(CPACK_PACKAGE_VERSION "${RELEASE_VERSION}")
    set(CPACK_PACKAGE_CONTACT "${GIT_DESCRIBE_PROJECT_REPOSITORY_URL}")
    include(CPack)
endif()