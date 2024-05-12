project (einit)

# Tools for using NK parser.
include (platform/image)

# Set compile flags.
project_header_default ("STANDARD_GNU_11:YES" "STRICT_WARNINGS:NO")


# find_package (vfs REQUIRED COMPONENTS FS_LIB)
include_directories (${vfs_INCLUDE})

# Add a package with the VFS program implementations.
find_package (precompiled_vfs REQUIRED)
include_directories (${precompiled_vfs_INCLUDE})

# Add package importing components for building audit entity and connecting to it.
find_package (klog REQUIRED)
include_directories (${klog_INCLUDE})

nk_build_edl_files (klog_edl_files NK_MODULE "ucm" EDL "resources/KlogEntity.edl")
add_executable (KlogEntityQemu "src/klog_entity.c")
target_link_libraries (KlogEntityQemu ${klog_SYSTEM_AUDIT_LIB})
add_dependencies (KlogEntityQemu klog_edl_files)
target_link_libraries (KlogEntityQemu kssaudit)
set_target_properties (${precompiled_vfsVfsNet} PROPERTIES
    EXTRA_ARGS "\
    - -l
    - devfs /dev devfs 0
    - -l
    - romfs /etc romfs 0")

add_executable (KlogEntityHw "src/klog_entity.c")
target_link_libraries (KlogEntityHw ${klog_SYSTEM_AUDIT_LIB})
add_dependencies (KlogEntityHw klog_edl_files)
set (INIT_KlogEntity_PATH "KlogEntityHw")
target_link_libraries (KlogEntityHw kssaudit)


# Define the ENTITIES variable with a list of entities.
set (ENTITIES
        ${precompiled_vfsVfsNet}
        OTA
        Master
        Interface
        KlogStorageEntity
)

# KasperskyOS image for target hardware platform.
build_kos_hw_image (kos-image
        EINIT_ENTITY EinitHw
        CONNECTIONS_CFG "src/init.yaml.in"
        SECURITY_PSL "src/security.psl"
        KLOG_ENTITY KlogEntityHw
        IMAGE_FILES ${ENTITIES})

set (INIT_KlogEntity_PATH "KlogEntityQemu")

# KasperskyOS image for QEMU with simulation targets (sim, gdbsim, gdb).
build_kos_qemu_image (kos-qemu-image
        EINIT_ENTITY EinitQemu
        CONNECTIONS_CFG "src/init.yaml.in"
        SECURITY_PSL "src/security.psl"
        KLOG_ENTITY KlogEntityQemu
        IMAGE_FILES ${ENTITIES})

# add_dependencies (kos-qemu-image KlogEntityQemu)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kos-qemu-image DESTINATION bin )
install(FILES ${SDK_PREFIX}/toolchain/bin/qemu-system-aarch64
        DESTINATION bin
        PERMISSIONS WORLD_EXECUTE
)
