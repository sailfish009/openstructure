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
#ifndef OST_SPATIAL_ORGANIZER_HI
#define OST_SPATIAL_ORGANIZER_HI

#include <vector>
#include <map>
#include <cmath>
#include <limits>

#include <ost/geom/geom.hh>

namespace ost { namespace mol {

//! spatial organizer
/*
  organizes ITEMs defined by positions
  in a spatial hash map, into bins of
  a defined size (as given in the ctor)
*/
template <class ITEM, class VEC=geom::Vec3>
class DLLEXPORT_OST_MOL SpatialOrganizer {
public:
  typedef std::vector<ITEM> ItemList;

private:
  struct Index {
    Index():
      u(0),v(0),w(0) {}
    Index(int uu, int vv, int ww): 
      u(uu),v(vv),w(ww) {}

    bool operator<(const Index& other) const {
      return w!=other.w ? w<other.w : (v!=other.v ? v<other.v : u<other.u);
    }

    int u,v,w;
  };

  struct Entry {
    Entry(const ITEM& i, const VEC& p): item(i), pos(p) {}

    ITEM item;
    VEC pos;
  };

  typedef std::vector<Entry> EntryList;

  typedef std::map<Index,EntryList> ItemMap;

public:

  SpatialOrganizer(Real delta): delta_(delta) {
    if(delta==0.0) {
      throw "delta cannot be zero";
    }
  }

  void Add(const ITEM& item, const VEC& pos) {
    Index indx=gen_index(pos);
    map_[indx].push_back(Entry(item,pos));
  }

  void Remove(const ITEM& item) {
    typename ItemMap::iterator i=map_.begin();
    for (; i!=map_.end(); ++i) {
      for (size_t j=0; j<i->second.size(); ++j) {
        if (i->second[j].item==item) {
          i->second.erase(i->second.begin()+j);
          return;        
        }        
      }
    }
  }

  ITEM FindClosest(const VEC& pos) {

    // find closest index with non-empty itemlist
    Real best_dist2=std::numeric_limits<Real>::max();
    Index i0;
    bool found_i0=false;
    for(typename ItemMap::const_iterator map_it = map_.begin();
        map_it!=map_.end();++map_it) {
      Real dist2=geom::Length2(pos-gen_middle(map_it->first));
      if(dist2<best_dist2 && !map_it->second.empty()) {
        best_dist2=dist2;
        i0 = map_it->first;
        found_i0=true;
      }
    }

    if(!found_i0) return ITEM();

    // now find the closest item in the 3x3x3 items centered on i0
    best_dist2=std::numeric_limits<Real>::max();
    ITEM* best_item=NULL;

    for(int wc=i0.w-1;wc<=i0.w+1;++wc) {
      for(int vc=i0.v-1;wc<=i0.v+1;++vc) {
        for(int uc=i0.u-1;wc<=i0.u+1;++uc) {
	  typename ItemMap::const_iterator map_it = map_.find(Index(uc,vc,wc));

	  if(map_it!=map_.end()) {
	    for(typename EntryList::iterator entry_it = map_it->second.begin();
		entry_it != map_it->second.end(); ++entry_it) {
	      Real delta_x = entry_it->pos[0]-pos[0];
              Real delta_y = entry_it->pos[1]-pos[1];
              Real delta_z = entry_it->pos[2]-pos[2];
              Real dist2=delta_x*delta_x+delta_y*delta_y+delta_z*delta_z;
              if(dist2<best_dist2) {
                best_dist2=dist2;
                best_item=entry_it;
	      }
	    }
	  }
	}
      }
    }
    if(best_item) {
      return &best_item;
    } else {
      return ITEM();
    }
  }

  ItemList FindWithin(const VEC& pos, Real dist) const {
    Real dist2=dist*dist;
    Index imin = gen_index(pos-VEC(dist,dist,dist));
    Index imax = gen_index(pos+VEC(dist,dist,dist));

    ItemList item_list;

    for(int wc=imin.w;wc<=imax.w;++wc) {
      for(int vc=imin.v;vc<=imax.v;++vc) {
	for(int uc=imin.u;uc<=imax.u;++uc) {
	  typename ItemMap::const_iterator map_it = map_.find(Index(uc,vc,wc));

	  if(map_it!=map_.end()) {
	    for(typename EntryList::const_iterator entry_it = map_it->second.begin();
		entry_it != map_it->second.end(); ++entry_it) {
              /*
                speed tests indicate that pre-calculating dx2 or dy2
                and pre-checking them with an additional if gives little
                speed improvement for very specific circumstances only,
                but most of the time the performance is worse. 
              */
	      Real delta_x = entry_it->pos[0]-pos[0];
              Real delta_y = entry_it->pos[1]-pos[1];
              Real delta_z = entry_it->pos[2]-pos[2];
              if(delta_x*delta_x+delta_y*delta_y+delta_z*delta_z<=dist2) {
                item_list.push_back(entry_it->item);
	      }
	    }
	  }
	}
      }
    }
    return item_list;
  }
  void Clear()
  {
    map_.clear();
  }
  void Swap(SpatialOrganizer& o) {
    map_.swap(o.map_);
    std::swap(delta_,o.delta_);
  }

private:

  ItemMap map_;
  Real delta_;

  Index gen_index(const VEC& pos) const {
    Index nrvo(static_cast<int>(round(pos[0]/delta_)),
               static_cast<int>(round(pos[1]/delta_)),
               static_cast<int>(round(pos[2]/delta_)));
    return nrvo;
  }

  geom::Vec3 gen_middle(const Index& i) const {
    geom::Vec3 nrvo((static_cast<Real>(i.u)+0.5)*delta_,
                    (static_cast<Real>(i.v)+0.5)*delta_,
                    (static_cast<Real>(i.w)+0.5)*delta_);
    return nrvo;
  }

};

}} // ns

#endif
