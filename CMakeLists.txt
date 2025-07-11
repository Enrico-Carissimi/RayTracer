cmake_minimum_required(VERSION 3.31.6)

project(RayTracer VERSION 1.0.0
        DESCRIPTION "Simple ray tracer for a university project")

# specify the C++ standard
add_library(compilerFlags INTERFACE)
target_compile_features(compilerFlags INTERFACE cxx_std_20)

# add the executable
add_executable(RayTracer src/main.cpp)
target_include_directories(RayTracer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/lib)
target_link_libraries(RayTracer PUBLIC compilerFlags)

# testing

# add test executables and add tests
function(custom_add_test testName fileName)
    add_executable(${testName} "${CMAKE_CURRENT_SOURCE_DIR}/test/${fileName}.cpp")
    target_include_directories(${testName} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/lib)
    target_link_libraries(${testName} PUBLIC compilerFlags)
    add_test(NAME ${testName} COMMAND ${testName})
endfunction()

enable_testing()

custom_add_test(TestColor testColor)
custom_add_test(TestHDRImage testHDRImage)
custom_add_test(TestPFMReader testPFMReader)
custom_add_test(TestGeometry testGeometry)
custom_add_test(TestRay testRay)
custom_add_test(TestCamera testCamera)
custom_add_test(TestShapes testShapes)
custom_add_test(TestPCG testPCG)
custom_add_test(TestTextures testTextures)
custom_add_test(TestRenderers testRenderers)
custom_add_test(TestScenefile testScenefile)