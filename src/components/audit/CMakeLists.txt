project (audit)

# Tools for using NK parser.
include (platform/nk)

# Set compile flags.
project_header_default ("STANDARD_GNU_11:YES" "STRICT_WARNINGS:NO")

# Add a package for working with the virtual file system.
find_package (vfs REQUIRED COMPONENTS FS_LIB CLIENT_LIB SERVER_LIB)
include_directories (${vfs_INCLUDE})

# Add a package with the VFS program implementations.
find_package (precompiled_vfs REQUIRED)
include_directories (${precompiled_vfs_INCLUDE})


# add_executable (KlogEntityHw "src/klog_entity.c")
# target_link_libraries (KlogEntityHw ${klog_SYSTEM_AUDIT_LIB})
# add_dependencies (KlogEntityHw klog_edl_files)


# set (INIT_KlogEntity_PATH "KlogEntityHw")
# target_link_libraries (KlogEntityHw kssaudit)

