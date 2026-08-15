cmake_minimum_required(VERSION 3.15)
project(QomMetro LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_compile_options(-Wall -Wextra)

# --- Dependencies -----------------------------------------------------
# nlohmann/json is fetched at configure time (requires internet access
# on first run only; CMake caches it under build/_deps afterward).
include(FetchContent)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
)
FetchContent_MakeAvailable(json)

# --- Sources ------------------------------------------------------------
# GLOB is used here for convenience (this is a course project, not a
# large production codebase); the trade-off is that CMake must be
# re-run manually (e.g. `cmake ..` again, or just re-running `make`
# after touching CMakeLists.txt) if a new .cpp file is added later,
# since GLOB results are cached at configure time.
file(GLOB_RECURSE QOMMETRO_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

add_executable(QomMetro
    ${QOMMETRO_SOURCES}
    cli/main.cpp
)

target_include_directories(QomMetro PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(QomMetro PRIVATE nlohmann_json::nlohmann_json)

# --- Data files -----------------------------------------------------
# The program reads paths like "data/stations.json" (see
# Utils::kDefaultStationsFile) RELATIVE TO THE CURRENT WORKING
# DIRECTORY at runtime -- not relative to where the executable lives.
# Since the executable normally runs from inside build/, copy data/
# there too, so `./QomMetro` from build/ finds it without the user
# needing to `cd ..` first.
add_custom_command(
    TARGET QomMetro POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/data
        $<TARGET_FILE_DIR:QomMetro>/data
    COMMENT "Copying data/ next to the QomMetro executable"
)

# --- Tests (optional, only built if tests/ has content) -----------------
# Uncomment and expand once tests/*.cpp exist and a test framework
# (e.g. GoogleTest, also via FetchContent) is wired in.
# enable_testing()
# add_subdirectory(tests)
