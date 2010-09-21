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
#include <ost/gui/gosty_app.hh>
#include "loader_manager.hh"

#include <ost/gui/perspective.hh>
#include <QDir>
#include <QAction>
#include <QMenu>
#include <QFileInfo>
#include <QMessageBox>
#include <QMenuBar>


namespace ost { namespace gui {

LoaderManager::LoaderManager()
{
  QMenuBar* menu_bar = GostyApp::Instance()->GetPerspective()->GetMenuBar();
  site_actions_=new QActionGroup(menu_bar);
  site_actions_->setExclusive(true);
  site_menu_=new QMenu();
}

void LoaderManager::AddRemoteSiteLoader(const QString& ident, RemoteSiteLoader* site_loader)
{
  site_loaders_.insert(ident,site_loader);

  QAction* action = new QAction(ident,site_actions_);
  action->setCheckable(true);
  site_actions_->addAction(action);
  if(site_actions_->checkedAction()==NULL){
    action->setChecked(true);
  }
  site_menu_->addAction(action);
}

void LoaderManager::RemoveRemoteSiteLoader(const QString& ident)
{
  site_loaders_.remove(ident);

  QList<QAction*> actions = site_actions_->actions();
  QAction* action = NULL;
  for (int i=0; i<actions.size(); i++){
    if(actions[i]->text()==ident){
      action = actions[i];
      break;
    }
  }
  if(action){
    site_actions_->removeAction(action);
    site_menu_->removeAction(action);
  }
}


RemoteSiteLoader* LoaderManager::GetRemoteSiteLoader(const QString& ident)
{
  if(site_loaders_.contains(ident)){
    return site_loaders_[ident];
  }
  return NULL;
}

RemoteSiteLoader* LoaderManager::GetCurrentSiteLoader()
{
  if(site_actions_->checkedAction()!=NULL)
    return site_loaders_[site_actions_->checkedAction()->text()];
  return NULL;
}

std::vector<String> LoaderManager::GetSiteLoaderIdents()
{
  QList<QString> keys = site_loaders_.keys();
  std::vector<String> sites;
  for(int i=0;i<keys.size();i++){
    sites.push_back(keys.at(i).toStdString());
  }
  return sites;
}

QMenu* LoaderManager::GetSiteMenu(){
  return site_menu_;
}

RemoteSiteLoader* LoaderManager::GetDefaultRemoteSiteLoader()
{
 return GetRemoteSiteLoader(GetDefaultRemoteSiteIdent());
}

QString LoaderManager::GetDefaultRemoteSiteIdent(){
  return default_site_;
}

void LoaderManager::SetDefaultRemoteSiteIdent(const QString& ident){
  default_site_ = ident;
}

LoaderManager::~LoaderManager(){}

} }
