/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/
#include "cxtestAcquisitionFixture.h"

#include <vtkImageData.h>
#include "catch.hpp"
#include <QApplication>

#include "cxDummyTool.h"
#include "cxDataLocations.h"
#include "cxLogger.h"
#include "cxUSFrameData.h"
#include "cxUsReconstructionFileReader.h"
#include "cxImageDataContainer.h"
#include "cxStringDataAdapterXml.h"
#include "cxProbeImpl.h"
#include "cxTrackingService.h"
#include "cxLogicManager.h"
#include "cxLegacySingletons.h"
#include "cxVideoService.h"
#include "cxUsReconstructionServiceProxy.h"
#include "cxPatientModelService.h"
#include "cxStreamerServiceUtilities.h"
#include "cxVideoSource.h"
#include "cxAcquisitionServiceProxy.h"


namespace cxtest
{

void AcquisitionFixture::setUp()
{
	cx::DataLocations::setTestMode();
	cx::LogicManager::initialize();
}

void AcquisitionFixture::tearDown()
{
	cx::LogicManager::shutdown();
}

AcquisitionFixture::AcquisitionFixture(QObject* parent) :
	QObject(parent),
	mRecordDuration(3000)
{
	this->setUp();
	mOptions = cx::XmlOptionFile(cx::DataLocations::getXmlSettingsFile()).descend("video");
	mNumberOfExpectedStreams = 1;
}

AcquisitionFixture::~AcquisitionFixture()
{
	this->tearDown();
}

cx::DataAdapterPtr AcquisitionFixture::getOption(QString uid)
{
	QDomElement element = mOptions.getElement("video");
	cx::StreamerService* streamer;
	streamer = cx::StreamerServiceUtilities::getStreamerService(mConnectionMethod,
																cx::logicManager()->getPluginContext());
	REQUIRE(streamer);
	cx::DataAdapterPtr option = cx::DataAdapter::findAdapter(streamer->getSettings(element), uid);
	REQUIRE(option.get());
	return option;
}

void AcquisitionFixture::initVideo()
{
	mConnectionMethod = "ImageFile";
	cx::videoService()->setConnectionMethod(mConnectionMethod);
	INFO("bundle path: "+cx::DataLocations::getBundlePath());

	this->getOption("filename")->setValueFromVariant(mAcqDataFilename);
	mOptions.save();
}


void AcquisitionFixture::setupVideo()
{
	mVideoSource = cx::videoService()->getActiveVideoSource();
	connect(mVideoSource.get(), SIGNAL(newFrame()), this, SLOT(newFrameSlot()));
	cx::videoService()->openConnection();
}

void AcquisitionFixture::setupProbe()
{
	SSC_LOG("");
	cx::DummyToolPtr dummyTool(new cx::DummyTool());
	dummyTool->setToolPositionMovement(dummyTool->createToolPositionMovementTranslationOnly(cx::DoubleBoundingBox3D(0,0,0,10,10,10)));
	std::pair<QString, cx::ProbeDefinition> probedata = cx::UsReconstructionFileReader::readProbeDataFromFile(mAcqDataFilename);
	cx::ProbeImplPtr probe = cx::ProbeImpl::New("","");
	probe->setProbeSector(probedata.second);
	dummyTool->setProbeSector(probe);
	CHECK(dummyTool->getProbe());
	CHECK(dummyTool->getProbe()->isValid());
	dummyTool->setVisible(true);
	// TODO: refactor toolmanager to be runnable in dummy mode (playback might benefit from this too)
	cx::trackingService()->runDummyTool(dummyTool);
	CHECK(dummyTool->getProbe()->getRTSource());
}

void AcquisitionFixture::initialize()
{
	// select video source: use a small one because old machines cannot handle RT streaming of 1024x768 color data.
	mAcqDataFilename = cx::DataLocations::getTestDataPath() + "/testing/us_videos/acq_256x192.mhd";

	qApp->processEvents(); // wait for stateservice to finish init of application states - needed before load patient.
	cx::patientService()->newPatient(cx::DataLocations::getTestDataPath() + "/temp/Acquisition/");

	//Mock UsReconstructionService with null object
	ctkPluginContext *pluginContext = cx::logicManager()->getPluginContext();
	mUsReconstructionService = cx::UsReconstructionServicePtr(new cx::UsReconstructionServiceProxy(pluginContext));
	mAcquisitionService = cx::AcquisitionServicePtr(new cx::AcquisitionServiceProxy(pluginContext));

	connect(mAcquisitionService.get(), &cx::AcquisitionService::readinessChanged, this, &AcquisitionFixture::readinessChangedSlot);
	connect(mAcquisitionService.get(), &cx::AcquisitionService::saveDataCompleted, this, &AcquisitionFixture::saveDataCompletedSlot);
	connect(mAcquisitionService.get(), &cx::AcquisitionService::acquisitionDataReady, this, &AcquisitionFixture::acquisitionDataReadySlot);

	// run setup of video, probe and start acquisition in series, each depending on the success of the previous:
	QTimer::singleShot(0, this, SLOT(setupVideo()));
	connect(cx::videoService().get(), SIGNAL(connected(bool)), this, SLOT(videoConnectedSlot()));
	connect(cx::trackingService().get(), &cx::TrackingService::stateChanged, this, &AcquisitionFixture::start);

	this->initVideo();
}

void AcquisitionFixture::videoConnectedSlot()
{
//	SSC_LOG("");

	// make sure all sources have started streaming before running probe setup (there might be several sources)
	if (cx::videoService()->getVideoSources().size() < mNumberOfExpectedStreams)
	{
		// loop back to this handler
		QTimer::singleShot(50, this, SLOT(videoConnectedSlot()));
		return;
	}
	QTimer::singleShot(50, this, SLOT(setupProbe()));
}

void AcquisitionFixture::start()
{
	if (cx::trackingService()->getState() < cx::Tool::tsTRACKING)
		return;

	SSC_LOG("");
	mAcquisitionService->startRecord();
	QTimer::singleShot(mRecordDuration, this, SLOT(stop()));
}

void AcquisitionFixture::stop()
{
	SSC_LOG("");
	mAcquisitionService->stopRecord();
}

void AcquisitionFixture::newFrameSlot()
{
	// add debug code here if needed.
}

void AcquisitionFixture::readinessChangedSlot()
{
	std::cout << QString("Acquisition Ready Status %1: %2")
				 .arg(mAcquisitionService->isReady())
				 .arg(mAcquisitionService->getInfoText()) << std::endl;
}

void AcquisitionFixture::acquisitionDataReadySlot()
{
	// read data and print info - this if the result of the memory pathway
	mMemOutputData = mUsReconstructionService->getSelectedFileData();
}

void AcquisitionFixture::saveDataCompletedSlot(QString path)
{
	// this is the last step: quit when finished
	if (!mAcquisitionService->getNumberOfSavingThreads())
		QTimer::singleShot(100,   qApp, SLOT(quit()) );

	// read file and print info - this is the result of the file pathway
	cx::UsReconstructionFileReaderPtr fileReader(new cx::UsReconstructionFileReader());
	mFileOutputData.push_back(fileReader->readAllFiles(path, ""));
}

void AcquisitionFixture::verifyFileData(cx::USReconstructInputData fileData)
{
	std::cout << "\tfilename: " << fileData.mFilename << std::endl;
	std::cout << "\tframe count: " << fileData.mFrames.size() << std::endl;
	std::cout << "\trecord duration: " << mRecordDuration << " ms" << std::endl;
	if (!fileData.mFrames.empty())
		std::cout << "\ttime: " << fileData.mFrames.back().mTime - fileData.mFrames.front().mTime << std::endl;

	double tolerance = 0.1;
	QString msg;

	REQUIRE(!fileData.mFilename.isEmpty());

	// check for enough received image frames
//	int framesPerSecond = 20; // minimum frame rate
	int framesPerSecond = 10; // minimum frame rate, reduced because tests on old computers have a hard time reaching 10 fps.
	msg = QString("Frames received: %1. Required: %2fps over %3ms").arg(fileData.mFrames.size()).arg(framesPerSecond).arg(mRecordDuration);
	INFO(string_cast(msg));
	CHECK(fileData.mFrames.size() > framesPerSecond*mRecordDuration/1000);

	// check for duration equal to input duration
	double frame_time_ms = fileData.mFrames.back().mTime - fileData.mFrames.front().mTime;
	msg = QString("Frames received over period: %1ms. Required: %2ms with a tolerance of %3").arg(frame_time_ms).arg(mRecordDuration).arg(tolerance);
	INFO(string_cast(msg));
	CHECK(cx::similar(frame_time_ms, mRecordDuration, tolerance*mRecordDuration));

	int positionsPerSecond = 10; // minimum tracker pos rate
	msg = QString("Tracker positions received: %1. Required: %2tps over %3ms").arg(fileData.mPositions.size()).arg(positionsPerSecond).arg(mRecordDuration);
	INFO(string_cast(msg));
	CHECK(fileData.mPositions.size() > positionsPerSecond*mRecordDuration/1000);

	// check for duration equal to input duration
	double pos_time_ms = fileData.mPositions.back().mTime - fileData.mPositions.front().mTime;
	msg = QString("Tracker positions received over period: %1ms. Required: %2ms with a tolerance of %3").arg(pos_time_ms).arg(mRecordDuration).arg(tolerance);
	INFO(string_cast(msg));
	CHECK(cx::similar(pos_time_ms, mRecordDuration, tolerance*mRecordDuration));

	CHECK(fileData.mProbeData.mData.getType()!=cx::ProbeDefinition::tNONE);

	// check content of images
	cx::ImageDataContainerPtr images = fileData.mUsRaw->getImageContainer();
	CHECK(images->size() == fileData.mFrames.size());
	for (unsigned i=0; i<images->size(); ++i)
	{
		CHECK(images->get(i));
		Eigen::Array3i dim(images->get(i)->GetDimensions());
		CHECK(dim[0]==fileData.mProbeData.mData.getSize().width());
		CHECK(dim[1]==fileData.mProbeData.mData.getSize().height());
	}
}

void AcquisitionFixture::verify()
{
	CHECK(mAcquisitionService->getNumberOfSavingThreads()==0);

	QString msg = QString("Got %1 streams, expected %2").arg(mFileOutputData.size()).arg(mNumberOfExpectedStreams);
	INFO(string_cast(msg));
	CHECK(mNumberOfExpectedStreams==mFileOutputData.size());

	std::cout << " ** Resulting USReconstructInputData memory content:" << std::endl;
	this->verifyFileData(mMemOutputData);

	for (unsigned i=0; i< mNumberOfExpectedStreams; ++i)
	{
		std::cout << QString(" ** Resulting USReconstructInputData file content [%1]:").arg(i) << std::endl;
		this->verifyFileData(mFileOutputData[i]);
	}
}

} // namespace cxtest
