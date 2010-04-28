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
#include "entity_detail.hh"

namespace ost { 

using namespace mol;

namespace gfx { namespace impl {

  namespace {

    static const float default_radius=0.28;

  }

void GfxView::Clear() 
{
  atom_map.clear();
  bond_list.clear();
  orphan_atom_list.clear();
}

void GfxView::AddAtom(const AtomView& av)
{
  AtomHandle a=av.GetHandle();
  AtomEntry ae(a,default_radius,
               a.GetRadius(),
               GfxObj::Ele2Color(a.GetElement()));
  atom_map[a.GetHashCode()]=ae;
  if(av.GetBondCount()==0) {
    orphan_atom_list.push_back(a.GetHashCode());
  }
}

void GfxView::AddBond(const BondHandle& b)
{
  AtomEntry* ae1=NULL;
  AtomEntry* ae2=NULL;  
  AtomEntryMap::iterator it1=atom_map.find(b.GetFirst().GetHashCode());
  if (it1!=atom_map.end()) {
    ae1=&it1->second;
  }

  AtomEntryMap::iterator it2=atom_map.find(b.GetSecond().GetHashCode());
  if (it2!=atom_map.end()) {
    ae2=&it2->second;
  }
  if(!(ae1 || ae2)) {
    return;
  }  
  // deal with exclusive bonds by adding the second bond partner to the atom 
  // map. 
  if (ae1 && !ae2) {
    AtomHandle a=b.GetSecond();
    AtomEntry ae(a,default_radius,
                 a.GetRadius(),
                 GfxObj::Ele2Color(a.GetElement()));
    atom_map[a.GetHashCode()]=ae;
    ae2=&atom_map[a.GetHashCode()];
  }
  if (!ae1 && ae2) {
    AtomHandle a=b.GetFirst();
    AtomEntry ae(a,default_radius,
                 a.GetRadius(),
                 GfxObj::Ele2Color(a.GetElement()));
    atom_map[a.GetHashCode()]=ae;
    ae1=&atom_map[a.GetHashCode()];
  }
  bond_list.push_back(BondEntry(b,default_radius,ae1,ae2));
}


void SplineEntry::ToQuat()
{
  // assert orthonormal system
  // TODO: this seems broken
  geom::Vec3 dir = geom::Normalize(direction);
  geom::Vec3 norm0 = geom::Normalize(normal);
  geom::Vec3 norm2 = geom::Cross(dir,norm0);
  geom::Vec3 norm1 = geom::Cross(norm2,dir);
  geom::Mat3 rmat(dir[0],norm1[0],norm2[0],
                  dir[1],norm1[1],norm2[1],
                  dir[2],norm1[2],norm2[2]);
  
  geom::Quat quat(rmat);
  quat_value[0]=quat.w;
  quat_value[1]=quat.x;
  quat_value[2]=quat.y;
  quat_value[3]=quat.z;
}

void SplineEntry::FromQuat() 
{
  /* 
     assert orthornormal system since direction was
     probably adjusted for curvature
  */
  // TODO: this seems broken
  geom::Quat quat(quat_value[0],quat_value[1],quat_value[2],quat_value[3]);
  geom::Mat3 rmat = quat.ToRotationMatrix();
  geom::Vec3 norm0 = geom::Normalize(geom::Vec3(rmat(0,1),rmat(1,1),rmat(2,1)));
  geom::Vec3 dir = geom::Normalize(direction);
  geom::Vec3 norm2 = geom::Normalize(geom::Cross(dir,norm0));
  normal = geom::Normalize(geom::Cross(norm2,dir));
}

////////////////////////////


Spline::Spline():
  entry_list_()
{}


SplineEntry& Spline::AddEntry(const geom::Vec3& pos, const geom::Vec3& dir, const geom::Vec3& normal, float rad, const Color& c1, const Color& c2, int type)
{
  entry_list_.push_back(SplineEntry(pos,
                                    geom::Normalize(dir),
                                    geom::Normalize(normal),
                                    rad,c1,c2,type));
  entry_list_.back().ToQuat();
  return entry_list_.back();
}

static int bsplineGen(float *x,float *y, int n, float yp1, float ypn, float *y2);
static int bsplineGet(float *xa, float *ya, float *y2a, int n, float x, float *y);

#define SPLINE_ENTRY_INTERPOLATE(COMPONENT)             \
    for(int c=0;c<size;++c) {                           \
      yc[c]=entry_list_[c]. COMPONENT ;                 \
    }                                                   \
    bsplineGen(xp,yp,size,1.0e30,1.0e30,y2p);           \
    for(int c=0;c<size;++c) {                           \
      for(int d=0;d<nsub;++d) {                         \
        float u=static_cast<float>(c*nsub+d)*i_nsub;    \
        float v=0.0;                                    \
        bsplineGet(xp,yp,y2p,size,u,&v);                \
        sublist[c*nsub+d]. COMPONENT = v;               \
      }                                                 \
    }                                                   

SplineEntryList Spline::Generate(int nsub) const
{
  if(nsub<=0) {
    return entry_list_;
  }
  int size=entry_list_.size();
  if (size==0) {
    return entry_list_;
  }
  int ipsize=(size)*nsub;
  float i_nsub=1.0/static_cast<float>(nsub);
  std::vector<float> xc(size);
  std::vector<float> yc(size);
  std::vector<float> y2c(size);

  // array index 1 corrected pointers for NR routine
  float* xp=&xc[0]-1;
  float* yp=&yc[0]-1;
  float* y2p=&y2c[0]-1;

  // prepare xcoords, which are always the same
  for(int c=0;c<size;++c) {
    xc[c]=static_cast<float>(c);
  }

  // create sublist with enough entries
  SplineEntryList sublist(ipsize);

  // interpolate internal quaternion and color
  for(int k=0;k<4;++k) {
    SPLINE_ENTRY_INTERPOLATE(quat_value[k]);
    SPLINE_ENTRY_INTERPOLATE(color1[k]);
    SPLINE_ENTRY_INTERPOLATE(color2[k]);
  }

  for(int k=0;k<3;++k) {
    SPLINE_ENTRY_INTERPOLATE(position[k]);
    //SPLINE_ENTRY_INTERPOLATE(v0[k]);
    SPLINE_ENTRY_INTERPOLATE(v1[k]);
    //SPLINE_ENTRY_INTERPOLATE(v2[k]);
  }

  SPLINE_ENTRY_INTERPOLATE(rad);

  // assign direction and normal
  // entity trace has the same algorithm

  geom::Vec3 p0 = sublist[0].position;
  geom::Vec3 p1 = sublist[1].position;
  geom::Vec3 p2 = sublist[2].position;
  // normal of 0 is set at the end
  sublist[0].direction=geom::Normalize(p1-p0);
  sublist[0].v1=geom::Normalize(sublist[0].v1);
  geom::Vec3 orth = geom::Cross(sublist[0].direction,sublist[0].v1);
  sublist[0].v0 = geom::Normalize(geom::Cross(orth,sublist[0].direction));

  // reference normal to avoid twisting
  //geom::Vec3 nref=geom::Normalize(geom::Cross(p0-p1,p2-p1));
  unsigned int i=1;
  for(;i<sublist.size()-1;++i) {
    geom::Vec3 p10 = p0-p1;
    geom::Vec3 p12 = p2-p1;
    // correction for perfectly aligned consecutive directions
    if(p10==-p12 || p10==p12) p12+=geom::Vec3(0.001,0.001,0.001);
    sublist[i].normal=geom::Normalize(geom::Cross(p10,p12));
    // paranoid error checking due to occasional roundoff troubles
    float cosw = geom::Dot(geom::Normalize(p10),geom::Normalize(p12));
    cosw = std::min(float(1.0),std::max(float(-1.0),cosw));
    float omega=0.5*acos(cosw);
    orth=geom::AxisRotation(sublist[i].normal, -omega)*p12;
    sublist[i].direction=geom::Normalize(geom::Cross(sublist[i].normal,orth));
    // twist avoidance
    sublist[i].v1=geom::Normalize(sublist[i].v1);
    orth = geom::Cross(sublist[i].direction,sublist[i].v1);
    sublist[i].v0 = geom::Normalize(geom::Cross(orth,sublist[i].direction));
    if(geom::Dot(sublist[i-1].v0,sublist[i].v0)<0.0) {
      sublist[i].v0=-sublist[i].v0;
      //sublist[i].nflip = !sublist[i].nflip;
    }

    // avoid twisting
    //if(geom::Dot(sublist[i].normal,nref)<0.0) sublist[i].normal=-sublist[i].normal;
    //nref=sublist[i].normal;
    // skip over shift for the last iteration
    if(i==sublist.size()-2) break;
    // shift to i+1 for next iteration
    p0 = sublist[i].position;
    p1 = sublist[i+1].position;
    p2 = sublist[i+2].position;
  }
  // assign remaining ones
  sublist[0].normal=sublist[1].normal;
  sublist[i+1].direction=geom::Normalize(p2-p1);
  sublist[i+1].normal=sublist[i].normal;
  sublist[i+1].v1=geom::Normalize(sublist[i+1].v1);
  orth = geom::Cross(sublist[i+1].direction,sublist[i+1].v1);
  sublist[i+1].v0 = geom::Normalize(geom::Cross(orth,sublist[i+1].direction));

  // hack
  // TODO: merge this with above routine
  for(unsigned int i=0;i<sublist.size()-1;++i) {
    sublist[i].normal = sublist[i].v0;
  }

  // finally the non-interpolated type
  // with some tweaks for proper strand rendering
  for(int c=0;c<size;++c) {
    for(int d=0;d<nsub;++d) {
      sublist[c*nsub+d].type=entry_list_[c].type;
      int type1=entry_list_[c].type;
      int type2=entry_list_[std::min(c+1,size-1)].type;
      //int type0=entry_list_[std::max(0,c-1)].type;
      if(type1==2 && type2==3) {
        type1=2;
        type2=2;
      } else if(type1==3) {
        type1=4;
        // uncommenting this causes the strand arrows
        // to blend into a tip instead of the n+1
        // profile - gives visual artefacts
        //type2=3;
      }
      sublist[c*nsub+d].type1=type1;
      sublist[c*nsub+d].type2=type2;
      sublist[c*nsub+d].frac=float(d)/float(nsub);
    }
  }                                                   

  // the nflip flags for helices for correct inside/outside assignment
  int c=0;
  bool nflip=false;
  while(c<nsub*size-1) {
    int cstart=c;
    if(sublist[c].type==1 && sublist[c+1].type==1) {
      geom::Vec3 n = geom::Normalize(geom::Cross(sublist[c].normal,
                                                 sublist[c].direction));
      geom::Vec3 p0 = sublist[c].position+n;
      geom::Vec3 q0 = sublist[c].position-n;
      float psum=0.0;
      float qsum=0.0;
      ++c;
      while(sublist[c].type==1 && c<nsub*size) {
        n = geom::Normalize(geom::Cross(sublist[c].normal,
                                        sublist[c].direction));
        geom::Vec3 p1 = sublist[c].position+n;
        geom::Vec3 q1 = sublist[c].position-n;
        psum+=Length(p1-p0);
        qsum+=Length(q1-q0);
        p0=p1;
        q0=q1;
        ++c;
      }
      
      nflip = qsum>psum;
      for(int cc=cstart;cc<c;++cc) {
        sublist[cc].nflip=nflip;
      }
    } else {
      ++c;
    }
  }
  sublist.back().nflip=nflip;

  // done
  return sublist;
}

#undef SPLINE_ENTRY_INTERPOLATE

//////////////////////////////////////////////////////

/*
  bsplineGen and bsplineGet

  from numerical recipes in C
*/

/*
  NOTE THAT ARRAYS START WITH 1
*/

int bsplineGen(float *x,float *y, int n, float yp1, float ypn, float *y2)
{
  int i,k;
  float p,qn,sig,un;

  std::vector<float> u(n+1); // little STL addon

  if(yp1>0.99e30) {
    y2[1]=u[1]=0.0;
  } else {
    y2[1]=-0.5;
    u[1]=(3.0/(x[2]-x[1]))*((y[2]-y[1])/(x[2]-x[1])-yp1);
  }
  for(i=2;i<=n-1;i++) {
    sig=(x[i]-x[i-1])/(x[i+1]-x[i-1]);
    p=sig*y2[i-1]+2.0;
    y2[i]=(sig-1.0)/p;
    u[i]=(y[i+1]-y[i])/(x[i+1]-x[i])-(y[i]-y[i-1])/(x[i]-x[i-1]);
    u[i]=(6.0*u[i]/(x[i+1]-x[i-1])-sig*u[i-1])/p;
  }
  if(ypn>0.99e30) {
    qn=un=0.0;
  } else {
    qn=0.5;
    un=(3.0/(x[n]-x[n-1]))*(ypn-(y[n]-y[n-1])/(x[n]-x[n-1]));
  }
  y2[n]=(un-qn*u[n-1])/(qn*y2[n-1]+1.0);
  for(k=n-1;k>=1;k--)
    y2[k]=y2[k]*y2[k+1]+u[k];

  return 0;
}

int bsplineGet(float *xa, float *ya, float *y2a, int n, float x, float *y)
{
  int klo,khi,k;
  float h,b,a;

  klo=1;
  khi=n;
  while(khi-klo>1) {
    k=(khi+klo)>>1;
    if(xa[k]>x) 
      khi=k;
    else
      klo=k;
  }
  h=xa[khi]-xa[klo];
  if(h==0.0)
    return -1;
  a=(xa[khi]-x)/h;
  b=(x-xa[klo])/h;
  (*y)=a*ya[klo]+b*ya[khi]+((a*a*a-a)*y2a[klo]+(b*b*b-b)*y2a[khi])*(h*h)/6.0;
  return 0;
}



}}} // ns
