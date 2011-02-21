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
#ifndef OST_GFX_SHADER_HH
#define OST_GFX_SHADER_HH

/*
  GLSL wrapper and organization

  Author: Ansgar Philippsen
*/

#include <string>
#include <map>
#include <stack>
#include <vector>

#include <ost/gfx/module_config.hh>
#include "glext_include.hh"

namespace ost { namespace gfx {

class DLLEXPORT_OST_GFX Shader {
public:
  // singleton implementation
  static Shader& Instance();

  static bool Compile(std::string shader_name, std::string shader_code,
                      GLenum shader_type, GLuint& shader_id);

  static bool Link(const std::vector<GLuint>& code_list, std::string pr_name, GLuint& shader_pr);

  bool Add(const std::string& name, const std::string& vs_code, const std::string& fs_code);

  void PreGLInit();
  void Setup();
  void Activate(const String& name);

  bool IsValid() const;
  bool IsActive() const;

  GLuint GetCurrentProgram() const;
  String GetCurrentName() const;

  void PushProgram();
  void PopProgram();

  void UpdateState();
private:
  Shader();

  bool valid_;
  GLuint current_program_;
  String current_name_;

  std::stack<String> program_stack_;

  std::map<String,GLuint> shader_code_map_;
  std::map<String,GLuint> shader_program_map_;
};

}} // ns

#endif
