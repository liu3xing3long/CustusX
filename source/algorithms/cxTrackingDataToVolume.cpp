
#include "cxTrackingDataToVolume.h"

#include <vtkImageData.h>

#include "sscBoundingBox3D.h"
#include "sscDataManager.h"
#include "sscToolManager.h"
#include "sscRegistrationTransform.h"
#include "sscCoordinateSystemHelpers.h"
#include "sscVolumeHelpers.h"
#include "sscMessageManager.h"
#include "cxStateMachineManager.h"
#include "cxPatientData.h"

namespace cx
{
TrackingDataToVolume::TrackingDataToVolume()
{}

TrackingDataToVolume::~TrackingDataToVolume()
{}

std::vector<ssc::Vector3D> TrackingDataToVolume::extractPoints(ssc::TimedTransformMap& map_prMt)
{
  ssc::TimedTransformMap::iterator mapIter = map_prMt.begin();
    std::vector<ssc::Vector3D> positions_pr;
    while(mapIter != map_prMt.end())
    {
      ssc::Vector3D point_t = ssc::Vector3D(0,0,0);
      positions_pr.push_back(mapIter->second.coord(point_t));
      mapIter++;
    }
    return positions_pr;
}

ssc::DoubleBoundingBox3D TrackingDataToVolume::getBoundingBox(ssc::TimedTransformMap& map_prMt)
{
  std::vector<ssc::Vector3D> positions_pr = extractPoints(map_prMt);
  return ssc::DoubleBoundingBox3D::fromCloud(positions_pr);
}

ssc::ImagePtr TrackingDataToVolume::createEmptyImage(ssc::DoubleBoundingBox3D bounds_pr, double spacing)
{
  std::cout << "bounds:" << bounds_pr << std::endl;
  std::cout << "range:" << bounds_pr.range() << std::endl;
  ssc::Vector3D dim = ceil(bounds_pr.range() / spacing) + ssc::Vector3D(1,1,1);

  double maxVolumeSize = 10000000;//TODO: Set a good max value or set this as a parameter
  double size = dim[0]*dim[1]*dim[2];
  if(size > maxVolumeSize)
  {
    ssc::messageManager()->sendWarning("Tool position volume is going to be to big, making a smaller one.");
    spacing *= pow(size / maxVolumeSize, 1.0/3);
    dim = ceil(bounds_pr.range() / spacing) + ssc::Vector3D(1,1,1);
  }

  ssc::Vector3D spacingVector = ssc::Vector3D(1,1,1) * spacing;
  std::cout << "dim: " << dim << std::endl;
  vtkImageDataPtr data_pr = ssc::generateVtkImageData(dim, spacingVector, 0);

  ssc::ImagePtr image = ssc::dataManager()->createImage(data_pr, "tc%1", "Tool positions #%1", "Images");
  ssc::dataManager()->loadData(image);
  return image;
}

void TrackingDataToVolume::insertPoints(ssc::ImagePtr image_d, std::vector<ssc::Vector3D> points_pr)
{
  //TODO get someone to "double-check these numbers..." :P

  vtkImageDataPtr data_pr = image_d->getBaseVtkImageData();

  //convert points into image space (d) and insert a binary value into the image at the points location in the image
  ssc::CoordinateSystem pr = ssc::CoordinateSystemHelpers::getPr();
  ssc::CoordinateSystem d = ssc::CoordinateSystemHelpers::getD(image_d);
  ssc::Transform3D dMpr = ssc::CoordinateSystemHelpers::get_toMfrom(pr, d);

  std::vector<ssc::Vector3D>::iterator it = points_pr.begin();
  unsigned char point_value = 255; //or 255???
  for(; it != points_pr.end(); ++it)
  {
    ssc::Vector3D point_d = dMpr.coord((*it));
    ssc::Vector3D point_voxel = divide_elems(point_d, ssc::Vector3D(data_pr->GetSpacing()));
    point_voxel = round(point_voxel);

    //TODO make function
    unsigned char* voxel_d;
    int a = 25;
    for(int i=-a; i<=a; ++i)
    {
      for(int j=-a; j<=a; ++j)
      {
        for(int k=-a; k<=a; ++k)
        {
          if(rangeCheck(point_voxel[0]+i, point_voxel[1]+j, point_voxel[2]+k, data_pr->GetExtent()))
          {
            voxel_d = static_cast<unsigned char*>(data_pr->GetScalarPointer(point_voxel[0]+i, point_voxel[1]+j, point_voxel[2]+k));
            (*voxel_d) = point_value;
          }
        }
      }
    }
  }
}

void TrackingDataToVolume::setInput(ssc::TimedTransformMap map_prMt)
{
  ssc::DoubleBoundingBox3D bounds = getBoundingBox(map_prMt);
  double initialSpacing = 0.2;
  mImage = createEmptyImage(bounds, initialSpacing);

  ssc::Transform3D rMpr = *ssc::toolManager()->get_rMpr();
  std::cout << "rMpr\n" << rMpr << std::endl;
  ssc::Transform3D rMd = rMpr * ssc::createTransformTranslate(bounds.corner(0,0,0)); // TODO + eller - ?????
  std::cout << "rMd\n" << rMd << std::endl;

  //TODO Should it be identity or rMpr.inv()???
  mImage->get_rMd_History()->setRegistration(rMd);

  std::vector<ssc::Vector3D> data_pr = this->extractPoints(map_prMt);
  this->insertPoints(mImage, data_pr);
}

ssc::ImagePtr TrackingDataToVolume::getOutput()
{
  return mImage;
}
}//cx

