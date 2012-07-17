//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2011 by the OpenStructure authors
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
#ifndef OST_MOL_BASE_TRANSFORM_HH
#define OST_MOL_BASE_TRANSFORM_HH

/*
  Author: Ansgar Philippsen
*/

#include <ost/geom/geom.hh>
#include <ost/info/info_fw.hh>
#include <ost/mol/module_config.hh>

namespace ost { namespace mol {

/// \brief basic and essential transformation class, including translation,
///    rotation and center of rotation
class DLLEXPORT_OST_MOL Transform {
public:
  Transform();

  geom::Mat4 GetMatrix() const;
  geom::Mat4 GetTransposedMatrix() const;

  void SetCenter(const geom::Vec3& c);
  geom::Vec3 GetCenter() const;

  // directly set tm, messing up rot/trans/cen !
  void SetMatrix(const geom::Mat4& m);

  /// \name rotation
  //@{
  /// \brief rotate around x-axis
  void ApplyXAxisRotation(float delta);
  void ApplyYAxisRotation(float delta);
  void ApplyZAxisRotation(float delta);
  void ApplyAxisRotation(float delta, const geom::Vec3& axis);  
  
  geom::Mat3 GetXAxisRotation(float delta);
  geom::Mat3 GetYAxisRotation(float delta);
  geom::Mat3 GetZAxisRotation(float delta);
  
  geom::Mat3 GetRot() const;
  void SetRot(const geom::Mat3& r);
  //@}
  /// \brief translation
  //@{
  void ApplyXAxisTranslation(float delta);
  void ApplyYAxisTranslation(float delta);
  void ApplyZAxisTranslation(float delta);
  void SetTrans(const geom::Vec3& t);
  geom::Vec3 GetTrans() const;  
  //@}
  
  geom::Vec3 Apply(const geom::Vec3& v) const;
  geom::Vec4 Apply(const geom::Vec4& v) const;

private:
  geom::Mat3 rot_;
  geom::Vec3 trans_;
  geom::Vec3 cen_;
  geom::Mat4 tm_;
  geom::Mat4 ttm_;

  void update_tm();
};

/// \brief read transformation from info group
/// \relates Transform
Transform DLLEXPORT_OST_MOL TransformFromInfo(const info::InfoGroup& group);
/// \brief store transformation in info group
/// \relates Transform
void DLLEXPORT_OST_MOL TransformToInfo(const Transform& transform,
                                       info::InfoGroup& group);
}} // ns

#endif
