cmake_minimum_required(VERSION 3.31.6)

project(RayTracer VERSION 0.1.0)

# specify the C++ standard
add_library(compilerFlags INTERFACE)
target_compile_features(compilerFlags INTERFACE cxx_std_17)

# add the executable
add_executable(RayTracer main.cpp)
target_link_libraries(RayTracer PUBLIC compilerFlags)

# add the test executables
add_executable(TestColor test/testColor.cpp)
target_link_libraries(TestColor PUBLIC compilerFlags)
add_executable(TestHDRImage test/testHDRImage.cpp)
target_link_libraries(TestHDRImage PUBLIC compilerFlags)
add_executable(TestPFMReader test/testPFMReader.cpp)
target_link_libraries(TestPFMReader PUBLIC compilerFlags)
add_executable(TestGeometry test/testGeometry.cpp)
target_link_libraries(TestGeometry PUBLIC compilerFlags)
add_executable(TestRay test/testRay.cpp)
target_link_libraries(TestRay PUBLIC compilerFlags)
add_executable(TestCamera test/testCamera.cpp)
target_link_libraries(TestCamera PUBLIC compilerFlags)

# testing
enable_testing()
add_test(NAME Color COMMAND TestColor)
add_test(NAME HDRImage COMMAND TestHDRImage)
add_test(NAME PFMReader COMMAND TestPFMReader)
add_test(NAME geometry COMMAND TestGeometry)
add_test(NAME Ray COMMAND TestRay)
add_test(NAME Camera COMMAND TestCamera)
