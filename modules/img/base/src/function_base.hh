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
  function base

  Author: Ansgar Philippsen
*/

#ifndef IMG_FUNCTION_BASE_H
#define IMG_FUNCTION_BASE_H

#include "module_config.hh"
#include "data.hh"
#include "data_observer.hh"
#include "observable.hh"
#include "pixel_sampling.hh"

namespace ost { namespace img {

// forward declaration
class NonModAlgorithm;
class ImageHandle;

//! Base class that defines the function interface
/*!
  Function only partially implementes the Data interface and 
  is thus still an abstract base class. In particular, the 
  methods GetType(), GetReal() and GetComplex() are still
  pure virtual methods.

  In contrast to ImageHandle, a ConstFunction class is not
  necessary, since assignement and copy-construction do
  not share the underlying implementation.
*/

/*
  Observer implementation will result in empty observer list
  upon copy construction or assignement.
*/

class DLLEXPORT_OST_IMG_BASE Function: public Data {
  typedef Observable<DataObserver> DataObservable;

 public:
  /*! @name Initialization and deconstruction
    No publically accessible ctors, Function is an abstract base class.
   */
  //@{
  virtual ~Function();
  //@}

  /*! @name Properties
  */
  //@{

  //! See Data::GetType
  virtual DataType GetType() const = 0;

  //! Returns data domain, defaults to \c SPATIAL
  virtual DataDomain GetDomain() const;

  //! Returns extent of function
  /*!
    defaults to [Point(0,0,0), Point(0,0,0)], may be
    modified by using SetExtent
  */
  virtual Extent GetExtent() const;
  //! set Extent that this function is evaluated in
  void SetExtent(const Extent& e);

  //! see Data::SetSpatialOrigin
  virtual void SetSpatialOrigin(const Point& o);
  //! see Data::GetSpatialOrigin
  virtual Point GetSpatialOrigin() const;

  //@}

  /*! @name Value retrieval
    The pure virtual methods GetReal() and GetComplex() are
    only given here for completeness. They still need to be 
    implemented by a derived class.
  */
  //@{

  //! see Data:GetReal
  virtual Real GetReal(const Point& p) const; 

  //! see Data:GetComplex
  virtual Complex GetComplex(const Point& p) const; 

  //! Get interpolated real value from 3D,2D or 1D vec
  virtual Real GetIntpolReal(const Vec3 &v) const=0;
  virtual Real GetIntpolReal(const Vec2 &v) const;
  virtual Real GetIntpolReal(const Real &d) const;

  //! Get interpolated complex value from 3D,2D or 1D vec
  virtual Complex GetIntpolComplex(const Vec3 &v) const=0;
  virtual Complex GetIntpolComplex(const Vec2 &v) const;
  virtual Complex GetIntpolComplex(const Real &d) const;

  //@}


  /*! @name Algorithm interface
    On the level of Function, only NonModAlgorithms may be applied
  */
  //@{
  // use doc from Data
  virtual void Apply(NonModAlgorithm& a) const;
  virtual void ApplyIP(NonModAlgorithm& a) const;
  //@}

  /*! @name Observer interface
   */
  // use doc from Data
  //@{
  virtual void Attach(DataObserver *o) const;
  virtual void Detach(DataObserver *o) const;
  virtual void Notify() const;
  //@}


 protected:
  //! Initialization with Domain (required) and Extent(defaults to Size(1))
  Function(DataDomain d, const Extent& e=Extent());

  Function(const Function& f);

  Function& operator=(const Function& f);

  /*! @name Sampling implementation
   */
  //@{
  virtual PixelSampling& Sampling();
  virtual const PixelSampling& Sampling() const;
  //@}

private:
  Extent extent_;
  DataDomain domain_;

  /*
    implement logical constness for Attach and Detach
  */
  mutable DataObservable obs_;

  PixelSampling sampling_;
};

}} // namespace

#endif
