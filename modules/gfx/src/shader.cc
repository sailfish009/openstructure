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

#include <vector>
#include <sstream>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <ost/platform.hh>
#include <ost/log.hh>

#include "glext_include.hh"

#include "shader.hh"
#include "scene.hh"

namespace ost { namespace gfx {

namespace bf = boost::filesystem;

Shader& Shader::Instance()
{
  static Shader instance;
  return instance;
}

Shader::Shader():
  valid_(false),
  current_program_(0),
  current_name_(""),
  shader_code_map_(),
  shader_program_map_()
{}


void Shader::PreGLInit() 
{
#if !defined(__APPLE__)
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    LOGN_ERROR("glew failure: " << glewGetErrorString(err));
    assert(false);
  }
#endif
}

namespace {

bool compile_shader(String shader_name, String shader_code, GLenum shader_type, GLuint& shader_id)
{
  shader_id = glCreateShader(shader_type);
  const char* tmp_ptr[] = {0};
  tmp_ptr[0]=shader_code.c_str();
  glShaderSource(shader_id, 1, tmp_ptr, NULL);
  glCompileShader(shader_id);

  GLint sh_compiled;
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &sh_compiled);
  if(sh_compiled==GL_TRUE) {
    LOGN_VERBOSE("shader [" << shader_name << "] successfully compiled (" << shader_id << ")");
  } else {
    GLint sh_log_length;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &sh_log_length);
    std::vector<GLchar> sh_log(sh_log_length+1);
    glGetShaderInfoLog(shader_id, sh_log_length, NULL, &sh_log[0]);
    LOGN_VERBOSE("shader [" << shader_name << "] compilation failed:" << std::endl << String(&sh_log[0]));
    return false;
  }
  return true;
}

bool link_shader(const std::vector<GLuint>& code_list, String pr_name, GLuint& shader_pr)
{
  shader_pr = glCreateProgram();
  for(std::vector<GLuint>::const_iterator it=code_list.begin();it!=code_list.end();++it) {
    if(*it == 0) {
      LOGN_VERBOSE("skipping shader [" << pr_name << "] due to missing compiled code");
      return false;
    }
    LOGN_DEBUG("attaching compiled shader id " << *it << " to " << shader_pr);
    glAttachShader(shader_pr,*it);
  }
    
  glLinkProgram(shader_pr);
  GLint pr_linked;
  glGetProgramiv(shader_pr,GL_LINK_STATUS,&pr_linked);
  if(pr_linked==GL_TRUE) {
    LOGN_VERBOSE("shader [" << pr_name << "] sucessfully linked");
  } else {
    GLint pr_log_length;
    glGetProgramiv(shader_pr, GL_INFO_LOG_LENGTH, &pr_log_length);
    std::vector<GLchar> pr_log(pr_log_length+1);
    glGetProgramInfoLog(shader_pr, pr_log_length, NULL, &pr_log[0]);
    LOGN_VERBOSE("shader [" << pr_name << "] linking failed:" << std::endl << String(&pr_log[0]));
    return false;
  }
  return true;
}

}

void Shader::Setup() 
{
  String ost_root = GetSharedDataPath();
  bf::path ost_root_dir(ost_root);
  bf::path shader_dir(ost_root_dir / "shader");

  struct ShaderListEntry {
    const char* file;
    GLenum type;
  } shader_list[] = {
    //////////////////////////////////////////////////////////////////
    // this is the master list of all shader code in lib/ost/shader

    {"basic_lf_vs.glsl", GL_VERTEX_SHADER},
    {"basic_lf_fs.glsl", GL_FRAGMENT_SHADER},
    {"fraglight_lf_vs.glsl", GL_VERTEX_SHADER},
    {"fraglight_lf_fs.glsl", GL_FRAGMENT_SHADER},
    {"basic_lfs_vs.glsl", GL_VERTEX_SHADER},
    {"basic_lfs_fs.glsl", GL_FRAGMENT_SHADER},
    {"fraglight_lfs_vs.glsl", GL_VERTEX_SHADER},
    {"fraglight_lfs_fs.glsl", GL_FRAGMENT_SHADER},
    {"basic_hf_vs.glsl", GL_VERTEX_SHADER},
    {"selfx_vs.glsl", GL_VERTEX_SHADER},
    {"selfx_fs.glsl", GL_FRAGMENT_SHADER},
    {"toon_vs.glsl", GL_VERTEX_SHADER},
    {"toon_fs.glsl", GL_FRAGMENT_SHADER},
    {"toon2_fs.glsl", GL_FRAGMENT_SHADER},
    {"noop_vs.glsl", GL_VERTEX_SHADER},
    {"aaline_fs.glsl", GL_FRAGMENT_SHADER},
    {"iso_vs.glsl", GL_VERTEX_SHADER},
    {"iso_fs.glsl", GL_FRAGMENT_SHADER},
    {"fast_sphere_vs.glsl", GL_VERTEX_SHADER},
    {"fast_sphere_fs.glsl", GL_FRAGMENT_SHADER},
    {"outline_vs.glsl", GL_VERTEX_SHADER}
    //////////////////////////////////////////////////////////////////
  };

  shader_code_map_.clear();

  for(unsigned int i=0;i<sizeof(shader_list)/sizeof(ShaderListEntry);++i) {
    String shader_name(shader_list[i].file);
    bf::path shader_file(shader_dir / shader_name);

    if(!bf::exists(shader_file)){
      LOGN_ERROR("not found: [" << shader_file.string() << "], cannot create shaders");
      continue;
    }

    bf::ifstream shader_file_stream(shader_file);
    String shader_code;
    String line;
    while (std::getline(shader_file_stream,line)) {
      shader_code += line + "\n";
    }

    GLuint shader_id;
    if(compile_shader(shader_name,shader_code,shader_list[i].type,shader_id)) {
      shader_code_map_[shader_name]=shader_id;
    } else {
      shader_code_map_[shader_name]=0;
    }
  }

  std::vector<GLuint> shader_program_list;
  GLuint shader_program_id;
  // basic shader
  shader_program_list.push_back(shader_code_map_["basic_lf_vs.glsl"]);
  shader_program_list.push_back(shader_code_map_["basic_lf_fs.glsl"]);
  if(link_shader(shader_program_list,"basic",shader_program_id)) {
    shader_program_map_["basic"]=shader_program_id;
  }
  // fraglight shader
  shader_program_list.clear();
  shader_program_list.push_back(shader_code_map_["fraglight_lf_vs.glsl"]);
  shader_program_list.push_back(shader_code_map_["fraglight_lf_fs.glsl"]);
  if(link_shader(shader_program_list,"fraglight",shader_program_id)) {
    shader_program_map_["fraglight"]=shader_program_id;
  }
  // basic shadow map shader
  shader_program_list.clear();
  shader_program_list.push_back(shader_code_map_["basic_lfs_vs.glsl"]);
  shader_program_list.push_back(shader_code_map_["basic_lfs_fs.glsl"]);
  if(link_shader(shader_program_list,"basic_shadow",shader_program_id)) {
    shader_program_map_["basic_shadow"]=shader_program_id;
  }
  // fraglight shader with shadow map
  shader_program_list.clear();
  shader_program_list.push_back(shader_code_map_["fraglight_lfs_vs.glsl"]);
  shader_program_list.push_back(shader_code_map_["fraglight_lfs_fs.glsl"]);
  if(link_shader(shader_program_list,"fraglight_shadow",shader_program_id)) {
    shader_program_map_["fraglight_shadow"]=shader_program_id;
  }
  // basic hemisphere lighting shader
  shader_program_list.clear();
  shader_program_list.push_back(shader_code_map_["basic_hf_vs.glsl"]);
  shader_program_list.push_back(shader_code_map_["basic_lf_fs.glsl"]);
  if(link_shader(shader_program_list,"hemilight",shader_program_id)) {
    shader_program_map_["hemilight"]=shader_program_id;
  }
  // selfx shader
  shader_program_list.clear();
  shader_program_list.push_back(shader_code_map_["selfx_vs.glsl"]);
  shader_program_list.push_back(shader_code_map_["selfx_fs.glsl"]);
  if(link_shader(shader_program_list,"selfx",shader_program_id)) {
    shader_program_map_["selfx"]=shader_program_id;
  }
  // toon shader
  shader_program_list.clear();
  shader_program_list.push_back(shader_code_map_["toon_vs.glsl"]);
  shader_program_list.push_back(shader_code_map_["toon_fs.glsl"]);
  if(link_shader(shader_program_list,"toon",shader_program_id)) {
    shader_program_map_["toon"]=shader_program_id;
  }
  // toon2 shader
  shader_program_list.clear();
  shader_program_list.push_back(shader_code_map_["toon_vs.glsl"]);
  shader_program_list.push_back(shader_code_map_["toon2_fs.glsl"]);
  if(link_shader(shader_program_list,"toon2",shader_program_id)) {
    shader_program_map_["toon2"]=shader_program_id;
  }
  // line shader
  shader_program_list.clear();
  shader_program_list.push_back(shader_code_map_["basic_lf_vs.glsl"]);
  shader_program_list.push_back(shader_code_map_["aaline_fs.glsl"]);
  if(link_shader(shader_program_list,"aaline",shader_program_id)) {
    shader_program_map_["aaline"]=shader_program_id;
  }
  // iso shader
  shader_program_list.clear();
  shader_program_list.push_back(shader_code_map_["iso_vs.glsl"]);
  shader_program_list.push_back(shader_code_map_["iso_fs.glsl"]);
  if(link_shader(shader_program_list,"iso",shader_program_id)) {
    shader_program_map_["iso"]=shader_program_id;
  }
  // fast sphere shader
  shader_program_list.clear();
  shader_program_list.push_back(shader_code_map_["fast_sphere_vs.glsl"]);
  shader_program_list.push_back(shader_code_map_["fast_sphere_fs.glsl"]);
  if(link_shader(shader_program_list,"fast_sphere",shader_program_id)) {
    shader_program_map_["fast_sphere"]=shader_program_id;
  }
  // basic shader
  shader_program_list.clear();
  shader_program_list.push_back(shader_code_map_["outline_vs.glsl"]);
  shader_program_list.push_back(shader_code_map_["basic_lf_fs.glsl"]);
  if(link_shader(shader_program_list,"outline",shader_program_id)) {
    shader_program_map_["outline"]=shader_program_id;
  }

  valid_=true;
}

void Shader::Activate(const String& name)
{
  if(!name.empty()) {
    std::map<String, GLuint>::iterator it = shader_program_map_.find(name);
    if(it!=shader_program_map_.end()) {
      LOGN_DUMP("switching to shader [" << name << "]");
      glUseProgram(it->second);
      current_program_=it->second;
      current_name_=name;

      if(name=="basic_shadow" || name=="fraglight_shadow") {
    	if(!Scene::Instance().GetShadow())
          Scene::Instance().SetShadow(true);
      } else {
      	if(Scene::Instance().GetShadow())
          Scene::Instance().SetShadow(false);
      }

      UpdateState();
      return;

    } else {
      LOGN_MESSAGE("shader program [" << name << "] not present");
      return;
    }
  }
  LOGN_DUMP("switching to fixed pipeline");
  glUseProgram(0);
  current_program_=0;
  current_name_="";
}

GLuint Shader::GetCurrentProgram() const
{
  return current_program_;
}

String Shader::GetCurrentName() const
{
  return current_name_;
}

bool Shader::IsValid() const
{
  return valid_;
}

bool Shader::IsActive() const
{
  return current_program_!=0;
}

void Shader::PushProgram()
{
  program_stack_.push(current_name_);
}

void Shader::PopProgram()
{
  if(!program_stack_.empty()) {
    current_name_ = program_stack_.top();
    program_stack_.pop();
    Activate(current_name_);
  }
}

void Shader::UpdateState()
{
  if(current_program_!=0) {
    // update current lighting and fog settings, valid for all shaders
    GLint result;
    glGetIntegerv(GL_LIGHTING,&result);
    LOGN_TRACE("setting lighting flag to " << result);
    glUniform1i(glGetUniformLocation(current_program_,"lighting_flag"),result);
    GLboolean bresult;
    glGetBooleanv(GL_LIGHT_MODEL_TWO_SIDE,&bresult);
    LOGN_TRACE("setting two_sided flag to " << bresult);
    glUniform1i(glGetUniformLocation(current_program_,"two_sided_flag"),bresult);
    glGetIntegerv(GL_FOG,&result);
    LOGN_TRACE("setting fog flag to " << result);
    glUniform1i(glGetUniformLocation(current_program_,"fog_flag"),result);
    glDisable(GL_COLOR_MATERIAL);
  } else {
    glEnable(GL_COLOR_MATERIAL);
  }
}

}} // ns
