cmake_minimum_required(VERSION 3.17)

project(verum C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

option(PACKAGE_TESTS "Build the tests" ON)

set(verum_INSTALL_default ON)
if(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
   set(verum_INSTALL_default OFF)
endif()
set(verum_INSTALL ${verum_INSTALL_default} CACHE BOOL "Generate installation target")
set(verum_INSTALL_BINDIR "bin" CACHE STRING "Installation directory for executables")
set(verum_INSTALL_LIBDIR "lib" CACHE STRING "Installation directory for libraries")
set(verum_INSTALL_INCLUDEDIR "include" CACHE STRING "Installation directory for headers")
set(verum_INSTALL_CMAKEDIR "lib/cmake/${PACKAGE_NAME}" CACHE STRING "Installation directory for cmake config files")

set(verum_OPTIMIZE ON CACHE BOOL "Optimize verum libraries")

#### third_parties ####

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

find_package(gRPC CONFIG REQUIRED)

#### set optimization options after potential third party overrides ####

if(tenncor_OPTIMIZE)
    message(STATUS "compiler option: ${CMAKE_CXX_COMPILER_ID}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -O3")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -openmp")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
	add_link_options(-lgomp)
    endif()
endif()

#### generate proto files ####

include_directories("${CMAKE_CURRENT_BINARY_DIR}")

find_program(_PROTOBUF_PROTOC protoc ${CONAN_BIN_DIRS_PROTOBUF} NO_DEFAULT_PATH)
set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)

# muta
get_filename_component(MUTA_PROTO "muta/mutation.proto" ABSOLUTE)
set(MUTA_PROTO_SRCS "${CMAKE_CURRENT_BINARY_DIR}/muta/mutation.pb.cc")
set(MUTA_PROTO_HDRS "${CMAKE_CURRENT_BINARY_DIR}/muta/mutation.pb.h")
set(MUTA_GRPC_SRCS "${CMAKE_CURRENT_BINARY_DIR}/muta/mutation.grpc.pb.cc")
set(MUTA_GRPC_HDRS "${CMAKE_CURRENT_BINARY_DIR}/muta/mutation.grpc.pb.h")
add_custom_command(
    OUTPUT "${MUTA_PROTO_SRCS}" "${MUTA_PROTO_HDRS}" "${MUTA_GRPC_SRCS}" "${MUTA_GRPC_HDRS}"
    COMMAND ${_PROTOBUF_PROTOC}
    ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${CMAKE_CURRENT_SOURCE_DIR}"
		--plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${MUTA_PROTO}"
    DEPENDS "${MUTA_PROTO}"
)
add_library(muta_proto_obj OBJECT ${MUTA_PROTO_SRCS} ${MUTA_GRPC_SRCS})

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

#### local libraries ####

include_directories(
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/.>
    $<INSTALL_INTERFACE:.>
)

# exam
add_library(exam INTERFACE)
target_link_libraries(exam INTERFACE ${CONAN_LIBS_CISAB} ${CONAN_LIBS_GTEST})

# muta
add_library(muta
    muta/src/mutator.cpp
    muta/src/random_generator.cpp
    $<TARGET_OBJECTS:muta_proto_obj>
)
target_link_libraries(muta PUBLIC
    ${CONAN_LIBS_CISAB} ${CONAN_LIBS_GTEST} gRPC::grpc++ gRPC::grpc++_unsecure)

#### install ####
if(verum_INSTALL)
    foreach(_HDR
        exam/exam.hpp
        exam/mock_log.hpp
        exam/nosupport_log.hpp
    )
        get_filename_component(_PATH ${_HDR} PATH)
        install(FILES ${_HDR} DESTINATION "${verum_INSTALL_INCLUDEDIR}/${_PATH}")
    endforeach()

    set(TargetFile ${PROJECT_NAME}Targets)
    install(TARGETS
        exam
        EXPORT ${TargetFile}
        RUNTIME DESTINATION ${verum_INSTALL_BINDIR}
        LIBRARY DESTINATION ${verum_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${verum_INSTALL_LIBDIR}
    )
    install(EXPORT ${TargetFile}
        DESTINATION ${verum_INSTALL_CMAKEDIR}
        NAMESPACE ${PROJECT_NAME}::
    )
endif()

#### tests ####

if(PACKAGE_TESTS)
    enable_testing()
endif()

# exam
set(EXAM_TEST exam_test)
add_executable(${EXAM_TEST} exam/test/main.cpp)
target_link_libraries(${EXAM_TEST} exam)
add_test(NAME ${EXAM_TEST} COMMAND ${EXAM_TEST})

# muta
set(MUTA_TEST muta_test)
add_executable(${MUTA_TEST} muta/test/main.cpp)
target_link_libraries(${MUTA_TEST} muta)
add_test(NAME ${MUTA_TEST} COMMAND ${MUTA_TEST})
