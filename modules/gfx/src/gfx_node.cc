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
#include <boost/bind.hpp>
#include <ost/dyn_cast.hh>
#include "gfx_node.hh"
#include "gfx_object.hh"
#include "scene.hh"

namespace ost { namespace gfx {

GfxNode::GfxNode(const String& name):
  name_(name),
  show_(true),
  node_vector_()
{}

GfxNode::~GfxNode()
{}

GfxNode::GfxNode(const GfxNode& o) {}

GfxNode& GfxNode::operator=(const GfxNode&) {return *this;}

GfxNodeP GfxNode::Copy() const
{
  return GfxNodeP(new GfxNode(*this));
}

int GfxNode::GetType() const
{
  return 0;
}


size_t GfxNode::GetChildCount() const
{
  return node_vector_.size();
}

void GfxNode::DeepSwap(GfxNode& n)
{
  std::swap(name_,n.name_);
  std::swap(show_,n.show_);
}

void GfxNode::Apply(GfxNodeVisitor& v,GfxNodeVisitor::Stack st)
{
  if(!v.VisitNode(this,st)) return;

  st.push(this);

  for(GfxNodeVector::iterator it =node_vector_.begin();it!=node_vector_.end();++it) {
    (*it)->Apply(v,st);
  }

  st.pop();

  v.LeaveNode(this,st);
}

void GfxNode::RenderGL(RenderPass pass)
{
  if(!IsVisible()) return;
  for(GfxNodeVector::iterator it =node_vector_.begin();it!=node_vector_.end();++it) {
    (*it)->RenderGL(pass);
  }
}

void GfxNode::RenderPov(PovState& pov)
{
  if(!IsVisible()) return;
  for(GfxNodeVector::iterator it=node_vector_.begin();it!=node_vector_.end();++it) {
    (*it)->RenderPov(pov);
  }
}

String GfxNode::GetName() const
{
  return name_;
}

void GfxNode::Rename(const String& name)
{
  name_=name;
  Scene::Instance().ObjectChanged(name_);
}

void GfxNode::Add(GfxObjP obj)
{
  node_vector_.push_back(obj);
  Scene::Instance().NodeAdded(obj);
}

void GfxNode::Remove(GfxObjP obj)
{
  GfxNodeVector::iterator it = find (node_vector_.begin(), node_vector_.end(), obj);
  if(it!=node_vector_.end()) {
    node_vector_.erase(it);
  }
}
using boost::bind;
void GfxNode::RemoveAll()
{
  GfxNodeVector v=node_vector_;
  node_vector_.clear();
  for (GfxNodeVector::iterator i=v.begin(), e=v.end(); i!=e; ++i) {
    if (GfxObjP o=dyn_cast<GfxObj>(*i)) {
      Scene::Instance().NotifyObservers(bind(&SceneObserver::NodeRemoved, _1, o));
    }
  }
}

void GfxNode::Add(GfxNodeP node)
{
  node_vector_.push_back(node);
  Scene::Instance().NodeAdded(node);    
}

void GfxNode::Remove(GfxNodeP node)
{
  GfxNodeVector::iterator it = find (node_vector_.begin(), node_vector_.end(), node);
  if(it!=node_vector_.end()) {
    node_vector_.erase(it);
  }
}

void GfxNode::Remove(const String& name)
{
  GfxNodeVector::iterator node;
  for(GfxNodeVector::iterator it=node_vector_.begin();
      it!=node_vector_.end();) {
    if((*it)->GetName()==name) {
      node = it;
      break;
    } else {
      ++it;
    }
  }
  node_vector_.erase(node);
}

void GfxNode::Hide()
{
  show_=false;
  Scene::Instance().RequestRedraw();
}

void GfxNode::Show()
{
  show_=true;
  Scene::Instance().RequestRedraw();
}

bool GfxNode::IsVisible() const
{
  return show_;
}

void GfxNode::ContextSwitch()
{
  for(GfxNodeVector::iterator it =node_vector_.begin();it!=node_vector_.end();++it) {
    (*it)->ContextSwitch();
  }
}

}} // ns
