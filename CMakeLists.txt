cmake_minimum_required (VERSION 3.0)
project(drift-scheme-test)
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
    
add_executable(drift_scheme_test
    ${core_files}
    main.cc)
    
set_property(TARGET drift_scheme_test PROPERTY CXX_STANDARD 14)

configure_file(test.scm test.scm COPYONLY)
configure_file(README.md README.md COPYONLY)

configure_file("env.hh" "${CMAKE_CURRENT_SOURCE_DIR}/drift-schemy/env.hh")
configure_file("lex.hh" "${CMAKE_CURRENT_SOURCE_DIR}/drift-schemy/lex.hh")
configure_file("variant.hh" "${CMAKE_CURRENT_SOURCE_DIR}/drift-schemy/variant.hh")
configure_file("context.hh" "${CMAKE_CURRENT_SOURCE_DIR}/drift-schemy/context.hh")
configure_file("tokens.hh" "${CMAKE_CURRENT_SOURCE_DIR}/drift-schemy/tokens.hh")
