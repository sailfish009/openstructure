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

#include <ost/gfx/gl_helper.hh>
#include <ost/gfx/scene.hh>
#include <ost/gfx/color.hh>

#include "entity_detail.hh"


namespace ost { 

using namespace mol;
using namespace geom; // this is used _so_ often, its worth pulling it in

namespace gfx { namespace impl {

namespace {

static const float default_radius=0.28;

struct BlurQuadEntry
{
  float zdist;
  Vec3 p1,p2,p3,p4;
  gfx::Color c1,c2,c3,c4;
};

struct BlurQuadEntryLess
{
  bool operator()(const BlurQuadEntry& e1, const BlurQuadEntry& e2)
  {
    // provides back-to-front sorting
    return e1.zdist<e2.zdist;
  }
};

} // anon ns

void DoRenderBlur(BondEntryList& bl, float bf1, float bf2)
{
  // add blur for this particular orientation!
  // don't use vertex array, but on-the-fly oriented and z-sorted quads
  mol::Transform tf = Scene::Instance().GetTransform();

  std::vector<BlurQuadEntry> bql;
  for (BondEntryList::iterator it=bl.begin(); it!=bl.end();++it) {

    if(!it->atom1 || !it->atom2) continue;

    const Vec3 p0=tf.Apply(it->atom1->atom.GetPos());
    const Vec3 p2=tf.Apply(it->atom2->atom.GetPos());
    Vec3 p1=(p0+p2)*0.5;

    const Vec3 q0=tf.Apply(it->pp1);
    const Vec3 q2=tf.Apply(it->pp2);
    Vec3 q1=(q0+q2)*0.5;

    float ll0 = Length2(p0-q0);
    float ll1 = Length2(p1-q1);
    float ll2 = Length2(p2-q2);

    if(ll0<1e-2 && ll1<1e-2 && ll2<1e-2) continue;

    float x0 = exp(-bf1*ll0);
    float x1 = exp(-bf1*ll1);
    float x2 = exp(-bf1*ll2);

    BlurQuadEntry bqe;

    bqe.zdist=0.25*(p0[2]+p2[2]+q0[2]+q2[2]);

    // first half
    bqe.p1 = p0;
    bqe.p2 = p1;
    bqe.p3 = q0;
    bqe.p4 = q1;
    bqe.c1 = it->atom1->color;
    bqe.c2 = it->atom1->color;
    bqe.c3 = it->atom1->color;
    bqe.c4 = it->atom1->color;
    bqe.c1[3] = x0;
    bqe.c2[3] = x1;
    bqe.c3[3]=x0*bf2;
    bqe.c4[3]=x1*bf2;

    bql.push_back(bqe);

    // second half
    bqe.p1 = p1;
    bqe.p2 = p2;
    bqe.p3 = q1;
    bqe.p4 = q2;
    bqe.c1 = it->atom2->color;
    bqe.c2 = it->atom2->color;
    bqe.c3 = it->atom2->color;
    bqe.c4 = it->atom2->color;
    bqe.c1[3] = x1;
    bqe.c2[3] = x2;
    bqe.c3[3]=x1*bf2;
    bqe.c4[3]=x2*bf2;

    bql.push_back(bqe);
  }

  std::sort(bql.begin(),bql.end(),BlurQuadEntryLess());

  glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_LIGHTING_BIT);
  glDisable(GL_LIGHTING);
  glDisable(GL_CULL_FACE);
  glDepthFunc(GL_LESS);
  glDepthMask(GL_FALSE);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glBegin(GL_QUADS);
  glNormal3f(0.0,0.0,0.0);
  for (std::vector<BlurQuadEntry>::const_iterator it=bql.begin();
       it!=bql.end();++it) {
    glColor4fv(it->c1);
    glVertex3v(it->p1.Data());
    glColor4fv(it->c2);
    glVertex3v(it->p2.Data());
    glColor4fv(it->c4);
    glVertex3v(it->p4.Data());
    glColor4fv(it->c3);
    glVertex3v(it->p3.Data());
  }

  glEnd();
  glPopMatrix();
  glPopAttrib();
}

void DoBlurSnapshot(BondEntryList& bl)
{
  for (BondEntryList::iterator it=bl.begin();
       it!=bl.end();++it) {
    if(it->atom1 && it->atom2) {
      it->pp1=it->atom1->atom.GetPos();
      it->pp2=it->atom2->atom.GetPos();
    }
  }
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


////////////////////////////


static int bsplineGen(float *x,float *y, int n, float yp1, float ypn, float *y2);
static int bsplineGet(float *xa, float *ya, float *y2a, int n, float x, float *y);

#define SPLINE_ENTRY_INTERPOLATE(COMPONENT)             \
    for(int c=0;c<size;++c) {                           \
      yc[c]=entry_list.at(c). COMPONENT ;		\
    }                                                   \
    bsplineGen(xp,yp,size,1.0e30,1.0e30,y2p);           \
    for(int c=0;c<size-1;++c) {                         \
      for(int d=0;d<nsub;++d) {                         \
        float u=static_cast<float>(c*nsub+d)*i_nsub;    \
        float v=0.0;                                    \
        bsplineGet(xp,yp,y2p,size,u,&v);                \
        sublist.at(c*nsub+d). COMPONENT = v;		\
      }                                                 \
      float u=static_cast<float>((size-1)*nsub)*i_nsub;	\
      float v=0.0;                                      \
      bsplineGet(xp,yp,y2p,size,u,&v);                  \
      sublist.at((size-1)*nsub). COMPONENT = v;		\
    }                                                   

SplineEntryList Spline::Generate(SplineEntryList& entry_list, int nsub, uint color_blend_mode)
{
  if(nsub<=0) {
    return entry_list;
  }
  int size=entry_list.size();
  if (size<2) {
    return entry_list;
  }
  // we want to go to the last point, but not beyond
  int ipsize=(size-1)*nsub+1;
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

  LOG_DEBUG("SplineGenerate: interpolating spline entry components");
  
  // create sublist with enough entries
  SplineEntryList sublist(ipsize);

  // interpolate color
  if(color_blend_mode==0) {
    for(int k=0;k<4;++k) {
      SPLINE_ENTRY_INTERPOLATE(color1[k]);
      SPLINE_ENTRY_INTERPOLATE(color2[k]);
    }
  } else {
    for(int c=0;c<size-1;++c) {
      for(int k=0;k<4;++k) {
        int d=0;
        for(;d<nsub/2;++d) {
          sublist.at(c*nsub+d).color1[k]=entry_list[c].color1[k];
          sublist.at(c*nsub+d).color2[k]=entry_list[c].color2[k];
        }
        for(;d<nsub;++d) {
          sublist.at(c*nsub+d).color1[k]=entry_list[c+1].color1[k];
          sublist.at(c*nsub+d).color2[k]=entry_list[c+1].color2[k];
        }
      }
    }
  }

  // interpolate position and helper vectors
  for(int k=0;k<3;++k) {
    SPLINE_ENTRY_INTERPOLATE(position[k]);
    //SPLINE_ENTRY_INTERPOLATE(v0[k]);
    SPLINE_ENTRY_INTERPOLATE(v1[k]);
    //SPLINE_ENTRY_INTERPOLATE(v2[k]);
  }

  SPLINE_ENTRY_INTERPOLATE(rad);

  LOG_DEBUG("SplineGenerate: assigning direction and normal components");

#if 1
  std::vector<Quat> quats(size+3);

  Vec3 prevn;
  for(int c=0;c<size;++c) {
    Vec3 d1,d2;
    if(c==0) {
      d1=Normalize(entry_list.at(c).position-entry_list.at(c+1).position);
      d2=Normalize(entry_list.at(c+1).position-entry_list.at(c+2).position);
    } else if(c==size-1) {
      d1=Normalize(entry_list.at(c-2).position-entry_list.at(c-1).position);
      d2=Normalize(entry_list.at(c-1).position-entry_list.at(c).position);
    } else {
      d1=Normalize(entry_list.at(c-1).position-entry_list.at(c).position);
      d2=Normalize(entry_list.at(c).position-entry_list.at(c+1).position);
    }
    Vec3 d=d2;
    Vec3 n=Normalize(Cross(d1,d2));
    //Vec3 n=entry_list.at(c).v1;
    Vec3 o = Normalize(Cross(d,n));

    if(c>0) {
      if(Dot(Normalize(Cross(d,prevn)),Normalize(Cross(d,n)))<0.0) {
        n=-n;
        o=-o;
      }
    }
    prevn=n;

    //n = Normalize(Cross(o,d));
    Quat q(Mat3(d[0],n[0],o[0],
                d[1],n[1],o[1],
                d[2],n[2],o[2]));
    quats[c+1]=q;
  }
  quats.at(0)=quats.at(1);
  quats.at(size)=quats.at(size-1);
  quats.at(size+1)=quats.at(size-1);
  
  std::vector<Quat> squats(size*nsub);
  // now for the quaternion interpolation, using squad
  for(int c=0;c<size;++c) {
    Quat q0=quats.at(c+0);
    Quat q1=quats.at(c+1);
    Quat q2=quats.at(c+2);
    Quat q3=quats.at(c+3);

    Quat s1=q1*Exp(-0.25*Log(Inv(q1)*q2)+Log(Inv(q1)*q0));
    Quat s2=q2*Exp(-0.25*Log(Inv(q2)*q3)+Log(Inv(q2)*q1));

    for(int d=0;d<nsub;++d) {
      float u=static_cast<float>(d)*i_nsub;
      squats.at(c*nsub+d) = Slerp(Slerp(q1,q2,u),Slerp(s1,s2,u),2.0*u*(1.0-u));
    }
  }

  assert(sublist.size()<=squats.size());

  for(unsigned int i=0;i<sublist.size();++i) {
    Mat3 m=squats[i].ToRotationMatrix();
    sublist.at(i).direction=Normalize(Vec3(m(0,0),m(1,0),m(2,0)));
    sublist.at(i).normal=Normalize(Vec3(m(0,1),m(1,1),m(2,1)));
    sublist.at(i).v2=Normalize(Vec3(m(0,2),m(1,2),m(2,2)));
  }

  std::vector<Vec3> nlist(sublist.size());
  for(uint i=0;i<sublist.size();++i) {
    Vec3 p0,p1;
    if(i<sublist.size()-1) {
      p0 = sublist.at(i).position;
      p1 = sublist.at(i+1).position;
    } else {
      p0 = sublist.at(i-1).position;
      p1 = sublist.at(i).position;
    }
    sublist.at(i).direction=Normalize(p1-p0);
    Vec3 orth = Normalize(Cross(sublist.at(i).direction,sublist.at(i).normal));
    nlist[i] = Normalize(Cross(orth,sublist.at(i).direction));
    sublist.at(i).v2=orth;
    if(i>0) {
      if(Dot(Normalize(Cross(sublist.at(i).direction,nlist[i-1])),orth)<0.0) {
        nlist[i]=-nlist[i];
        sublist.at(i).v2=-sublist.at(i).v2;
      }
    }
  }

  sublist.at(0).normal=nlist[0];
  sublist.back().normal=nlist.back();
  for(uint i=1;i<sublist.size()-1;++i) {
    sublist.at(i).normal=(nlist[i-1]+nlist[i]+nlist[i+1])/3.0;   
  }
#else

  // assign direction and normal
  // entity trace has the same algorithm

  Vec3 p0 = sublist.at(0).position;
  Vec3 p1 = sublist.at(1).position;
  Vec3 p2 = ipsize>2 ? sublist.at(2).position : p1+(p1-p0);
  // normal of 0 is set at the end
  sublist.at(0).direction=Normalize(p1-p0);
  sublist.at(0).v1=Normalize(sublist.at(0).v1);
  Vec3 orth = Cross(sublist.at(0).direction,sublist.at(0).v1);
  sublist.at(0).v0 = Normalize(Cross(orth,sublist.at(0).direction));

  // reference normal to avoid twisting
  //Vec3 nref=Normalize(Cross(p0-p1,p2-p1));
  unsigned int i=1;
  for(;i<sublist.size()-1;++i) {
    Vec3 p10 = p0-p1;
    Vec3 p12 = p2-p1;
    // correction for perfectly aligned consecutive directions
    if(p10==-p12 || p10==p12) p12+=Vec3(0.001,0.001,0.001);
    sublist.at(i).normal=Normalize(Cross(p10,p12));
    // paranoid error checking due to occasional roundoff troubles
    float cosw = Dot(Normalize(p10),Normalize(p12));
    cosw = std::min(float(1.0),std::max(float(-1.0),cosw));
    float omega=0.5*acos(cosw);
    orth=AxisRotation(sublist.at(i).normal, -omega)*p12;
    sublist.at(i).direction=Normalize(Cross(sublist.at(i).normal,orth));
    // twist avoidance
    sublist.at(i).v1=Normalize(sublist.at(i).v1);
    orth = Cross(sublist.at(i).direction,sublist.at(i).v1);
    sublist.at(i).v0 = Normalize(Cross(orth,sublist.at(i).direction));
    if(Dot(sublist.at(i-1).v0,sublist.at(i).v0)<0.0) {
      sublist.at(i).v0=-sublist.at(i).v0;
      //sublist.at(i).nflip = !sublist.at(i).nflip;
    }

    // avoid twisting
    //if(Dot(sublist.at(i).normal,nref)<0.0) sublist.at(i).normal=-sublist.at(i).normal;
    //nref=sublist.at(i).normal;
    // skip over shift for the last iteration
    if(i==sublist.size()-2) break;
    // shift to i+1 for next iteration
    p0 = sublist.at(i).position;
    p1 = sublist.at(i+1).position;
    p2 = sublist.at(i+2).position;
  }
  // assign remaining ones
  sublist.at(0).normal=sublist.at(1).normal;
  sublist.at(i+1).direction=Normalize(p2-p1);
  sublist.at(i+1).normal=sublist.at(i).normal;
  sublist.at(i+1).v1=Normalize(sublist.at(i+1).v1);
  orth = Cross(sublist.at(i+1).direction,sublist.at(i+1).v1);
  sublist.at(i+1).v0 = Normalize(Cross(orth,sublist.at(i+1).direction));
  if(Dot(sublist.at(i).v0,sublist.at(i+1).v0)<0.0) {
    sublist.at(i+1).v0=-sublist.at(i+1).v0;
  }

  // hack
  // TODO: merge this with above routine
  for(unsigned int i=0;i<sublist.size();++i) {
    sublist.at(i).normal = sublist.at(i).v0;
  }

#endif

  LOG_DEBUG("SplineGenerate: assigning non-interpolated entry components");
  // finally the non-interpolated type
  // with some tweaks for proper strand rendering
  // part of this probably belongs into cartoon renderer
  for(int c=0;c<size-1;++c) {
    int type1=entry_list[c].type;
    int type2=entry_list[std::min(c+1,size-1)].type;
    for(int d=0;d<nsub;++d) {
      sublist.at(c*nsub+d).type=entry_list[c].type;
      sublist.at(c*nsub+d).type1=type1;
      sublist.at(c*nsub+d).type2=type2;
      float frac=float(d)/float(nsub);
      sublist.at(c*nsub+d).frac=frac;
    }
  }
  int type1=entry_list.back().type;
  int type2=type1;
  sublist.back().type=entry_list.back().type;
  sublist.back().type1=type1;
  sublist.back().type2=type2;
  sublist.back().frac=0.0;

  float insub=1.0/static_cast<float>(nsub);
  for(size_t c=0;c<sublist.size();++c) {
    sublist[c].running_length=static_cast<float>(c)*insub;
  }

  // the id for selections, shifted by one half
  for(int c=0;c<size-1;++c) {
    int d=0;
    for(;d<nsub/2;++d) {
      sublist.at(c*nsub+d).id=entry_list[c].id;
    }
    for(;d<nsub;++d) {
      sublist.at(c*nsub+d).id=entry_list[c+1].id;
    }
  }
  sublist.back().id=entry_list.back().id;

  // the nflip flags for helices for correct inside/outside assignment
  // this probably belongs into cartoon renderer
  LOG_DEBUG("SplineGenerate: setting nflip flags for helices");
  unsigned int c=0;
  bool nflip=false;
  while(c<sublist.size()-1) {
    int cstart=c;
    if(sublist.at(c).type==1 && sublist.at(c+1).type==1) {
      Vec3 n = Normalize(Cross(sublist.at(c).normal,
                                                 sublist.at(c).direction));
      Vec3 p0 = sublist.at(c).position+n;
      Vec3 q0 = sublist.at(c).position-n;
      float psum=0.0;
      float qsum=0.0;
      ++c;
      while(c<sublist.size() && sublist.at(c).type==1) {
        n = Normalize(Cross(sublist.at(c).normal,
                                        sublist.at(c).direction));
        Vec3 p1 = sublist.at(c).position+n;
        Vec3 q1 = sublist.at(c).position-n;
        psum+=Length(p1-p0);
        qsum+=Length(q1-q0);
        p0=p1;
        q0=q1;
        ++c;
      }
      
      nflip = qsum>psum;
      for(unsigned int cc=cstart;cc<c;++cc) {
        sublist.at(cc).nflip=nflip;
      }
    } else {
      ++c;
    }
  }
  sublist.back().nflip=nflip;

  LOG_DEBUG("SplineGenerate: done");

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
