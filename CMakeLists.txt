cmake_minimum_required(VERSION 3.16)

project(resurse-naturale-server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  crowcpp-crow
  GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
  GIT_TAG v1.2.1.0
)

FetchContent_MakeAvailable(crowcpp-crow)

FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(nlohmann_json)

add_executable(resurse-naturale-server  main.cpp
)

target_link_libraries(resurse-naturale-server PRIVATE Crow::Crow nlohmann_json::nlohmann_json)

include(GNUInstallDirs)
install(TARGETS resurse-naturale-server
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
