#include "scene_fx.hh"

#include <ost/log.hh>

#include <ost/gfx/gfx_node.hh>
#include <ost/gfx/shader.hh>
#include <ost/gfx/scene.hh>

namespace ost { namespace gfx { namespace impl {

SceneFX& SceneFX::Instance()
{
  static SceneFX inst;
  return inst;
}

SceneFX::SceneFX():
  shadow_flag(false),
  shadow_quality(1),
  depth_dark_flag(false),
  amb_occl_flag(false),
  scene_tex_id_(),
  depth_tex_id_(),
  shadow_tex_id_(),
  occl_tex_id_(),
  dark_tex_id_(),
  norm_tex_id_(),
  scene_tex2_id_(),
  norm_tex2_id_(),
  scene_fb_(),
  depth_rb_(),
  use_fb_(false)
{}

SceneFX::~SceneFX()
{}

void SceneFX::Setup()
{
  LOGN_VERBOSE("SceneFX: setup");

  glGenTextures(1,&scene_tex_id_);
  glGenTextures(1,&depth_tex_id_);
  glGenTextures(1,&shadow_tex_id_);
  glGenTextures(1,&occl_tex_id_);
  glGenTextures(1,&dark_tex_id_);
  glGenTextures(1,&norm_tex_id_);

  glGenFramebuffers(1,&scene_fb_);
  glGenRenderbuffers(1,&depth_rb_);
  glGenTextures(1,&scene_tex2_id_);
  glGenTextures(1,&norm_tex2_id_);

  glActiveTexture(GL_TEXTURE0);

  glBindTexture(GL_TEXTURE_2D, scene_tex_id_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  glBindTexture(GL_TEXTURE_2D, depth_tex_id_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  glBindTexture(GL_TEXTURE_2D, shadow_tex_id_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  glBindTexture(GL_TEXTURE_2D, occl_tex_id_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  glBindTexture(GL_TEXTURE_2D, dark_tex_id_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  glBindTexture(GL_TEXTURE_2D, norm_tex_id_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

}

void SceneFX::Resize(unsigned int w, unsigned int h)
{
#if 0
  glBindTexture(GL_TEXTURE_2D, scene_tex2_id_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
  glBindTexture(GL_TEXTURE_2D, norm_tex2_id_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
  glBindTexture(GL_TEXTURE_2D, 0);

  glBindFramebuffer(GL_FRAMEBUFFER, scene_fb_);

  glBindRenderbuffer(GL_RENDERBUFFER, depth_rb_);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,w,h);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rb_);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, scene_tex2_id_, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, norm_tex2_id_, 0);

  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

  if(status!=GL_FRAMEBUFFER_COMPLETE) {
    use_fb_=false;
    LOGN_VERBOSE("SceneFX: framebuffer error code " << status);
  } else {
    use_fb_=true;
  }

  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
}

void SceneFX::Preprocess() 
{
  if(use_fb_) {
    glBindFramebuffer(GL_FRAMEBUFFER, scene_fb_);
  }
}

void SceneFX::Postprocess()
{
  Viewport vp=Scene::Instance().GetViewport();

  if(use_fb_) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  } else {
    // grab color buffer
    glBindTexture(GL_TEXTURE_2D, scene_tex_id_);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, vp.x, vp.y, vp.width, vp.height, 0);
    // and depth buffer
    glBindTexture(GL_TEXTURE_2D, depth_tex_id_);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, vp.x, vp.y, vp.width, vp.height, 0);

    if(amb_occl_flag) {
      // now for the normal buffer hack if the framebuffer stuff failed
      Shader::Instance().PushProgram();
      Shader::Instance().Activate("dumpnorm");
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      Scene::Instance().GetRootNode()->RenderGL(STANDARD_RENDER_PASS);
      Shader::Instance().PopProgram();
      glBindTexture(GL_TEXTURE_2D, norm_tex_id_);
      glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, vp.x, vp.y, vp.width, vp.height, 0);
    }
  }

  if(shadow_flag) {
    prep_shadow_map();
  }
  if(amb_occl_flag) {
    prep_amb_occlusion();
    draw_debug_tex(vp.width,vp.height,occl_tex_id_);
    return;
  }
  if(depth_dark_flag) {
    //prep_depth_darkening();
  }


  Shader::Instance().PushProgram();
  glEnable(GL_TEXTURE_2D);
  Shader::Instance().Activate("scenefx");
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D,depth_tex_id_);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,scene_tex_id_);
  GLuint cpr=Shader::Instance().GetCurrentProgram();
  glUniform1i(glGetUniformLocation(cpr,"scene"),0);
  glUniform1i(glGetUniformLocation(cpr,"depth"),1);
  glUniform2f(glGetUniformLocation(cpr,"scalef"),
	      1.0f/static_cast<float>(vp.width),
	      1.0f/static_cast<float>(vp.height));
  glUniform2f(glGetUniformLocation(cpr,"zmorph"),
	      Scene::Instance().GetFar()-Scene::Instance().GetNear(),
	      Scene::Instance().GetNear());

  if(shadow_flag) {
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D,shadow_tex_id_);
    glUniform1i(glGetUniformLocation(cpr,"shadow_flag"),1);
    glUniform1i(glGetUniformLocation(cpr,"shadow_map"),2);
    glUniform1f(glGetUniformLocation(cpr,"shadow_depth_bias"),0.008);
    glUniform1f(glGetUniformLocation(cpr,"shadow_epsilon"),0.002);
    glUniform1f(glGetUniformLocation(cpr,"shadow_multiplier"),0.4);
  } else {
    glUniform1i(glGetUniformLocation(cpr,"shadow_flag"),0);
  }
#if 0
  if(amb_occl_flag) {
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D,occl_tex_id_);
    glUniform1i(glGetUniformLocation(cpr,"occl_flag"),1);
    glUniform1i(glGetUniformLocation(cpr,"occl_map"),3);
  } else {
    glUniform1i(glGetUniformLocation(cpr,"occl_flag"),0);
  }
#endif

  draw_screen_quad(vp.width,vp.height);
  glDisable(GL_TEXTURE_2D);
  Shader::Instance().PopProgram();
}

void SceneFX::prep_shadow_map()
{
  GLint smap_size=256 << shadow_quality;

  // modelview transform for the lightsource pov
  mol::Transform ltrans(Scene::Instance().GetTransform());
  ltrans.SetRot(Scene::Instance().GetLightRot()*Scene::Instance().GetTransform().GetRot());

  // calculate encompassing box for ortho projection
  geom::AlignedCuboid bb=Scene::Instance().GetBoundingBox(ltrans);
  geom::Vec3 tmin=bb.GetMin();
  geom::Vec3 tmax=bb.GetMax();

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
  Scene::Instance().GetRootNode()->RenderGL(DEPTH_RENDER_PASS);

  // now get the shadow map
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, shadow_tex_id_);
  glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0,0, smap_size,smap_size, 0);

  // restore settings
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);

  glPopAttrib();
  glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

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

void SceneFX::prep_amb_occlusion()
{
  Viewport vp=Scene::Instance().GetViewport();

  Shader::Instance().PushProgram();
  Shader::Instance().Activate("amboccl");
  GLuint cpr=Shader::Instance().GetCurrentProgram();
  // assign tex units
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,depth_tex_id_);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D,norm_tex_id_);
  glUniform1i(glGetUniformLocation(cpr,"depth"),0);
  glUniform1i(glGetUniformLocation(cpr,"norm"),1);

  // set up viewport filling quad to run the fragment shader
  draw_screen_quad(vp.width,vp.height);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, occl_tex_id_);
  glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0,0, vp.width, vp.height, 0);

  Shader::Instance().PopProgram();
}

void SceneFX::prep_depth_darkening()
{
  Viewport vp=Scene::Instance().GetViewport();

  // kernel is static for now, inside the convolution shader
  Shader::Instance().PushProgram();
  Shader::Instance().Activate("convolute1");
  GLuint cpr=Shader::Instance().GetCurrentProgram();
  // assign tex units
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D,depth_tex_id_);
  glUniform1i(glGetUniformLocation(cpr,"data"),1);
  glUniform1i(glGetUniformLocation(cpr,"vp_width"),vp.width/2);
  glUniform1i(glGetUniformLocation(cpr,"vp_height"),vp.height/2);

  // set up viewport filling quad to run the fragment shader
  draw_screen_quad(vp.width/2,vp.height/2);

  glBindTexture(GL_TEXTURE_2D, dark_tex_id_);
  glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0,0,vp.width/2, vp.height/2, 0);

  Shader::Instance().PopProgram();
}

void SceneFX::draw_screen_quad(unsigned int w, unsigned int h) 
{
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  
  // setup
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glDisable(GL_COLOR_MATERIAL);
  glDisable(GL_FOG);
  glDisable(GL_CULL_FACE);
  glDisable(GL_BLEND);
  glDisable(GL_LINE_SMOOTH);
  glDisable(GL_POINT_SMOOTH);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,w,h);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0,1,0,1,-1,1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glEnable(GL_TEXTURE_2D);
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
  
  // restore settings
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glPopAttrib();
}

// this debug code draws the given texture across the complete screen
void SceneFX::draw_debug_tex(unsigned int w, unsigned int h, GLuint texid)
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
