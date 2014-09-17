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

#ifndef CXVIEWWIDGETLAYOUT_H_
#define CXVIEWWIDGETLAYOUT_H_

#include "cxView.h"
#include "cxLayoutData.h"
#include "cxViewCache.h"
#include "cxViewWidget.h"
#include "cxLayoutWidget.h"

class QGridLayout;

namespace cx
{

/** A widget containing all the views generated by ViewManager,
 *  described by a LayoutData.
 *
 * \ingroup cx_service_visualization
 * \date 05.11.2013
 * \author christiana
 */
class LayoutWidgetUsingViewWidgets : public LayoutWidget
{
	Q_OBJECT
public:
	LayoutWidgetUsingViewWidgets();
	~LayoutWidgetUsingViewWidgets();

	ViewPtr addView(View::Type type, LayoutRegion region);
	void showViews();
	void clearViews();

private:
	void setStretchFactors(LayoutRegion region, int stretchFactor); ///< Set the stretch factors of columns and rows in mLayout.
	void addView(ViewWidget* view, LayoutRegion region);

	boost::shared_ptr<ViewCache<ViewWidget> > mViewCache2D;
	boost::shared_ptr<ViewCache<ViewWidget> > mViewCache3D;
	boost::shared_ptr<ViewCache<ViewWidget> > mViewCacheRT;
	boost::shared_ptr<ViewCache<ViewWidget> > mViewCache;
	QGridLayout* mLayout; ///< the layout
	std::vector<ViewWidget*> mViews;
};


} // namespace cx

#endif /* CXVIEWWIDGETLAYOUT_H_ */
