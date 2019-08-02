project(game_of_life)

set(SOURCE_FILES
        main.cpp
        window_processor.cpp window_processor.h
        game_of_life.cpp game_of_life.h
        ui.cpp ui.h
        )

add_executable(game_of_life ${SOURCE_FILES})
FILE(COPY res DESTINATION "${CMAKE_BINARY_DIR}\\src")
target_link_libraries(game_of_life sfml-graphics sfml-system sfml-window)