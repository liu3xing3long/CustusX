// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#include "sscPickerRep.h"

#include "boost/bind.hpp"
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkProperty.h>
#include <vtkImageData.h>
#include <vtkLineSource.h>
//#include <vtkDoubleArray.h>
#include <vtkProbeFilter.h>
#include <vtkSphereSource.h>
#include <vtkRenderWindow.h>
#include <vtkCallbackCommand.h>
#include <vtkPolyDataMapper.h>
//#include <vtkDataSetAttributes.h>
#include <vtkEventQtSlotConnect.h>
//#include <vtkCellPicker.h>
#include <vtkVolumePicker.h>
#include "sscMesh.h"
#include "sscDataManager.h"
#include "sscMessageManager.h"
#include "sscImage.h"
#include "sscView.h"
#include "sscTool.h"
#include "sscToolManager.h"

namespace ssc
{
PickerRepPtr PickerRep::New(const QString& uid, const QString& name)
{
	PickerRepPtr retval(new PickerRep(uid, name));
	retval->mSelf = retval;
	return retval;
}
PickerRep::PickerRep(const QString& uid, const QString& name) :
		RepImpl(uid, name), mPickedPoint(), mSphereRadius(2) //, mConnections(vtkEventQtSlotConnectPtr::New())
{
	mIsDragging = false;
	mViewportListener.reset(new ssc::ViewportListener);
	mViewportListener->setCallback(boost::bind(&PickerRep::scaleSphere, this));

	this->mCallbackCommand = vtkCallbackCommandPtr::New();
	this->mCallbackCommand->SetClientData(this);
	this->mCallbackCommand->SetCallback(PickerRep::ProcessEvents);

	mView = NULL;
	mEnabled = false;
	mConnected = false;
}

void PickerRep::scaleSphere()
{
	if (!mGraphicalPoint)
		return;

	double size = mViewportListener->getVpnZoom();
	double sphereSize = mSphereRadius / 100 / size;
	mGraphicalPoint->setRadius(sphereSize);
//	mGraphicalPoint->getActor()->GetProperty()->SetRepresentationToWireframe();
}

PickerRep::~PickerRep()
{
}
QString PickerRep::getType() const
{
	return "ssc::PickerRep";
}

void PickerRep::setSphereRadius(double radius)
{
	mSphereRadius = radius;
	if (mGraphicalPoint)
		mGraphicalPoint->setRadius(mSphereRadius);
}

void PickerRep::setTool(ToolPtr tool)
{
	if (tool == mTool)
		return;

	if (mTool)
	{
		disconnect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this,
				SLOT(receiveTransforms(Transform3D, double)));
	}

	mTool = tool;

	if (mTool)
	{
		receiveTransforms(mTool->get_prMt(), 0);

		connect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this,
				SLOT(receiveTransforms(Transform3D, double)));
	}
}

void PickerRep::setGlyph(vtkPolyDataAlgorithmPtr glyph)
{
	return; // ignore glyph stuff for now..

	 if (!mGlyph)
	 {
		 mGlyph.reset(new ssc::GraphicalPolyData3D());
		 mGlyph->getActor()->GetProperty()->SetRepresentationToWireframe();
		 mGlyph->getActor()->SetVisibility(false);
		 mGlyph->setColor(ssc::Vector3D(0, 0, 1));

		 if (mView)
			mGlyph->setRenderer(mView->getRenderer());
	 }
	 mGlyph->setSource(glyph);



//	mGraphicalPoint.reset(new ssc::GraphicalPoint3D(mView->getRenderer()));
//	mGraphicalPoint->setColor(ssc::Vector3D(0, 0, 1));
//	mGraphicalPoint->setRadius(mSphereRadius);
//	mGraphicalPoint->getActor()->SetVisibility(false);
}

typedef vtkSmartPointer<class vtkVolumePicker> vtkVolumePickerPtr;
typedef vtkSmartPointer<class vtkDataSet> vtkDataSetPtr;

/**
 * Trace a ray from clickPosition along the camera view direction and intersect
 * the image.
 * \param[in] clickPosition the click position in DISPLAY coordinates
 * \param[in] renderer the renderer from which to get the camera
 * \return the point where the ray intersects the image
 */
void PickerRep::pickLandmark(const Vector3D& clickPosition, vtkRendererPtr renderer)
{
	vtkVolumePickerPtr picker = vtkVolumePickerPtr::New();
//	std::cout << "pixel pos " << clickPosition << std::endl;
// default values:
//	picker->SetVolumeOpacityIsovalue(0.05);
//	picker->SetTolerance(1.0E-6);
	int hit = picker->Pick(clickPosition[0], clickPosition[1], 0, renderer);
//	std::cout << "  pick pos  " << clickPosition << std::endl;

	// search for picked data in manager, emit uid if found.
	vtkDataSetPtr data = picker->GetDataSet();
//	std::cout << "data " << data << std::endl;
	if (data)
	{
//		data->Print(std::cout);
//		std::cout << "looking for " << data.GetPointer() << std::endl;

		std::map<QString, DataPtr> allData = ssc::dataManager()->getData();
		for (std::map<QString, DataPtr>::iterator iter = allData.begin(); iter != allData.end(); ++iter)
		{
			ssc::MeshPtr mesh = boost::shared_dynamic_cast<ssc::Mesh>(iter->second);
			if (mesh && mesh->getVtkPolyData() == data)
				emit dataPicked(iter->first);

			ssc::ImagePtr image = boost::shared_dynamic_cast<ssc::Image>(iter->second);
//			if (image)
//				std::cout << "  checking " << image->getBaseVtkImageData().GetPointer() << std::endl;
			if (image && image->getBaseVtkImageData() == data)
				emit dataPicked(iter->first);
		}
	}
//	data->Print(std::cout);
//	std::cout << "  hit: " << data.GetPointer() << std::endl;
//	std::cout << "  pt : " << mGraphicalPoint->getPolyData().GetPointer() << std::endl;
	ssc::Vector3D pick_w(picker->GetPickPosition());

	if (data == mGraphicalPoint->getPolyData() || (mGlyph && data==(mGlyph->getPolyData())))
	{
		// We have clicked the picker/tool itself.
		// Store click pos and wait for dragging.
		mClickedPoint = pick_w;
		mIsDragging = true;
		mCallbackCommand->SetAbortFlag(1); // abort this event: interactor does not receive it.
		return;
	}
	else
	{
		mIsDragging = false;
	}

	if (!hit)
		return;

	mPickedPoint = pick_w;

	if (mGraphicalPoint)
		mGraphicalPoint->setValue(mPickedPoint);
	if (mGlyph)
		mGlyph->setPosition(mPickedPoint);

	emit pointPicked(mPickedPoint);
//	return mPickedPoint;
}

void PickerRep::pickLandmarkSlot(vtkObject* renderWindowInteractor)
{
//	std::cout << "PickerRep::pickLandmarkSlot" << std::endl;
	vtkRenderWindowInteractorPtr iren = vtkRenderWindowInteractor::SafeDownCast(renderWindowInteractor);

	if (iren == NULL)
		return;

	int pickedPoint[2]; //<x,y>
	iren->GetEventPosition(pickedPoint); //mouse positions are measured in pixels

	vtkRendererPtr renderer = this->getRendererFromRenderWindow(*iren);
	if (renderer == NULL)
		return;

	Vector3D clickPoint(pickedPoint[0], pickedPoint[1], 0);
	this->pickLandmark(clickPoint, renderer);
}

void PickerRep::receiveTransforms(Transform3D prMt, double timestamp)
{
	Transform3D rMpr = *ToolManager::getInstance()->get_rMpr();
	Transform3D rMt = rMpr * prMt;
	Vector3D p_r = rMt.coord(ssc::Vector3D(0, 0, mTool->getTooltipOffset()));

	mPickedPoint = p_r;
	if (mGraphicalPoint)
		mGraphicalPoint->setValue(mPickedPoint);
	if (mGlyph)
		mGlyph->setPosition(mPickedPoint);

}

void PickerRep::setEnabled(bool on)
{
	if (mEnabled == on)
		return;

	mEnabled = on;

	if (mEnabled)
	{
		this->connectInteractor();
		if (mGraphicalPoint)
			mGraphicalPoint->getActor()->SetVisibility(true);
		if (mGlyph)
		{
			mGlyph->getActor()->SetVisibility(true);
		}
	}
	else
	{
		this->disconnectInteractor();
		if (mGraphicalPoint)
			mGraphicalPoint->getActor()->SetVisibility(false);
		if (mGlyph)
			mGlyph->getActor()->SetVisibility(false);
	}
}

void PickerRep::ProcessEvents(vtkObject* vtkNotUsed(object), unsigned long event, void* clientdata,
		void* vtkNotUsed(calldata))
{
	PickerRep* self = reinterpret_cast<PickerRep *>(clientdata);

	//okay, let's do the right thing
	switch (event)
	{
	case vtkCommand::LeftButtonPressEvent:
		self->OnLeftButtonDown();
		break;
	case vtkCommand::LeftButtonReleaseEvent:
		self->OnLeftButtonUp();
		break;
	case vtkCommand::MouseMoveEvent:
		self->OnMouseMove();
		break;
	}
}

/**Convert a point in display to world.
 * Based on method in vtkInteractorObserver
 */
Vector3D PickerRep::ComputeDisplayToWorld(Vector3D p_d)
{
	double worldPt[4];
	vtkRendererPtr ren = mView->getRenderer();
	ren->SetDisplayPoint(p_d.data());
	ren->DisplayToWorld();
	ren->GetWorldPoint(worldPt);
	return Vector3D(worldPt)/worldPt[3];
}

/**Convert a point in world to display
 * Based on method in vtkInteractorObserver
 */
Vector3D PickerRep::ComputeWorldToDisplay(Vector3D p_w)
{
	Vector3D p_d;
	vtkRendererPtr ren = mView->getRenderer();
	ren->SetWorldPoint(p_w[0], p_w[1], p_w[2], 1.0);
	ren->WorldToDisplay();
	ren->GetDisplayPoint(p_d.data());
	return p_d;
}

/** Get the displacement in the view plane since last event.
 */
Vector3D PickerRep::getDisplacement()
{
	vtkRenderWindowInteractorPtr interactor = mView->getRenderWindow()->GetInteractor();

//	// find previous pos in world and display:
	Vector3D p_prev_w = mClickedPoint;
//	std::cout << "  p_prev_w  = \t" << p_prev_w << std::endl;
	Vector3D p_prev_d = this->ComputeWorldToDisplay(p_prev_w);
//	std::cout << "  p_prev_d  = \t" << p_prev_d << std::endl;

	// find current pos in world and display, set z-pos in d equal to previous z-pos:
	Vector3D p_current_d(interactor->GetEventPosition()[0], interactor->GetEventPosition()[1], p_prev_d[2]);
//	std::cout << "  p_current_d = \t" << p_current_d << std::endl;
	Vector3D p_current_w = this->ComputeDisplayToWorld(p_current_d);
//	std::cout << "  p_current_w  = \t" << p_current_w << std::endl;

	// both positions are now in the camera focal plane: the diff lies in the view plane.
//	std::cout << "  diff_d   = \t" << p_current_d - p_prev_d << std::endl;
//	std::cout << "  diff_w   = \t" << p_current_w - p_prev_w << std::endl;
	return p_current_w - p_prev_w;
}

void PickerRep::OnLeftButtonDown()
{
	this->pickLandmarkSlot(mView->GetRenderWindow()->GetInteractor());
//	std::cout << "PickerRep::OnLeftButtonDown " << std::endl;
//	mCallbackCommand->SetAbortFlag(1);
}

void PickerRep::OnMouseMove()
{
	if (mIsDragging)
	{
		mPickedPoint += this->getDisplacement();
		mClickedPoint = mPickedPoint;

		if (mGraphicalPoint)
			mGraphicalPoint->setValue(mPickedPoint);
		if (mGlyph)
			mGlyph->setPosition(mPickedPoint);
		emit pointPicked(mPickedPoint);

		mCallbackCommand->SetAbortFlag(1);
	}
}

void PickerRep::OnLeftButtonUp()
{
	if (mIsDragging)
	{
		mIsDragging = false;
		mCallbackCommand->SetAbortFlag(1); // abort this event: interactor does not receive it.
	}
}

void PickerRep::connectInteractor()
{
	if (!mView)
		return;
	if (mConnected)
		return;

	vtkRenderWindowInteractorPtr i = mView->GetRenderWindow()->GetInteractor();
	i->AddObserver(vtkCommand::MouseMoveEvent, this->mCallbackCommand, 1.0);
	i->AddObserver(vtkCommand::LeftButtonPressEvent, this->mCallbackCommand, 1.0);
	i->AddObserver(vtkCommand::LeftButtonReleaseEvent, this->mCallbackCommand, 1.0);

	mConnected = true;
}

void PickerRep::disconnectInteractor()
{
	if (!mView)
		return;
	if (!mConnected)
		return;

	// don't listen for events any more
	mView->GetRenderWindow()->GetInteractor()->RemoveObserver(this->mCallbackCommand);

	mConnected = false;
}

void PickerRep::addRepActorsToViewRenderer(View* view)
{
	if (view == NULL)
	{
		messageManager()->sendDebug("Cannot add rep actor to a NULL view.");
		return;
	}

	if (mEnabled)
		this->connectInteractor();

	mView = view;


	mGraphicalPoint.reset(new ssc::GraphicalPoint3D(mView->getRenderer()));
	mGraphicalPoint->setColor(ssc::Vector3D(0, 0, 1));
	mGraphicalPoint->setRadius(mSphereRadius);
	mGraphicalPoint->getActor()->SetVisibility(false);

//	// teset code
//	vtkSphereSourcePtr glyph = vtkSphereSourcePtr::New();
//	glyph->SetRadius(40);
//	glyph->SetThetaResolution(16);
//	glyph->SetPhiResolution(12);
//	glyph->LatLongTessellationOn();
//
//	 mGlyph.reset(new ssc::GraphicalPolyData3D(glyph));
//	 mGlyph->getActor()->GetProperty()->SetRepresentationToWireframe();
//	 mGlyph->getActor()->SetVisibility(false);
//	 std::cout << "addRepActorsToViewRenderer " << std::endl;

//	 if (mView)
//		mGlyph->setRenderer(mView->getRenderer());
//	 // end test code

	 if (mGlyph)
		mGlyph->setRenderer(mView->getRenderer());


	mViewportListener->startListen(mView->getRenderer());
	this->scaleSphere();
}

void PickerRep::removeRepActorsFromViewRenderer(View* view)
{
	if (view == NULL)
		return;

	this->disconnectInteractor();
	mViewportListener->stopListen();
	mGraphicalPoint.reset();
	if (mGlyph)
		mGlyph->setRenderer(NULL);
	mView = NULL;
}

vtkRendererPtr PickerRep::getRendererFromRenderWindow(vtkRenderWindowInteractor& iren)
{
	vtkRendererPtr renderer = NULL;
	std::set<ssc::View*>::const_iterator it = mViews.begin();
	for (; it != mViews.end(); ++it)
	{
		if (iren.GetRenderWindow() == (*it)->getRenderWindow())
			renderer = (*it)->getRenderer();
	}
	return renderer;
}

Vector3D PickerRep::getPosition() const
{
	return mPickedPoint;
}

} //namespace ssc
