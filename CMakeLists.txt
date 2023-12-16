cmake_minimum_required(VERSION 3.25)
project(main)

set(CMAKE_CXX_STANDARD 11)

add_executable(
    main 
    #WIN32
    "main.cpp" "Renderable/SpriteSheet.h" "Renderable/Renderable.h" "Renderable/Text.h" "Renderable/Button.h" "Renderable/Rectangle.h" "Renderable/AnimatedSprite.h" "utils/Random.h" "utils/Rect.h" "utils/String.h" "utils/Vec2.h" "Layout.h")

target_include_directories(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)

target_link_libraries(main PRIVATE $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main> $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>)
target_link_libraries(main PRIVATE $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>)
target_link_libraries(main PRIVATE $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})