# This file is part of CustusX, an Image Guided Therapy Application.
#
# Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
#
# CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
# code and binaries can only be used by SMT and those with explicit permission
# from SMT. CustusX shall not be distributed to anyone else.
#
# CustusX is a research tool. It is NOT intended for use or certified for use
# in a normal clinical setting. SMT does not take responsibility for its use
# in any way.
#
# See CustusX_License.txt for more information.

###############################################################################
## Installation script for CustusX
#
# Include into CustusX app dir.
#
###############################################################################


include(cxInstallUtilities)

cx_install_configuration_files()
cx_install_documentation_files()

cx_install_windows_runtime_libs(${CX_INSTALL_ROOT_DIR}/bin)

#--------------------------------------------------------------------------------
# Bundle utilities
#--------------------------------------------------------------------------------

set(CUSTUSX_EXECUTABLE "${CX_INSTALL_BINARY_DIR}/${CX_BUNDLE_NAME}")
if(CX_WINDOWS)
	set( CUSTUSX_EXECUTABLE "${CUSTUSX_EXECUTABLE}.exe")
endif()

set(CX_ALL_LIBRARY_DIRS CACHE INTERNAL "List of dirs to look for libraries to use in fixup_bundle")
set(CX_ALL_LIBRARY_DIRS
    ${CX_ALL_LIBRARY_DIRS}
    ${ULTERIUS_BIN_DIR}
    ${QT_LIBRARY_DIRS}
    ${QT_BINARY_DIR}
    ${GEStreamer_LIBRARY_DIRS}
    ${OpenCV_LIB_DIR}
    ${OPENCL_LIBRARY_DIRS}
    ${IGSTK_LIBRARY_DIRS}
    ${OpenIGTLink_LIBRARY_DIRS}
    ${VTK_DIR}/lib
    ${CTK_DIR}/CTK-build/bin
	${CTK_DIR}/DCMTK-build/lib # remove?
	${CTK_DCMTK_DIR}/lib
    ${ITK_DIR}/lib
    ${Level-Set-Segmentation_LIBRARY_DIRS}
    ${Tube-Segmentation-Framework_LIBRARY_DIRS}
    ${OpenCLUtilityLibrary_LIBRARY_DIRS}

	${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
	${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/plugins
	)

cx_install_all_stored_targets(${CX_INSTALL_BINARY_DIR})

cx_fixup_and_add_qtplugins_to_bundle(
	"${CUSTUSX_EXECUTABLE}"
	${CX_INSTALL_BINARY_DIR}
	"${CX_ALL_LIBRARY_DIRS}")


include(CPack)

  #hiding packages created by ctk: we install the plugins ourselves
  cpack_add_component(Development HIDDEN DISABLED)
  cpack_add_component(RuntimePlugins HIDDEN DISABLED)
