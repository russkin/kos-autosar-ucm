project (Master)

# Tools for using NK parser.
include (platform/nk)

# Set compile flags.
project_header_default ("STANDARD_GNU_11:YES" "STRICT_WARNINGS:NO")

nk_build_idl_files (${PROJECT_NAME}_idl_files NK_MODULE "ucm" IDL "resources/${PROJECT_NAME}.idl")
nk_build_cdl_files (${PROJECT_NAME}_cdl_files IDL_TARGET ${PROJECT_NAME}_idl_files NK_MODULE "ucm" CDL "resources/${PROJECT_NAME}.cdl")
nk_build_edl_files (${PROJECT_NAME}_edl_files CDL_TARGET ${PROJECT_NAME}_cdl_files NK_MODULE "ucm" EDL "resources/${PROJECT_NAME}.edl")

add_executable (${PROJECT_NAME} "src/master.c")
add_dependencies (${PROJECT_NAME} ${PROJECT_NAME}_edl_files OTA_idl_files)
