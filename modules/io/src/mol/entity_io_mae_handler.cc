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
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>

#include <ctype.h>

#include <boost/iostreams/filter/gzip.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/regex.hpp>

#include <ost/log.hh>
#include <ost/conop/conop.hh>
#include <ost/mol/xcs_editor.hh>
#include <ost/profile.hh>

#include <ost/io/io_exception.hh>
#include <ost/io/swap_util.hh>

#include "entity_io_mae_handler.hh"

namespace ost { namespace io {

using boost::format;

  namespace {

    std::vector<std::string> tokenize(const std::string& line)
    {
      std::vector<std::string> nrvo;
      bool in_string=false;
      int p0=0;
      for(size_t p1=1;p1<line.size();++p1) {
        if(in_string) {
          if(line[p1]=='"') {
            in_string=false;
            nrvo.push_back(std::string(line.substr(p0,p1-p0)));
            ++p1;
            p0=p1;
            while(isspace(line[p1]) && p1<line.size()) {++p0;++p1;}
            --p0; --p1;
          }
        } else {
          if(isspace(line[p1])) {
            nrvo.push_back(std::string(line.substr(p0+1,p1-p0-1)));
            ++p1;
            p0=p1;
            while(isspace(line[p1]) && p1<line.size()) {++p0;++p1;}
            --p0; --p1;
          } else if(line[p1]=='"') {
            in_string=true;
            p0=p1+1;
          }
        }
      }
      return nrvo;
    }

  }

MAEReader::MAEReader(const boost::filesystem::path& loc):
  curr_chain_(),
  curr_residue_(),
  chain_count_(0),
  residue_count_(0),
  atom_count_(0),
  infile_(loc),
  in_()
{
  if (boost::iequals(".gz", boost::filesystem::extension(loc))) {
    in_.push(boost::iostreams::gzip_decompressor());    
  }
  in_.push(infile_);  
  if(!infile_) throw IOException("could not open "+loc.string());
}

void MAEReader::Import(mol::EntityHandle& ent)
{
  Profile profile_import("MAEReader::Import");

  static const boost::regex r_ct_block("f_m_ct +\\{");
  static const boost::regex r_atom_block("m_atom\\[.*\\] +\\{");
  static const boost::regex r_delim(":::");

  String line;
  bool in_ct_block=false;
  bool in_atom_block=false;
  bool parsing_atoms=false;
  std::vector<std::string> prop_list;
  int i_atom_name=-1;
  int i_atom_xpos=-1;
  int i_atom_ypos=-1;
  int i_atom_zpos=-1;
  int i_res_name=-1;
  int i_res_num=-1;
  int i_chain_name=-1;

  mol::XCSEditor editor=ent.EditXCS(mol::BUFFERED_EDIT);
  
  while(std::getline(in_,line)) {
    line = boost::trim_copy(line);
    if(in_ct_block) {
      if(in_atom_block) {
        if(parsing_atoms) {
          if(boost::regex_match(line,r_delim)) {
            LOG_TRACE( "stopping atom parsing" );
            parsing_atoms=false;
            prop_list.clear();
            i_atom_name=-1;
            i_atom_xpos=-1;
            i_atom_ypos=-1;
            i_atom_zpos=-1;
            i_res_name=-1;
            i_res_num=-1;
            i_chain_name=-1;
          } else {
            // parsing atom line
            std::vector<std::string> tokens=tokenize(line);
            for(size_t i=0;i<tokens.size();++i) {
              LOG_TRACE( "[" << tokens[i] << "] ");
            }
            LOG_TRACE("");

            add_atom(ent,editor,
                     i_atom_name>=0 && i_atom_name<(int)tokens.size() ? tokens[i_atom_name] : "X",
                     i_atom_xpos>=0 && i_atom_xpos<(int)tokens.size() ? tokens[i_atom_xpos] : "0",
                     i_atom_ypos>=0 && i_atom_ypos<(int)tokens.size() ? tokens[i_atom_ypos] : "0",
                     i_atom_zpos>=0 && i_atom_zpos<(int)tokens.size() ? tokens[i_atom_zpos] : "0",
                     i_res_name>=0 && i_res_name<(int)tokens.size() ? tokens[i_res_name] : "X",
                     i_res_num>=0 && i_res_num<(int)tokens.size() ? tokens[i_res_num] : "0",
                     i_chain_name>=0 && i_chain_name<(int)tokens.size() ? tokens[i_chain_name] : "X");
          }
        } else { // not parsing atoms
          if(boost::regex_match(line,r_delim)) {
            if(i_atom_name==-1 ||
               i_atom_xpos==-1 ||
               i_atom_ypos==-1 ||
               i_atom_zpos==-1 ||
               i_res_name==-1 ||
               i_res_num==-1) {
              throw IOException("missing atom prop");
            }
            LOG_TRACE( "starting atom parsing" );
            parsing_atoms=true;
          } else if(line[0]=='}') {
            LOG_TRACE( "exiting atom block" );
            in_atom_block=false;
          } else {
            // parsing property line
            if(line[0]!='#') {
              int pid=prop_list.size()+1;
              prop_list.push_back(line);
              LOG_TRACE( "found property '" << prop_list.back() << "' id=" << pid );
              if(line=="s_m_pdb_atom_name") i_atom_name=pid;
              else if(line=="r_m_x_coord") i_atom_xpos=pid;
              else if(line=="r_m_y_coord") i_atom_ypos=pid;
              else if(line=="r_m_z_coord") i_atom_zpos=pid;
              else if(line=="s_m_pdb_residue_name") i_res_name=pid;
              else if(line=="i_m_residue_number") i_res_num=pid;
              else if(line=="s_m_pdb_segment_name") i_chain_name=pid;
            }
          }
        }
      } else { // not in atom block
        if(boost::regex_match(line,r_atom_block)) {
          LOG_TRACE( "entering atom block" );
          in_atom_block=true;
        } else if(line[0]=='}') {
          LOG_TRACE( "exiting ct block" );
          in_ct_block=false;
        }
      }
    } else { // not in ct block
      if(boost::regex_match(line,r_ct_block)) {
        LOG_TRACE( "entering ct block" );
        in_ct_block=true;
      }
    }
  }

  LOG_INFO("imported " << chain_count_ << " chains, " << residue_count_
                << " residues, " << atom_count_ << " atoms");  
}


void MAEReader::add_atom(mol::EntityHandle ent,
                         mol::XCSEditor& editor,
                         const std::string& aname2, 
                         const std::string& s_axpos, 
                         const std::string& s_aypos, 
                         const std::string& s_azpos, 
                         const std::string& rname2,
                         const std::string& s_rnum,
                         const std::string& cname2)
{
  std::string aname=boost::trim_copy(aname2);
  std::string rname=boost::trim_copy(rname2);
  std::string cname=boost::trim_copy(cname2);
  std::string ele=aname.empty() ? "X" : aname.substr(0,1);
  if(isdigit(ele[0])) ele="H";

  int irnum = atoi(s_rnum.c_str());
  geom::Vec3 apos(atof(s_axpos.c_str()),
                  atof(s_aypos.c_str()),
                  atof(s_azpos.c_str()));

  mol::ResidueKey rkey(rname);
  
  mol::ResNum rnum(irnum);
  
  // determine chain and residue update
  bool update_chain=false;
  bool update_residue=false;
  if(!curr_chain_) {
    update_chain=true;
    update_residue=true;
  } else if(curr_chain_.GetName()!=cname) {
    update_chain=true;
    update_residue=true;
  }

  if(!curr_residue_) {
    update_residue=true;
  } else if(curr_residue_.GetNumber()!=rnum) {
    update_residue=true;
  }

  if(update_chain) {  
    if (!(curr_chain_=ent.FindChain(cname))) {
      curr_chain_=editor.InsertChain(cname);
      LOG_TRACE("new chain " << curr_chain_);      
      ++chain_count_;      
    } else {
      LOG_TRACE("old chain " << curr_chain_);      
    }
  }

  if(update_residue) {
    if (!(curr_residue_=curr_chain_.FindResidue(rnum))) {
      curr_residue_=editor.AppendResidue(curr_chain_, rkey, rnum);
      assert(curr_residue_.IsValid());
      LOG_TRACE(" new residue " << curr_residue_);
      ++residue_count_;
    } else {
      LOG_TRACE(" old residue " << curr_residue_);
    }
  }

  // finally add atom
  LOG_TRACE("  atom " << aname << " (" << ele << ") @" << apos);
  mol::AtomHandle ah = editor.InsertAtom(curr_residue_, aname, apos, ele);
}
  



bool EntityIOMAEHandler::RequiresBuilder() const
{
  return true;
}

void EntityIOMAEHandler::Import(mol::EntityHandle& ent,
                                const boost::filesystem::path& loc)
{
  MAEReader reader(loc);
  reader.Import(ent);
}

void EntityIOMAEHandler::Export(const mol::EntityView& ent,
                                const boost::filesystem::path& loc) const
{
}

namespace {

bool mae_handler_is_responsible_for(const boost::filesystem::path& loc,
                                    const String& type) {
  if(type=="auto") {
    String match_suf_string=loc.string();
    std::transform(match_suf_string.begin(),match_suf_string.end(),match_suf_string.begin(),tolower);
    if( detail::FilenameEndsWith(match_suf_string,".mae") || detail::FilenameEndsWith(match_suf_string,".mae.gz") ) {
      return true;
    }
  } else if(type=="mae") {
    return true;
  }

  return false;
}

}

bool EntityIOMAEHandler::ProvidesImport(const boost::filesystem::path& loc,
                                        const String& type)
{
  return mae_handler_is_responsible_for(loc, type);
}

bool EntityIOMAEHandler::ProvidesExport(const boost::filesystem::path& loc,
                                        const String& type)
{
  return false;
}

mol::EntityHandle LoadMAE(const String& file_name) 
{
  Profile profile_load("LoadMAE");
  conop::BuilderP builder = conop::Conopology::Instance().GetBuilder();  
  MAEReader reader(file_name);
  mol::EntityHandle ent=mol::CreateEntity();
  mol::XCSEditor editor=ent.EditXCS(mol::BUFFERED_EDIT);
  reader.Import(ent);
  conop::Conopology::Instance().ConnectAll(builder,ent);    
  return ent;
}


void EntityIOMAEHandler::Import(mol::EntityHandle& ent, 
                                std::istream& stream)
{
  throw IOException("MAE format does not support import from stream");
}


void EntityIOMAEHandler::Export(const mol::EntityView& ent, 
                                std::ostream& stream) const
{
  throw IOException("MAE format does not support export to stream");
}

}} // ns
