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

#include "cxToolMetricRep.h"

#include "sscView.h"
#include "boost/bind.hpp"
#include "cxGraphicalAxes3D.h"

namespace cx
{

ToolMetricRepPtr ToolMetricRep::New(const QString& uid, const QString& name)
{
	ToolMetricRepPtr retval(new ToolMetricRep(uid, name));
	return retval;
}

ToolMetricRep::ToolMetricRep(const QString& uid, const QString& name) :
				DataMetricRep(uid, name)
{
	mViewportListener.reset(new ViewportListener);
	mViewportListener->setCallback(boost::bind(&ToolMetricRep::rescale, this));
}

void ToolMetricRep::clear()
{
	mToolTip.reset();
	mToolOffset.reset();

	mAxes.reset();
	DataMetricRep::clear();
}

void ToolMetricRep::addRepActorsToViewRenderer(View *view)
{
	mViewportListener->startListen(view->getRenderer());
	DataMetricRep::addRepActorsToViewRenderer(view);
}

void ToolMetricRep::removeRepActorsFromViewRenderer(View *view)
{
	DataMetricRep::removeRepActorsFromViewRenderer(view);
	mViewportListener->stopListen();
}

ToolMetricPtr ToolMetricRep::getToolMetric()
{
	return boost::dynamic_pointer_cast<ToolMetric>(mMetric);
}

void ToolMetricRep::onModifiedStartRender()
{
	ToolMetricPtr metric = this->getToolMetric();

	if (!metric || !metric->isValid() || !mView)
		return;

	if (!mAxes || !mToolTip || !mToolOffset)
	{
		mAxes.reset(new GraphicalAxes3D());
		mToolTip.reset(new GraphicalPoint3D(mView->getRenderer()));
		mToolOffset.reset(new GraphicalLine3D(mView->getRenderer()));
	}

	mAxes->setFontSize(0.04);
	mAxes->setAxisLength(0.05);
//	mAxes->setAxisLength(0.2);
	mAxes->setShowAxesLabels(false);
	mAxes->setRenderer(mView->getRenderer());

	Vector3D p0_r = mMetric->getRefCoord();
	Transform3D rMt = metric->getRefFrame();
	Vector3D toolTip_r = rMt.coord(Vector3D(0,0,-metric->getToolOffset()));

	mToolTip->setValue(toolTip_r);
	mToolTip->setColor(mMetric->getColor());

	mToolOffset->setValue(p0_r, toolTip_r);
	mToolOffset->setColor(mMetric->getColor());

	mAxes->setTransform(metric->getRefFrame());
	this->drawText();
	this->rescale();
}

void ToolMetricRep::rescale()
{
	if (!mToolTip)
		return;

	double size = mViewportListener->getVpnZoom();
	double sphereSize = mGraphicsSize / 100 / size;
	mToolTip->setRadius(sphereSize);
}


} // namespace cx
