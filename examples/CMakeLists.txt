# ========================
# Examples CMake
# ========================

file(GLOB EXAMPLE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

foreach(example ${EXAMPLE_SOURCES})
    get_filename_component(example_name ${example} NAME_WE)

    add_executable(${example_name} ${example})

    target_link_libraries(${example_name}
        PRIVATE
            softadastra::fs
    )

    target_compile_features(${example_name} PRIVATE cxx_std_20)
endforeach()
