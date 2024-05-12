project(KlogStorageEntity)

project_header_default ("STANDARD_GNU_11:YES" "STRICT_WARNINGS:NO")

# Add package importing `klog_storage_entity`.
find_package (klog_storage REQUIRED)
include_directories (${klog_storage_INCLUDE})

nk_build_edl_files (klog_storage_edl_files NK_MODULE "ucm" EDL "resources/KlogStorageEntity.edl")

add_executable (${PROJECT_NAME} "src/klog_storage_entity.c")
target_link_libraries (${PROJECT_NAME} ${klog_storage_SERVER_LIB})
add_dependencies (${PROJECT_NAME} klog_edl_files klog_storage_edl_files)