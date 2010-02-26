//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2010 by the OpenStructure authors
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
#ifndef OST_SURFACE_IMPL_RSURF_HH
#define OST_SURFACE_IMPL_RSURF_HH

/*
  A fast reduced surface implementation to generate molecular surfaces
  
  The concept of a reduced surface was introduced by M Sanner et al, 1984

  Author: Ansgar Philippsen
*/

#include <vector>
#include <deque>

#include <boost/shared_ptr.hpp>

#include <ost/geom/geom.hh>
#include <ost/mol/spatial_organizer.hh>
#include "surface_impl_fw.hh"
#include <ost/mol/surface_prop.hh>

namespace ost { namespace mol { namespace rsurf {

// helper types for triangulation
// \internal
struct EdgePoint
{
  EdgePoint(): id(0),pos() {}
  EdgePoint(SurfaceVertexID i, const geom::Vec3& p, const geom::Vec3& n): 
    id(i), pos(p), norm(n) {}
  SurfaceVertexID id;
  geom::Vec3 pos,norm;
};

/// \internal
typedef std::vector<EdgePoint> EdgePointList;

// required before sphere
/// \internal
struct Arc;

/// \internal
typedef boost::shared_ptr<Arc> ArcP;

/// \internal
typedef std::vector<ArcP> ArcList;

/// \internal
struct ArcDirEntry {
  EdgePointList epl;
  EdgePoint v,w;
  ArcP arc;
  bool inv_arc;
};

/// \internal
typedef std::deque<ArcDirEntry> ArcDirDeque;

// basic sphere
struct Sphere {
  Sphere(const geom::Vec3& p, Real r): pos(p), rad(r) {}
  geom::Vec3 pos;
  Real rad;
  String name;
  ArcDirDeque arc_list;
  std::vector<geom::Vec3> top_list;
  std::vector<ArcDirDeque> arc_list_list;
};
  
typedef boost::shared_ptr<Sphere> SphereP;
typedef std::vector<SphereP> SphereList;

struct Tet;
typedef boost::shared_ptr<Tet> TetP;


// arc that bridges two tets
/*
  the orientation of H,K,S,T is such that the rotation of S
  around the HK axis towards T is positive right handed:

    S  T   (S is in front, T in back)
     \/
   K----H

   i.e. for HK=Vec3(0,0,1) and S=Vec3(1,0,0), a 90deg
   positive rotation results in Vec3(0,1,0)
 
*/
struct Arc {
  Arc();
  Arc(const SphereP& h, const SphereP& k, const SphereP& s);
  void SetT(const SphereP& t);

  SphereP H,K;         // the two sphere that form the axis
  SphereP S,T;         // the start and end solvent sphere
  geom::Vec3 axis;     // normalized axis from H to K
  geom::Vec3 fixpoint; // point on axis where sdir and tdir start
  geom::Vec3 sdir;     // direction normal from axis to S
  geom::Vec3 tdir;     // direction normal from axis to T
  Real phi;          // angle from S to T
  geom::Mat3 xmat;     // transformation matrix needed during traversal
  geom::Vec3 mid;
  EdgePointList eplS, eplT; // book-keeping for the triangulation
  TetP A,B;
  uint step;           // sync edge stepsize for triangulation
};
  

// the constellation of three spheres with a locked solvent sphere
/*
  when viewed with the solvent sphere in front of the abc triplet,
  [ABC] are arranged clockwise
*/
struct Tet {
  // use already determined sphere
  Tet(const SphereP& a, const SphereP& b, const SphereP& c, const Sphere& s);
  // use existing S
  Tet(const SphereP& a, const SphereP& b, const SphereP& c, const SphereP& s);

  ArcP GetArc(const SphereP& h, const SphereP& k);

  SphereP A,B,C,S;
  ArcP U,V,W;

  geom::Vec3 cA,cB,cC; // contact points of S with A,B and C

  bool parity_swap;

  void init();
};
  
typedef std::vector<TetP> TetList;

// spatial organizer for optimized search
typedef SpatialOrganizer<SphereP> SpatOrg;

class DLLEXPORT_OST_MOL RSurf
{
public:
  // ctor
  RSurf(Real probe_radius);

  void AddSphere(const geom::Vec3& pos, Real rad, const String& name);

  // construct reduced surface from spheres
  void Build();

  // triangulate rsurf and thereby build full molecular surface
  void Triangulate(impl::SurfaceImplP& surface, Real patch_size);

  // return probe radius
  Real GetProbeRadius() const {return probe_radius_;}

  // internal use
  ArcList GetArcList() const {return arc_list_;}
  // internal use
  TetList GetTetList() const {return tet_list_;}
  // internal use
  SphereList GetSphereList() const {return sphere_list_;}

private:

  // main recursive routine
  void traverse_arc(TetP tet, ArcP& arc, SphereP c);

  // get tet from list, optionally creating it if necessary
  // the solvent s is either used as a parity check, or as the new sphere
  std::pair<TetP,bool> get_tet(SphereP a, SphereP b, SphereP c, const Sphere& s, bool create);

  // find best tet for an arc traversal
  std::pair<TetP,bool> find_best_tet(ArcP arc, SphereP c);

  SphereList get_nearest_spheres(const geom::Vec3& pos, SphereP a, SphereP b);

  Real calc_phi(ArcP arc, SphereP x, bool prune_zero);

  SphereList sphere_list_;
  ArcList arc_list_;
  TetList tet_list_;
  Real probe_radius_;
  Real max_rad_;
  SpatOrg spat_org_;
};


}}} // ns

#endif
