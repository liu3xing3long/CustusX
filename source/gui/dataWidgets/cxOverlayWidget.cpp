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

#include "cxOverlayWidget.h"
#include "cxDataViewSelectionWidget.h"
#include "cxTransferFunction2DOpacityWidget.h"
#include "cxDataSelectWidget.h"
#include "cxSelectDataStringDataAdapter.h"
#include "cxViewWrapper2D.h"

namespace cx {

OverlayWidget::OverlayWidget(PatientModelServicePtr patientModelService, QWidget* parent) :
		BaseWidget(parent, "OverlayWidget", "Overlay")
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->addWidget(new TransferFunction2DOpacityWidget(patientModelService, this), 0);
	layout->addWidget(new DataViewSelectionWidget(this), 1);
	this->setEnabled(ViewWrapper2D::overlayIsEnabled());
}

QString OverlayWidget::defaultWhatsThis() const{
	return "<html>"
			"<h3>2D overlay.</h3>"
			"<p>"
			"Allow visualizing several 2D slices  in top of each other "
			"by making them partly transparent (alpha), "
			"or by making parts of the fully transparent (LLR)"
			"</p>"
			"<p>"
			"<b>NB!</b> The functionality in this widget only works on Linux for now"
			"</p>"
			"<p><i></i></p>"
			"</html>";
}

} /* namespace cx */
