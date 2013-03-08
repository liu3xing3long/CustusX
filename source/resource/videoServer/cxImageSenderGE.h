/*
 * cxImageSenderGE.h
 *
 *  Created on: Sep 19, 2012
 *      Author: olevs
 */

#ifndef CXIMAGESENDERGE_H_
#define CXIMAGESENDERGE_H_

#include <QObject>

#ifdef CX_USE_ISB_GE
#include "boost/shared_ptr.hpp"
#include <QTcpSocket>
#include <QDateTime>
#include <QSize>
class QTimer;
#include <QStringList>
#include "igtlImageMessage.h"
#include "cxImageSenderFactory.h"
#include "cxIGTLinkImageMessage.h"

#include "GEStreamer.h"

namespace cx
{
/**An object sending images out on an ip port.
 * In order to operate within a nongui thread,
 * it must be created within the run() method
 * of a qthread.
 *
 * This version uses the NTNU ISB data streamer module (provided by Gabriel Kiss)
 * to grab images from the E9 EG scanner (and similar)
 */
class ImageSenderGE: public ImageSender
{
	Q_OBJECT
public:
	ImageSenderGE(QObject* parent = NULL);
	virtual ~ImageSenderGE() {}

	virtual void initialize(StringMap arguments);
	virtual bool startStreaming(GrabberSenderPtr sender);
	virtual void stopStreaming();

	virtual QString getType();
	virtual QStringList getArgumentDescription();

protected:
private:
	GrabberSenderPtr mSender;
//	QTcpSocket* mSocket;
	bool mInitialized;
	QTimer* mSendTimer;
	QTimer* mGrabTimer;
	StringMap mArguments;

	//The GE Connection code from ISB
	data_streaming::GEStreamer mGEStreamer;

	vtkSmartPointer<data_streaming::vtkExportedStreamData> mImgExportedStream;//Last image from GE

	igstk::RealTimeClock::TimeStampType mLastGrabTime;
	data_streaming::frame_geometry mFrameGeometry;///<Frame geometry from GE
	bool mFrameGeometryChanged; ///< Have frame geometry changed since last frame

	data_streaming::frame_geometry mFlowGeometry;///<Frame geometry for flow data (doppler) from GE
	bool mFlowGeometryChanged; ///< Have flow data frame geometry changed since last frame

	//What kind of video streams are requested?
   	bool mExportScanconverted;
   	bool mExportTissue;
   	bool mExportBandwidth;
   	bool mExportFrequency;

	bool initialize_local();
	void deinitialize_local();
	//Deprecated
//	IGTLinkImageMessage::Pointer getImageMessage();
//	IGTLinkUSStatusMessage::Pointer getFrameStatus();
	ssc::ProbeData getFrameStatus(QString uid, data_streaming::frame_geometry geometry, vtkSmartPointer<vtkImageData> img);
	void send(const QString& uid, const vtkImageDataPtr& img, data_streaming::frame_geometry geometry, bool geometryChanged);

	//Compare to geometry structs
	bool equal(data_streaming::frame_geometry a, data_streaming::frame_geometry b);

private slots:
	void grab();
	void send();
};

}

#endif // CX_USE_ISB_GE
#endif // CXIMAGESENDERGE_H_
