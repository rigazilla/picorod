cmake_minimum_required(VERSION 3.0)
project(picorod VERSION 0.0.1 DESCRIPTION "Minimal support to execute basic Infinispan operations")
add_library(picorod SHARED
    src/picorod.c)
set_target_properties(picorod PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(picorod PROPERTIES PUBLIC_HEADER include/picorod.h)
target_include_directories(picorod PRIVATE .)
include(GNUInstallDirs)
install(TARGETS picorod
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
configure_file(picorod.pc.in picorod.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/picorod.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

add_library(hotrod_over_tcp SHARED
    src/hotrod_over_tcp.c)

add_executable(example src/example.c)
