project(cell_autom_tests)

set(SOURCE_FILES game_of_life_test.cpp
        ../src/cell_autom.h ../src/cell_autom.cpp
        ../src/ui.h ../src/ui.cpp
        )

add_executable(cell_autom_tests ${SOURCE_FILES})

target_link_libraries(cell_autom_tests
        sfml-graphics sfml-system sfml-window)