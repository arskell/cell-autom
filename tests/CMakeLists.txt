project(game_of_life_tests)

set(SOURCE_FILES game_of_life_test.cpp
        ../src/game_of_life.h ../src/game_of_life.cpp
        ../src/ui.h ../src/ui.cpp
        )

add_executable(game_of_life_tests ${SOURCE_FILES})

target_link_libraries(game_of_life_tests
        sfml-graphics sfml-system sfml-window)