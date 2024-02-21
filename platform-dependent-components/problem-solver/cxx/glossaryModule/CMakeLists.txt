set(GLOSSARY_MODULE_SRC "${PROBLEM_SOLVER_PATH}/cxx/glossaryModule")

set(SOURCES
        "keynodes/GlossaryKeynodes.cpp" 
        "keynodes/GlossaryKeynodes.hpp"
        "GlossaryModule.cpp" 
        "GlossaryModule.hpp"
)

include_directories(
    ${CMAKE_CURRENT_LIST_DIR}
    ${SC_MEMORY_SRC}
    ${SC_KPM_SRC}
)

add_library(glossary SHARED ${SOURCES})
target_link_libraries(glossary sc-memory sc-agents-common)

sc_codegen_ex(glossary ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/generated)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(glossary)
endif()

add_definitions(-DGLOSSARY_MODULE_PATH="${CMAKE_CURRENT_LIST_DIR}/")
# if(${SC_BUILD_TESTS})
#     include(${CMAKE_CURRENT_LIST_DIR}/test/tests.cmake)
# endif()