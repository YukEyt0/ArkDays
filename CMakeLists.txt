cmake_minimum_required(VERSION 3.15)

project(final LANGUAGES CXX)

set(SOURCE_PATH ${CMAKE_SOURCE_DIR}/src)
set(THRID_PARTY_LIBRARY_PATH ${CMAKE_SOURCE_DIR}/external)

#copy data
file(COPY "media/" DESTINATION "media")
file(COPY "screenshot/" DESTINATION "screenshot")

add_subdirectory(${THRID_PARTY_LIBRARY_PATH}/glad)
add_subdirectory(${THRID_PARTY_LIBRARY_PATH}/glfw)
add_subdirectory(${THRID_PARTY_LIBRARY_PATH}/glm)
add_subdirectory(${THRID_PARTY_LIBRARY_PATH}/imgui)
add_subdirectory(${THRID_PARTY_LIBRARY_PATH}/stb)

file(GLOB BASE_HDR ${SOURCE_PATH}/base/*.h)
file(GLOB BASE_SRC ${SOURCE_PATH}/base/*.cpp)
file(GLOB PROJECT_HDR ${SOURCE_PATH}/*.h)
file(GLOB PROJECT_SRC ${SOURCE_PATH}/*.cpp)

add_executable(final ${BASE_HDR} ${BASE_SRC} ${PROJECT_HDR} ${PROJECT_SRC})

if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
elseif(UNIX)
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        set_target_properties(${PROJECT_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Debug")
    else()
        set_target_properties(${PROJECT_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Release")
    endif()
endif()

target_include_directories(
    final PUBLIC
    ${THRID_PARTY_LIBRARY_PATH}/glfw/include
    ${THRID_PARTY_LIBRARY_PATH}/glad/include
    ${THRID_PARTY_LIBRARY_PATH}/glm
    ${THRID_PARTY_LIBRARY_PATH}/imgui
    ${THRID_PARTY_LIBRARY_PATH}/stb
)

target_link_libraries(final PUBLIC glfw glad glm imgui stb)