//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2010 by the OpenStructure authors
// Copyright (C) 2003-2010 by the IPLT authors
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 3.0 of the License, or (at your option)
// any later version.
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//------------------------------------------------------------------------------

/*
  Authors: Ansgar Philippsen, Andreas Schenk
*/

#include <ost/message.hh>
//#include <ost/gui/snapshot.hh>

#include "data_viewer_proxy.hh"

namespace ost { namespace img { namespace gui {

DataViewerProxy::DataViewerProxy(DataViewer* v):
  ost::gui::SipHandler<ost::img::gui::DataViewer>(v)
{}


NormalizerPtr DataViewerProxy::GetNormalizer() const
{
  return Me()->GetNormalizer();
}

void DataViewerProxy::Renormalize()
{
  Me()->Renormalize();
}

void DataViewerProxy::UpdateView()
{
  Me()->UpdateView();
}

void DataViewerProxy::Recenter()
{
  Me()->Recenter();
}

Extent DataViewerProxy::GetSelection() const
{
  return Me()->GetSelection();
}

void DataViewerProxy::SetData(const Data& d)
{
  Me()->SetData(d);
}

void DataViewerProxy::SetName(const String& name)
{
  Me()->SetName(name);
}

int DataViewerProxy::AddOverlay(const OverlayPtr& ov, bool make_active)
{
  return Me()->AddOverlay(ov,make_active);
}

void DataViewerProxy::ClearOverlays()
{
  Me()->ClearOverlays();
}

OverlayManagerPtr DataViewerProxy::GetOverlayManager() const
{
  return Me()->GetOverlayManager();
}


void DataViewerProxy::AddDockWidget(QWidget* w, const QString& name, bool shown)
{
  Me()->AddDockWidget(w,name, shown);
}

void DataViewerProxy::RemoveDockWidget(QWidget* w)
{
  Me()->RemoveDockWidget(w);
}

void DataViewerProxy::SetAntialiasing(bool f)
{
  Me()->SetAntialiasing(f);
}

void DataViewerProxy::Show()
{
  Me()->show();
}

void DataViewerProxy::Hide() 
{
  Me()->hide();
}



}}}  //ns
