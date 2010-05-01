#include "scene_fx.hh"

#include <ost/gfx/gfx_node.hh>
#include <ost/gfx/shader.hh>
#include <ost/gfx/scene.hh>

namespace ost { namespace gfx { namespace impl {

void prep_shadow_map(Scene& scene, GLuint texunit, GLuint texid, uint quality)
{
  GLint smap_size=256 << quality;

  // modelview transform for the lightsource pov
  mol::Transform ltrans(scene.GetTransform());
  ltrans.SetRot(scene.GetLightRot()*scene.GetTransform().GetRot());

  // calculate encompassing box for ortho projection
  geom::AlignedCuboid bb=scene.GetBoundingBox(ltrans);
  geom::Vec3 tmin=bb.GetMin();
  geom::Vec3 tmax=bb.GetMax();

  // render pass 1 - without shadows

  // turn shadowing off for subsequent rendering
  Shader::Instance().SetShadowMapping(false,0);

  // save overall gl settings
  glPushAttrib(GL_VIEWPORT_BIT | GL_ENABLE_BIT);
  // maximize rendering for depth-only information
  glDisable(GL_LIGHTING);
  glDisable(GL_FOG);
  glDisable(GL_COLOR_MATERIAL);
  glDisable(GL_NORMALIZE);
  glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);

  // render scene with only depth components
  // seen from the light's perspective
  glViewport(0,0,smap_size,smap_size);
  glClear(GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(tmin[0],tmax[0],tmin[1],tmax[1],-tmax[2],-tmin[2]);
  //glFrustum(tmin[0],tmax[0],tmin[1],tmax[1],-tmax[2],-tmin[2]);
  float glpmat[16];
  glGetv(GL_PROJECTION_MATRIX, glpmat);
  geom::Mat4 pmat(Transpose(geom::Mat4(glpmat)));

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glMultMatrix(ltrans.GetTransposedMatrix().Data());

  // only render non-transparent objects for the shadow map
  scene.GetRootNode()->RenderGL(DEPTH_RENDER_PASS);

  // now get the shadow map
  glActiveTexture(GL_TEXTURE0+texunit);
  glBindTexture(GL_TEXTURE_2D, texid);

  glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                   0,0, smap_size,smap_size, 0);

  // all of the following texture and shader params need to be moved
  // to a one-time initialization place
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  // assign tex unit 0 to shadow map
  Shader::Instance().SetShadowMapping(true,texunit);

  // restore settings
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);

  glPopAttrib();
  glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

  glEnable(GL_TEXTURE_2D);

  // set up appropriate texture matrix
  geom::Mat4 bias(0.5,0.0,0.0,0.5,
                  0.0,0.5,0.0,0.5,
                  0.0,0.0,0.5,0.5,
                  0.0,0.0,0.0,1.0);
  
  geom::Mat4 texm = bias*pmat*ltrans.GetMatrix();

  glMatrixMode(GL_TEXTURE);
  // make explicit object instead of temporary to avoid potential crash with Data()
  geom::Mat4 ttmp=Transpose(texm);
  glLoadMatrix(ttmp.Data());
  glMatrixMode(GL_MODELVIEW);
}

void prep_amb_occlusion(Scene& scene, GLuint texunit, GLuint texid)
{
}

void prep_depth_darkening(Scene& scene, GLuint texunit, GLuint texid)
{
  Viewport vp=scene.GetViewport();
  prep_depth_map(scene,vp.width/2,vp.height/2,texunit,texid);

  // kernel is static for now, inside the convolution shader

  // now convolute the depth map with the kernel
  Shader::Instance().PushProgram();
  Shader::Instance().Activate("convolute1");
  GLuint cpr=Shader::Instance().GetCurrentProgram();
  // assign tex units
  glUniform1i(glGetUniformLocation(cpr,"data"),1);
  glUniform1i(glGetUniformLocation(cpr,"vp_width"),vp.width/2);
  glUniform1i(glGetUniformLocation(cpr,"vp_height"),vp.height/2);

  // set up viewport filling quad to run the fragment shader
  draw_screen_quad(vp.width/2,vp.height/2,texunit, texid);

  Shader::Instance().PopProgram();

  // mode 1, tex unit 1
  Shader::Instance().SetDepthMapping(1,texunit);
}

void prep_depth_map(Scene& scene, unsigned int width, unsigned int height,GLuint texunit, GLuint texid)
{
  // render pass 1 - without shadows
  Shader::Instance().SetShadowMapping(false,0);
  // save overall gl settings
  glPushAttrib(GL_ENABLE_BIT | GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT);
  // maximize rendering for depth-only information
  glDisable(GL_LIGHTING);
  glDisable(GL_FOG);
  glDisable(GL_COLOR_MATERIAL);
  glDisable(GL_NORMALIZE);
  glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);

  // render scene with only depth components
  glClear(GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,width,height);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glMultMatrix(scene.GetTransform().GetTransposedMatrix().Data());
  scene.GetRootNode()->RenderGL(DEPTH_RENDER_PASS);

  // now get the depth map
  glActiveTexture(texunit);
  glBindTexture(GL_TEXTURE_2D, texid);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0,0, width, height, 0);

  // restore settings
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glPopAttrib();
  glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
}

void draw_screen_quad(unsigned int w, unsigned int h, GLuint gltex, GLuint tex_id) 
{
  glPushAttrib(GL_ENABLE_BIT | GL_VIEWPORT_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_FOG_BIT);
  
  // setup
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glDisable(GL_COLOR_MATERIAL);
  glDisable(GL_FOG);
  glDisable(GL_CULL_FACE);
  glDisable(GL_BLEND);
  glDisable(GL_LINE_SMOOTH);
  glDisable(GL_POINT_SMOOTH);
  glClear(GL_COLOR_BUFFER_BIT);
  glViewport(0,0,w,h);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0,1,0,1,-1,1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  
  // draw
  glColor3f(1.0,0.0,1.0);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0,0.0);
  glVertex2f(0.0,0.0);
  glTexCoord2f(0.0,1.0);
  glVertex2f(0.0,1.0);
  glTexCoord2f(1.0,1.0);
  glVertex2f(1.0,1.0);
  glTexCoord2f(1.0,0.0);
  glVertex2f(1.0,0.0);
  glEnd();
  
  // grab the result
  glEnable(GL_TEXTURE_2D);
  glActiveTexture(gltex);
  glBindTexture(GL_TEXTURE_2D, tex_id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0,0, w, h, 0);
  
  // restore settings
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glPopAttrib();
}

// this debug code draws the given texture across the complete screen
void draw_debug_tex(unsigned int w, unsigned int h, GLuint texid)
{
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  Shader::Instance().PushProgram();
  
  Shader::Instance().Activate("");
  glViewport(0,0,w,h);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0,1,0,1,-1,1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glColor3f(1.0,0.0,1.0);
  glEnable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texid);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0,0.0);
  glVertex2f(0.01,0.01);
  glTexCoord2f(0.0,1.0);
  glVertex2f(0.01,0.99);
  glTexCoord2f(1.0,1.0);
  glVertex2f(0.99,0.99);
  glTexCoord2f(1.0,0.0);
  glVertex2f(0.99,0.01);
  glEnd();
  glPopAttrib();
  Shader::Instance().PopProgram();
}


}}} // ns
