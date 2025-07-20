cmake_minimum_required(VERSION 3.31.6)

project(RayTracer VERSION 1.1.0
        DESCRIPTION "Simple ray tracer for a university project")

# specify the C++ standard with an interface library
add_library(compilerFlags INTERFACE)
target_compile_features(compilerFlags INTERFACE cxx_std_20)

# add compiler flags 
set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")

target_compile_options(compilerFlags INTERFACE
  "$<$<CONFIG:Debug>:$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wformat=2;-Wunused;-Wno-missing-field-initializers>>>"
  "$<$<CONFIG:Debug>:$<${msvc_cxx}:$<BUILD_INTERFACE:-W3>>>"
)



# library containing all cpp files (other than the main)
add_library(raylib src/scenefile.cpp src/PFMReader.cpp src/HDRImage.cpp src/utils.cpp)
target_include_directories(raylib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/external)
target_link_libraries(raylib PUBLIC compilerFlags)

# add the executable
add_executable(RayTracer src/main.cpp)
target_include_directories(RayTracer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/external)
target_link_libraries(RayTracer PUBLIC compilerFlags raylib)



# testing

# add test executables and tests
function(custom_add_test testName fileName)
    add_executable(${testName} "test/${fileName}.cpp")
    target_include_directories(${testName} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/external)
    target_link_libraries(${testName} PUBLIC compilerFlags raylib)
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