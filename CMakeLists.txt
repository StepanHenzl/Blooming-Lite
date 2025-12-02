cmake_minimum_required(VERSION 3.16)
set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "")
project(index)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")


file(GLOB SOURCES "src/*.cpp" "custom/*.cpp")
file(GLOB HEADERS "src/*.h" "custom/*.h")

add_executable(index ${SOURCES} ${HEADERS})

target_compile_options(index PRIVATE -Wall -Wextra -Wpedantic)

if(EMSCRIPTEN)
  set(CMAKE_EXECUTABLE_SUFFIX ".html" CACHE INTERNAL "")
  target_link_options(index PRIVATE
    "--pre-js=${CMAKE_SOURCE_DIR}/preload.js" 
    "--preload-file=${CMAKE_SOURCE_DIR}/assets@/assets"
    "-sSTACK_SIZE=16777216"
    "--shell-file=${CMAKE_SOURCE_DIR}/Template.html"
    "-sALLOW_MEMORY_GROWTH=1"
    "--js-library=${CMAKE_SOURCE_DIR}/gl_render.js"
    "-sMAX_WEBGL_VERSION=2"
    "-sFULL_ES3=1"
    "-sEXPORTED_RUNTIME_METHODS=[\"ccall\",\"HEAPU8\"]"
    "-sEXPORTED_FUNCTIONS=['_main','_start_game']"
    "-sOFFSCREEN_FRAMEBUFFER=1"
    "-sFORCE_FILESYSTEM=1"
    "-sEXIT_RUNTIME=1"
  )
endif()

file(GLOB JS_FILES "${CMAKE_SOURCE_DIR}/js/*.js")

foreach(JS_FILE ${JS_FILES})
    add_custom_command(
        TARGET index POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${JS_FILE}
            $<TARGET_FILE_DIR:index>
    )
endforeach()

add_custom_command(
    TARGET index POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:index>/CSS
)

file(GLOB PNG_FILES "${CMAKE_SOURCE_DIR}/CSS/*.png")

foreach(PNG_FILE ${PNG_FILES})
    add_custom_command(
        TARGET index POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${PNG_FILE}
            $<TARGET_FILE_DIR:index>/CSS
    )
endforeach()

file(GLOB WAV_FILES "${CMAKE_SOURCE_DIR}/CSS/*.wav")

foreach(WAV_FILE ${WAV_FILES})
    add_custom_command(
        TARGET index POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${WAV_FILE}
            $<TARGET_FILE_DIR:index>/CSS
    )
endforeach()