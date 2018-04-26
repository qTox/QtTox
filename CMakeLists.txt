project(QtTox CXX)

cmake_minimum_required(VERSION 3.10)

add_library(libqttox
    STATIC
    include/chatlist.h
    include/common.h
    include/conference.h
    include/core.h
    include/files.h
    include/lowlevel.h
    include/messenger.h
    include/options.h
    include/self.h
    include/toxstring.h
    include/version.h
    src/chatlist.cpp
    src/messenger.cpp
    src/toxstring.cpp
)

find_package(Qt5Core   REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_search_module(TOXCORE        REQUIRED toxcore)
pkg_search_module(TOXAV          REQUIRED toxav)
pkg_search_module(TOXENCRYPTSAVE REQUIRED toxencryptsave)

target_link_libraries(libqttox Qt5::Core)
include_directories(
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_SOURCE_DIR}/src"
        )

