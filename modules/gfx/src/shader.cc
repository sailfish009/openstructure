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
{
  if(!OST_GL_VERSION_2_0) {
    LOG_VERBOSE("OpenGL version less than 2.0, deactivating shader functionality");
  }
}


void Shader::PreGLInit() 
{
#if !defined(__APPLE__)
#if OST_MESA_SUPPORT_ENABLED
  // skip glew initialization
#else
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    LOG_ERROR("glew failure: " << glewGetErrorString(err));
    assert(false);
  }
#endif
#endif
}

bool Shader::Compile(std::string shader_name, std::string shader_code, 
                     GLenum shader_type, GLuint& shader_id)
{
  shader_id = glCreateShader(shader_type);
  const char* tmp_ptr[] = {0};
  tmp_ptr[0]=shader_code.c_str();
  glShaderSource(shader_id, 1, tmp_ptr, NULL);
  glCompileShader(shader_id);

  GLint sh_compiled;
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &sh_compiled);
  if(sh_compiled==GL_TRUE) {
    LOG_VERBOSE("shader [" << shader_name << "] successfully compiled (" << shader_id << ")");
  } else {
    GLint sh_log_length;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &sh_log_length);
    std::vector<GLchar> sh_log(sh_log_length+1);
    glGetShaderInfoLog(shader_id, sh_log_length, NULL, &sh_log[0]);
    LOG_ERROR("shader [" << shader_name << "] compilation failed:" << std::endl << String(&sh_log[0]));
    return false;
  }
  return true;
}

bool Shader::Link(const std::vector<GLuint>& code_list, std::string pr_name, GLuint& shader_pr)
{
  shader_pr = glCreateProgram();
  for(std::vector<GLuint>::const_iterator it=code_list.begin();it!=code_list.end();++it) {
    if(*it == 0) {
      LOG_WARNING("skipping shader [" << pr_name << "] due to missing compiled code");
      return false;
    }
    LOG_VERBOSE("attaching compiled shader id " << *it << " to " << shader_pr);
    glAttachShader(shader_pr,*it);
  }
    
  glLinkProgram(shader_pr);
  GLint pr_linked;
  glGetProgramiv(shader_pr,GL_LINK_STATUS,&pr_linked);
  if(pr_linked==GL_TRUE) {
    LOG_VERBOSE("shader [" << pr_name << "] sucessfully linked");
  } else {
    GLint pr_log_length;
    glGetProgramiv(shader_pr, GL_INFO_LOG_LENGTH, &pr_log_length);
    std::vector<GLchar> pr_log(pr_log_length+1);
    glGetProgramInfoLog(shader_pr, pr_log_length, NULL, &pr_log[0]);
    LOG_ERROR("shader [" << pr_name << "] linking failed:" << std::endl << String(&pr_log[0]));
    return false;
  }
  return true;
}

bool Shader::Add(const std::string& name, const std::string& vs_code, const std::string& fs_code)
{
  GLuint vs_id=0;
  if(!Shader::Compile(name,vs_code,GL_VERTEX_SHADER,vs_id)) {
    return false;
  }
  
  GLuint fs_id=0;
  if(!Shader::Compile(name,fs_code,GL_FRAGMENT_SHADER,fs_id)) {
    return false;
  }
  
  std::vector<GLuint> shader_program_list;
  GLuint id;

  shader_program_list.push_back(vs_id);
  shader_program_list.push_back(fs_id);
  if(!Shader::Link(shader_program_list,"hatch",id)) {
    return false;
  }

  shader_program_map_[name]=id;
  return true;
}


void Shader::Setup() 
{
  if(!OST_GL_VERSION_2_0) return;

  String ost_root = GetSharedDataPath();
  bf::path ost_root_dir(ost_root);
  bf::path shader_dir(ost_root_dir / "shader");

  struct ShaderListEntry {
    const char* file;
    GLenum type;
  } shader_list[] = {
    //////////////////////////////////////////////////////////////////
    // this is the master list of all shader code in shader/

    {"basic_vs.glsl", GL_VERTEX_SHADER},
    {"basic_fs.glsl", GL_FRAGMENT_SHADER},
    {"fraglight_vs.glsl", GL_VERTEX_SHADER},
    {"fraglight_fs.glsl", GL_FRAGMENT_SHADER},
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
    {"outline_vs.glsl", GL_VERTEX_SHADER},
    {"dumpnorm_vs.glsl", GL_VERTEX_SHADER},
    {"dumpnorm_fs.glsl", GL_FRAGMENT_SHADER},
    {"quadpp_vs.glsl", GL_VERTEX_SHADER},
    {"convolute1_fs.glsl", GL_FRAGMENT_SHADER},
    {"amboccl_fs.glsl", GL_FRAGMENT_SHADER},
    {"scenefx_vs.glsl", GL_VERTEX_SHADER},
    {"scenefx_fs.glsl", GL_FRAGMENT_SHADER},
    {"beacon_fs.glsl", GL_FRAGMENT_SHADER},
    {"screenblur4_fs.glsl", GL_FRAGMENT_SHADER},
    {"test_tex_fs.glsl", GL_FRAGMENT_SHADER}
    //////////////////////////////////////////////////////////////////
  };

  shader_code_map_.clear();

  for(unsigned int i=0;i<sizeof(shader_list)/sizeof(ShaderListEntry);++i) {
    String shader_name(shader_list[i].file);
    bf::path shader_file(shader_dir / shader_name);

    if(!bf::exists(shader_file)){
      LOG_ERROR("not found: [" << shader_file.string() << "], cannot create shaders");
      continue;
    }

    bf::ifstream shader_file_stream(shader_file);
    String shader_code;
    String line;
    while (std::getline(shader_file_stream,line)) {
      shader_code += line + "\n";
    }

    GLuint shader_id;
    if(Shader::Compile(shader_name,shader_code,shader_list[i].type,shader_id)) {
      shader_code_map_[shader_name]=shader_id;
    } else {
      shader_code_map_[shader_name]=0;
    }
  }

  std::vector<GLuint> shader_program_list;
  GLuint shader_program_id;
  // basic shader
  shader_program_list.push_back(shader_code_map_["basic_vs.glsl"]);
  shader_program_list.push_back(shader_code_map_["basic_fs.glsl"]);
  if(Shader::Link(shader_program_list,"basic",shader_program_id)) {
    shader_program_map_["basic"]=shader_program_id;
  }
  // fraglight shader
  shader_program_list.clear();
  shader_program_list.push_back(shader_code_map_["fraglight_vs.glsl"]);
  shader_program_list.push_back(shader_code_map_["fraglight_fs.glsl"]);
  if(Shader::Link(shader_program_list,"fraglight",shader_program_id)) {
    shader_program_map_["fraglight"]=shader_program_id;
  }
  // basic hemisphere lighting shader
  shader_program_list.clear();
  shader_program_list.push_back(shader_code_map_["basic_hf_vs.glsl"]);
  shader_program_list.push_back(shader_code_map_["basic_fs.glsl"]);
  if(Shader::Link(shader_program_list,"hemilight",shader_program_id)) {
    shader_program_map_["hemilight"]=shader_program_id;
    // alias
    shader_program_map_["hf"]=shader_program_id;
  }
  // selfx shader
  shader_program_list.clear();
  shader_program_list.push_back(shader_code_map_["selfx_vs.glsl"]);
  shader_program_list.push_back(shader_code_map_["selfx_fs.glsl"]);
  if(Shader::Link(shader_program_list,"selfx",shader_program_id)) {
    shader_program_map_["selfx"]=shader_program_id;
  }
  // toon shader
  shader_program_list.clear();
  shader_program_list.push_back(shader_code_map_["toon_vs.glsl"]);
  shader_program_list.push_back(shader_code_map_["toon_fs.glsl"]);
  if(Shader::Link(shader_program_list,"toon1",shader_program_id)) {
    shader_program_map_["toon1"]=shader_program_id;
  }
  // toon2 shader
  shader_program_list.clear();
  shader_program_list.push_back(shader_code_map_["toon_vs.glsl"]);
  shader_program_list.push_back(shader_code_map_["toon2_fs.glsl"]);
  if(Shader::Link(shader_program_list,"toon2",shader_program_id)) {
    shader_program_map_["toon2"]=shader_program_id;
  }
  // line shader
  shader_program_list.clear();
  shader_program_list.push_back(shader_code_map_["basic_vs.glsl"]);
  shader_program_list.push_back(shader_code_map_["aaline_fs.glsl"]);
  if(Shader::Link(shader_program_list,"aaline",shader_program_id)) {
    shader_program_map_["aaline"]=shader_program_id;
  }
  // iso shader
  shader_program_list.clear();
  shader_program_list.push_back(shader_code_map_["iso_vs.glsl"]);
  shader_program_list.push_back(shader_code_map_["iso_fs.glsl"]);
  if(Shader::Link(shader_program_list,"iso",shader_program_id)) {
    shader_program_map_["iso"]=shader_program_id;
  }
  // fast sphere shader
  shader_program_list.clear();
  shader_program_list.push_back(shader_code_map_["fast_sphere_vs.glsl"]);
  shader_program_list.push_back(shader_code_map_["fast_sphere_fs.glsl"]);
  if(Shader::Link(shader_program_list,"fast_sphere",shader_program_id)) {
    shader_program_map_["fast_sphere"]=shader_program_id;
  }
  // outline shader
  shader_program_list.clear();
  shader_program_list.push_back(shader_code_map_["outline_vs.glsl"]);
  shader_program_list.push_back(shader_code_map_["basic_fs.glsl"]);
  if(Shader::Link(shader_program_list,"outline",shader_program_id)) {
    shader_program_map_["outline"]=shader_program_id;
  }
  // dumpnorm shader
  shader_program_list.clear();
  shader_program_list.push_back(shader_code_map_["dumpnorm_vs.glsl"]);
  shader_program_list.push_back(shader_code_map_["dumpnorm_fs.glsl"]);
  if(Shader::Link(shader_program_list,"dumpnorm",shader_program_id)) {
    shader_program_map_["dumpnorm"]=shader_program_id;
  }
  // convolute1 shader
  shader_program_list.clear();
  shader_program_list.push_back(shader_code_map_["quadpp_vs.glsl"]);
  shader_program_list.push_back(shader_code_map_["convolute1_fs.glsl"]);
  if(Shader::Link(shader_program_list,"convolute1",shader_program_id)) {
    shader_program_map_["convolute1"]=shader_program_id;
  }
  // amboccl shader
  shader_program_list.clear();
  shader_program_list.push_back(shader_code_map_["quadpp_vs.glsl"]);
  shader_program_list.push_back(shader_code_map_["amboccl_fs.glsl"]);
  if(Shader::Link(shader_program_list,"amboccl",shader_program_id)) {
    shader_program_map_["amboccl"]=shader_program_id;
  }
  // beacon shader
  shader_program_list.clear();
  shader_program_list.push_back(shader_code_map_["scenefx_vs.glsl"]);
  shader_program_list.push_back(shader_code_map_["beacon_fs.glsl"]);
  if(Shader::Link(shader_program_list,"beacon",shader_program_id)) {
    shader_program_map_["beacon"]=shader_program_id;
  }
  // scenefx shader
  shader_program_list.clear();
  shader_program_list.push_back(shader_code_map_["scenefx_vs.glsl"]);
  shader_program_list.push_back(shader_code_map_["scenefx_fs.glsl"]);
  if(Shader::Link(shader_program_list,"scenefx",shader_program_id)) {
    shader_program_map_["scenefx"]=shader_program_id;
  }
  // screen blur shader
  shader_program_list.clear();
  shader_program_list.push_back(shader_code_map_["quadpp_vs.glsl"]);
  shader_program_list.push_back(shader_code_map_["screenblur4_fs.glsl"]);
  if(Shader::Link(shader_program_list,"screenblur4",shader_program_id)) {
    shader_program_map_["screenblur4"]=shader_program_id;
  }
  // test tex shader
  shader_program_list.clear();
  shader_program_list.push_back(shader_code_map_["fraglight_vs.glsl"]);
  shader_program_list.push_back(shader_code_map_["test_tex_fs.glsl"]);
  if(Shader::Link(shader_program_list,"test_tex",shader_program_id)) {
    shader_program_map_["test_tex"]=shader_program_id;
  }

  valid_=true;
}

void Shader::Activate(const String& name)
{
  if(!OST_GL_VERSION_2_0) return;

  if(!name.empty()) {
    std::map<String, GLuint>::iterator it = shader_program_map_.find(name);
    if(it!=shader_program_map_.end()) {
      LOG_VERBOSE("switching to shader [" << name << "]");
      glUseProgram(it->second);
      current_program_=it->second;
      current_name_=name;

      UpdateState();
      return;

    } else {
      LOG_WARNING("shader program [" << name << "] not present");
      return;
    }
  }
  LOG_VERBOSE("switching to fixed pipeline");
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
  if(!OST_GL_VERSION_2_0) return false;
  return valid_;
}

bool Shader::IsActive() const
{
  return current_program_!=0;
}

void Shader::PushProgram()
{
  if(!OST_GL_VERSION_2_0) return;
  program_stack_.push(current_name_);
}

void Shader::PopProgram()
{
  if(!OST_GL_VERSION_2_0) return;
  if(!program_stack_.empty()) {
    current_name_ = program_stack_.top();
    program_stack_.pop();
    Activate(current_name_);
  }
}

void Shader::UpdateState()
{
  if(!OST_GL_VERSION_2_0) return;
  if(current_program_!=0) {
    // update all settings
    GLint result;
    glGetIntegerv(GL_LIGHTING,&result);
    LOG_TRACE("setting lighting flag to " << result);
    glUniform1i(glGetUniformLocation(current_program_,"lighting_flag"),result);
    GLboolean bresult;
    glGetBooleanv(GL_LIGHT_MODEL_TWO_SIDE,&bresult);
    LOG_TRACE("setting two_sided flag to " << bresult);
    glUniform1i(glGetUniformLocation(current_program_,"two_sided_flag"),bresult);
    glGetIntegerv(GL_FOG,&result);
    LOG_TRACE("setting fog flag to " << result);
    glUniform1i(glGetUniformLocation(current_program_,"fog_flag"),result);
    glDisable(GL_COLOR_MATERIAL);

  } else {
    glEnable(GL_COLOR_MATERIAL);
  }
}

}} // ns
