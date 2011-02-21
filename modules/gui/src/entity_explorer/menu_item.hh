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
#ifndef OST_GUI_MENU_ITEM_HH
#define OST_GUI_MENU_ITEM_HH

#include <boost/checked_delete.hpp>

#include <ost/dyn_cast.hh>
#include <ost/mol/mol.hh>

#include <ost/gfx/gfx_object.hh>
#include <ost/gfx/scene.hh>
#include <ost/gfx/entity.hh>

// Qt includes must come last
#include <QVariant>

namespace ost { namespace gui {

class DLLEXPORT_OST_GUI MenuItem {
public:
  MenuItem(MenuItem* parent)
    :parent_(parent) {    
  }
  
  MenuItem* Parent() {
    return parent_;
  }
  
  std::vector<MenuItem*>& Children() {
    return children_;
  }
  
  virtual ~MenuItem() {
    std::for_each(children_.begin(), children_.end(), 
                  boost::checked_deleter<MenuItem>());
  }
  virtual int Row() const {
    return 0;
  }
  virtual QVariant Data() {return QVariant();};
private:
  MenuItem*               parent_;
  std::vector<MenuItem*>  children_;
};

class DLLEXPORT_OST_GUI AtomViewMenuItem : public MenuItem {
public:
 AtomViewMenuItem(MenuItem* parent, const mol::AtomView& atom)
   : MenuItem(parent), atom_(atom) {
 }
 
 mol::AtomView& Atom() {
   return atom_;
 }
 
 virtual QVariant Data() {
   return QVariant(QString(atom_.GetName().c_str()));
 }
 
 virtual int Row() const {
   mol::ResidueView r=atom_.GetResidue();
   mol::AtomViewList::const_iterator i=std::find(r.GetAtomList().begin(),
                                            r.GetAtomList().end(), 
                                            atom_);
   assert(i!=r.GetAtomList().end());
   return std::distance(r.GetAtomList().begin(), i);
 }
 
private:
  mol::AtomView atom_;
};

class DLLEXPORT_OST_GUI ResidueViewMenuItem : public MenuItem {
public:
 ResidueViewMenuItem(MenuItem* parent, const mol::ResidueView& residue) 
   : MenuItem(parent), residue_(residue) {
   mol::AtomViewList::const_iterator i=residue.GetAtomList().begin();
   for (; i!= residue.GetAtomList().end(); ++i) {
     Children().push_back(new AtomViewMenuItem(this, *i));
   }
 }
 
 mol::ResidueView& Residue() {
   return residue_;
 }
 
 virtual QVariant Data() {
   String text=residue_.GetKey()+residue_.GetNumber().AsString();
   return QVariant(QString(text.c_str()));
 }
 
 virtual int Row() const {
   mol::ChainView c=residue_.GetChain();
   mol::ResidueViewList::const_iterator i=std::find(c.GetResidueList().begin(),
                                               c.GetResidueList().end(), 
                                               residue_);
   assert(i!=c.GetResidueList().end());
   return std::distance(c.GetResidueList().begin(), i);
 }
 
private:
  mol::ResidueView residue_;
};

class DLLEXPORT_OST_GUI ChainViewMenuItem : public MenuItem {
public:
 ChainViewMenuItem(MenuItem* parent, const mol::ChainView& chain) 
  : MenuItem(parent), chain_(chain) {
  mol::ResidueViewList::const_iterator i=chain.GetResidueList().begin();
  for (; i!= chain.GetResidueList().end(); ++i) {
    Children().push_back(new ResidueViewMenuItem(this, *i));
  }
 }
 
 mol::ChainView& Chain() {
   return chain_;
 }
 virtual QVariant Data() {
   return QVariant(QString(chain_.GetName().c_str()));
 }
 virtual int Row() const {
   mol::EntityView e=chain_.GetEntity();
   mol::ChainViewList::const_iterator i=std::find(e.GetChainList().begin(),
                                             e.GetChainList().end(), 
                                             chain_);
   assert(i!=e.GetChainList().end());
   return std::distance(e.GetChainList().begin(), i);
 } 
private:
  mol::ChainView chain_;
};

class DLLEXPORT_OST_GUI GfxObjMenuItem : public MenuItem {
public:
  GfxObjMenuItem(MenuItem* parent, const gfx::GfxObjP& p):
    MenuItem(parent), object_(p) {
   mol::EntityView ev;
   gfx::EntityP ecp = dyn_cast<gfx::Entity>(p);
   if(ecp) {
     ev=ecp->GetView();
   }

   if(!ev) return;
  
   mol::ChainViewList::const_iterator i=ev.GetChainList().begin();
   for (; i!= ev.GetChainList().end(); ++i) {
     Children().push_back(new ChainViewMenuItem(this, *i));
   }
 }

 gfx::GfxObjP Object() {
   return object_;
 }
  
 virtual QVariant Data() {
    return QVariant(object_->GetName().c_str());
  } 

  virtual int Row() const {
#if 0
  // needs new GfxNodeVisitor implementation
    gfx::Scene& sc=gfx::Scene::Instance();
    gfx::GfxObjList::iterator i=std::find(sc.SceneObjects().begin(),
                                             sc.SceneObjects().begin(), 
                                             object_);
    assert(i!=sc.SceneObjects().end());
    return std::distance(sc.SceneObjects().begin(), i);
#else
    return 0;
#endif
  } 

private:
  gfx::GfxObjP object_;
};

}}

#endif
