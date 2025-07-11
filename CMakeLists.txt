cmake_minimum_required(VERSION 3.31.6)

project(RayTracer VERSION 0.1.0)

# specify the C++ standard
add_library(compilerFlags INTERFACE)
target_compile_features(compilerFlags INTERFACE cxx_std_20)

# add the executable
add_executable(RayTracer src/main.cpp)
target_include_directories(RayTracer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(RayTracer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib)
target_link_libraries(RayTracer PUBLIC compilerFlags)



# add the test executables
add_executable(TestColor test/testColor.cpp)
target_include_directories(TestColor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(TestColor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib)
target_link_libraries(TestColor PUBLIC compilerFlags)

add_executable(TestHDRImage test/testHDRImage.cpp)
target_include_directories(TestHDRImage PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(TestHDRImage PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib)
target_link_libraries(TestHDRImage PUBLIC compilerFlags)

add_executable(TestPFMReader test/testPFMReader.cpp)
target_include_directories(TestPFMReader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(TestPFMReader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib)
target_link_libraries(TestPFMReader PUBLIC compilerFlags)

add_executable(TestGeometry test/testGeometry.cpp)
target_include_directories(TestGeometry PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(TestGeometry PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib)
target_link_libraries(TestGeometry PUBLIC compilerFlags)

add_executable(TestRay test/testRay.cpp)
target_include_directories(TestRay PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(TestRay PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib)
target_link_libraries(TestRay PUBLIC compilerFlags)

add_executable(TestCamera test/testCamera.cpp)
target_include_directories(TestCamera PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(TestCamera PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib)
target_link_libraries(TestCamera PUBLIC compilerFlags)

add_executable(TestShapes test/testShapes.cpp)
target_include_directories(TestShapes PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(TestShapes PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib)
target_link_libraries(TestShapes PUBLIC compilerFlags)

add_executable(TestPCG test/testPCG.cpp)
target_include_directories(TestPCG PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(TestPCG PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib)
target_link_libraries(TestPCG PUBLIC compilerFlags)

add_executable(TestTextures test/testTextures.cpp)
target_include_directories(TestTextures PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(TestTextures PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib)
target_link_libraries(TestTextures PUBLIC compilerFlags)

add_executable(TestRenderers test/testRenderers.cpp)
target_include_directories(TestRenderers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(TestRenderers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib)
target_link_libraries(TestRenderers PUBLIC compilerFlags)

add_executable(TestScenefile test/testScenefile.cpp)
target_include_directories(TestScenefile PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(TestScenefile PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib)
target_link_libraries(TestScenefile PUBLIC compilerFlags)

# testing
enable_testing()
add_test(NAME Color COMMAND TestColor)
add_test(NAME HDRImage COMMAND TestHDRImage)
add_test(NAME PFMReader COMMAND TestPFMReader)
add_test(NAME geometry COMMAND TestGeometry)
add_test(NAME Ray COMMAND TestRay)
add_test(NAME Camera COMMAND TestCamera)
add_test(NAME Shapes COMMAND TestShapes)
add_test(NAME PCG COMMAND TestPCG)
add_test(NAME textures COMMAND TestTextures)
add_test(NAME renderers COMMAND TestRenderers)
add_test(NAME scenefile COMMAND TestScenefile)