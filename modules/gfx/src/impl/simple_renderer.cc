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
  Authors: Ansgar Philippsen, Stefan Scheuber, Marco Biasini
 */

#include <ost/gfx/gl_helper.hh>
#include <ost/gfx/scene.hh>
#include <ost/gfx/entity.hh>

#include "simple_renderer.hh"

namespace ost { namespace gfx { namespace impl {


namespace {

struct BlurQuadEntry
{
  float zdist;
  geom::Vec3 p1,p2,p3,p4;
  Color c1,c2,c3,c4;
};

struct BlurQuadEntryLess
{
  bool operator()(const BlurQuadEntry& e1, const BlurQuadEntry& e2)
  {
    // provides back-to-front sorting
    return e1.zdist<e2.zdist;
  }
};

}
  
SimpleRenderer::SimpleRenderer(): options_(new SimpleRenderOptions()) 
{
    this->SetFixedPickRadius(0.2);
}

bool SimpleRenderer::CanSetOptions(RenderOptionsPtr& render_options)
{
 return render_options.get()->GetRenderMode()==RenderMode::SIMPLE;
}

void SimpleRenderer::SetOptions(RenderOptionsPtr& render_options)
{
  options_ = boost::static_pointer_cast<SimpleRenderOptions>(render_options);
}

RenderOptionsPtr SimpleRenderer::GetOptions()
{
 return options_;
}

void SimpleRenderer::PrepareRendering()
{
  ConnectRendererBase::PrepareRendering();
  va_.Clear();
  this->PrepareRendering(view_, va_);
  sel_va_.Clear();  
  if (this->HasSelection()) {
    this->PrepareRendering(sel_view_, sel_va_);
    sel_va_.SetLineWidth(options_->GetLineWidth()+5.0);
    sel_va_.SetColorMaterial(false);
  }
  sel_state_=0;
  state_=0;
}

geom::Vec3 SimpleRenderer::GetDoubleBondPlane(mol::BondHandle b)
{
  int n=0;
  geom::Vec3 vec;
  mol::AtomHandle a1=b.GetFirst();
  mol::AtomHandle a2=b.GetSecond();
  mol::AtomHandleList partner1=a1.GetBondPartners();
  mol::AtomHandleList partner2=a2.GetBondPartners();
  geom::Vec3 bond_vec=a2.GetPos()-a1.GetPos();
  GetBondPartnerNormal(vec, n, bond_vec, partner1, a1, a2);
  GetBondPartnerNormal(vec, n, bond_vec, partner2, a1, a2);
  if(n==0) {
    //set vec into x direction if no partners are present
    vec.SetX(1);
  }
  vec=geom::Normalize(geom::Cross(vec,bond_vec));
  return vec;
}

void SimpleRenderer::GetBondPartnerNormal(geom::Vec3& vec,
                                           int& n,
                                           geom::Vec3& bond_vec,
                                           mol::AtomHandleList& ahl,
                                           mol::AtomHandle& a1,
                                           mol::AtomHandle& a2)
{
  // find plane by calculating average cross product between double
  // bond vector and all neighboring bond vectors
  for (mol::AtomHandleList::const_iterator i=ahl.begin();i!=ahl.end();++i) {
    if((*i)!=a1 && (*i)!=a2) {
      geom::Vec3 vec_=geom::Normalize(geom::Cross(
                                              i->GetPos()-a1.GetPos(),
                                              bond_vec));
      if(geom::Dot(vec,vec_)<0) vec_*=-1;
      vec+=vec_;
      n++;
    }
  }
}

void SimpleRenderer::PrepareRendering(GfxView& view, IndexedVertexArray& va)
{
  if (options_!=NULL) {
    static const float cross_size=0.25;
    va.SetLighting(false);
    va.SetCullFace(false);
    va.SetColorMaterial(false);
    va.SetLineWidth(options_->GetLineWidth());
    va.SetPointSize(options_->GetLineWidth());
    va.SetMode(0x2);
    va.SetTwoSided(true);
    va.SetAALines(options_->GetAALines());

    for (BondEntryList::const_iterator it=view.bond_list.begin();
         it!=view.bond_list.end();++it) {
      const geom::Vec3& p0=it->bond.GetFirst().GetPos();
      const geom::Vec3& p2=it->bond.GetSecond().GetPos();
      geom::Vec3 p1=(p0+p2)*0.5;

      if(it->bond.GetBondOrder()!=2 || !options_->GetBondOrderFlag()) {
        VertexID id0 = va.Add(p0,geom::Vec3(),it->atom1->color);
        VertexID id1 = va.Add(p1,geom::Vec3(),it->atom1->color);
        VertexID id2 = va.Add(p1,geom::Vec3(),it->atom2->color);
        VertexID id3 = va.Add(p2,geom::Vec3(),it->atom2->color);
        va.AddLine(id0,id1);
        va.AddLine(id2,id3);
      }
      if(it->bond.GetBondOrder()>=2 && options_->GetBondOrderFlag()) {
        //draw double bond lines for all higher order bonds
        geom::Vec3 vec;
        if(it->bond.GetBondOrder()==2) {
          vec=GetDoubleBondPlane(it->bond);
        } else {
          vec.SetX(1);
        }
        Real dbd=options_->GetBondOrderDistance();
        VertexID id00 = va.Add(p0+dbd*vec,geom::Vec3(),it->atom1->color);
        VertexID id01 = va.Add(p0-dbd*vec,geom::Vec3(),it->atom1->color);
        VertexID id10 = va.Add(p1+dbd*vec,geom::Vec3(),it->atom1->color);
        VertexID id11 = va.Add(p1-dbd*vec,geom::Vec3(),it->atom1->color);
        VertexID id20 = va.Add(p1+dbd*vec,geom::Vec3(),it->atom2->color);
        VertexID id21 = va.Add(p1-dbd*vec,geom::Vec3(),it->atom2->color);
        VertexID id30 = va.Add(p2+dbd*vec,geom::Vec3(),it->atom2->color);
        VertexID id31 = va.Add(p2-dbd*vec,geom::Vec3(),it->atom2->color);
        va.AddLine(id00,id10);
        va.AddLine(id20,id30);
        va.AddLine(id01,id11);
        va.AddLine(id21,id31);
      }
    }
    for (std::vector<long>::const_iterator oit=view.orphan_atom_list.begin();
         oit!=view.orphan_atom_list.end();++oit) {
      AtomEntryMap::const_iterator it = view.atom_map.find(*oit);
      if(it==view.atom_map.end()) continue;
      geom::Vec3 p0 = it->second.atom.GetPos();
      VertexID id0 = va.Add(p0-geom::Vec3(cross_size,0.0,0.0),
                             geom::Vec3(),it->second.color);
      VertexID id1 = va.Add(p0+geom::Vec3(cross_size,0.0,0.0),
                             geom::Vec3(),it->second.color);
      va.AddLine(id0,id1);
      id0 = va.Add(p0-geom::Vec3(0.0,cross_size,0.0),
                    geom::Vec3(),it->second.color);
      id1 = va.Add(p0+geom::Vec3(0.0,cross_size,0.0),
                    geom::Vec3(),it->second.color);
      va.AddLine(id0,id1);
      id0 = va.Add(p0-geom::Vec3(0.0,0.0,cross_size),
                    geom::Vec3(),it->second.color);
      id1 = va.Add(p0+geom::Vec3(0.0,0.0,cross_size),
                    geom::Vec3(),it->second.color);
      va.AddLine(id0,id1);
    }
  }  
}

void SimpleRenderer::RenderBlur()
{
  // add blur for this particular orientation!
  // don't use vertex array, but on-the-fly oriented and z-sorted quads
  mol::Transform tf = Scene::Instance().GetTransform();

  std::vector<BlurQuadEntry> bql;
  const std::pair<Real, Real>& bf=options_->GetBlurFactors();
  for (BondEntryList::iterator it=view_.bond_list.begin();
       it!=view_.bond_list.end();++it) {

    const geom::Vec3 p0=tf.Apply(it->bond.GetFirst().GetPos());
    const geom::Vec3 p2=tf.Apply(it->bond.GetSecond().GetPos());
    geom::Vec3 p1=(p0+p2)*0.5;

    const geom::Vec3 q0=tf.Apply(it->pp1);
    const geom::Vec3 q2=tf.Apply(it->pp2);
    geom::Vec3 q1=(q0+q2)*0.5;

    float ll0 = geom::Length2(p0-q0);
    float ll1 = geom::Length2(p1-q1);
    float ll2 = geom::Length2(p2-q2);

    if(ll0<1e-2 && ll1<1e-2 && ll2<1e-2) continue;

    float x0 = exp(-bf.first*ll0);
    float x1 = exp(-bf.first*ll1);
    float x2 = exp(-bf.first*ll2);

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
    bqe.c3[3]=x0*bf.second;
    bqe.c4[3]=x1*bf.second;

    bql.push_back(bqe);

    // first half
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
    bqe.c3[3]=x1*bf.second;
    bqe.c4[3]=x2*bf.second;

    bql.push_back(bqe);
  }

  std::sort(bql.begin(),bql.end(),BlurQuadEntryLess());

  glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT);
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

void SimpleRenderer::BlurSnapshot()
{
  for (BondEntryList::iterator it=view_.bond_list.begin();
       it!=view_.bond_list.end();++it) {
    it->pp1=it->atom1->atom.GetPos();
    it->pp2=it->atom2->atom.GetPos();
  }
}
void SimpleRenderer::Render(RenderPass pass)
{
  ConnectRendererBase::Render(pass);
  if (pass==STANDARD_RENDER_PASS && options_->GetBlurFlag()) {
    this->RenderBlur();
  }
}

SimpleRenderer::~SimpleRenderer() 
{

}

}}}
