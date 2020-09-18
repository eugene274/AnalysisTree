

add_library(AnalysisTreeRDF SHARED AnalysisTreeBranchRDataSourceImpl.cpp AnalysisTreeBranchRDataSourceImpl.hpp AnalysisTreeRDF.cpp AnalysisTreeRDF.hpp)
target_link_libraries(AnalysisTreeRDF PUBLIC AnalysisTreeBase ${ROOT_LIBRARIES})
target_include_directories(AnalysisTreeRDF
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
        )

if (AnalysisTree_BUILD_TESTS)
    include(GoogleTest)
    add_executable(AnalysisTreeRDF_UnitTests AnalysisTreeBranchRDataSourceImpl.test.cpp)
    target_link_libraries(AnalysisTreeRDF_UnitTests PRIVATE gtest_main AnalysisTreeRDF )
    gtest_add_tests(TARGET AnalysisTreeRDF_UnitTests)
endif(AnalysisTree_BUILD_TESTS)

