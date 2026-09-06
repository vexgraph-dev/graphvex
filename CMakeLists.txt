cmake_minimum_required(VERSION 3.28)
project(graphvex C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(APPLE)
    enable_language(OBJC)
endif()

# Release: dead-code stripping so unused symbols cost zero bytes in artifacts.
# (Duplicated per repo for standalone builds; harmless when built in-tree.)
if(APPLE)
    add_compile_options($<$<CONFIG:Release>:-ffunction-sections> $<$<CONFIG:Release>:-fdata-sections>)
    add_link_options($<$<CONFIG:Release>:-Wl,-dead_strip>)
elseif(UNIX)
    add_compile_options($<$<CONFIG:Release>:-ffunction-sections> $<$<CONFIG:Release>:-fdata-sections>)
    add_link_options($<$<CONFIG:Release>:-Wl,--gc-sections>)
endif()

# Upstream dependency (if not already provided by parent build).
# graphvex is a graphics FOUNDATION layer: it may depend ONLY on vexspoke.
# It must NEVER include headers from hotcwap, darling, or api-haven
# (Rule 17). Those layers link graphvex, never the reverse.
if(NOT TARGET vexspoke)
    option(GRAPHVEX_USE_UPSTREAM "Depend on vexspoke via FetchContent" OFF)
    if(GRAPHVEX_USE_UPSTREAM)
        include(FetchContent)
        FetchContent_Declare(
            vexspoke
            GIT_REPOSITORY https://github.com/vexgraph-dev/vexspoke.git
            GIT_TAG main
        )
        FetchContent_MakeAvailable(vexspoke)
    endif()
endif()

if(NOT TARGET graphvex)
add_library(graphvex STATIC
    src/graphvex/version.c
    src/buffer/buffer.c
    src/buffer/color_buffer.c
    src/buffer/depth_buffer.c
    src/buffer/frame_buffer.c
    src/buffer/height_buffer.c
    src/buffer/normal_buffer.c
    src/buffer/shadow_buffer.c
    src/buffer/stencil_buffer.c
    src/font/font.c
    src/font/font_bake.c
    src/io/vfs.c
    src/vulkan/sdf_gpu.c
    src/vulkan/vk_scene.c
    src/vulkan/vk_view.c
    src/vulkan/vk_iosurface.c
    src/vulkan/texture/texture.c
)

target_include_directories(graphvex PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_options(graphvex PRIVATE -Wall -Wextra -Werror -mcpu=native)

find_package(Vulkan QUIET)
if(Vulkan_FOUND)
    target_include_directories(graphvex PUBLIC ${Vulkan_INCLUDE_DIRS})
elseif(EXISTS "/opt/homebrew/include/vulkan/vulkan.h")
    target_include_directories(graphvex PUBLIC /opt/homebrew/include)
endif()

if(TARGET vexspoke)
    target_link_libraries(graphvex PUBLIC vexspoke)
endif()

# Apple GPU/bridge frameworks (vk_iosurface.c, texture.c image decode).
if(APPLE)
    target_sources(graphvex PRIVATE
        src/objc/font_cocoa.m
    )
    set_source_files_properties(src/objc/font_cocoa.m PROPERTIES
        COMPILE_OPTIONS "-fobjc-arc")
    target_link_libraries(graphvex PUBLIC
        "-framework CoreFoundation"
        "-framework CoreGraphics"
        "-framework IOSurface"
        "-framework Foundation"
        "-framework AppKit"
        "-framework CoreText"
    )
endif()
endif() # NOT TARGET graphvex

# Buffer smoke test (relocated from the vexspoke `anti` harness with the move).
add_executable(buffer_test
    tests/buffer_test.c
)
target_link_libraries(buffer_test PRIVATE graphvex)
