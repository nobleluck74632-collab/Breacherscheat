cmake_minimum_required(VERSION 3.18.1)

project(BreachersCheatQuest2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(breachers_cheat SHARED
    gl_renderer.cpp
    memory_manager.cpp
    anti_cheat_bypass.cpp
    secure_payload.cpp
    persistence_service.cpp
)

find_library(log-lib log)
find_library(android-lib android)
find_library(EGL-lib EGL)
find_library(GLESv3-lib GLESv3)

target_link_libraries(breachers_cheat
    ${log-lib}
    ${android-lib}
    ${EGL-lib}
    ${GLESv3-lib}
    -landroid
    -lEGL
    -lGLESv3
)

target_include_directories(breachers_cheat PRIVATE ${CMAKE_SOURCE_DIR}/../../../cpp/include)
