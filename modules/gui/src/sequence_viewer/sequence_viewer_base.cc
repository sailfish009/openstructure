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
#include "sequence_viewer_base.hh"
#include "sequence_search_bar.hh"
#include <QShortcut>
#include <QVBoxLayout>
#include <QPainter>

namespace ost { namespace gui {

SequenceViewerBase::SequenceViewerBase(QWidget* parent):
  Widget(NULL, parent),  style_(DENSE), sel_mode_(ROW),
  scene_(new SequenceScene(this)), search_bar_(new SequenceSearchBar(this)),
  view_(new QGraphicsView(NULL, this))
{
  QVBoxLayout* l=new QVBoxLayout;
  this->setLayout(l);
  l->setMargin(0);
  l->setSpacing(0);
  view_->setScene(scene_);
  l->addWidget(search_bar_, 0);  
  l->addWidget(view_, 1);  
  view_->setScene(scene_);
  view_->setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing);
  view_->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);  
  QShortcut* shortcut=new QShortcut(QKeySequence(tr("Ctrl+F")), this);  
  connect(shortcut, SIGNAL(activated()), this, SLOT(FindInSequence()));    
  connect(search_bar_, SIGNAL(Changed(const QString&, bool, int)), this,
          SLOT(OnSearchBarUpdate(const QString&, bool, int)));  
  search_bar_->hide();
  view_->setAlignment(Qt::AlignLeft|Qt::AlignTop);
}

namespace {
  
size_t update_select_state(seq::SequenceHandle seq, const String& subject,
                           std::vector<bool>& sel_state)
{
  size_t pos=0;
  size_t first=String::npos;
  String seq_str=seq.GetString();
  while ((pos=seq_str.find(subject, pos))!=String::npos) {
    if (first==String::npos) {
      first=pos;
    }
    std::fill_n(sel_state.begin()+pos, subject.length(), true);
    pos+=subject.length();
  }
  return first;
}


void set_sel(std::vector<bool>& selected, SequenceItem* item)
{
  std::vector<bool>::iterator c=selected.begin();
  item->ClearSelection();
  while (c!=selected.end()) {
    std::vector<bool>::iterator e=c;
    while ((*(++e))==*c && e!=selected.end()) {}
    if (*c) {
      // this is really ugly!
      item->BeginSelectionChange();      
      item->Select(c-selected.begin(), e-selected.begin(), false);
    }
    c=e;
  }
  item->EndSelectionChange();
}

}

void SequenceViewerBase::FindInSequence()
{
  if (scene_->GetSequences().empty()) {
    return;
  }
  search_bar_->Show(scene_->GetSequences());
}

int SequenceViewerBase::GetLongestSequenceLength() const
{
  std::vector<SequenceItem*>::iterator i=scene_->GetSequences().begin(),
                                       e=scene_->GetSequences().end();  
  int longest=0;
  for (; i!=e; ++i) {
    longest=std::max(longest, (*i)->GetSequence().GetLength());
  }
  return longest;
}

void SequenceViewerBase::SelectColumns(std::vector<bool>& selected_cols)
{
  std::vector<SequenceItem*>::iterator i=scene_->GetSequences().begin(),
                                       e=scene_->GetSequences().end();
  for (; i!=e; ++i) {
    set_sel(selected_cols, *i);
  }
}

void SequenceViewerBase::SelectColumns(SequenceItem* seq_item,
                                       std::vector<bool>& selected_cols)
{
  set_sel(selected_cols, seq_item);
}

void SequenceViewerBase::ClearSelection()
{
  std::vector<SequenceItem*>::iterator i=scene_->GetSequences().begin(),
                                       e=scene_->GetSequences().end();
  for (; i!=e; ++i) {
    (*i)->ClearSelection();
  }
}

void SequenceViewerBase::OnSearchBarUpdate(const QString& subject, 
                                           bool search_in_all, int seq_id)
{
  String std_subj=subject.toStdString();
  this->ClearSelection();  
  if (scene_->GetSequences().empty() || (seq_id==-1 && !search_in_all)) {
    return;
  }  
  if (subject.size()==0) {
    return;
  }  
  
  std::vector<bool> selected;
  selected.resize(GetLongestSequenceLength(), false);
  /// \todo implement selection for SelMode==COLUMN
  std::pair<int, size_t> first(0, String::npos);
  if (search_in_all) {
    for (size_t i=0; i<scene_->GetSequences().size(); ++i) {
      (scene_->GetSequences())[i]->ClearSelection();
      seq::SequenceHandle s=(scene_->GetSequences())[i]->GetSequence();
      size_t pos=update_select_state(s, std_subj, selected);
      if (pos!=String::npos && first.second==String::npos) {
        first.second=pos;
        first.first=i;
      }
      if (sel_mode_==ROW) {
        set_sel(selected, (scene_->GetSequences())[i]); 
        std::fill_n(selected.begin(), selected.size(), false);        
      }
    }
  } else {
    first.first=seq_id;
    SequenceItem* s=(scene_->GetSequences())[seq_id];
    size_t pos=update_select_state(s->GetSequence(), std_subj, selected);
    if (pos!=String::npos) {
      first.second=pos;
    }
    if (sel_mode_==ROW) {
      set_sel(selected, s);
    }
  } 
  if (sel_mode_==COLUMN) {
    this->SelectColumns(selected);
  }
  if (first.second!=String::npos) {
    QRectF r=(scene_->GetSequences()[first.first]->GetCharBounds(first.second));

    view_->ensureVisible(r);
  }
  scene_->update();
}

void SequenceViewerBase::SetDisplayStyle(Style style)
{
  style_=style;
  for (std::vector<SequenceItem*>::iterator i=scene_->GetSequences().begin(),
       e=scene_->GetSequences().end(); i!=e; ++i) {
    (*i)->SetShowSecStructure(style_==LOOSE);
  }
  scene_->RepackSequences();
}

void SequenceViewerBase::SetSelMode(SelMode mode)
{
  sel_mode_=mode;
}

SequenceViewerBase::SelMode SequenceViewerBase::GetSelMode() const
{
  return sel_mode_;
}

SequenceViewerBase::Style SequenceViewerBase::GetDisplayStyle() const
{
  return style_;
}

void SequenceViewerBase::AddSequence(SequenceItem* seq)
{
  seq->SetShowSecStructure(style_==LOOSE);
  scene_->AddSequence(seq);
  QRectF rect=scene_->itemsBoundingRect();
  view_->setSceneRect(QRectF(0, 0, +rect.left()+rect.width(), 
                             rect.height()+rect.top()));
}

void SequenceViewerBase::RemoveSequence(SequenceItem* seq)
{
  scene_->RemoveSequence(seq);
}

void SequenceViewerBase::Clear()
{

}

bool SequenceViewerBase::Restore(const QString& prefix)
{
  return true;
}

bool SequenceViewerBase::Save(const QString& prefix)
{
  return true;
}

}}
