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

#include <ost/base.hh>

#include "python_namespace_tree_item.hh"



namespace ost{namespace gui{

PythonNamespaceTreeItem::PythonNamespaceTreeItem(const bp::object& ns, 
                                                 const QString& name, 
                                                 PythonNamespaceTreeItem* parent):
  parent_(parent),
  children_(),
  namespace_(ns),
  name_(name),
  initialized_(false)
{
}

PythonNamespaceTreeItem::~PythonNamespaceTreeItem()
{
  DeleteChildren();
}

void PythonNamespaceTreeItem::DeleteChildren()
{
  qDeleteAll(children_.begin(), children_.end());
  children_.clear();
  initialized_=false;
}
unsigned int PythonNamespaceTreeItem::ChildCount() const
{
  return children_.size();
}

PythonNamespaceTreeItem* PythonNamespaceTreeItem::GetParent() const
{
  return parent_;
}
PythonNamespaceTreeItem* PythonNamespaceTreeItem::GetChild(unsigned int index) const
{
  return children_.value(index);
}
unsigned int PythonNamespaceTreeItem::GetRow() const
{
  if(parent_){
    return parent_->children_.indexOf(const_cast<PythonNamespaceTreeItem*>(this));
  }
  return 0;
}

QString PythonNamespaceTreeItem::GetName()  const
{
  return name_;
}

bool PythonNamespaceTreeItem::HasChildren() const 
{
  if (initialized_) {
    return children_.size()>0;
  } else {
    return true;
  }
}

bool PythonNamespaceTreeItem::CanFetchMore() const
{
  return !initialized_;
}

void PythonNamespaceTreeItem::FetchMore()
{
  initialized_=true;  
  bp::object dir=bp::import("__main__").attr("__builtins__").attr("dir");  
  bp::list keys=bp::extract<bp::list>(dir(namespace_)); 
  unsigned int dict_length=bp::len(keys);
  for (unsigned int i=0;i<dict_length;++i) {
    QString child_name=QString::fromStdString(bp::extract<ost::String>(keys[i]));
    if(child_name.startsWith("__")){
      continue;
    }
    bp::object child_namespace;
    bool found=false;
    try{
      String keystring=bp::extract<ost::String>(keys[i]);
      child_namespace=namespace_.attr(keystring.c_str());
      if (bp::len(dir(child_namespace))>0) {
        found=true;
      }      
    } catch(bp::error_already_set) {
      PyErr_Clear();
    }
    children_.append(new PythonNamespaceTreeItem(child_namespace,
                                                 child_name, this));
  }
}

}}//ns
