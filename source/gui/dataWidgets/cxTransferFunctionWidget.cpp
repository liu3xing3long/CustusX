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

#include "cxTransferFunctionWidget.h"

#include <QVBoxLayout>
#include <QComboBox>
#include <QStringList>
#include <QInputDialog>
#include <QPushButton>
#include <QActionGroup>
#include <QToolButton>
#include <QAction>
#include <QMessageBox>

#include "cxImageTF3D.h"
#include "cxImageLUT2D.h"

#include "cxTypeConversions.h"
#include "cxTransferFunctionPresetWidget.h"
#include "cxTransferFunctionAlphaWidget.h"
#include "cxTransferFunctionColorWidget.h"
#include "cxShadingWidget.h"
#include "cxDataViewSelectionWidget.h"
#include "cxShadingParamsInterfaces.h"
#include "cxSettings.h"
#include "cxPatientModelService.h"
#include "cxLogicManager.h"

namespace cx
{

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

DoubleDataAdapterImageTFDataBase::DoubleDataAdapterImageTFDataBase()
{
}

void DoubleDataAdapterImageTFDataBase::setImageTFData(ImageTFDataPtr tfData, ImagePtr image)
{
  if (mImageTFData)
	disconnect(mImageTFData.get(), &ImageTFData::transferFunctionsChanged, this, &DataAdapter::changed);

  mImageTFData = tfData;
  mImage = image;

  if (mImageTFData)
	connect(mImageTFData.get(), &ImageTFData::transferFunctionsChanged, this, &DataAdapter::changed);

  emit changed();
}

double DoubleDataAdapterImageTFDataBase::getValue() const
{
  if (!mImageTFData)
    return 0.0;
  return this->getValueInternal();
}

bool DoubleDataAdapterImageTFDataBase::setValue(double val)
{
  if (!mImageTFData)
    return false;
  this->setValueInternal(val);
  return true;
}

//---------------------------------------------------------
//---------------------------------------------------------

double DoubleDataAdapterImageTFDataWindow::getValueInternal() const
{
  return mImageTFData->getWindow();
}

void DoubleDataAdapterImageTFDataWindow::setValueInternal(double val)
{
  mImageTFData->setWindow(val);
}

DoubleRange DoubleDataAdapterImageTFDataWindow::getValueRange() const
{
  if (!mImage)
    return DoubleRange();
  double range = mImage->getMax() - mImage->getMin();
  return DoubleRange(1,range,range/1000.0);
}

//---------------------------------------------------------
//---------------------------------------------------------

double DoubleDataAdapterImageTFDataLevel::getValueInternal() const
{
  return mImageTFData->getLevel();
}

void DoubleDataAdapterImageTFDataLevel::setValueInternal(double val)
{
  mImageTFData->setLevel(val);
}

DoubleRange DoubleDataAdapterImageTFDataLevel::getValueRange() const
{
  if (!mImageTFData)
    return DoubleRange();

  double max = mImage->getMax();
  double min = mImage->getMin();
  return DoubleRange(min,max,1);
}

//---------------------------------------------------------
//---------------------------------------------------------

double DoubleDataAdapterImageTFDataLLR::getValueInternal() const
{
  return mImageTFData->getLLR();
}
void DoubleDataAdapterImageTFDataLLR::setValueInternal(double val)
{
  mImageTFData->setLLR(val);
}
DoubleRange DoubleDataAdapterImageTFDataLLR::getValueRange() const
{
  if (!mImageTFData)
    return DoubleRange();

  double max = mImage->getMax();
  double min = mImage->getMin();
	//Set range to min - 1 to allow an llr that shows all values
	return DoubleRange(min - 1,max,(max-min)/1000.0);
}

//---------------------------------------------------------
//---------------------------------------------------------

double DoubleDataAdapterImageTFDataAlpha::getValueInternal() const
{
  return mImageTFData->getAlpha();
}
void DoubleDataAdapterImageTFDataAlpha::setValueInternal(double val)
{
  mImageTFData->setAlpha(val);
}
DoubleRange DoubleDataAdapterImageTFDataAlpha::getValueRange() const
{
  if (!mImageTFData)
    return DoubleRange();

  double max = 1.0;
  return DoubleRange(0,max,max/100.0);
}

//---------------------------------------------------------
//---------------------------------------------------------

TransferFunction3DWidget::TransferFunction3DWidget(PatientModelServicePtr patientModelService, QWidget* parent) :
  BaseWidget(parent, "TransferFunction3DWidget", "3D"),
  mLayout(new QVBoxLayout(this)),
  mPatientModelService(patientModelService)
{
  mTransferFunctionAlphaWidget = new TransferFunctionAlphaWidget(patientModelService, this);
  mTransferFunctionColorWidget = new TransferFunctionColorWidget(patientModelService, this);

//  mDataWindow.reset(new DoubleDataAdapterImageTFDataWindow);
//  mDataLevel.reset(new DoubleDataAdapterImageTFDataLevel);
//  mDataAlpha.reset(new DoubleDataAdapterImageTFDataAlpha);
//  mDataLLR.reset(new DoubleDataAdapterImageTFDataLLR);

  mActiveImageProxy = ActiveImageProxy::New(patientModelService);
  connect(mActiveImageProxy.get(), &ActiveImageProxy::activeImageChanged, this, &TransferFunction3DWidget::activeImageChangedSlot);
  connect(mActiveImageProxy.get(), &ActiveImageProxy::transferFunctionsChanged, this, &TransferFunction3DWidget::activeImageChangedSlot);

  mTransferFunctionAlphaWidget->setSizePolicy(QSizePolicy::MinimumExpanding,
                                              QSizePolicy::MinimumExpanding);
  mTransferFunctionColorWidget->setSizePolicy(QSizePolicy::Expanding,
                                              QSizePolicy::Fixed);

  mLayout->addWidget(mTransferFunctionAlphaWidget);
  mLayout->addWidget(mTransferFunctionColorWidget);

//  QGridLayout* gridLayout = new QGridLayout;
//  mLayout->addLayout(gridLayout);
//  new SliderGroupWidget(this, mDataWindow, gridLayout, 0);
//  new SliderGroupWidget(this, mDataLevel,  gridLayout, 1);
//  new SliderGroupWidget(this, mDataAlpha,  gridLayout, 2);
//  new SliderGroupWidget(this, mDataLLR,    gridLayout, 3);

  this->setLayout(mLayout);
}

QString TransferFunction3DWidget::defaultWhatsThis() const
{
  return "<html>"
    "<h3>3D Transfer Function</h3>"
    "<p>Lets you set a transfer function on a 3D volume.</p>"
    "<p><i></i></p>"
    "</html>";
}

void TransferFunction3DWidget::activeImageChangedSlot()
{
  ImagePtr image = mPatientModelService->getActiveImage();
  ImageTFDataPtr tf;
  if (image)
    tf = image->getTransferFunctions3D();
  else
    image.reset();

  mTransferFunctionAlphaWidget->setData(image, tf);
  mTransferFunctionColorWidget->setData(image, tf);

//  mDataWindow->setImageTFData(tf);
//  mDataLevel->setImageTFData(tf);
//  mDataAlpha->setImageTFData(tf);
//  mDataLLR->setImageTFData(tf);
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

TransferFunction2DWidget::TransferFunction2DWidget(PatientModelServicePtr patientModelService, QWidget* parent) :
  BaseWidget(parent, "TransferFunction2DWidget", "2D"),
  mLayout(new QVBoxLayout(this)),
  mPatientModelService(patientModelService)
{
  mTransferFunctionAlphaWidget = new TransferFunctionAlphaWidget(patientModelService, this);
  mTransferFunctionAlphaWidget->setReadOnly(true);
  mTransferFunctionColorWidget = new TransferFunctionColorWidget(patientModelService, this);

  mDataWindow.reset(new DoubleDataAdapterImageTFDataWindow);
  mDataLevel.reset(new DoubleDataAdapterImageTFDataLevel);
  mDataAlpha.reset(new DoubleDataAdapterImageTFDataAlpha);
  mDataLLR.reset(new DoubleDataAdapterImageTFDataLLR);

  mActiveImageProxy = ActiveImageProxy::New(patientModelService);
  connect(mActiveImageProxy.get(), &ActiveImageProxy::activeImageChanged, this, &TransferFunction2DWidget::activeImageChangedSlot);
  connect(mActiveImageProxy.get(), &ActiveImageProxy::transferFunctionsChanged, this, &TransferFunction2DWidget::activeImageChangedSlot);

  mTransferFunctionAlphaWidget->setSizePolicy(QSizePolicy::MinimumExpanding,
                                              QSizePolicy::MinimumExpanding);
  mTransferFunctionColorWidget->setSizePolicy(QSizePolicy::Expanding,
                                              QSizePolicy::Fixed);

  mLayout->addWidget(mTransferFunctionAlphaWidget);
  mLayout->addWidget(mTransferFunctionColorWidget);

  QGridLayout* gridLayout = new QGridLayout;
  mLayout->addLayout(gridLayout);
  new SliderGroupWidget(this, mDataWindow, gridLayout, 0);
  new SliderGroupWidget(this, mDataLevel,  gridLayout, 1);
  new SliderGroupWidget(this, mDataAlpha,  gridLayout, 2);
  new SliderGroupWidget(this, mDataLLR,    gridLayout, 3);

  this->setLayout(mLayout);
}

QString TransferFunction2DWidget::defaultWhatsThis() const
{
  return "<html>"
    "<h3>2D Transfer Function</h3>"
    "<p>Lets you set a transfer function on a 2D image.</p>"
    "<p><i></i></p>"
    "</html>";
}

void TransferFunction2DWidget::activeImageChangedSlot()
{
  ImagePtr image = mPatientModelService->getActiveImage();
  ImageTFDataPtr tf;
  if (image)
    tf = image->getLookupTable2D();
  else
    image.reset();

  mTransferFunctionAlphaWidget->setData(image, tf);
  mTransferFunctionColorWidget->setData(image, tf);

  mDataWindow->setImageTFData(tf, image);
  mDataLevel->setImageTFData(tf, image);
  mDataAlpha->setImageTFData(tf, image);
  mDataLLR->setImageTFData(tf, image);
}


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

TransferFunctionWidget::TransferFunctionWidget(PatientModelServicePtr patientModelService, QWidget* parent) :
  BaseWidget(parent, "TransferFunctionWidget", "Transfer Function")
{
  QVBoxLayout* mLayout = new QVBoxLayout(this);

  mLayout->setMargin(0);
  mLayout->addWidget(new TransferFunction3DWidget(patientModelService, this));
  mLayout->addWidget(new TransferFunctionPresetWidget(patientModelService, this, true));

  this->setLayout(mLayout);
}

QString TransferFunctionWidget::defaultWhatsThis() const
{
  return "<html>"
    "<h3>Transfer Function.</h3>"
    "<p>Lets you set a new or predefined transfer function on a volume.</p>"
    "<p><i></i></p>"
    "</html>";
}
}//namespace cx
