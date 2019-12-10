project (zip)

# Add a library with the above sources
add_library(${PROJECT_NAME} src/zip.c)
add_library(Subproject::zip ALIAS ${PROJECT_NAME})

target_include_directories( ${PROJECT_NAME}
        PUBLIC
        ${PROJECT_SOURCE_DIR}/src
)
