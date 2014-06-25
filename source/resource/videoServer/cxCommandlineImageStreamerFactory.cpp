#include "cxCommandlineImageStreamerFactory.h"

#include "cxReporter.h"
#include "cxImageStreamerOpenCV.h"
#include "cxMHDImageStreamer.h"
#include "cxImageStreamerSonix.h"
#include "cxImageStreamerGE.h"
#include "cxConfig.h"

namespace cx
{

int convertStringWithDefault(QString text, int def)
{
	bool ok = true;
	int retval = text.toInt(&ok,0);
	if (ok)
		return retval;
	return def;
}

StringMap extractCommandlineOptions(QStringList cmdline)
{
	StringMap retval;

	for (int i = 0; i < cmdline.size(); ++i)
	{
		if (!cmdline[i].startsWith("--"))
			continue;
		QString name, val;
		name = cmdline[i].remove(0, 2);
		if (i + 1 < cmdline.size())
			val = cmdline[i + 1];
		retval[name] = val;
	}

	return retval;
}


///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

StreamerPtr CommandlineImageStreamerFactory::getFromArguments(StringMap args)
{
	QString type = this->getDefaultSenderType();
	if (args.count("type"))
		type = args["type"];

	StreamerPtr streamer = this->getImageSender(type);

	if (streamer)
		reportSuccess("Created sender of type: "+type);
	else
	{
		reportError("Failed to create sender based on type: "+type);
		return streamer;
	}

	CommandLineStreamerPtr commandlinestreamer = boost::dynamic_pointer_cast<CommandLineStreamer>(streamer);
	if(commandlinestreamer)
		commandlinestreamer->initialize(args);

	DummyImageStreamerPtr mhdimagestreamer = boost::dynamic_pointer_cast<DummyImageStreamer>(streamer);
	QString filename = args["filename"];
	bool secondary = args.count("secondary") ? true : false;
	if(mhdimagestreamer)
		mhdimagestreamer->initialize(filename, secondary);

	return streamer;
}


CommandlineImageStreamerFactory::CommandlineImageStreamerFactory()
{
#ifdef CX_WIN32
	mCommandLineStreamers.push_back(CommandLineStreamerPtr(new ImageStreamerSonix()));
#endif
#ifdef CX_USE_OpenCV
	std::cout << "!" << std::endl;
	mCommandLineStreamers.push_back(CommandLineStreamerPtr(new ImageStreamerOpenCV()));
#endif
#ifdef CX_USE_ISB_GE
	mCommandLineStreamers.push_back(CommandLineStreamerPtr(new ImageStreamerGE()));
#endif
	mImageStreamers.push_back(DummyImageStreamerPtr(new DummyImageStreamer()));
}

QString CommandlineImageStreamerFactory::getDefaultSenderType() const
{
	// use the FIRST sender available
  QString retval = mCommandLineStreamers.front()->getType();
  return retval;
}

QStringList CommandlineImageStreamerFactory::getSenderTypes() const
{
	QStringList retval;
	for (unsigned i=0; i< mCommandLineStreamers.size(); ++i)
		retval << mCommandLineStreamers[i]->getType();
	for (unsigned i=0; i< mImageStreamers.size(); ++i)
		retval << mImageStreamers[i]->getType();
	return retval;
}

QStringList CommandlineImageStreamerFactory::getArgumentDescription(QString type) const
{
	QStringList retval;
	for (unsigned i=0; i< mCommandLineStreamers.size(); ++i)
	{
		if (mCommandLineStreamers[i]->getType()==type)
			return mCommandLineStreamers[i]->getArgumentDescription();
	}
	return retval;
}

StreamerPtr CommandlineImageStreamerFactory::getImageSender(QString type)
{
	for (unsigned i=0; i< mCommandLineStreamers.size(); ++i)
	{
		if (mCommandLineStreamers[i]->getType()==type)
			return mCommandLineStreamers[i];
	}
	for (unsigned i=0; i< mImageStreamers.size(); ++i)
	{
		if (mImageStreamers[i]->getType()==type)
			return mImageStreamers[i];
	}
	return StreamerPtr();
}


}