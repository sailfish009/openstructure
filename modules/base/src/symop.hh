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
#ifndef OST_SYMOP_HH
#define OST_SYMOP_HH

#include <stdexcept>
#include <ost/module_config.hh>
#include <ost/geom/vec3.hh>
#include <ost/geom/mat3.hh>
#include <ost/geom/vecmat3_op.hh>
#include <ost/geom/mat4.hh>
namespace ost {

class UnitCell;
  
/// \brief RT operator operating in fractional coordinates.
/// 
class DLLEXPORT_OST_BASE FractionalTransform {
public:
  FractionalTransform() : rot_(), trans_() { }
  
  explicit FractionalTransform(const geom::Mat3& rot, 
                               const geom::Vec3& trans=geom::Vec3()):
    rot_(rot), trans_(trans)
  { }
  
  const geom::Mat3& Rot() const { return rot_; }
  
  const geom::Vec3& Trans() const { return trans_; }
  
  // \brief get transformation in orthogonal
  FractionalTransform GetOrthoTF(const UnitCell& cell) const;
  
  geom::Mat4 ToMat() const
  {
    geom::Mat4 mat(rot_);
    mat.PasteTranslation(trans_);
    return mat;
  }
  
  bool operator==(const FractionalTransform& rhs) const
  {
    return rot_==rhs.rot_ && trans_==rhs.trans_;
  }
  
  bool operator!=(const FractionalTransform& rhs) const
  {
    return !this->operator==(rhs);
  }
private:
  geom::Mat3 rot_;
  geom::Vec3 trans_;
};

inline geom::Vec3 operator *(const FractionalTransform& r, 
                             const geom::Vec3& v) 
{ 
  return r.Rot()*v + r.Trans(); 
}

inline FractionalTransform operator *(const FractionalTransform& r1, 
                                      const FractionalTransform& r2) 
{ 
  return FractionalTransform(r1.Rot()*r2.Rot(), 
                             r1.Rot()*r2.Trans()+r1.Trans());
}

class DLLEXPORT SymopParseError : public std::exception {
public:
  SymopParseError(const String& message, const String& parse_string) throw():
    message_(message), parse_string_(parse_string)
  {
    std::stringstream ss;
    ss << "error while parsing '" << parse_string_ << "': " << message_;
    full_message_=ss.str();
  }
  ~SymopParseError() throw() { }
  virtual const char* what() const  throw() { return full_message_.c_str(); }
private:
  String message_;
  String parse_string_;
  String full_message_;
};


class DLLEXPORT_OST_BASE Symop : public FractionalTransform {
public:
  Symop(const geom::Mat3& rot, const geom::Vec3& trans): 
    FractionalTransform(rot, trans)
  { }
  
  Symop(): FractionalTransform()
  { }
  static Symop FromString(const String& symop_string);
  
}; 


typedef std::vector<Symop> SymopList;


}

#endif
