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
  Author: Stefan Scheuber
 */


#include <QtGui>

#include <ost/mol/mol.hh>
#include <ost/mol/view_op.hh>

#include <ost/seq/aligned_region.hh>

#include "sequence_row.hh"
#include "secstr_row.hh"

#include "painter.hh"
#include "background_painter.hh"
#include "seq_secstr_painter.hh"
#include "seq_selection_painter.hh"
#include "seq_text_painter.hh"

#include "alignment_view_object.hh"

namespace ost { namespace gui {

namespace {
QMap<QString,int> GetGroupMap(){
  QMap<QString,int> map;
  map["G"]=1;
  map["A"]=1;
  map["V"]=1;
  map["L"]=1;
  map["I"]=1;
  map["F"]=2;
  map["Y"]=2;
  map["W"]=2;
  map["C"]=3;
  map["M"]=3;
  map["S"]=4;
  map["T"]=4;
  map["K"]=5;
  map["R"]=5;
  map["H"]=5;
  map["D"]=6;
  map["E"]=6;
  map["N"]=6;
  map["Q"]=6;
  map["P"]=7;
  return map;
}

QColor GetColor(int cons){
  int color = 255 - int((float(cons) / 100) * 255);
  return QColor(color,color,color);
}

QColor GetForeGroundColor(QColor background){
  if(background == Qt::transparent){
    return Qt::black;
  }
  int gray = 255 - background.red();
  return QColor(gray,gray,gray);
}

}


QMap<QString,int> AlignmentViewObject::group_map_ = GetGroupMap();

AlignmentViewObject::AlignmentViewObject(const seq::AlignmentHandle& alignment, QObject* parent): SequenceViewObject(parent), alignment_(alignment)
{
  for(int i=0; i<alignment.GetCount(); i++){
    seq::SequenceHandle seq_handle = alignment.GetSequence(i).Copy();
    this->AddSequence(seq_handle, seq_handle.GetName().c_str());
  }

  for(int j=0; j<alignment.GetLength(); j++){
    int group = 0;
    QString element = "";
    for(int i=0; i<alignment.GetCount();i++){
      QString code = QString(alignment.GetOneLetterCode(i,j));
      if(element.size()==0){
        element = code;
      }
      else if (element != code){
        element = "  ";
      }
      if(group_map_.contains(code) && group>=0){
        if(group == 0) {
            group = group_map_[code];
        }
        else if(group_map_[code] != group){
          group = -1;
        }
      }
      else{
        group = -1;
      }

    }
    if(element.size()==1){
      conservation_[j] = GetColor(100);
    }
    else if(group > 0){
      conservation_[j] = GetColor(70);
    }
    else{
      conservation_[j] = Qt::transparent;
    }
  }
}


QVariant AlignmentViewObject::GetData(int row, int column, int role)
{
  if(column > 0 && column <= alignment_.GetLength()){
    if(role == Qt::UserRole+3 ){
      if(column -1 < conservation_.size()){
        return QVariant(conservation_[column-1]);
      }
        return QVariant(Qt::transparent);
    }

    if(role == Qt::ForegroundRole){
      if(column -1 < conservation_.size()){
        return QVariant(GetForeGroundColor(conservation_[column-1]));
      }
        return QVariant(Qt::transparent);
    }

  }

  return BaseViewObject::GetData(row,column,role);
}

}}
