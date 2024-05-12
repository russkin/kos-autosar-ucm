include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGING_INSTALL_PREFIX "/opt/kos-ucm")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_COMPONENTS_GROUPING ALL_COMPONENTS_IN_ONE)
set(CPACK_DEB_COMPONENT_INSTALL YES)
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_CONTACT "Nikolay.Russkin@kaspersky.com")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Nikolay Russkin")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libxml2, libusb-1.0-0, libpulse0, libsdl2-2.0-0, libpixman-1-0, libpng16-16, libglib2.0-0, libasound2")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "KasperskyOS CE AUTOSAR UCM demo")
include(CPack)