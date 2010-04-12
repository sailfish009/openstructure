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

/*
  Author: Ansgar Philippsen
*/

#include <map>
#include <vector>
#include <limits>

#include <ost/geom/geom.hh>
#include <ost/gfx/vertex_array.hh>

#include "calc_ambient.hh"

using namespace ost;
using namespace ost::gfx;

namespace {

  typedef IndexedVertexArray::EntryList EntryList;
  typedef IndexedVertexArray::IndexList IndexList;
  

  std::pair<geom::Vec3, geom::Vec3> calc_limits(const EntryList& elist)
  {
    geom::Vec3 minc(std::numeric_limits<float>::max(),
		    std::numeric_limits<float>::max(),
		    std::numeric_limits<float>::max());
    geom::Vec3 maxc(-std::numeric_limits<float>::max(),
		    -std::numeric_limits<float>::max(),
		    -std::numeric_limits<float>::max());

    for(EntryList::const_iterator it=elist.begin();it!=elist.end();++it) {
      minc[0]=std::min(it->v[0],minc[0]);
      minc[1]=std::min(it->v[1],minc[1]);
      minc[2]=std::min(it->v[2],minc[2]);
      maxc[0]=std::max(it->v[0],maxc[0]);
      maxc[1]=std::max(it->v[1],maxc[1]);
      maxc[2]=std::max(it->v[2],maxc[2]);
    }
    return std::make_pair(minc,maxc);
  }

  struct CIndex { 
    CIndex(): u(0),v(0),w(0) {}
    CIndex(long uu, long vv, long ww): u(uu), v(vv), w(ww) {}
    long u,v,w;
    bool operator<(const CIndex& rhs) const {
      return u==rhs.u ? (v==rhs.v ? w<rhs.w : v<rhs.v) : u<rhs.u;
    }
  };

  struct CEntry {
    CEntry(): id(0), v(),n(),c() {}
    CEntry(unsigned int ii, const geom::Vec3& vv, const geom::Vec3& nn, const Color& cc): id(ii), v(vv), n(nn), c(cc) {}
    unsigned int id;
    geom::Vec3 v;
    geom::Vec3 n;
    Color c;
  };

  typedef std::map<CIndex,std::vector<CEntry> > CMap;

  class AmbientOcclusionBuilder {
  public:
    AmbientOcclusionBuilder(IndexedVertexArray& va): va_(va), cmap_(), bsize_(5.0), weight_(0.2), cutoff2_(25.0) {}

    CIndex coord_to_index(const geom::Vec3& v) 
    {
      return CIndex(static_cast<int>(floor(v[0]/bsize_)),
		    static_cast<int>(floor(v[1]/bsize_)),
		    static_cast<int>(floor(v[2]/bsize_)));
    }
    
    void build_cmap() {
      const EntryList& elist = va_.GetEntries();
      const IndexList& tlist = va_.GetTriIndices();
      const IndexList& qlist = va_.GetQuadIndices();

      cmap_.clear();
      
      for(unsigned int c=0;c<tlist.size();c+=3) {
	const float* v0=elist[tlist[c+0]].v;
	const float* v1=elist[tlist[c+1]].v;
	const float* v2=elist[tlist[c+2]].v;
	const float* n0=elist[tlist[c+0]].n;
	const float* n1=elist[tlist[c+1]].n;
	const float* n2=elist[tlist[c+2]].n;
	const float* c0=elist[tlist[c+0]].c;
	const float* c1=elist[tlist[c+1]].c;
	const float* c2=elist[tlist[c+2]].c;
	add_to_cmap(c,
		    geom::Vec3((v0[0]+v1[0]+v2[0])/3.0,
			       (v0[1]+v1[1]+v2[1])/3.0,
			       (v0[2]+v1[2]+v2[2])/3.0),
		    geom::Normalize(geom::Vec3((n0[0]+n1[0]+n2[0]),
					       (n0[1]+n1[1]+n2[1]),
					       (n0[2]+n1[2]+n2[2]))),
		    Color((c0[0]+c1[0]+c2[0])/3.0,
			  (c0[1]+c1[1]+c2[1])/3.0,
			  (c0[2]+c1[2]+c2[2])/3.0,
			  (c0[3]+c1[3]+c2[3])/3.0));
      }
      for(unsigned int c=0;c<qlist.size();c+=4) {
	const float* v0=elist[tlist[c+0]].v;
	const float* v1=elist[tlist[c+1]].v;
	const float* v2=elist[tlist[c+2]].v;
	const float* v3=elist[tlist[c+3]].v;
	const float* n0=elist[tlist[c+0]].n;
	const float* n1=elist[tlist[c+1]].n;
	const float* n2=elist[tlist[c+2]].n;
	const float* n3=elist[tlist[c+3]].n;
	const float* c0=elist[tlist[c+0]].c;
	const float* c1=elist[tlist[c+1]].c;
	const float* c2=elist[tlist[c+2]].c;
	const float* c3=elist[tlist[c+3]].c;
	add_to_cmap(c,
		    geom::Vec3((v0[0]+v1[0]+v2[0]+v3[0])/4.0,
			       (v0[1]+v1[1]+v2[1]+v3[1])/4.0,
			       (v0[2]+v1[2]+v2[2]+v3[2])/4.0),
		    geom::Normalize(geom::Vec3((n0[0]+n1[0]+n2[0]+n3[0]),
					       (n0[1]+n1[1]+n2[1]+n3[1]),
					       (n0[2]+n1[2]+n2[2]+n3[2]))),
		    Color((c0[0]+c1[0]+c2[0]+c3[0])/4.0,
			  (c0[1]+c1[1]+c2[1]+c3[1])/4.0,
			  (c0[2]+c1[2]+c2[2]+c3[2])/4.0,
			  (c0[3]+c1[3]+c2[3]+c3[3])/4.0));
      }
    }

    void add_to_cmap(unsigned int id, const geom::Vec3& v, const geom::Vec3& n, const Color& c) 
    {
      CIndex cindex=coord_to_index(v);
      CMap::iterator it=cmap_.find(cindex);
      if(it==cmap_.end()) {
	std::vector<CEntry> tmplist(1);
	tmplist[0]=CEntry(id,v,n,c);
	cmap_[cindex]=tmplist;
      } else {
	it->second.push_back(CEntry(id,v,n,c));
      }
    }

    void accumulate(unsigned int eid, const CIndex& cindex, const geom::Vec3& epos, const geom::Vec3& enorm, Color& color, float& factor)
    {
      CMap::iterator cit=cmap_.find(cindex);
      if(cit==cmap_.end()) return;
      std::vector<CEntry>::const_iterator eit2=cit->second.end();
      for(std::vector<CEntry>::const_iterator eit=cit->second.begin();eit!=eit2;++eit) {
	geom::Vec3 diff=(eit->v-epos);
	float l2=geom::Length2(diff);
	if(l2<cutoff2_) {
	  // angle between vertex normal and direction to close face
	  float cw1=geom::Dot(geom::Normalize(diff),enorm);
	  // only consider those that are "in front"
	  if(cw1>0.0) {
	    // angle between direction and close face normal
	    float cw2=1.0-geom::Dot(diff,eit->n);
	    // only consider those that point towards each other
	    if(cw2>0.0) {
	      cw2*=cw1;
	      color[0]+=cw2*eit->c[0];
	      color[1]+=cw2*eit->c[1];
	      color[2]+=cw2*eit->c[2];
	      factor+=1.0;
	    }
	  }
	}
      }
    }

    void calc_all() {
      const EntryList& elist = va_.GetEntries();

      for(unsigned int c=0;c<elist.size();++c) {
	geom::Vec3 epos(elist[c].v[0],elist[c].v[1],elist[c].v[2]);
	geom::Vec3 enorm(elist[c].n[0],elist[c].n[1],elist[c].n[2]);
	CIndex cindex0=coord_to_index(epos);
	Color color;
	float factor;

	for(int w=-1;w<=1;++w) {
	  for(int v=-1;v<=1;++v) {
	    for(int u=-1;u<=1;++u) {
	      accumulate(c,CIndex(cindex0.u+u,cindex0.v+v,cindex0.w+w),epos,enorm,color,factor);
	    }
	  }
	}

	color[0]/=factor;
	color[1]/=factor;
	color[2]/=factor;
	color[3]=weight_;
	va_.SetAmbientColor(c,color);
      }

    }

  private:
    IndexedVertexArray& va_;
    CMap cmap_;
    float bsize_;
    float weight_;
    float cutoff2_;
  };
  
} // ns



void ost::gfx::CalcAmbientTerms(IndexedVertexArray& va)
{
  AmbientOcclusionBuilder aob(va);
  aob.build_cmap();
  aob.calc_all();
}
