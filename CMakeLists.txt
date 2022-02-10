cmake_minimum_required(VERSION 3.17)

project(verum C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(PACKAGE_TESTS "Build the tests" ON)

set(verum_INSTALL_default ON)
if(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
   set(verum_INSTALL_default OFF)
endif()
set(verum_INSTALL ${cppkg_INSTALL_default} CACHE BOOL "Generate installation target")
set(verum_INSTALL_BINDIR "bin" CACHE STRING "Installation directory for executables")
set(verum_INSTALL_LIBDIR "lib" CACHE STRING "Installation directory for libraries")
set(verum_INSTALL_INCLUDEDIR "include" CACHE STRING "Installation directory for headers")
set(verum_INSTALL_CMAKEDIR "lib/cmake/${PACKAGE_NAME}" CACHE STRING "Installation directory for cmake config files")

#### third_parties ####

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

#### local libraries ####

include_directories(
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/.>
    $<INSTALL_INTERFACE:.>
)

# exam
add_library(exam INTERFACE)
target_link_libraries(exam INTERFACE ${CONAN_LIBS_CISAB} ${CONAN_LIBS_GTEST})

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
