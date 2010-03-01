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
  DataViewer proxy

  Authors: Ansgar Philippsen, Andreas Schenk
*/

#ifndef IMG_GUI_DATA_VIEWER_PROXY_H
#define IMG_GUI_DATA_VIEWER_PROXY_H

#include <boost/shared_ptr.hpp>

#include <ost/img/point.hh>
#include <ost/img/extent.hh>
#include <ost/img/normalizer_impl.hh>

#include <ost/gui/data_viewer/overlay_base_fw.hh>
#include <ost/gui/data_viewer/overlay_manager_fw.hh>
#include <ost/gui/data_viewer/data_viewer.hh>
#include <ost/gui/module_config.hh>

#include "sip_handler.hh"

class QWidget;
class QString;

namespace ost { namespace img { namespace gui {

// fw decl
class DataViewer;

class TEMPLATE_EXPORT DataViewerProxy : public ost::gui::SipHandler<DataViewer>{
public:
  DataViewerProxy(DataViewer* v);

  // data viewer interface

  void SetData(const Data& data);
  void SetName(const String& name);

  NormalizerPtr GetNormalizer() const;
  void Renormalize();
  void Recenter();
  void UpdateView();

  Extent GetSelection() const;
  int AddOverlay(const OverlayPtr& ov, bool make_active=true);
  OverlayManagerPtr GetOverlayManager() const;
  void ClearOverlays();

  void AddDockWidget(QWidget* w, const QString& name, bool shown=true);
  void RemoveDockWidget(QWidget* w);

  void SetAntialiasing(bool f);

  // ensure that data viewer is visible
  void Show();

  //void ShowClickedPosition(bool show=true);
  //Vec3 GetClickedPosition();

  void Hide();  

};

typedef boost::shared_ptr<DataViewerProxy> DataViewerProxyPtr;

}}}  //ns

#endif


