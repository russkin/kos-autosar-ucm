project (Interface)

# Tools for using NK parser.
include (platform/nk)

nk_build_edl_files (${PROJECT_NAME}_edl_files NK_MODULE "ucm" EDL "resources/Interface.edl")

# Set compile flags.
project_header_default ("STANDARD_GNU_11:YES" "STRICT_WARNINGS:NO")

add_executable (${PROJECT_NAME} "src/interface.c")
add_dependencies (${PROJECT_NAME} Master_idl_files ${PROJECT_NAME}_edl_files)
