cmake_minimum_required (VERSION 3.16)

SET (CMAKE_C_COMPILER             "clang")
SET (CMAKE_CXX_COMPILER           "clang++")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -v")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lc++")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libstdc++ -v")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++")

ENABLE_LANGUAGE(CXX)
project(tp VERSION 1.0)

# add_definitions(-DDUSE_PTHREAD)

set(CMAKE_CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_THREAD_LIBS_INIT "-lpthread")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")


# includes
set (COMMON_INCLUDES
	# $ENV{HOME}/repo/oss/ac-library
	/usr/lib/llvm-18/include
	/usr/lib/llvm-18/c++/v1
	# "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include"
	# "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1"
	# "/opt/homebrew/opt/llvm/include"
	# "/opt/homebrew/opt/llvm/include/c++/v1"
	# "/opt/homebrew/opt/llvm/include/clang/ASG"
	# "/opt/homebrew/opt/llvm/lib/clang/12.0.1/include"
	)
# sources
set(COMMON_SOURCES )

set(TARGET_SOURCES main.cpp
	# operator_overload.cpp
	)
set(TEST_SOURCES test_main.cpp)

# default target 'spike.out'
add_executable(${PROJECT_NAME}.out ${COMMON_SOURCES} ${TARGET_SOURCES})
add_executable(test_${PROJECT_NAME}.out ${COMMON_SOURCES} ${TEST_SOURCES})

# target options
target_compile_options(${PROJECT_NAME}.out PRIVATE -O0 -g -Wall)
target_compile_features(${PROJECT_NAME}.out PRIVATE cxx_std_20)

target_compile_options(test_${PROJECT_NAME}.out PRIVATE -O0 -g -Wall)
target_compile_features(test_${PROJECT_NAME}.out PRIVATE cxx_std_20)

# use precompiled header
target_precompile_headers(${PROJECT_NAME}.out PRIVATE headers.hpp)
target_precompile_headers(test_${PROJECT_NAME}.out PRIVATE headers.hpp)

# include
target_include_directories(${PROJECT_NAME}.out PUBLIC
	"${PROJECT_BINARY_DIR}"
	"${PROJECT_SOURCE_DIR}"
	"${COMMON_INCLUDES}"
	)
target_include_directories(test_${PROJECT_NAME}.out PUBLIC
	"${PROJECT_BINARY_DIR}"
	"${PROJECT_SOURCE_DIR}"
	"/opt/homebrew/opt/googletest/include/"
	"$ENV{HOME}/repo/oss/googletest/googletest/include/"
	"${COMMON_INCLUDES}"
	)
target_link_directories(test_${PROJECT_NAME}.out PRIVATE
	"/opt/homebrew/opt/googletest/lib/"
	"$ENV{HOME}/repo/oss/googletest/build/lib/"
	"/usr/lib/aarch64-linux-gnu"
	"/usr/lib/llvm-18/lib"
	)

target_link_libraries(test_${PROJECT_NAME}.out PRIVATE
	"c++"
	"c++abi"
	"gtest_main"
	"gtest"
	"pthread"
	)

# custom target: run
# uses custom command. otherwise, stdin for lldb is closed by ninja
add_custom_target(run
	DEPENDS ${PROJECT_NAME}.out
	DEPENDS run_command
	)
add_custom_command(OUTPUT run_command
	USES_TERMINAL
	COMMAND ${PROJECT_NAME}.out
	)

# custom target: copy
if (APPLE)
	set(CLIP pbcopy)
else()
	set(CLIP clip.exe)
endif()

add_custom_target(copy COMMAND echo > cmake_tmp
	COMMAND tail +2 ${PROJECT_SOURCE_DIR}/main.cpp >> cmake_tmp
	COMMAND cat ${PROJECT_SOURCE_DIR}/headers.hpp cmake_tmp | ${CLIP}
	COMMAND rm cmake_tmp
	DEPENDS ${PROJECT_NAME}.out
	)

# custom target: runtest
add_custom_target(runtest COMMAND test_${PROJECT_NAME}.out)

# custom target: lldb
# uses custom command. otherwise, stdin for lldb is closed by ninja
add_custom_target(lldb
	DEPENDS ${PROJECT_NAME}.out
	DEPENDS lldb_command
	)

add_custom_command(OUTPUT lldb_command
	USES_TERMINAL
	COMMAND lldb ${PROJECT_NAME}.out
	)
