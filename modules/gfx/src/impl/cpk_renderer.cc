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
#include "cpk_renderer.hh"

#include <ost/gfx/gl_helper.hh>

#include <ost/gfx/entity.hh>

#include <ost/gfx/scene.hh>
#if OST_SHADER_SUPPORT_ENABLED
#include <ost/gfx/shader.hh>
#endif //OST_SHADER_SUPPORT_ENABLED
#include <ost/gfx/vertex_array_helper.hh>
#include <ost/gfx/render_mode.hh>
#include <ost/gfx/povray.hh>

namespace ost { namespace gfx { namespace impl {

CPKRenderer::CPKRenderer(): options_(new CPKRenderOptions()) {
  this->SetName("Spheres");
}

void CPKRenderer::PrepareRendering()
{
  ConnectRendererBase::PrepareRendering();
  va_.Clear();
  this->PrepareRendering(view_, va_, false);
  sel_va_.Clear();
  if (this->HasSelection()) {
    this->PrepareRendering(sel_view_, sel_va_, true);
    sel_va_.SetLighting(false);
    sel_va_.SetColorMaterial(true);
  }
}

void CPKRenderer::PrepareRendering(GfxView& view, IndexedVertexArray& va, bool is_sel){
  const Color& sel_clr=this->GetSelectionColor();
  float factor=is_sel ? 1.2 : 1.0;
  if(options_!=NULL){
    va.SetLighting(true);
    va.SetCullFace(true);
    va.SetColorMaterial(true);
    va.SetMode(0x4);

    // draw all spheres
    for(AtomEntryMap::const_iterator it=view.atom_map.begin();it!=view.atom_map.end();++it) {
      va.AddSphere(SpherePrim(it->second.atom.GetPos(),
                              it->second.vdwr*factor,
                              is_sel? sel_clr : it->second.color),
                              options_->GetSphereDetail());
    }
  }
  sel_state_=0;
  state_=0;
}

void CPKRenderer::Render(RenderPass pass)
{
  if(options_!=NULL){
#if OST_SHADER_SUPPORT_ENABLED
    if(pass==STANDARD_RENDER_PASS && (options_->GetSphereMode()==1 || options_->GetSphereMode()==2)) {
      this->Render3DSprites();
      return;
    }
#endif
  }
  // fall back to parent if above did not fire
  EntityRenderer::Render(pass);
}

void CPKRenderer::RenderPov(PovState& pov, const std::string& name)
{
  if(view_.atom_map.empty()) return;
  pov.write_merge_or_union(name);
  
  for (AtomEntryMap::const_iterator it=view_.atom_map.begin();it!=view_.atom_map.end();++it) {
    pov.write_sphere(it->second.atom.GetPos(),
                     it->second.vdwr,
                     it->second.color,
                     name);
  }

  pov.inc() << " }\n";
}

bool CPKRenderer::CanSetOptions(RenderOptionsPtr& render_options){
 return render_options.get()->GetRenderMode()==RenderMode::CPK;
}

void CPKRenderer::SetOptions(RenderOptionsPtr& render_options){
  options_ = boost::static_pointer_cast<CPKRenderOptions>(render_options);
}

RenderOptionsPtr CPKRenderer::GetOptions(){
 return options_;
}

namespace {

void Render3DSpritesInnerLoop(const AtomEntry* ae, const geom::Vec3& cx, 
			      const geom::Vec3& cy, const geom::Vec3& cz, 
			      GLdouble* gl_mmat, GLdouble* gl_pmat, GLint* gl_vp)
{
  geom::Vec3 pos = ae->atom.GetPos();
  float rad = ae->vdwr;
  GLdouble r1[3],r2[3];
  gluProject(pos[0],pos[1],pos[2],
             gl_mmat,gl_pmat,gl_vp,
             &r1[0],&r1[1],&r1[2]);
  gluProject(pos[0]-rad*cz[0],pos[1]-rad*cz[1],pos[2]-rad*cz[2],
             gl_mmat,gl_pmat,gl_vp,
             &r2[0],&r2[1],&r2[2]);
  float scale = r1[2]-r2[2];
  glColor3fv(ae->color);
  glTexCoord4f(0.0,0.0,-rad,scale);
  glVertex3v((pos-rad*cx-rad*cy).Data());
  glTexCoord4f(0.0,1.0,-rad,scale);
  glVertex3v((pos-rad*cx+rad*cy).Data());
  glTexCoord4f(1.0,1.0,-rad,scale);
  glVertex3v((pos+rad*cx+rad*cy).Data());
  glTexCoord4f(1.0,0.0,-rad,scale);
  glVertex3v((pos+rad*cx-rad*cy).Data());
}

}

void CPKRenderer::Render3DSprites()
{
#if OST_SHADER_SUPPORT_ENABLED
  if(options_!=NULL){
    geom::Mat3 irot=geom::Transpose(Scene::Instance().GetTransform().GetRot());
    geom::Vec3 cx=irot*geom::Vec3(1.0,0.0,0.0);
    geom::Vec3 cy=irot*geom::Vec3(0.0,1.0,0.0);
    geom::Vec3 cz=irot*geom::Vec3(0.0,0.0,1.0);

    uint write_normals = Shader::Instance().GetCurrentName()=="dumpnorm" ? 1 : 0;
    Shader::Instance().PushProgram();
    Shader::Instance().Activate("fast_sphere");
    Shader::Instance().UpdateState();
    glUniform1i(glGetUniformLocation(Shader::Instance().GetCurrentProgram(),"write_normals"),write_normals);

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);

    GLdouble gl_mmat[16];
    glGetDoublev(GL_MODELVIEW_MATRIX,gl_mmat);
    GLdouble gl_pmat[16];
    glGetDoublev(GL_PROJECTION_MATRIX,gl_pmat);
    GLint gl_vp[]={0,0,1,1};

    glBegin(GL_QUADS);

    for(AtomEntryMap::const_iterator it=view_.atom_map.begin();it!=view_.atom_map.end();++it) {
      Render3DSpritesInnerLoop(&it->second,cx,cy,cz,gl_mmat,gl_pmat,gl_vp);
    }

    glEnd();
    glPopAttrib();

    Shader::Instance().PopProgram();
    Shader::Instance().UpdateState();
  }
#endif
}

}}}
