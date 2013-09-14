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

#include "cxProbe.h"

#include <QStringList>
#include "sscVideoSource.h"
#include "sscMessageManager.h"
#include "sscProbeSector.h"
#include "sscProbeAdapterRTSource.h"
#include "sscTypeConversions.h"
#include "sscVector3D.h"
#include "sscLogger.h"
#include "cxDataLocations.h"
#include "cxCreateProbeDataFromConfiguration.h"

namespace cx
{

ProbePtr cxProbe::New(QString instrumentUid, QString scannerUid, ProbeXmlConfigParserPtr xml)
{
	cxProbe* object = new cxProbe(instrumentUid, scannerUid);
	ProbePtr retval(object);
	object->mSelf = retval;
	retval->initProbeXmlConfigParser(xml);
	retval->initConfigId();
	return retval;
}

bool cxProbe::isValid() const
{
	return this->getProbeData("active").getType() != ssc::ProbeData::tNONE;
}

QStringList cxProbe::getAvailableVideoSources()
{
	QStringList retval;
	for (std::map<QString, ssc::VideoSourcePtr>::iterator iter=mSource.begin(); iter!=mSource.end(); ++iter)
		retval << iter->first;
	return retval;
}

ssc::VideoSourcePtr cxProbe::getRTSource(QString uid) const
{
	if (mSource.empty())
		return ssc::VideoSourcePtr();
	if (uid=="active")
		uid = mActiveUid;
	if (mSource.count(uid))
		return mSource.find(uid)->second;
	return mSource.begin()->second;
}

ssc::ProbeData cxProbe::getProbeData(QString uid) const
{
	ssc::ProbeData retval;

	if (uid=="active")
		uid = mActiveUid;
	if (mProbeData.count(uid))
		retval = mProbeData.find(uid)->second;
	else if (mProbeData.count("default"))
		retval = mProbeData.find("default")->second;
	else
		retval = mProbeData.begin()->second;

	// ensure uid is matching the requested uid even if not found.
	retval.setUid(uid);
	return retval;
}

ssc::ProbeSectorPtr cxProbe::getSector(QString uid)
{
	ssc::ProbeSectorPtr retval(new ssc::ProbeSector());
	retval->setData(this->getProbeData());
	return retval;
}

void cxProbe::addXml(QDomNode& dataNode)
{
	QDomDocument doc = dataNode.ownerDocument();
	dataNode.toElement().setAttribute("config", mConfigurationId);
}

void cxProbe::parseXml(QDomNode& dataNode)
{
	if (dataNode.isNull())
		return;
	QString cfg = dataNode.toElement().attribute("config");
	if (cfg.isEmpty())
		return;
	this->applyNewConfigurationWithId(cfg);
}

QStringList cxProbe::getConfigIdList() const
{
	if (!this->hasRtSource())
		return QStringList();
	QStringList configIdList = mXml->getConfigIdList(
			this->getInstrumentScannerId(), this->getInstrumentId(), this->getRtSourceName());
	return configIdList;
}

QString cxProbe::getConfigName(QString configString) ///< get a name for the given configuration
{
	ProbeXmlConfigParser::Configuration config = this->getConfiguration(configString);
	return config.mName;
}

QString cxProbe::getConfigId() const
{
	return mConfigurationId;
}

QString cxProbe::getConfigurationPath() const
{
	if (!this->hasRtSource())
		return "";
	QStringList retval;
	retval << this->getInstrumentScannerId() << this->getInstrumentId() << this->getRtSourceName() << this->getConfigId();
	return retval.join(":");
}

void cxProbe::applyNewConfigurationWithId(QString uid)
{
	this->setConfigId(uid);
	this->updateProbeSector();
	this->updateTemporalCalibration();
	this->setSoundSpeedCompensationFactor(mSoundSpeedCompensationFactor);
	emit sectorChanged();
	emit activeConfigChanged();
}

void cxProbe::setTemporalCalibration(double val)
{
	mOverrideTemporalCalibration = true;
	mTemporalCalibration = val;
	for (std::map<QString, ssc::ProbeData>::iterator iter=mProbeData.begin(); iter!=mProbeData.end(); ++iter)
		iter->second.setTemporalCalibration(mTemporalCalibration);
}

void cxProbe::setSoundSpeedCompensationFactor(double factor)
{
	if(ssc::similar(mSoundSpeedCompensationFactor, factor))
		return;
	mSoundSpeedCompensationFactor = factor;
	for (std::map<QString, ssc::ProbeData>::iterator iter=mProbeData.begin(); iter!=mProbeData.end(); ++iter)
		iter->second.applySoundSpeedCompensationFactor(mSoundSpeedCompensationFactor);
	emit sectorChanged();
}

void cxProbe::setProbeSector(ssc::ProbeData probeSector)
{
	if (probeSector.getUid().isEmpty())
		probeSector.setUid(mActiveUid);

	mProbeData[probeSector.getUid()] = probeSector;
	emit sectorChanged();
}

void cxProbe::setRTSource(ssc::VideoSourcePtr source)
{
	SSC_ASSERT(source); // not handled after refactoring - add clear method??
	if (!source)
		return;

	// uid already exist: check if base object is the same
	if (mSource.count(source->getUid()))
	{
		ssc::VideoSourcePtr old = mSource.find(source->getUid())->second;

		boost::shared_ptr<ssc::ProbeAdapterRTSource> oldAdapter;
		oldAdapter = boost::dynamic_pointer_cast<ssc::ProbeAdapterRTSource>(old);
		// check for identity, ignore if no change
		if (oldAdapter && (source==oldAdapter->getBaseSource()))
			return;
	}

	// must have same uid as original: the uid identifies the video source
	mSource[source->getUid()].reset(new ssc::ProbeAdapterRTSource(source->getUid(), mSelf.lock(), source));
	emit sectorChanged();
}

void cxProbe::removeRTSource(ssc::VideoSourcePtr source)
{
	if (!source)
		return;
	if (!mSource.count(source->getUid()))
		return;

	mSource.erase(source->getUid());
	mProbeData.erase(source->getUid());
	emit sectorChanged();
}

void cxProbe::setActiveStream(QString uid)
{
	if (uid.isEmpty())
		return;
	mActiveUid = uid;
	emit sectorChanged();
}

QString cxProbe::getActiveStream() const
{
	return mActiveUid;
}

ProbeXmlConfigParser::Configuration cxProbe::getConfiguration() const
{
	ProbeXmlConfigParser::Configuration config = this->getConfiguration(this->getConfigId());
	return config;
}

void cxProbe::removeCurrentConfig()
{
	ProbeXmlConfigParser::Configuration config = this->getConfiguration();

	int index = this->getConfigIdList().indexOf(config.mConfigId);
	if (index<0)
		return;
	if (index!=0)
		--index;

	mXml->removeConfig(config.mUsScanner, config.mUsProbe, config.mRtSource, config.mConfigId);
	if (index < this->getConfigIdList().size())
		this->applyNewConfigurationWithId(this->getConfigIdList()[index]);
	emit sectorChanged();
}

void cxProbe::saveCurrentConfig(QString uid, QString name)
{
	ProbeXmlConfigParser::Configuration config = this->getConfiguration();
	config.mConfigId = uid;
	config.mName = name;
	config = createConfigurationFromProbeData(config, this->getProbeData("active"));

	mXml->saveCurrentConfig(config);
	this->applyNewConfigurationWithId(uid);
}

void cxProbe::useDigitalVideo(bool digitalStatus)
{
	mDigitalInterface = digitalStatus;
	if (mDigitalInterface)
		this->applyNewConfigurationWithId("Digital");
}

bool cxProbe::isUsingDigitalVideo() const
{
	return mDigitalInterface;
}

QString cxProbe::getRtSourceName() const
{
	QStringList rtSourceList = mXml->getRtSourceList(this->getInstrumentScannerId(), this->getInstrumentId());
	if (rtSourceList.empty())
		return QString();
	QString rtSource = rtSourceList.at(0);
	if (this->isUsingDigitalVideo())
		rtSource = "Digital";
	return rtSource;
}

cxProbe::cxProbe(QString instrumentUid, QString scannerUid) :
		mInstrumentUid(instrumentUid),
		mScannerUid(scannerUid),
		mSoundSpeedCompensationFactor(1.0),
		mOverrideTemporalCalibration(false),
		mTemporalCalibration(0.0),
		mDigitalInterface(false)
{
	ssc::ProbeData probeData;
	mProbeData[probeData.getUid()] = probeData;
	mActiveUid = probeData.getUid();
}

void cxProbe::initProbeXmlConfigParser(ProbeXmlConfigParserPtr xml = ProbeXmlConfigParserPtr())
{
	if (!xml)
	{
		QString xmlFileName = cx::DataLocations::getRootConfigPath() + QString("/tool/ProbeCalibConfigs.xml");
		mXml.reset(new ProbeXmlConfigParserImpl(xmlFileName));
	} else
		mXml = xml;
}

void cxProbe::initConfigId()
{
	QStringList configs = this->getConfigIdList();
	if (!configs.isEmpty())
		this->applyNewConfigurationWithId(configs[0]);
	else
	{
		ssc::messageManager()->sendWarning(QString("Found no probe configuration for:\n"
			"scanner=[%1] instrument=[%2].\n"
			"Check that your %3 file contains entries\n"
			"<USScanner> <Name>%1</Name> ... <USProbe> <Name>%2</Name>").arg(mScannerUid).arg(mInstrumentUid).arg(mXml->getFileName()));
	}
}

ProbeXmlConfigParser::Configuration cxProbe::getConfiguration(QString uid) const
{
	ProbeXmlConfigParser::Configuration config;
	if(this->hasRtSource())
		config = mXml->getConfiguration(mScannerUid, mInstrumentUid, this->getRtSourceName(), uid);
	return config;
}

QString cxProbe::getInstrumentId() const
{
	return mInstrumentUid;
}

QString cxProbe::getInstrumentScannerId() const
{
	return mScannerUid;
}

bool cxProbe::hasRtSource() const
{
	return !(this->getRtSourceName().isEmpty());
}

void cxProbe::setConfigId(QString uid)
{
	mConfigurationId = uid;
}

void cxProbe::updateProbeSector()
{
	if(this->isValidConfigId() && !this->isUsingDigitalVideo())
	{
		ssc::ProbeData probeSector = this->createProbeSector();
		this->setProbeSector(probeSector);
	}
}

bool cxProbe::isValidConfigId()
{
	//May need to create ProbeXmlConfigParser::isValidConfig(...) also
	return !this->getConfiguration(this->getConfigId()).isEmpty();
}

ssc::ProbeData cxProbe::createProbeSector()
{
	ProbeXmlConfigParser::Configuration config = this->getConfiguration(this->getConfigId());
	ssc::ProbeData probeSector = createProbeDataFromConfiguration(config);
	probeSector.setUid(mActiveUid);
	return probeSector;
}

void cxProbe::updateTemporalCalibration()
{
	if (mOverrideTemporalCalibration)
		this->setTemporalCalibration(mTemporalCalibration);
}


} //namespace cx
