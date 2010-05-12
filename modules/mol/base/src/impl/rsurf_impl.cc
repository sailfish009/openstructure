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

/*
  parallelization notes:

  during the build round, ie the recursive travel along
  the surface to identify the reduced surface points, the
  majority of calls depends on the previous one, and the
  core algorithm is a recursive traversal; it is unclear
  how to add parallelization to this type of logic.

  during the triangulization, the creation of the patches,
  the saddles and the sphere caps are in their internal
  loop independent from each other; the only restriction
  in terms of sequence is that the patches must be complete
  before the saddles are build, and the patches must be
  complete prior to the sphere caps. 

*/

#include <cmath>

#include <ost/mol/entity_view.hh>
#include <ost/mol/impl/rsurf_impl.hh>
#include <ost/mol/impl/surface_impl.hh>

#include <ost/geom/geom.hh>
#include <ost/log.hh>

namespace ost { namespace mol { namespace rsurf {

using namespace geom;

// almost copy from gfx... bad
// TODO: refactor to geom
void build_sphere(int level,
                  std::vector<Vec3>& vlist,
                  std::vector<uint>& ilist)
{
  vlist.clear();
  ilist.clear();
  unsigned int step1 = level*4;
  unsigned int step2 = level*2;
  Real fact1 = M_PI*2.0/static_cast<Real>(step1);
  Real fact2 = M_PI/static_cast<Real>(step2);
  
  vlist.push_back(geom::Vec3(0.0,1.0,0.0));
  for(unsigned int i=0;i<step1;++i) {
    for(unsigned int k=1;k<step2;++k) {
      Real ii = M_PI+fact1*static_cast<Real>(i);
      Real kk = fact2*static_cast<Real>(k);
      vlist.push_back(geom::Vec3(sin(kk)*cos(ii), cos(kk), sin(kk)*sin(ii)));
    }
  }
  vlist.push_back(geom::Vec3(0.0,-1.0,0.0));
  
  unsigned int point_count=vlist.size();
  
  // top
  unsigned int j=1;
  for(unsigned int i=0;i<step1;++i) {
    unsigned int k=i+1;
    k%=step1;
    ilist.push_back(0);
    ilist.push_back(k*(step2-1)+j);
    ilist.push_back(i*(step2-1)+j);
  }
  
  // bottom
  j=step2-1;
  for(unsigned int i=0;i<step1;++i) {
    unsigned int k=i+1;
    k%=step1;
    ilist.push_back(point_count-1);
    ilist.push_back(i*(step2-1)+j);
    ilist.push_back(k*(step2-1)+j);
  }  
  
  // middle
  for(unsigned int i=0;i<step1;++i) {
    unsigned int k=i+1;
    k%=step1;
    for(unsigned j=1;j<step2-1;++j) {
      unsigned l=j+1;
      l%=(step2);
      // add 2 triangles, counterclockwise
      unsigned int p1=i*(step2-1)+j;
      unsigned int p2=i*(step2-1)+l;
      unsigned int p3=k*(step2-1)+j;
      unsigned int p4=k*(step2-1)+l;
      ilist.push_back(p1);
      ilist.push_back(p3);
      ilist.push_back(p2);
      ilist.push_back(p2);
      ilist.push_back(p3);
      ilist.push_back(p4);
    }
  }
}

    
Arc::Arc()
{}

Arc::Arc(const SphereP& h, const SphereP& k, const SphereP& s):
  H(h), K(k), S(s), T()
{
  Vec3 o=h->pos;
  Vec3 d=o-k->pos;
  Vec3 p=s->pos;

  Real r = (Dot(d,p)-Dot(d,o))/Dot(d,d);
  axis = Normalize(d);
  fixpoint = o+r*d;
  sdir = Normalize(p-fixpoint);
  tdir=Vec3(0,0,0);

  // transformation matrix into normalized system, for calc_phi
  Vec3 orth = Normalize(Cross(axis,sdir));
  xmat = Mat3(sdir[0],sdir[1],sdir[2],
              orth[0],orth[1],orth[2],
              axis[0],axis[1],axis[2]);

  LOGN_DUMP("new arc");
  LOGN_DUMP(" " << H->name << " " << K->name);
  LOGN_DUMP("  ax=" << axis << " fp=" << fixpoint << " sd=" << sdir << "  ax.sd=" << Dot(axis,sdir));
}

void Arc::SetT(const SphereP& t)
{
  T=t;
  tdir=Normalize(t->pos-fixpoint);
}

Tet::Tet(const SphereP& a, const SphereP& b, const SphereP& c, const Sphere& s):
  A(a), B(b), C(c), S(new Sphere(s))
{
  init();
}

Tet::Tet(const SphereP& a, const SphereP& b, const SphereP& c, const SphereP& s):
  A(a), B(b), C(c), S(s)
{
  init();
}

void Tet::init()
{
  LOGN_DUMP("new tet");
  LOGN_DUMP(" " << A->name << " " << B->name << " " << C->name);
  LOGN_DUMP(" " << S->pos);
  LOGN_DUMP(" a-s: " << Length(A->pos-S->pos) << " " << A->rad+S->rad);
  LOGN_DUMP(" b-s: " << Length(B->pos-S->pos) << " " << B->rad+S->rad);
  LOGN_DUMP(" c-s: " << Length(C->pos-S->pos) << " " << C->rad+S->rad);
  Real ps = Dot(Cross(A->pos-B->pos,C->pos-B->pos),S->pos-(A->pos+B->pos+C->pos)/3.0);
  LOGN_DUMP(" p.s: " << ps );
  if(ps<0.0) {
    LOGN_DUMP(" swapping B/C for correct parity");
    std::swap(B,C);
    parity_swap=true;
  } else {
    parity_swap=false;
  }
  cA=S->pos+(S->rad/(S->rad+A->rad))*(A->pos-S->pos);
  cB=S->pos+(S->rad/(S->rad+B->rad))*(B->pos-S->pos);
  cC=S->pos+(S->rad/(S->rad+C->rad))*(C->pos-S->pos);
}

ArcP Tet::GetArc(const SphereP& h, const SphereP& k)
{
  if(U->H==h && U->K==k) return U;
  else if(U->H==k && U->K==h) return U;
  else if(V->H==h && V->K==k) return V;
  else if(V->H==k && V->K==h) return V;
  else if(W->H==h && W->K==k) return W;
  else if(W->H==k && W->K==h) return W;
  return ArcP();
}


RSurf::RSurf(Real probe_radius):
  sphere_list_(),
  arc_list_(),
  tet_list_(),
  probe_radius_(probe_radius),
  max_rad_(0.0),
  spat_org_(probe_radius_*2.0)
{}

void RSurf::AddSphere(const Vec3& pos, Real rad, const String& name)
{
  sphere_list_.push_back(SphereP(new Sphere(pos,rad)));
  sphere_list_.back()->name=name;
  spat_org_.Add(sphere_list_.back(),pos);
  max_rad_=std::max(max_rad_,rad);
}

namespace {
bool comp(const Vec3& p1, const Vec3& p2)
{
  return p1[2]!=p2[2] ? p1[2]<p2[2] : (p1[1]!=p2[1] ? p1[1]<p2[1] : p1[0]<p2[0]);
}
}

void RSurf::Build()
{
  LOGN_DUMP("entering RSurf::Build");
  for(SphereList::iterator it=sphere_list_.begin();it!=sphere_list_.end();++it) {
    LOGN_DUMP("sphere " << it->get() << " @ " << (*it)->pos);
  }

  // find sphere with lowest extension in order z,y,x
  SphereP best_a=*(sphere_list_.begin());
  Vec3 best_pos=best_a->pos-Vec3(best_a->rad,best_a->rad,best_a->rad);
  for(SphereList::iterator it=sphere_list_.begin();it!=sphere_list_.end();++it) {
    // radius corrected
    Vec3 pp=(*it)->pos-Vec3((*it)->rad,(*it)->rad,(*it)->rad);
    if(comp(pp,best_pos)) {
      best_a=*it;
      best_pos=pp;
    }
  }

  // a temporary solvent sphere, only touching best_a
  SphereP temp_s(new Sphere(best_a->pos-Vec3(0,0,best_a->rad+probe_radius_),probe_radius_));
  // a temporary arc to find the second starting sphere
  ArcP temp_arc(new Arc());
  temp_arc->S=temp_s;
  temp_arc->axis = Vec3(-1,0,0);
  temp_arc->fixpoint = best_a->pos;
  temp_arc->sdir = Vec3(0,0,-1);
  Vec3 orth = Normalize(Cross(temp_arc->axis,temp_arc->sdir));
  temp_arc->xmat = Mat3(temp_arc->sdir[0],temp_arc->sdir[1],temp_arc->sdir[2],
                        orth[0],orth[1],orth[2],
                        temp_arc->axis[0],temp_arc->axis[1],temp_arc->axis[2]);

  // find best second sphere - almost identical to find_best_tet()
  SphereList clist = get_nearest_spheres(best_a->pos, best_a,best_a);
  Real best_phi = 7.0;
  SphereP best_b;

  for(SphereList::const_iterator it=clist.begin();it!=clist.end();++it) {
    Real phi = calc_phi(temp_arc,*it,false);
    if(phi>=0.0 && phi<best_phi) {
      best_phi = phi;
      best_b = *it;
    }
  }

  if(!best_b) {
    LOGN_MESSAGE("singular sphere detected, no reduced surface build");
    return;
  }

  // rotate solvent around axis to get updated pos
  temp_s->pos = AxisRotation(temp_arc->axis,best_phi)*
    (temp_s->pos-temp_arc->fixpoint)+
    temp_arc->fixpoint;

  // now find third sphere with updated temporary arc - again similar to find_best_tet()
  temp_arc = ArcP(new Arc(best_a,best_b,temp_s));

  clist = get_nearest_spheres(temp_arc->fixpoint,best_a,best_b);
  best_phi = 7.0;
  SphereP best_c;

  for(SphereList::const_iterator it=clist.begin();it!=clist.end();++it) {
    Real phi = calc_phi(temp_arc,*it,false);
    if(phi>=0.0 && phi<best_phi) {
      best_phi = phi;
      best_c = *it;
    }
  }

  if(!best_c) {
    LOGN_MESSAGE("singular arc detected, no reduced surface build");
    return;
  }

  temp_s->pos = AxisRotation(temp_arc->axis,best_phi)*
    (temp_s->pos-temp_arc->fixpoint)+
    temp_arc->fixpoint;

  // finally assemble first tet
  TetP ntet(new Tet(best_a, best_b, best_c,temp_s));
  tet_list_.push_back(ntet);
  // initiate the recursive traversal along the three first arcs
  LOGN_DUMP("starting recursive arc traversal");
  ntet->U = ArcP(new Arc(ntet->A, ntet->B, ntet->S));
  ntet->U->A = ntet;
  ntet->V = ArcP(new Arc(ntet->B, ntet->C, ntet->S));
  ntet->V->A = ntet;
  ntet->W = ArcP(new Arc(ntet->C, ntet->A, ntet->S));
  ntet->W->A = ntet;
  // note: creation of all three prior to traversal is necessary!
  
  ntet->depth=0;
  traverse_arc(ntet,ntet->U,ntet->C,1);
  //traverse_arc(ntet,ntet->V,ntet->A,1);
  //traverse_arc(ntet,ntet->W,ntet->B,1);

  LOGN_DUMP("build " << tet_list_.size() << " tets and " << arc_list_.size() << " arcs");

  for(ArcList::const_iterator it=arc_list_.begin();it!=arc_list_.end();++it) {
    if(!(*it)->S) LOGN_VERBOSE("invalid arc detected, null S: " << (*it)->H->name << " " << (*it)->K->name);
    if(!(*it)->T) LOGN_VERBOSE("invalid arc detected, null T: " << (*it)->H->name << " " << (*it)->K->name);
  }
}

void RSurf::traverse_arc(TetP tet, ArcP& arc, SphereP c, uint depth)
{
  if(depth>4) return;

  if(tet->S!=arc->S) {
    LOGN_VERBOSE("unexpected error during arc traversal: tet->S != arc->S; aborting arc traversal (" <<arc->H->name << "," << arc->K->name << ")");
    return;
  }
  // find the closest contact point when traversing the arc around [ab],
  // away from c, and return as a tet
  std::pair<TetP,bool> ret = find_best_tet(arc,c);
  TetP ntet=ret.first;

 if(!ntet) {
    LOGN_VERBOSE("unexpected failure during find_best_tet(); aborting arc traversal (" <<arc->H->name << "," << arc->K->name << ")");
    return;
  }

  // dest tet already exists
  if(ret.second) {
    // grab already allocated arc from tet
    ArcP narc = ntet->GetArc(arc->H,arc->K);
    if(narc) {
      // replace source arc with existing one
      arc=narc;
      // close this arc if necessary
      if(!narc->T) {
        narc->SetT(tet->S);
        narc->B=tet;
        // and add to overall list
        arc_list_.push_back(narc);
      }
    } else {
      LOGN_VERBOSE("unexpected missing arc in existing tet");
    }

    return; // existing tet: no reason to continue traversal
  }


  // given the new tet, handle current arc
  ntet->depth=depth;
  ntet->U = arc;
  arc->SetT(ntet->S);
  arc->B=ntet;
  arc_list_.push_back(arc);

  if(ntet->parity_swap) {
    // B and C were swapped
    ntet->V = ArcP(new Arc(ntet->C, ntet->B, ntet->S));
    ntet->V->A = ntet;
    ntet->W = ArcP(new Arc(ntet->B, ntet->A, ntet->S));
    ntet->W->A = ntet;
    traverse_arc(ntet,ntet->V,ntet->A,depth+1);
    traverse_arc(ntet,ntet->W,ntet->C,depth+1);
  } else {
    ntet->V = ArcP(new Arc(ntet->B, ntet->C, ntet->S));
    ntet->V->A = ntet;
    ntet->W = ArcP(new Arc(ntet->C, ntet->A, ntet->S));
    ntet->W->A = ntet;
    traverse_arc(ntet,ntet->V,ntet->A,depth+1);
    traverse_arc(ntet,ntet->W,ntet->B,depth+1);
  }

}

std::pair<TetP,bool> RSurf::get_tet(SphereP a, SphereP b, SphereP c, const Sphere& s, bool create)
{
  // TODO: this could use a hash table instead of a list for faster lookup
  bool parity_swap=false;
  TetList::const_iterator it=tet_list_.begin();
  for(;it!=tet_list_.end();++it) {
    Tet& tet=*(*it);

    // first find matching triplet
    if((tet.A==a && tet.B==b && tet.C==c) ||
       (tet.A==c && tet.B==a && tet.C==b) ||
       (tet.A==b && tet.B==c && tet.C==a)) {
      parity_swap=false;
      LOGN_DUMP("found tet with order 0");
      break;
    } else if((tet.A==a && tet.C==b && tet.B==c) ||
	      (tet.A==c && tet.C==a && tet.B==b) ||
	      (tet.A==b && tet.C==c && tet.B==a)) {
      parity_swap=true;
      LOGN_DUMP("found tet with order 1");
      break;
    }
  }

  if(it==tet_list_.end()) {
    if(create) {
      LOGN_DUMP("no tet found, making new one");
      TetP tet(new Tet(a,b,c,s));
      tet_list_.push_back(tet);
      return std::make_pair(tet,false);
    }

  } else {
    if(parity_swap==(*it)->parity_swap) {
      LOGN_DUMP("parities match, returning old one");
      return std::make_pair(*it,true);
    } else {
      // re-use ABC assignment from tet
      if(create) {
	LOGN_DUMP("parities don't match, creating new one");
	TetP tet(new Tet((*it)->A,(*it)->B,(*it)->C,s));
	tet_list_.push_back(tet);
	return std::make_pair(tet,false);
      }
    }
  }
  return std::make_pair(TetP(),false);
}

std::pair<TetP,bool> RSurf::find_best_tet(ArcP arc, SphereP c)
{
  SphereList clist = get_nearest_spheres(arc->fixpoint,arc->H,arc->K);
  Real best_phi = 7.0;
  SphereP best_c;

  if(clist.empty()) {
    LOGN_VERBOSE("unexpected zero length sphere list in rsurf::find_best_tet()");
    return std::make_pair(TetP(),true);
  }
  for(SphereList::const_iterator it=clist.begin();it!=clist.end();++it) {
    Real phi = calc_phi(arc,*it,true);
    if(phi<best_phi) {
      if(phi<1e-8 && (*it)==c) continue;
      best_phi = phi;
      best_c = *it;
    }
  }

  if(!best_c) {
    LOGN_VERBOSE("unexpected missing best_c in rsurf::find_best_tet()");
    return std::make_pair(TetP(),true);
  }

  // rotate solvent around axis to get new pos
  arc->phi=best_phi;
  Vec3 new_pos = AxisRotation(arc->axis,best_phi)*(arc->S->pos-arc->fixpoint)+arc->fixpoint;
  arc->mid = AxisRotation(arc->axis,0.5*best_phi)*(arc->S->pos-arc->fixpoint)+arc->fixpoint;

  // switch HK order for correct parity
  // (actually, this is questionable and in fact verified in tet ctor)
  return get_tet(arc->K,arc->H,best_c,Sphere(new_pos, probe_radius_),true);
}

SphereList RSurf::get_nearest_spheres(const Vec3& pos, SphereP a, SphereP b)
{
  SphereList nrvo;
  SpatOrg::ItemList slist = spat_org_.FindWithin(pos,2.0*(probe_radius_+max_rad_));
  for(SpatOrg::ItemList::const_iterator it=slist.begin();it!=slist.end();++it) {
    if(*it!=a && *it!=b) nrvo.push_back(*it);
  } 
  return nrvo;
}

Real RSurf::calc_phi(ArcP arc, SphereP x, bool prune_zero)
{
  // calculate angle phi around axis given by ab, when 
  // starting from s at zero, and touching x upon rotating
  
  Vec3 p = arc->xmat*(x->pos-arc->fixpoint);

  Real r = Length(arc->S->pos-arc->fixpoint);
  Real r2 = r*r;
  Real x2 = p[0]*p[0];
  Real y2 = p[1]*p[1];
  Real z2 = p[2]*p[2];
  Real d = probe_radius_+x->rad;
  Real d2 = d*d;

  Real phi1 = 7.0;
  Real phi2 = 7.0;

  if(std::abs(p[0])<1e-8) {

    Real c4 = 1.0/(2.0*y2);
    Real c5 = p[1]*(r2-d2+y2+z2);
    Real ys1 = c4*c5;

    phi1 = atan2(ys1,Real(0.0));
    phi2 = phi1;

  } else {

    Real c0 = -d2+x2+y2+z2;
    Real c1 = -x2*(r2*r2-2.0*r2*(d2+x2+y2-z2)+c0*c0);
    if(c1<0.0) {
      if(std::abs(c1)<1e-5) {
        c1=0.0;
      } else {
        return 7.0;
      }
    }
    c1 = sqrt(c1);
    Real c2 = 1.0/(2.0*p[0]*(x2+y2));
    Real c3 = x2*(r2-d2+x2+y2+z2);
    Real xs1 = c2*(c3-p[1]*c1);
    Real xs2 = c2*(c3+p[1]*c1);
    Real c4 = 1.0/(2.0*(x2+y2));
    Real c5 = p[1]*(r2-d2+x2+y2+z2);
    Real ys1 = c4*(c5+c1);
    Real ys2 = c4*(c5-c1);

    phi1 = atan2(ys1,xs1);
    phi2 = atan2(ys2,xs2);
  }

  if(phi1<0.0) phi1+=2.0*M_PI;
  if(phi2<0.0) phi2+=2.0*M_PI;
  if(prune_zero) {
    if(phi1<1e-8) phi1=7.0;
    if(phi2<1e-8) phi2=7.0;
  }

  return std::min(phi1,phi2);
}


////////////////////////////////
// Triangulation stuff
////////////////////////////////

namespace { 

// all helper functions are in namespace anon

// split given list into two at position p (which is excluded)
void split_edge(const EdgePointList& in, unsigned int p, EdgePointList& o1, EdgePointList& o2)
{
  for(unsigned int i=0;i<p;++i) {
    o1.push_back(in[i]);
  }
  for(unsigned int i=p+1;i<in.size();++i) {
    o2.push_back(in[i]);
  }
}

// build edges on great arc between e1 and e2, _not_ including these two points
EdgePointList build_edge(const EdgePoint& e1, const EdgePoint& e2, 
                         const Vec3& ori, Real patch, Real mult, 
                         impl::SurfaceImplP& surf,int type)
{
  Vec3 d1 = e1.pos-ori;
  Real rad = Length(d1);
  Vec3 n1=Normalize(d1);
  Vec3 n2=Normalize(e2.pos-ori);

  Vec3 ax=Normalize(Cross(n1,n2));
  Real phi=acos(Dot(n1,n2));
  int step=static_cast<int>(floor(rad*phi/patch));
  Real delta_phi=phi/static_cast<Real>(step);

  EdgePointList e12;

  for(int k=1;k<step;++k) {
    Vec3 dd = AxisRotation(ax,static_cast<Real>(k)*delta_phi)*d1;
    Vec3 nn = Normalize(mult*dd);
    Vec3 vv = dd+ori;
    e12.push_back(EdgePoint(surf->AddVertex(SurfaceVertex(vv,nn,type)),vv,nn));
  }

  return e12;
}

// variant to above, with explicit step size
EdgePointList build_edge(const EdgePoint& e1, const EdgePoint& e2, 
                         const Vec3& ori, int step, Real mult,
                         impl::SurfaceImplP& surf, int type)
{
  EdgePointList e12;

  if(step==0) return e12;

  Vec3 d1 = e1.pos-ori;
  Vec3 n1=Normalize(d1);
  Vec3 n2=Normalize(e2.pos-ori);

  Vec3 ax=Normalize(Cross(n1,n2));
  Real phi=acos(Dot(n1,n2));
  Real delta_phi=phi/static_cast<Real>(step);

  for(int k=1;k<step;++k) {
    Vec3 dd = AxisRotation(ax,static_cast<Real>(k)*delta_phi)*d1;
    Vec3 nn = Normalize(mult*dd);
    Vec3 vv = dd+ori;
    e12.push_back(EdgePoint(surf->AddVertex(SurfaceVertex(vv,nn,type)),vv,nn));
  }

  return e12;
}

EdgePointList revert_epl(const EdgePointList& epl)
{
  EdgePointList nrvo(epl.size());
  std::reverse_copy(epl.begin(),epl.end(),nrvo.begin());
  return nrvo;
}
    
/*
  general scheme for subdivisioning
  (e=corner, l=edge, p=new point, x=new edge)

              e2
       ^      /\                  /\
      /      /b \             x2 /  \ x3
    l12  p3 /____\ p1 l23       /_x7_\
           /\ d  /\    \       /\    /\
          /a \  /c \   `'   x1/  x9 x8 \ x4
         /    \/    \        /    \/    \
      e1 ------------ e3     ------------
              p2               x6     x5 
          <-  l31

  special cases are implemented when one or two
  edges contain no more points
*/

#define SUBDIVIDE_ONEZERO(E1,E2,E3,L1,L2,L3)      \
    EdgePointList x2(L2.size()-1);                \
    std::copy(&L2[1],&L2[L2.size()],&x2[0]);      \
    EdgePointList x3(L3.size()-1);                \
    std::copy(&L3[0],&L3[L3.size()-1],&x3[0]);    \
    surf->AddTri(E2.id, E1.id, L2[0].id);         \
    surf->AddTri(L2[0].id, E1.id, L3.back().id);  \
    subdivide(L3.back(),L2[0],E3,L1,x2,x3,ori,patch,mult,surf,type);


#define SUBDIVIDE_TWOZERO(E1,E2,E3,L1)            \
    surf->AddTri(L1[0].id,E1.id,E3.id);           \
    for(unsigned int ii=0;ii<L1.size()-1;++ii) {  \
      surf->AddTri(L1[ii+1].id,L1[ii].id,E3.id);  \
    }                                             \
    surf->AddTri(E2.id,L1.back().id,E3.id);

void subdivide(const EdgePoint& e1, const EdgePoint& e2, const EdgePoint& e3,
               const EdgePointList& l12, 
               const EdgePointList& l23, 
               const EdgePointList& l31,
               const Vec3& ori, Real patch, Real mult,
               impl::SurfaceImplP& surf, int type)
{
  if(l12.empty() && l23.empty() && l31.empty()) {
    surf->AddTri(e2.id,e1.id,e3.id);
  } else if(l12.empty() && !l23.empty() && !l31.empty()) {
    SUBDIVIDE_ONEZERO(e1,e2,e3,l12,l23,l31);
  } else if(!l12.empty() && l23.empty() && !l31.empty()) {
    SUBDIVIDE_ONEZERO(e2,e3,e1,l23,l31,l12);
  } else if(!l12.empty() && !l23.empty() && l31.empty()) {
    SUBDIVIDE_ONEZERO(e3,e1,e2,l31,l12,l23);
  } else if(l12.empty() && l23.empty() && !l31.empty()) {
    SUBDIVIDE_TWOZERO(e3,e1,e2,l31);
  } else if(l12.empty() && !l23.empty() && l31.empty()) {
    SUBDIVIDE_TWOZERO(e2,e3,e1,l23);
  } else if(!l12.empty() && l23.empty() && l31.empty()) {
    SUBDIVIDE_TWOZERO(e1,e2,e3,l12);
  } else {

    unsigned int cp1=(l23.size()+1)/2-1;
    unsigned int cp2=(l31.size()+1)/2-1;
    unsigned int cp3=(l12.size()+1)/2-1;
    
    EdgePoint p1 = l23[cp1];
    EdgePoint p2 = l31[cp2];
    EdgePoint p3 = l12[cp3];
    
    EdgePointList x1,x2,x3,x4,x5,x6;
    
    // new outer edges
    split_edge(l12,cp3,x1,x2);
    split_edge(l23,cp1,x3,x4);
    split_edge(l31,cp2,x5,x6); 
    
#if 0
    EdgePointList x7 = build_edge(p1,p3,ori,patch,mult,surf,type);
    EdgePointList x8 = build_edge(p2,p1,ori,patch,mult,surf,type);
    EdgePointList x9 = build_edge(p3,p2,ori,patch,mult,surf,type);
#else
    EdgePointList x7 = build_edge(p1,p3,ori,(int)((x2.size()+x3.size())/2),mult,surf,type);
    EdgePointList x8 = build_edge(p2,p1,ori,(int)((x4.size()+x5.size())/2),mult,surf,type);
    EdgePointList x9 = build_edge(p3,p2,ori,(int)((x1.size()+x6.size())/2),mult,surf,type);
#endif
    
    subdivide(e1,p3,p2,x1,x9,x6,ori,patch,mult,surf,type);
    subdivide(p3,e2,p1,x2,x3,x7,ori,patch,mult,surf,type);
    subdivide(p2,p1,e3,x8,x4,x5,ori,patch,mult,surf,type);
    subdivide(p1,p2,p3,revert_epl(x8),revert_epl(x9),revert_epl(x7),ori,patch,mult,surf,type);
  }
}

struct EdgePointList3 {
  EdgePointList l1,l2,l3;
};

EdgePointList3 build_patch(const Tet& tet, Real patch, Real mult,
                           impl::SurfaceImplP& surf)
{
  Vec3 ori=tet.S->pos;
  Vec3 n1=-Normalize(tet.cA-ori);
  EdgePoint e1(surf->AddVertex(SurfaceVertex(tet.cA,n1,1)),tet.cA,n1);
  Vec3 n2=-Normalize(tet.cB-ori);
  EdgePoint e2(surf->AddVertex(SurfaceVertex(tet.cB,n2,1)),tet.cB,n2);
  Vec3 n3=-Normalize(tet.cC-ori);
  EdgePoint e3(surf->AddVertex(SurfaceVertex(tet.cC,n3,1)),tet.cC,n3);

#if 1
  EdgePointList l12 = build_edge(e1,e2,ori,(int)tet.U->step,mult,surf,1);
  EdgePointList l23 = build_edge(e2,e3,ori,(int)tet.V->step,mult,surf,1);
  EdgePointList l31 = build_edge(e3,e1,ori,(int)tet.W->step,mult,surf,1);
#else
  EdgePointList l12 = build_edge(e1,e2,ori,patch,mult,surf,1);
  EdgePointList l23 = build_edge(e2,e3,ori,patch,mult,surf,1);
  EdgePointList l31 = build_edge(e3,e1,ori,patch,mult,surf,1);
#endif

  subdivide(e1,e2,e3,l12,l23,l31,ori,patch,mult,surf,1);

  l12.insert(l12.begin(),e1);
  l12.push_back(e2);
  l23.insert(l23.begin(),e2);
  l23.push_back(e3);
  l31.insert(l31.begin(),e3);
  l31.push_back(e1);

  EdgePointList3 epl3={l12,l23,l31};
  return epl3;
}

void build_saddle(ArcP& arcp, Real patch, impl::SurfaceImplP& surf)
{
  Arc& arc = *arcp;
  if(arc.eplS.empty() || arc.eplT.empty()) {
    LOGN_VERBOSE("unexpected empty arc EdgePointList");
    return;
  } 
  if(arc.eplS.size()!=arc.eplT.size()) {
    LOGN_VERBOSE("unexpected size missmatch in arc EdgePointLists (" << arc.H->name << "," << arc.K->name << ")");
    return;
  }

  std::vector<EdgePointList> epll(arc.eplS.size());

  int step=static_cast<int>(floor(arc.H->rad*arc.phi/patch));
  Real delta_phi = arc.phi/static_cast<Real>(step);

  // initialize first component of each convex running edge list
  // is equivalent to step zero
  for(unsigned int k=0;k<arc.eplS.size();++k) {
    epll[k].push_back(arc.eplS[k]);
  }

  // outer loop goes over the convex part of saddle
  for(int c=1;c<step;++c) {
    Mat3 vmat=AxisRotation(arc.axis,static_cast<Real>(c)*delta_phi);
    // inner loop goes over concave part
    for(unsigned int k=0;k<arc.eplS.size();++k) {
      // this is the direction vector which is rotated around the arc axis
      Vec3 p0 = arc.eplS[k].pos-arc.fixpoint;
      Vec3 p1 = vmat*p0+arc.fixpoint;
      // the normal is retrieved from a previous calculation
      Vec3 n1 = vmat*arc.eplS[k].norm;
      epll[k].push_back(EdgePoint(surf->AddVertex(SurfaceVertex(p1,n1,2)),p1,n1));
    }
  }

  // this should be equivalent to the above rotation for c==step...
  for(unsigned int k=0;k<arc.eplS.size();++k) {
    epll[k].push_back(arc.eplT[k]);
  }

  // now that all vertices are created, assign triangles
  for(unsigned int k=0;k<arc.eplS.size()-1;++k) {
    for(unsigned int c=0;c<epll[k].size()-1;++c) {
      surf->AddTri(epll[k+1][c+1].id,
                   epll[k][c+1].id,
                   epll[k][c].id);
      surf->AddTri(epll[k+1][c].id,
                   epll[k+1][c+1].id,
                   epll[k][c].id);
    }
  }

  // book-keeping for the spheres
  ArcDirEntry adeH,adeK;
  adeH.v=arc.eplS.front();
  adeH.w=arc.eplT.front();
  adeH.arc=arcp;
  adeH.inv_arc=false;
  adeK.v=arc.eplT.back();
  adeK.w=arc.eplS.back();
  adeK.arc=arcp;
  adeK.inv_arc=true;
  for(int c=0;c<step;++c) {
    adeH.epl.push_back(epll.front()[c]);
    adeK.epl.push_back(epll.back()[step-c-1]);
  }

  arc.H->arc_list.push_back(adeH);
  arc.K->arc_list.push_back(adeK);
}

void fix_arc_list(ArcDirDeque& elist)
{
  for(uint a=0;a<elist.size()-1;++a) {
    for(uint b=a+1;b<elist.size();++b) {
      if(elist[a].v.id != elist[b].v.id) {
        if(Length(elist[a].v.pos-elist[b].v.pos)<1.0e-4) {
          elist[b].v.id = elist[a].v.id;
        }
      }
      if(elist[a].v.id != elist[b].w.id) {
        if(Length(elist[a].v.pos-elist[b].w.pos)<1.0e-4) {
          elist[b].w.id = elist[a].v.id;
        }
      }
      if(elist[a].w.id != elist[b].v.id) {
        if(Length(elist[a].w.pos-elist[b].v.pos)<1.0e-4) {
          elist[b].v.id = elist[a].w.id;
        }
      }
      if(elist[a].w.id != elist[b].w.id) {
        if(Length(elist[a].w.pos-elist[b].w.pos)<1.0e-4) {
          elist[b].w.id = elist[a].w.id;
        }
      }
    }
  }
}


std::vector<ArcDirDeque> sort_arc_list(ArcDirDeque arc_list,
                                       std::vector<Vec3>& topsumlist,
                                       Sphere& sp)
{
  std::vector<ArcDirDeque> elistlist;
  while(!arc_list.empty()) {
    bool used=false;
    for(ArcDirDeque::iterator ait=arc_list.begin();ait!=arc_list.end();++ait) {
      for(uint l=0;l<elistlist.size();++l) {
        if(ait->v.id == elistlist[l].back().w.id) {
          elistlist[l].push_back(*ait);
          arc_list.erase(ait);
          used=true;
        } else if(ait->w.id == elistlist[l].front().v.id) {
          elistlist[l].push_front(*ait);
          arc_list.erase(ait);
          used=true;
        }
        if(used) {
          topsumlist[l]+=(ait->arc->mid-sp.pos)*ait->arc->phi;
          break;
        }
      }
      if(used) break;
    }
    if(!used) {
      elistlist.push_back(ArcDirDeque());
      elistlist.back().push_back(arc_list.back());
      topsumlist.push_back((arc_list.back().arc->mid-sp.pos)*arc_list.back().arc->phi);
      arc_list.pop_back();
    }
  }
  return elistlist;
}


struct PolarMap
{
  struct Entry {
    Real theta, phi;
    uint id;
  };

  typedef std::vector<Entry> EntryList;

  PolarMap(): theta_off(0.0), phi_off(0.0) {}
  void Add(const Vec3& v, uint id) {
    Real l = Length(v);
    Real theta=atan2(v[0],v[2]);
    Real phi=acos(v[1]/l);
    Real theta1=theta+theta_off;
    Real phi1=phi+phi_off;

    if(!entry_list.empty()) {
      Real theta0=entry_list.back().theta;
      Real phi0=entry_list.back().phi;

      /*Real theta2=theta1;
      Real phi2=phi1;*/
      if(theta1-theta0>M_PI) {
        theta1-=2.0*M_PI;
        theta_off=-2.0*M_PI;
      } else if(theta1-theta0<-M_PI) {
        theta1+=2.0*M_PI;
        theta_off+=2.0*M_PI;
      }
      if(phi1-phi0>M_PI_2) {
        phi1-=M_PI;
        phi_off=-M_PI;
      } else if(phi1-phi0<-M_PI_2) {
        phi1+=M_PI;
        phi_off+=M_PI;
      }

      theta_min=std::min(theta_min,theta1);
      theta_max=std::max(theta_max,theta1);
      phi_min=std::min(phi_min,phi1);
      phi_max=std::max(phi_max,phi1);

    } else {
      theta_min=theta1;
      theta_max=theta1;
      phi_min=phi1;
      phi_max=phi1;
    }
    Entry e={theta1,phi1,id};
    entry_list.push_back(e);
  }

  Real theta_off,phi_off;
  Real theta_min,theta_max;
  Real phi_min,phi_max;
  EntryList entry_list;
};

struct CapEdgeEntry {
  CapEdgeEntry() {}
  CapEdgeEntry(uint i, Real t, Real p): eid(i), theta(t), phi(p) {}
  uint eid;
  Real theta, phi;
};

typedef std::vector<CapEdgeEntry> CapEdgeList;

struct CapVecEntry {
};

typedef std::vector<CapVecEntry> CapVecList;

void build_cap(Sphere& sp, Real patch, impl::SurfaceImplP& surf)
{
  // assemble sorted lists of spheres and edge-point-lists
  std::vector<Vec3> topsumlist;
  std::vector<ArcDirDeque> alistlist = sort_arc_list(sp.arc_list,topsumlist,sp);
  
  sp.arc_list_list=alistlist;

  bool check=true;
  // for each independent list, build cap
  for(uint l=0;l<alistlist.size();++l) {
    ArcDirDeque& alist = alistlist[l];
    if(alist.front().v.id != alist.back().w.id) {
      check=false;
      break;
    }
  }
  if(!check) {
    LOGN_DEBUG("non-circular arc-dir list found, attempting fix");
    ArcDirDeque tmp_list=sp.arc_list;
    fix_arc_list(tmp_list);
    topsumlist.clear();
    alistlist = sort_arc_list(tmp_list,topsumlist,sp);
    sp.arc_list_list=alistlist;
  }

  sp.top_list.resize(alistlist.size());

  std::vector<Vec3> vlist;
  std::vector<uint> ilist;
  build_sphere(4,vlist,ilist);

  for(uint l=0;l<alistlist.size();++l) {
    ArcDirDeque& alist = alistlist[l];
    if(alist.front().v.id != alist.back().w.id) {
      LOGN_VERBOSE("unexpected non-circular arc-dir list for [" << sp.name << "]");
      continue;
    }

    // generate the top position
    Vec3 sum1;
    Vec3 sum2;
    Vec3 prev;
    float plan=0.0;
    for(uint i=0;i<alist.size();++i) {
      uint j = (i-1+alist.size())%alist.size();
      uint k = (i+1)%alist.size();
      sum1+=(alist[i].arc->mid-sp.pos)*alist[i].arc->phi;
      Vec3 d0 = alist[i].arc->S->pos-sp.pos;
      Vec3 d1 = alist[i].arc->mid-sp.pos;
      Vec3 d2 = alist[i].arc->T->pos-sp.pos;
      
      Vec3 p0 = sp.pos+sp.rad*(d0)/(sp.rad+alist[i].arc->S->rad);
      Vec3 p1 = sp.pos+sp.rad*(d1)/(sp.rad+alist[i].arc->S->rad);
      Vec3 p2 = sp.pos+sp.rad*(d2)/(sp.rad+alist[i].arc->S->rad);
      Vec3 c0 = Normalize(Cross(Normalize(alist[i].w.pos-alist[i].v.pos),
                                Normalize(alist[k].w.pos-alist[k].v.pos))+
                          Cross(Normalize(alist[j].w.pos-alist[j].v.pos),
                                Normalize(alist[i].w.pos-alist[i].v.pos)));
      if(i>0) {
        plan+=Dot(prev,Normalize(d1));
      }
      prev=Normalize(d1);
      sum2+=c0*alist[i].arc->phi;
    }
    Real f = plan/static_cast<Real>(alist.size());
    // this is a combination of two algorithms, weighted by planarity
    Vec3 topn = Normalize(f*sum1+(1.0-f)*sum2);
    Vec3 topv = sp.pos+sp.rad*topn;

    sp.top_list[l]=topv;

    // now for the actual cap

    PolarMap polar_map;
    for(ArcDirDeque::const_iterator ait=alist.begin();ait!=alist.end();++ait) {
      std::cerr << ait->v.id << std::endl;
      for(EdgePointList::const_iterator eit=ait->epl.begin();eit!=ait->epl.end();++eit) {
        polar_map.Add(eit->pos-sp.pos,eit->id);
      }
    }
    if(polar_map.entry_list.empty()) continue;
    polar_map.entry_list.push_back(polar_map.entry_list.front());

    for(uint i=0;i<polar_map.entry_list.size();++i) {
      PolarMap::Entry& e = polar_map.entry_list[i];
      std::cerr << i+1 << " " << (e.theta-polar_map.theta_min)*57.3 << " " << (e.phi+(M_PI-polar_map.phi_max))*57.3 << std::endl;
    }

  }
}

} // ns
  
void RSurf::Triangulate(impl::SurfaceImplP& surface, Real patch_size)
{
  for(ArcList::iterator it=arc_list_.begin();it!=arc_list_.end();++it) {
    Arc& arc = *(*it);
    Real da=arc.H->rad+probe_radius_;
    Real db=arc.K->rad+probe_radius_;
    Real dc=Length(arc.H->pos-arc.K->pos);
    Real cosphi = -0.5*(dc*dc-da*da-db*db)/(da*db);
    cosphi = std::min(Real(1.0),std::max(Real(-1.0),cosphi));
    arc.step = static_cast<int>(floor(acos(cosphi)*probe_radius_/patch_size));
  }

  for(TetList::const_iterator it=tet_list_.begin();it!=tet_list_.end();++it) {
    Tet& tet=*(*it);
    // make S sphere concave patch
    EdgePointList3 epl3=build_patch(tet, patch_size, -1.0, surface);

    // assign stored edgepointlists
    if(tet.U->S==tet.S) {
      tet.U->eplS=epl3.l1;
    } else if(tet.U->T==tet.S) {
      tet.U->eplT=revert_epl(epl3.l1);
    }
    if(tet.V->S==tet.S) {
      tet.V->eplS=epl3.l2;
    } else if(tet.V->T==tet.S) {
      tet.V->eplT=revert_epl(epl3.l2);
    }
    if(tet.W->S==tet.S) {
      tet.W->eplS=epl3.l3;
    } else if(tet.W->T==tet.S) {
      tet.W->eplT=revert_epl(epl3.l3);
    }
  }

  for(ArcList::iterator it=arc_list_.begin();it!=arc_list_.end();++it) {
    build_saddle(*it,patch_size,surface);
  }

  for(SphereList::const_iterator it=sphere_list_.begin();it!=sphere_list_.end();++it) {
    if((*it)->arc_list.empty()) continue;
    build_cap(*(*it),patch_size,surface);
  }

  std::vector<SurfaceVertexID> vlist = surface->GetVertexIDList();
  std::vector<SurfaceTriID> tlist = surface->GetTriIDList();
  LOGN_VERBOSE("added " << vlist.size() << " vertices and " << tlist.size() << " tris");
}


}}} // ns
