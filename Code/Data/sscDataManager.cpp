#include "sscDataManager.h"

#include <vtkImageData.h>
#include <vtkMetaImageReader.h>
#include <vtkSmartPointer.h>
typedef vtkSmartPointer<class vtkMetaImageReader> vtkMetaImageReaderPtr;

#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
#include <vtkSTLReader.h>
typedef vtkSmartPointer<class vtkPolyDataReader> vtkPolyDataReaderPtr;
typedef vtkSmartPointer<class vtkSTLReader> vtkSTLReaderPtr;

#include "sscDataManagerImpl.h"
#include "sscImage.h"
#include "sscMesh.h"

namespace ssc
{

// --------------------------------------------------------
DataManager* DataManager::mInstance = NULL; ///< static member
// --------------------------------------------------------

DataManager* DataManager::getInstance()
{
	if (!mInstance)
	{
		DataManagerImpl::initialize();
	}
	return mInstance;
}

void DataManager::setInstance(DataManager* instance)
{
	if (mInstance)
	{
		delete mInstance;
	}
	mInstance = instance;
}

DataManager::DataManager()
{
}

DataManager::~DataManager()
{
}

ImagePtr DataManager::getActiveImage() const { return ImagePtr(); } ///< used for system state
void DataManager::setActiveImage(ImagePtr activeImage) {} ///< used for system state


} // namespace ssc
