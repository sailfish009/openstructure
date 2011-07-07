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
  Author: Marco Biasini
 */
#include <cassert>
#include <sstream>
#include <ost/io/io_exception.hh>
#include <ost/io/mol/star_parser.hh>

namespace ost { namespace io {

StarParser::StarParser(std::istream& stream):
  stream_(stream), line_num_(0), has_current_line_(false), current_line_()
{
  
}

bool StarParser::SplitLine(const StringRef& line, 
                            std::vector<StringRef>& parts, bool clear)
{
  if (clear) {
    parts.clear();    
  }
  const char* s=line.begin();
  while (s!=line.end()) {
    while (isspace(*s)) {
      ++s;
      if (s==line.end()) {
        return true;
      }
    }
    if (*s=='\'' || *s=='"') {
      char delim=*s;
      const char* start=++s;
      while (s!=line.end()) {
        ++s;
        if (delim==*(s-1) && (s==line.end() || isspace(*s))) {
          break;
        }
      }
      parts.push_back(StringRef(start, s-start-1));
    } else {
      const char* start=s;
      while (s!=line.end() && !isspace(*s)) {
        ++s;
      }
      if (s-start) {       
        parts.push_back(StringRef(start, s-start));
      } else {
        return false;
      }
    }
  }
  return true;
}

bool StarParser::ParseMultilineValue(String& value, bool skip)
{
  std::stringstream valuebuf;
  StringRef line;
  bool r=this->GetLine(line);
  assert(r);r=r;
  valuebuf << line.substr(1);
  while (this->NextLine(line)) {
    StringRef tline=line.rtrim();
    if (!tline.empty() && tline[0]==';') {
      break;
    }
    if (!skip) {
      valuebuf << tline << "\n";      
    }
  }  
  if (!skip) {
    value=valuebuf.str();
  }
  this->ConsumeLine();
  return true;
}

void StarParser::ParseLoop()
{
  StringRef line;
  int prefix_len=0;
  int leave=false;
  StarLoopDesc header;
  this->ConsumeLine();
  while (this->GetLine(line)) {
    StringRef tline=line.rtrim();
    if (tline.empty()) {
      this->ConsumeLine();
      continue;
    }
    switch(tline[0]) {
      case '_':
        if (prefix_len==0) {
          prefix_len=tline.find('.')-tline.begin();
          header.SetCategory(tline.substr(1, prefix_len-1));
        } else {
          assert(tline[prefix_len]=='.');
        }
        header.Add(tline.substr(prefix_len+1));
        this->ConsumeLine();
        break;
      case '#':
        this->ConsumeLine();
        break;        
      default:
        leave=true;
        break;
    }
    if (leave) {
      break;
    }
  }
  bool process_rows=this->OnBeginLoop(header);
  std::vector<StringRef> columns;
  std::vector<String> tmp_values;
  // optimized for the common case where all values are present on the same 
  // line.
  while (this->GetLine(line)) {
    StringRef tline=line.rtrim();
    if (tline.empty()) {
      this->ConsumeLine();
      continue;
    }
    switch (tline[0]) {
      case '#':
        this->ConsumeLine();
        break;
      case ';':
        if (process_rows) {
          tmp_values.push_back(String());
          bool r=this->ParseMultilineValue(tmp_values.back());
          assert(r);r=r;
          columns.push_back(StringRef(tmp_values.back().data(), 
                                      tmp_values.back().length()).trim());
          if (columns.size()==header.GetSize()) {
            this->OnDataRow(header, columns);
            tmp_values.clear();
            columns.clear();
          }          
        } else {
          String s;
          bool r=this->ParseMultilineValue(s, true);
          assert(r);r=r;
        }
        break;
        case 'd':
          if (tline.length()>=5 && StringRef("data_", 5)==tline.substr(0, 5)) {
            return;
          }
      case 'l':
        if (StringRef("loop_", 5)==tline) {
          return;
        }
      default:
        if (process_rows) {
          int before=columns.size();
          bool r=StarParser::SplitLine(tline, columns, false);
          assert(r);r=r;
          if (columns.size()==header.GetSize()) {
            this->OnDataRow(header, columns);            
            tmp_values.clear();
            columns.clear();
          } else {
            tmp_values.push_back(tline.str());
            const char* d=tmp_values.back().c_str();
            for (size_t i=std::max(before-1, 0); i<columns.size(); ++i) {
              columns[i]=StringRef(d+(columns[i].begin()-tline.begin()), 
                                   columns[i].size());
            }
          }
        }
        this->ConsumeLine();
        break;
    }    
  }
  if (process_rows) {
    this->OnEndLoop();    
  }
}

void StarParser::ParseDataItem()
{
  StringRef line;
  bool r=this->GetLine(line);
  assert(r);r=r;
  // optimize for common case when name/value are present on the same line. 
  // We don't have to allocate any additional strings in that case.
  std::vector<StringRef> nv;
  StarParser::SplitLine(line, nv);
  if (nv.size()==1) {
    // remember identifier. 
    String identifier=line.str();
    String value;

    while (this->NextLine(line)) {
      StringRef tline=line.rtrim();
      if (tline.empty()) {
        this->ConsumeLine();
        continue;
      }
      if (tline[0]==';') {
        this->ParseMultilineValue(value);
      } else {
        value=line.str();
        StarParser::SplitLine(StringRef(value.data(), value.length()), 
                              nv, false);
        if (nv.size()!=2) {
          std::cout << "ERROR:" << line_num_ << ":" << tline << std::endl;
        }
        assert(nv.size()==2);
        this->ConsumeLine();        
      }
      break;
    }
    size_t i=identifier.find('.');
    assert(i!=String::npos);
    StringRef id_ref(identifier.data(), identifier.size());
    StringRef cat=StringRef(id_ref.substr(1, i-1));
    StringRef name=id_ref.substr(i+1);
    StringRef value_ref=StringRef(value.data(),
                                  value.length()).trim();
    StarDataItem data_item(cat, name, value_ref);
    this->OnDataItem(data_item);
  } else {
    if (nv.size()!=2) {
      std::cout << "ERROR:" << line_num_ << ":" << line << std::endl;
    }    
    assert(nv.size()==2);
    StringRef::const_iterator i=nv[0].find('.');
    assert(i!=nv[0].end());
    StringRef cat=nv[0].substr(1, i-nv[0].begin()-1);
    StringRef name=nv[0].substr(i-nv[0].begin()+1);
    StarDataItem data_item(cat, name, nv[1]);
    this->OnDataItem(data_item);
    this->ConsumeLine();
  }
  
}

void StarParser::ParseData()
{
  StringRef line;
  bool r=this->GetLine(line);
  assert(r);r=r;
  StringRef data_id=line.rtrim().substr(5);
  bool skip=!this->OnBeginData(data_id);
  this->ConsumeLine();
  while (this->GetLine(line)) {
    StringRef tline=line.rtrim();
    if (tline.empty()) {
      this->ConsumeLine();
      continue;
    }    
    switch (tline[0]) {
      case '_':
        if (skip) {
          this->ConsumeLine();
        } else {
          this->ParseDataItem();          
        }
        break;
      case 'd':
        if (tline.length()>=5 && StringRef("data_", 5)==tline.substr(0, 5)) {
          this->OnEndData();
          return;
        }
      case ';':
        if (skip) {
          String s;
          bool r=this->ParseMultilineValue(s, true);
          assert(r);r=r;
        } else {
          assert(0 && "';' when skip==false");
        }
        break;
      case 'l':
        if (tline==StringRef("loop_", 5)) {
          this->ParseLoop();
          break;
        }
        this->DiagnoseUnknown();
        break;
      case '#':
        // consume comment
        this->ConsumeLine();
        break;
      default:
        return;
    }
  }
  this->OnEndData();
}

void StarParser::DiagnoseUnknown()
{
  std::stringstream ss;
  StringRef line;
  bool r=this->GetLine(line);
  assert(r);r=r;
  
  ss << "unknown control structure '"<< line.rtrim() << "' on line " 
     << line_num_ << "." << std::endl;
  throw IOException(ss.str());
}

void StarParser::ParseGlobal()
{
  assert(0 && "unhandled global section");
}

void StarParser::Parse()
{
  StringRef line;
  std::stringstream ss;
  while (this->GetLine(line)) {
    StringRef tline=line.rtrim();
    if (tline.empty()) {
      this->ConsumeLine();
      continue;
    }
    switch (tline[0]) {
      case 'd':
        if (tline.length()>=5 && StringRef("data_", 5)==tline.substr(0, 5)) {
          this->ParseData();
        }
        break;
      case 'g':
        if (tline.length()>=7 && StringRef("global_", 7)==tline.substr(0, 7)) {
          this->ParseGlobal();
        }
        break;
      case '#':
        // consume comment
        this->ConsumeLine();
        break;
      default:
        ss << "Missing 'data_' control structure." << std::endl;
        throw IOException(ss.str());
        break;
    }
  }
}

}}
