set(SEMANTIC_NEIGHBORHOOD_MODULE_SRC "${PROBLEM_SOLVER_PATH}/cxx/semanticNeighborhoodModule")

set(SOURCES
        "utils/Utils.hpp"
        "utils/Utils.cpp"
        "agent/ObtainingSemanticNeighborhoodAgent.hpp"
        "agent/ObtainingSemanticNeighborhoodAgent.cpp"
        "keynodes/Keynodes.cpp" 
        "keynodes/Keynodes.hpp"
        "SemanticNeighborhoodModule.cpp" 
        "SemanticNeighborhoodModule.hpp"
)

include_directories(
    ${CMAKE_CURRENT_LIST_DIR}
    ${SC_MEMORY_SRC}
    ${SC_KPM_SRC}
)

add_library(semanticNeignborhood SHARED ${SOURCES})
target_link_libraries(semanticNeignborhood sc-memory sc-agents-common)

sc_codegen_ex(semanticNeignborhood ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/generated)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(semanticNeignborhood)
endif()

add_definitions(-DSEMANTIC_NEIGHBORHOOD_MODULE_PATH="${CMAKE_CURRENT_LIST_DIR}/")
if(${SC_BUILD_TESTS})
    include(${CMAKE_CURRENT_LIST_DIR}/test/tests.cmake)
endif()