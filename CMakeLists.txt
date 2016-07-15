cmake_minimum_required (VERSION 3.0)
project(drift-schemy)
set(CXX_STANDARD_REQUIRED ON)

set(core_files
    cmd.hh
    env.hh
    lex.hh
    lib.hh
    tokens.hh
    variant.hh
    env.cc
    lex.cc
    lib.cc
    parse.cc
    variant.cc
    context.hh
    context.cc
	schemyfn.cc)

add_library(drift_schemy ${core_files})

add_executable(drift_schemy_test main.cc)

target_link_libraries(drift_schemy_test drift_schemy)
    
set_property(TARGET drift_schemy PROPERTY CXX_STANDARD 14)
set_property(TARGET drift_schemy_test PROPERTY CXX_STANDARD 14)

configure_file(test.scm test.scm COPYONLY)
configure_file(README.md README.md COPYONLY)

configure_file("env.hh" "${CMAKE_CURRENT_SOURCE_DIR}/drift-schemy/env.hh")
configure_file("lex.hh" "${CMAKE_CURRENT_SOURCE_DIR}/drift-schemy/lex.hh")
configure_file("variant.hh" "${CMAKE_CURRENT_SOURCE_DIR}/drift-schemy/variant.hh")
configure_file("context.hh" "${CMAKE_CURRENT_SOURCE_DIR}/drift-schemy/context.hh")
configure_file("tokens.hh" "${CMAKE_CURRENT_SOURCE_DIR}/drift-schemy/tokens.hh")

if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG OR CMAKE_COMPILER_IS_CLANGCXX)
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
endif()
