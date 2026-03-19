# ========================
# Tests CMake
# ========================

file(GLOB TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

foreach(test_file ${TEST_SOURCES})
    get_filename_component(test_name ${test_file} NAME_WE)

    add_executable(${test_name} ${test_file})

    target_link_libraries(${test_name}
        PRIVATE
            softadastra::fs
    )

    target_compile_features(${test_name} PRIVATE cxx_std_20)

    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
