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
#include <QMenu>

#include <ost/dyn_cast.hh>
#include <ost/mol/view_op.hh>

#include <ost/gfx/entity.hh>
#include <ost/gfx/scene.hh>

#include <ost/gui/widget_registry.hh>
#include <ost/gui/gosty_app.hh>

#include "sequence_viewer.hh"


namespace ost { namespace gui {

class SequenceViewerFactory: public WidgetFactory {
public:
	SequenceViewerFactory() :
    WidgetFactory("ost::gui::SequenceViewer", "Sequence Viewer") {
  }

  virtual Widget* Create(QWidget* parent) {
    return GostyApp::Instance()->GetSequenceViewer();
  }
};

OST_REGISTER_WIDGET(SequenceViewer, SequenceViewerFactory);

SequenceViewer::SequenceViewer(QWidget* parent):
  SequenceViewerBase(parent), we_are_changing_the_selection_(false)
{
  gfx::Scene::Instance().AttachObserver(this);
  this->SetDisplayStyle(SequenceViewer::LOOSE);
}

SequenceViewer::~SequenceViewer()
{
  gfx::Scene::Instance().DetachObserver(this);
}

void SequenceViewer::NodeAdded(const gfx::GfxNodeP& n)
{
  if (gfx::EntityP o=dyn_cast<gfx::Entity>(n)) {
    // extract all chains 
    mol::EntityView v=o->GetView();
    for (mol::ChainViewList::const_iterator c=v.GetChainList().begin(),
         e1=v.GetChainList().end(); c!=e1; ++c) {
      mol::ChainView chain=*c;
      String seq_str;
      seq_str.reserve(chain.GetResidueCount());
      for (mol::ResidueViewList::const_iterator r=chain.GetResidueList().begin(),
           e2=chain.GetResidueList().end(); r!=e2; ++r) {
        mol::ResidueView res=*r;
        seq_str.append(1, res.GetOneLetterCode());
      }
      if (seq_str.empty()) {
        continue;
      }      
      String name=o->GetName();
      if (chain.GetName()!="" && chain.GetName()!=" ") {
        name+=" ("+chain.GetName()+")";
      }
      seq::SequenceHandle seq=seq::CreateSequence(name, seq_str);
      mol::EntityView v_one_chain=v.GetHandle().CreateEmptyView();
      v_one_chain.AddChain(chain, mol::ViewAddFlag::INCLUDE_ALL);
      seq.AttachView(v_one_chain);
      SequenceItem* item=new SequenceItem(seq);
      connect(item, SIGNAL(SelectionChanged(SequenceItem*)),
              this, SLOT(ItemSelectionChanged(SequenceItem*)));
      this->AddSequence(item);
      obj_map_.insert(std::make_pair(item, o));      
    }
  }
}

void SequenceViewer::ItemSelectionChanged(SequenceItem* item)
{
  if (we_are_changing_the_selection_==true) {
    return;
  }
  we_are_changing_the_selection_=true;
  // map sequence item back to graphical object
  if (gfx::GfxObjP p=this->GfxObjForSeqItem(item)) {
    gfx::EntityP ec=dyn_cast<gfx::Entity>(p);
    seq::SequenceHandle seq=item->GetSequence();
    mol::EntityView att_v=seq.GetAttachedView();
    if (!att_v) {
      we_are_changing_the_selection_=false;      
      return;
    }
    const SequenceItem::Selection& sel=item->GetSelection();
    // reset selection of "our" chain
    mol::EntityView sel_v=ec->GetSelection();
    if (mol::ChainView c=sel_v.FindChain(att_v.GetChainList().front().GetHandle())) {
      sel_v.RemoveChain(c);
    }
    if (sel.empty()) {
      try {
        ec->SetSelection(sel_v);        
      } catch(...) {
      }
      we_are_changing_the_selection_=false;      
      return;
    }   
    for (SequenceItem::Selection::const_iterator j=sel.begin(),
         e=sel.end(); j!=e; ++j) {
      for (size_t k=j->Loc; k<j->End(); ++k) {
        if (int(k)>seq.GetLength() || seq.GetOneLetterCode(k)=='-') {
          continue;
        }
        if (mol::ResidueView rv=seq.GetResidue(k)) {
          sel_v.AddResidue(rv, mol::ViewAddFlag::INCLUDE_ATOMS);
        }
      }
    }
    sel_v.AddAllInclusiveBonds();
    try {
      ec->SetSelection(sel_v);      
    } catch(...) {
    }

  }
  we_are_changing_the_selection_=false;
}

void SequenceViewer::NodeRemoved(const gfx::GfxNodeP& node)
{
  if (gfx::EntityP o=dyn_cast<gfx::Entity>(node)) {
    SequenceItemList seq_items=this->SeqItemsForGfxObj(o);
    for (SequenceItemList::iterator i=seq_items.begin(),
     e=seq_items.end(); i!=e; ++i) {
    SequenceItem* seq_item=*i;
    this->RemoveSequence(seq_item);
    delete seq_item;
    }
  }
}

void SequenceViewer::SelectionChanged(const gfx::GfxObjP& o, 
                                      const mol::EntityView& view)
{
  if (we_are_changing_the_selection_) {
    return;
  }
  we_are_changing_the_selection_=true;
  gfx::EntityP ec=dyn_cast<gfx::Entity>(o);
  

  std::vector<bool> selected_cols;
  selected_cols.resize(this->GetLongestSequenceLength(), false);

  // get affected sequence items
  SequenceItemList seq_items=this->SeqItemsForGfxObj(o);

  for (SequenceItemList::iterator i=seq_items.begin(), 
       e=seq_items.end(); i!=e; ++i) {
    SequenceItem* seq_item=*i;

    if(view.GetChainCount()==0){
        this->SelectColumns(seq_item, selected_cols);
    }
    else
    {
    seq::SequenceHandle seq=seq_item->GetSequence();
    mol::ChainView dst_chain=(seq.GetAttachedView().GetChainList())[0];
    if (mol::ChainView src_chain=view.FindChain(dst_chain.GetName())) {
        // for each residue in the selection deduce index in sequence
        for (mol::ResidueViewList::const_iterator j=src_chain.GetResidueList().begin(),
           e2=src_chain.GetResidueList().end(); j!=e2; ++j) {
        mol::ResidueView dst_res=dst_chain.FindResidue(j->GetHandle());
        assert(dst_res.IsValid());
        int p=seq.GetPos(dst_res.GetIndex());
        assert(p>=0 && p<=seq.GetLength());
        selected_cols[p]=true;
        }
        this->SelectColumns(seq_item, selected_cols);
        std::fill(selected_cols.begin(), selected_cols.end(), false);
      }
    }
  }
  we_are_changing_the_selection_=false;  
}

SequenceItemList SequenceViewer::SeqItemsForGfxObj(const gfx::GfxObjP& obj)
{
  SequenceItemList seq_items;
  gfx::EntityP ec=dyn_cast<gfx::Entity>(obj);
  std::map<SequenceItem*, gfx::EntityP>::iterator i,e;
  for (i=obj_map_.begin(), e=obj_map_.end(); i!=e; ++i) {
    if (i->second==ec) {
      seq_items.push_back(i->first);
    }
  }
  return seq_items;
}

gfx::GfxObjP SequenceViewer::GfxObjForSeqItem(SequenceItem* item)
{
  std::map<SequenceItem*, gfx::EntityP>::iterator i=obj_map_.find(item);
  return (i!=obj_map_.end()) ? i->second : gfx::EntityP();
}

}}
