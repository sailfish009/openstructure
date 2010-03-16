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
#include <iostream>
#include <sstream>
#include <iomanip>

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <ost/log.hh>
#include <ost/conop/conop.hh>
#include <ost/io/io_exception.hh>
#include "entity_io_sdf_handler.hh"

namespace ost { namespace io {

using boost::format;

bool EntityIOSDFHandler::RequiresBuilder() const
{
  return false;
}

// import data from provided stream
void EntityIOSDFHandler::Import(mol::EntityHandle& ent, std::istream& instream)
{
  String line;
  mol::XCSEditor editor=ent.RequestXCSEditor(mol::BUFFERED_EDIT);
  while (std::getline(instream,line)) {
    ++line_num;

    if (line_num<=4) {
      ParseAndAddHeader(line, line_num, ent, editor);
    } else if (line_num<=atom_count_+4) {
      ParseAndAddAtom(line, line_num, ent, true, editor);
    } else if (line_num<=bond_count_+atom_count_+4) {
      ParseAndAddBond(line, line_num, ent, editor);
    } else if (boost::iequals(line.substr(0,2), "> ")) {
      // parse data items
      int data_header_start = line.find('<');
      String data_header = line.substr(data_header_start+1,line.rfind('>')-data_header_start-1);
      if(data_header.empty()) {
        String msg="Bad data line %d: Could not find data header";
        throw IOException(str(format(msg) % line_num));
      }
      String data_value="";
      while(std::getline(instream,line) && !boost::iequals(line, "")) {
        data_value.append(line);
      }
      curr_chain_.SetGenericStringProperty(data_header, data_value);
    } else if (boost::iequals(line, "$$$$")) {
      LOGN_MESSAGE("MOLECULE " << curr_chain_.GetName() << " (" << chain_count_ << ") added.")
      NextMolecule();
    }
  }

  LOGN_MESSAGE("imported " << chain_count_ << " chains, " << residue_count_ 
               << " residues, " << atom_count_ << " atoms");
}



void EntityIOSDFHandler::Import(mol::EntityHandle& ent,
                                const boost::filesystem::path& loc)
{
  ClearState();
  boost::filesystem::ifstream instream(loc);
  if(!instream) throw IOException("could not open "+loc.string());  
  this->Import(ent, instream);
}

void EntityIOSDFHandler::ClearState()
{
  curr_chain_=mol::ChainHandle();
  curr_residue_=mol::ResidueHandle();
  chain_count_=0;
  residue_count_=0;
  atom_count_=0;
  bond_count_=0;
  line_num=0;
}

void EntityIOSDFHandler::NextMolecule()
{
  residue_count_=0;
  atom_count_=0;
  bond_count_=0;
  line_num=0;
}

void EntityIOSDFHandler::ParseAndAddHeader(const String& line, int line_num,
                                           mol::EntityHandle& ent, mol::XCSEditor& editor)
{
  LOGN_TRACE( "line: [" << line << "]" );
  format chain_fmter("%05i_%s");
  switch(line_num)
  {
    case 1:  // title line
    {
      ++chain_count_;
      String s_title=line;
      String s_chain;
      chain_fmter % chain_count_ % boost::trim_copy(s_title);
      s_chain=chain_fmter.str();
      if(s_chain.empty()) {
        String msg="Bad molecule name line %d: Line is empty";
        throw IOException(str(format(msg) % line_num));
      }
      curr_chain_=editor.InsertChain(s_chain);
      LOGN_DUMP("new chain " << s_chain);

      mol::ResidueKey rkey=boost::trim_copy(s_title);
      mol::ResNum rnum(++residue_count_);
      curr_residue_=editor.AppendResidue(curr_chain_, rkey, rnum);
      LOGN_DUMP("new residue " << rkey << "(" << rnum << ")");
      break;
    }
    case 2:  // user information line
      break;
    case 3:  // comments line
      break;
    case 4:  // counts line
    {
      String s_anum=line.substr(0,3);
      try {
        atom_count_=boost::lexical_cast<int>(boost::trim_copy(s_anum));
      } catch(boost::bad_lexical_cast&) {
        String msg="Bad counts line %d: Can't convert number of atoms"
                   " '%s' to integral constant.";
        throw IOException(str(format(msg) % line_num % s_anum));
      }
      String s_bnum=line.substr(3,3);
      try {
        bond_count_=boost::lexical_cast<int>(boost::trim_copy(s_bnum));
      } catch(boost::bad_lexical_cast&) {
        String msg="Bad counts line %d: Can't convert number of bonds"
                   " '%s' to integral constant.";
        throw IOException(str(format(msg) % line_num % s_bnum));
      }
      break;
    }
  }
}

void EntityIOSDFHandler::ParseAndAddAtom(const String& line, int line_num,
                                         mol::EntityHandle& ent, bool hetatm,
                                         mol::XCSEditor& editor)
{

  LOGN_TRACE( "line: [" << line << "]" );

  if(line.length()<48 || line.length()>69) {
    String msg="Bad atom line %d: Not correct number of characters on the"
               " line: %i (should be between 48 and 69)";
    throw IOException(str(format(msg) % line_num % line.length()));
  }
  int anum = line_num-4;  // start at 1 on fifth line since first four lines are header
  String s_posx=line.substr(0,10);
  String s_posy=line.substr(10,10);
  String s_posz=line.substr(20,10);
  String s_ele=line.substr(31,3);
  String s_charge=line.substr(36,3);

  geom::Vec3 apos;
  try {
    apos=geom::Vec3(boost::lexical_cast<Real>(boost::trim_copy(s_posx)),
                    boost::lexical_cast<Real>(boost::trim_copy(s_posy)),
                    boost::lexical_cast<Real>(boost::trim_copy(s_posz)));
  } catch(boost::bad_lexical_cast&) {
    String msg="Bad atom line %d: Can't convert coordinates to "
               "floating point numbers";
    throw IOException(str(format(msg) % line_num));
  }

  String ele=boost::trim_copy(s_ele);
  String aname=boost::lexical_cast<String>(anum);

  mol::AtomProp aprop;
  aprop.element=ele;
  aprop.radius=conop::Conopology::Instance().GetDefaultAtomRadius(ele);
  aprop.mass=conop::Conopology::Instance().GetDefaultAtomMass(ele);
  aprop.is_hetatm=hetatm;
  
  try {
    aprop.charge=boost::lexical_cast<Real>(boost::trim_copy(s_charge));
    if(aprop.charge != 0) {
      aprop.charge=4-aprop.charge;
    } //4-sdf_charge=real_charge if not 0
  } catch(boost::bad_lexical_cast&) {
    String msg="Bad atom line %d: Can't convert charge"
               " '%s' to integral constant.";
    throw IOException(str(format(msg) % line_num % s_charge));
  }

  LOGN_DUMP("adding atom " << aname << " (" << s_ele << ") @" << apos);

  editor.InsertAtom(curr_residue_, aname,apos,aprop);
}


void EntityIOSDFHandler::ParseAndAddBond(const String& line, int line_num,
                                         mol::EntityHandle& ent, mol::XCSEditor& editor)
{

  LOGN_TRACE( "line: [" << line << "]" );

  if(line.length()<18 || line.length()>21) {
    String msg="Bad bond line %d: Not correct number of characters on the"
               " line: %i (should be between 18 and 21)";
    throw IOException(str(format(msg) % line_num % line.length()));
  }

  String s_first_name=line.substr(0,3);
  String s_second_name=line.substr(3,3);
  String s_type=line.substr(6,3);
  String first_name, second_name;
  unsigned char type;
  mol::BondHandle bond;
  
  first_name=boost::trim_copy(s_first_name);
  second_name=boost::trim_copy(s_second_name);

  try {
    type=boost::lexical_cast<int>(boost::trim_copy(s_type));
  } catch(boost::bad_lexical_cast&) {
    String msg="Bad bond line %d: Can't convert bond type number"
                " '%s' to integral constant.";
    throw IOException(str(format(msg) % line_num % s_type));
  }

  mol::AtomHandle first,second;

  first = ent.FindAtom(curr_chain_.GetName(), mol::ResNum(residue_count_), first_name);
  second = ent.FindAtom(curr_chain_.GetName(), mol::ResNum(residue_count_), second_name);

  if(first.IsValid() && second.IsValid()) {
    bond = editor.Connect(first, second);
    bond.SetBondOrder(type);
  } else {
    String msg="Bad bond line %d: Can't find the atom names '%s', '%s'"
               " in entity.";
    throw IOException(str(format(msg) % line_num % first % second));
  }

  LOGN_DUMP("adding bond " << s_first_name << " " << s_second_name << " (" << s_type << ") ");
}

namespace {

  using boost::format;
  
  class SDFAtomWriter : public mol::EntityVisitor {
    public:
      SDFAtomWriter(std::ostream& ostream, std::map<long, int>& atom_indices) 
      : ostr_(ostream), atom_indices_(atom_indices), counter_(0) {
	atom_indices_.clear();
      }
    private:
    public:
      virtual bool VisitAtom(const mol::AtomHandle& atom) {
        atom_indices_[atom.GetHashCode()] = ++counter_;
        ostr_ << format("%10.4f") % atom.GetPos()[0]
              << format("%10.4f") % atom.GetPos()[1]
              << format("%10.4f ") % atom.GetPos()[2]
              << format("%-3s") % atom.GetProp().element
              << " 0  0  0  0  0  0"
              << std::endl;
        return true;
      }  
    private:
      std::ostream&      ostr_;
      std::map<long, int>& atom_indices_;
      int counter_;
  };
  
  class SDFBondWriter : public mol::EntityVisitor {
  public:
    SDFBondWriter(std::ostream& ostream, std::map<long, int>& atom_indices) 
      : ostr_(ostream), atom_indices_(atom_indices), counter_(0) {
    }
  private:
  public:
    virtual bool VisitAtom(const mol::AtomHandle& atom) {
      counter_++;
      mol::AtomHandleList atoms = atom.GetBondPartners();
      mol::AtomHandleList::iterator atom_iter = atoms.begin();
      for(; atom_iter != atoms.end(); ++atom_iter) {
        int atom_index = atom_indices_.find((*atom_iter).GetHashCode())->second;
        if(atom_index > counter_) {
          int type = 1;
          mol::BondHandle bond = atom.FindBondToAtom(*atom_iter);
          if(bond.IsValid()) type = bond.GetBondOrder();
          ostr_ << format("%3i") % counter_
                << format("%3i") % atom_index
                << format("%3i") % type
                << "  0  0  0"
                << std::endl;
        }
      }
      return true;
    }
     
  private:
    std::ostream&      ostr_;
    std::map<long, int>& atom_indices_;
    int counter_;
  };

  class SDFWriter : public mol::EntityVisitor {
  public:
    SDFWriter(std::ostream& ostream)
      : ostr_(ostream), counter_(0) {
    }
  private:
  public:
    virtual bool VisitChain(const mol::ChainHandle& chain) {
      // print end of molecule line
      if(counter_ != 0) {
        ostr_ << "$$$$" << std::endl;
        counter_ = 0;
        atom_indices_.clear();
      }
      // print header lines
      ostr_ << chain.GetName() << std::endl;
      ostr_ << std::endl;
      ostr_ << std::endl;

      // print counts line
      ostr_ << format("%3d") % chain.GetAtomCount()
            << format("%3d") % chain.GetBondCount()
            << "  0  0  0  0  0  0  0  0999 V2000"
            << std::endl;
      
      // write atom block
      SDFAtomWriter atom_writer(ostr_, atom_indices_);
      mol::ChainHandle non_const_chain = chain;
      non_const_chain.Apply(atom_writer);
      
      // write bond block
      SDFBondWriter bond_writer(ostr_, atom_indices_);
      non_const_chain.Apply(bond_writer);
      
      // write property block
      //TODO: wirte property block
      ostr_ << "M  END" << std::endl;
      
      // write data block
      std::map<String,GenericPropertyValue> prop_map = non_const_chain.GetGenericPropertyMap();
      std::map<String,GenericPropertyValue>::iterator iter;
      for(iter = prop_map.begin(); iter != prop_map.end(); ++iter) {
        ostr_ << "> <" << (*iter).first << ">" << std::endl;
        ostr_ << (*iter).second << std::endl;
        ostr_ << std::endl;
      }
      
      // write molecule endline
      ostr_ << "$$$$" << std::endl;
      
      return true;
    }


    
  private:
    std::ostream&      ostr_;
    int                counter_;
    std::map<long,int> atom_indices_;
  };

}

void EntityIOSDFHandler::Export(const mol::EntityView& ent,
                                const boost::filesystem::path& loc) const {

  boost::filesystem::ofstream outfile(loc);
  if(!outfile) throw IOException("could not open "+loc.string()+" for writing");
  this->Export(ent, outfile);
}

// export data from entity view to provided stream
void EntityIOSDFHandler::Export(const mol::EntityView& ent, 
                    std::ostream& outstream) const
{
  SDFWriter writer(outstream);  
  mol::EntityView non_const_view = ent;
  non_const_view.Apply(writer);
}

namespace {

bool sdf_handler_is_responsible_for(const boost::filesystem::path& loc,
                                    const String& type) {
  if(type=="auto") {
    String ext=extension(loc);
    if(boost::iequals(ext,".sdf")) {
      return true;
    }

  } else if(type=="sdf") {
    return true;
  }

  return false;
}

}

bool EntityIOSDFHandler::ProvidesImport(const boost::filesystem::path& loc,
                                        const String& type)
{
  return sdf_handler_is_responsible_for(loc, type);
}

bool EntityIOSDFHandler::ProvidesExport(const boost::filesystem::path& loc,
                                        const String& type)
{
  return sdf_handler_is_responsible_for(loc, type);
}



mol::EntityHandle LoadSDF(const String& file_name) {

    mol::EntityHandle ent_handle=mol::CreateEntity();
    EntityIOSDFHandler ent_io;
    ent_io.Import(ent_handle,file_name);

    LOG_DUMP("running conopology" << std::endl);

    return ent_handle;
}

}} // ns

