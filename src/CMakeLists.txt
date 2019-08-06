project(cell_autom)

set(SOURCE_FILES
        main.cpp
        window_processor.cpp window_processor.h
        cell_autom.cpp cell_autom.h
        ui.cpp ui.h
        )

add_executable(cell_autom ${SOURCE_FILES})
FILE(COPY res DESTINATION "${CMAKE_BINARY_DIR}\\src")
target_link_libraries(cell_autom sfml-graphics sfml-system sfml-window)