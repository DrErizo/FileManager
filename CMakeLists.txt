cmake_minimum_required(VERSION 3.14.15926)
project(file-manager)
set(CMAKE_CXX_STANDARD 20)
set(SFML_BUILD_AUDIO FALSE)
set(SFML_BUILD_NETWORK FALSE)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

add_compile_options(-g)
add_compile_options(-Os)

file(GLOB_RECURSE SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)
set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)
