cmake_minimum_required(VERSION 3.10)
project("ExtremePong")

# Set C++ standard (optional)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

# Create the exe
add_executable(ExtremePong "src/main.cpp" "src/systems.cpp") 

target_link_libraries(ExtremePong PRIVATE 
    sfml-graphics 
    sfml-window
    sfml-system
    sfml-network
    sfml-audio
)
