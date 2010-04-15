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
#ifndef OST_GFX_ENTITY_DETAIL_HH
#define OST_GFX_ENTITY_DETAIL_HH

#include <map>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <ost/geom/geom.hh>

#include <ost/mol/mol.hh>
#include <ost/mol/entity_view.hh>
#include <ost/mol/query.hh>

#include <ost/gfx/module_config.hh>
#include <ost/gfx/color.hh>
#include <ost/gfx/gfx.hh>
#include <ost/gfx/gfx_prim.hh>
#include <ost/gfx/entity_fw.hh>


namespace ost { namespace gfx { namespace impl {

struct DLLEXPORT_OST_GFX AtomEntry 
{
  AtomEntry() {}
  AtomEntry(const mol::AtomHandle& a, float r, float v, const Color& c):
    atom(a),color(c),radius(r), vdwr(v) {}
  mol::AtomHandle atom;
  Color color;
  float radius;
  float vdwr;
};

// hash table for fast bond<->atom lookup
typedef std::map<long, AtomEntry> AtomEntryMap;
typedef std::vector<AtomEntry> AtomEntryList;

struct DLLEXPORT_OST_GFX BondEntry 
{
  BondEntry(const mol::BondHandle& b, float r,AtomEntry* ae1, AtomEntry* ae2):
    bond(b),atom1(ae1),atom2(ae2),radius(r),
    pp1(ae1 ? ae1->atom.GetPos() : geom::Vec3()),
    pp2(ae2 ? ae2->atom.GetPos() : geom::Vec3())
  {}
  mol::BondHandle bond;
  AtomEntry* atom1;
  AtomEntry* atom2;
  float radius;
  geom::Vec3 pp1,pp2; // prev atom positions
};

typedef std::vector<BondEntry> BondEntryList;

class DLLEXPORT_OST_GFX GfxView {
public:
  void Clear();
  void AddAtom(const mol::AtomView& av);
  void AddBond(const mol::BondHandle& bv);
  
  AtomEntryMap atom_map;
  BondEntryList bond_list;
  std::vector<long> orphan_atom_list;
};

typedef boost::shared_ptr<GfxView> GfxViewPtr;

struct DLLEXPORT_OST_GFX NodeEntry {
  mol::AtomHandle atom;
  Color color1, color2;
  geom::Vec3 direction,normal;
  float rad;
  geom::Vec3 v0,v1,v2; // helper vectors
  bool nflip;
};

typedef std::vector<NodeEntry> NodeEntryList;
typedef std::vector<NodeEntryList> NodeEntryListList;
typedef boost::shared_ptr<NodeEntryListList> NodeEntryListListPtr;

struct DLLEXPORT_OST_GFX TraceProfileEntry {
  TraceProfileEntry(): v(), n(), id(0) {}
  TraceProfileEntry(const geom::Vec3& v_,
                    const geom::Vec3& n_): v(v_),n(n_),id(0) {}
  geom::Vec3 v;
  geom::Vec3 n;
  VertexID id;
};
  
typedef std::vector<TraceProfileEntry> TraceProfile;

struct DLLEXPORT_OST_GFX SplineEntry {
  SplineEntry():
    position(0.0,0.0,0.0),
    direction(0.0,0.0,1.0),
    normal(0.0,1.0,0.0),
    color1(1.0,1.0,1.0,1.0),
    color2(1.0,1.0,1.0,1.0),
    rad(1.0),
    type(0)
  {
  }
  SplineEntry(const geom::Vec3& p, 
              const geom::Vec3& d,
              const geom::Vec3& n,
              float r,
              const Color& c1, const Color& c2,
              int t):
    position(p),direction(d),normal(n),color1(c1),color2(c2),rad(r),type(t),
    type1(t),type2(t),frac(0.0),v0(),v1(),v2(),nflip(false)
  {
  }

  geom::Vec3 position,direction,normal;
  Color color1, color2;
  float rad;
  int type;
  int type1, type2;
  float frac;
  geom::Vec3 v0,v1,v2; // helper vectors
  bool nflip;
};
  
typedef std::vector<SplineEntry> SplineEntryList;
typedef std::vector<SplineEntryList> SplineEntryListList;

class DLLEXPORT_OST_GFX Spline {
public:
  static SplineEntryList Generate(const SplineEntryList& entry_list,int nsub);
};

}}} // ns


#endif
