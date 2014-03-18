// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.
#ifndef CXNAVIGATION_H
#define CXNAVIGATION_H

#include "cxDefinitions.h"
#include "cxForwardDeclarations.h"
#include "cxVector3D.h"

namespace cx
{
typedef boost::shared_ptr<class VisualizationServiceBackend> VisualizationServiceBackendPtr;
typedef boost::shared_ptr<class CameraControl> CameraControlPtr;

/** Functions for navigating in the visualization scene(s).
 *
 * \ingroup cx_service_visualization
 */
class Navigation
{
public:
	enum VIEW_TYPE { v2D = 0x01, v3D=0x02, vBOTH=0x03 };
	Navigation(VisualizationServiceBackendPtr backend, CameraControlPtr camera3D=CameraControlPtr());
	void centerToData(DataPtr image);
	void centerToView(const std::vector<DataPtr>& images);
	void centerToGlobalDataCenter();
	void centerToTooltip();
	void centerToPosition(Vector3D p_r, QFlags<VIEW_TYPE> viewType=vBOTH);

private:
	void moveManualToolToPosition(Vector3D& p_r);
	Vector3D findViewCenter(const std::vector<DataPtr>& images);
	Vector3D findGlobalDataCenter();
	Vector3D findDataCenter(std::vector<DataPtr> data);

	VisualizationServiceBackendPtr mBackend;
	CameraControlPtr mCamera3D;
};

typedef boost::shared_ptr<Navigation> NavigationPtr;


} // namespace cx

#endif // CXNAVIGATION_H