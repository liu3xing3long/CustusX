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

#define _USE_MATH_DEFINES
#include "cxTool.h"

#include <vtkPolyData.h>
#include <vtkConeSource.h>
#include <vtkSTLReader.h>
#include <QDir>
#include <QDateTime>
#include <QStringList>
#include <QTextStream>
#include "sscMessageManager.h"
#include "sscTypeConversions.h"
#include "sscProbeData.h"
#include "cxToolManager.h"
#include "cxProbe.h"
#include "cxIgstkTool.h"

namespace cx
{

cxTool::cxTool(IgstkToolPtr igstkTool) :
				mTool(igstkTool), mPositionHistory(new ssc::TimedTransformMap()), mPolyData(NULL), m_prMt(
								new ssc::Transform3D(ssc::Transform3D::Identity())), mValid(false), mConfigured(false), mTracked(
								false)
{
	ssc::Tool::mUid = mTool->getInternalStructure().mUid;
	ssc::Tool::mName = mTool->getInternalStructure().mName;
	mValid = igstkTool->isValid();

	this->createPolyData();

	connect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this,
					SLOT(toolTransformAndTimestampSlot(Transform3D, double)));
	connect(mTool.get(), SIGNAL(attachedToTracker(bool)), this, SIGNAL(attachedToTracker(bool)));
	connect(mTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(toolVisibleSlot(bool)));
	connect(mTool.get(), SIGNAL(toolVisible(bool)), this, SIGNAL(toolVisible(bool)));
	connect(&mTpsTimer, SIGNAL(timeout()), this, SLOT(calculateTpsSlot()));

	if (mTool->getInternalStructure().mIsProbe)
	{
		mProbe = cxProbe::New(mTool->getInternalStructure().mInstrumentId,
						mTool->getInternalStructure().mInstrumentScannerId);
		connect(mProbe.get(), SIGNAL(sectorChanged()), this, SIGNAL(toolProbeSector()));
	}
	connect(ssc::toolManager(), SIGNAL(tooltipOffset(double)), this, SIGNAL(tooltipOffset(double)));
}

cxTool::~cxTool()
{
}

std::set<cxTool::Type> cxTool::getTypes() const
{
	std::set<Type> retval;

	if (mTool->getInternalStructure().mIsReference)
		retval.insert(cxTool::TOOL_REFERENCE);
	if (mTool->getInternalStructure().mIsPointer)
		retval.insert(cxTool::TOOL_POINTER);
	if (mTool->getInternalStructure().mIsProbe)
		retval.insert(cxTool::TOOL_US_PROBE);

	return retval;
}

QString cxTool::getGraphicsFileName() const
{
	return mTool->getInternalStructure().mGraphicsFileName;
}

vtkPolyDataPtr cxTool::getGraphicsPolyData() const
{
	return mPolyData;
}

ssc::TimedTransformMapPtr cxTool::getPositionHistory()
{
	return mPositionHistory;
}

ssc::ProbePtr cxTool::getProbe() const
{
	return mProbe;
}

ssc::Transform3D cxTool::get_prMt() const
{
	return *m_prMt;
}

bool cxTool::getVisible() const
{
	return mTool->isVisible();
}

bool cxTool::isInitialized() const
{
	return mTool->isInitialized();
}

QString cxTool::getUid() const
{
	return ssc::Tool::mUid;
}

QString cxTool::getName() const
{
	return ssc::Tool::mName;
}

double cxTool::getTooltipOffset() const
{
	return ssc::toolManager()->getTooltipOffset();
}

void cxTool::setTooltipOffset(double val)
{
	ssc::toolManager()->setTooltipOffset(val);
}

bool cxTool::isValid() const
{
	return mValid;
}

void cxTool::createPolyData()
{
	QDir dir;
	if (!mTool->getInternalStructure().mGraphicsFileName.isEmpty()
					&& dir.exists(mTool->getInternalStructure().mGraphicsFileName))
	{
		vtkSTLReaderPtr reader = vtkSTLReaderPtr::New();
		reader->SetFileName(cstring_cast(mTool->getInternalStructure().mGraphicsFileName));
		mPolyData = reader->GetOutput();
	}
	else
	{
		vtkConeSourcePtr coneSource = vtkConeSourcePtr::New();
		coneSource->SetResolution(25);
		coneSource->SetRadius(10);
		coneSource->SetHeight(100);

		coneSource->SetDirection(0, 0, 1);
		double newCenter[3];
		coneSource->GetCenter(newCenter);
		newCenter[2] = newCenter[2] - coneSource->GetHeight() / 2;
		coneSource->SetCenter(newCenter);

		mPolyData = coneSource->GetOutput();
	}
}

bool cxTool::isCalibrated() const
{
	ssc::Transform3D identity = ssc::Transform3D::Identity();
	ssc::Transform3D sMt = mTool->getInternalStructure().getCalibrationAsSSC();
	return !ssc::similar(sMt, identity);
}

ssc::Transform3D cxTool::getCalibration_sMt() const
{
	ssc::Transform3D sMt = mTool->getInternalStructure().getCalibrationAsSSC();

	return sMt;
}

void cxTool::setCalibration_sMt(ssc::Transform3D calibration)
{
	mTool->updateCalibration(calibration);
}

QString cxTool::getCalibrationFileName() const
{
	return mTool->getInternalStructure().mCalibrationFilename;
}

ssc::TRACKING_SYSTEM cxTool::getTrackerType()
{
	return mTool->getInternalStructure().mTrackerType;
}

void cxTool::printInternalStructure()
{
	mTool->printInternalStructure();
}

ssc::ProbeData cxTool::getProbeSector() const
{
	if (mProbe)
		return mProbe->getProbeData();
	return ssc::ProbeData();
}

std::map<int, ssc::Vector3D> cxTool::getReferencePoints() const
{
	return mTool->getInternalStructure().mReferencePoints;
}

bool cxTool::hasReferencePointWithId(int id)
{
  return this->getReferencePoints().count(id);
}

ssc::TimedTransformMap cxTool::getSessionHistory(double startTime, double stopTime)
{
	ssc::TimedTransformMap::iterator startIt = mPositionHistory->lower_bound(startTime);
	ssc::TimedTransformMap::iterator stopIt = mPositionHistory->upper_bound(stopTime);

	ssc::TimedTransformMap retval(startIt, stopIt);
	return retval;
}

void cxTool::addXml(QDomNode& dataNode)
{
	QDomDocument doc = dataNode.ownerDocument();
	dataNode.toElement().setAttribute("uid", qstring_cast(this->getUid()));
	if (mProbe && mProbe->isValid())
	{
		QDomElement probeNode = doc.createElement("probe");
		mProbe->addXml(probeNode);
		dataNode.appendChild(probeNode);
	}
}

void cxTool::parseXml(QDomNode& dataNode)
{
	if (dataNode.isNull())
		return;
	if (mProbe)
	{
		QDomNode probeNode = dataNode.namedItem("probe");
		mProbe->parseXml(probeNode);
	}
}

void cxTool::toolTransformAndTimestampSlot(Transform3D matrix, double timestamp)
{
	m_prMt = ssc::Transform3DPtr(new ssc::Transform3D(matrix));
	(*mPositionHistory)[timestamp] = *m_prMt;

	emit toolTransformAndTimestamp((*m_prMt), timestamp);
}

void cxTool::calculateTpsSlot()
{
	int tpsNr = 0;

	int numberOfTransformsToCheck = ((mPositionHistory->size() >= 10) ? 10 : mPositionHistory->size());
	if (	numberOfTransformsToCheck == 0)
		return;

	ssc::TimedTransformMap::reverse_iterator it = mPositionHistory->rbegin();
	double lastTransform = it->first;
	for (int i = 0; i < numberOfTransformsToCheck; ++i)
		++it;
	double firstTransform = it->first;
	double secondsPassed = (lastTransform - firstTransform) / 1000;

	tpsNr = (int) (numberOfTransformsToCheck / secondsPassed);

	emit tps(tpsNr);
}

void cxTool::toolVisibleSlot(bool on)
{
	if (on)
		mTpsTimer.start(1000); //calculate tps every 1 seconds
	else
		mTpsTimer.stop();
}

}//namespace cx
