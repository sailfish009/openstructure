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
#include "file_loader.hh"

#include <QDir>
#include <QAction>
#include <QMenu>
#include <QFileInfo>
#include <QMessageBox>
#include <QMenuBar>


#include <ost/config.hh>
#include <ost/mol/mol.hh>
#include <ost/mol/surface.hh>

#include <ost/io/io_manager.hh>
#include <ost/io/mol/pdb_reader.hh>
#include <ost/io/mol/load_entity.hh>
#include <ost/io/mol/load_surface.hh>
#include <ost/io/mol/entity_io_pdb_handler.hh>

#include <ost/conop/conop.hh>

#include <ost/seq/sequence_list.hh>
#include <ost/seq/alignment_handle.hh>

#include <ost/gfx/entity.hh>
#include <ost/gfx/surface.hh>
#include <ost/gfx/scene.hh>

#include <ost/gui/gosty_app.hh>
#include <ost/gui/perspective.hh>
#include <ost/gui/python_shell/python_interpreter.hh>
#include <ost/gui/main_area.hh>
#include <ost/gui/file_type_dialog.hh>
#include <ost/gui/sequence_viewer/sequence_viewer.hh>

#if OST_IMG_ENABLED
  #include <ost/io/img/load_map.hh>
  #include <ost/gfx/map_iso.hh>
  #include <ost/img/extent.hh>
#endif

namespace ost { namespace gui {

LoaderManagerPtr FileLoader::loader_manager_ = LoaderManagerPtr();

#if OST_IMG_ENABLED
  QList<img::ImageHandle> FileLoader::loaded_images_;
#endif

FileLoader::FileLoader(){}

void FileLoader::LoadObject(const QString& filename, const QString& selection)
{
  gfx::GfxObjP obj;
  if(filename.endsWith(".py",Qt::CaseInsensitive)){
    FileLoader::RunScript(filename);
  }
  else if(filename.endsWith(".pdb",Qt::CaseInsensitive)||
      filename.endsWith(".ent",Qt::CaseInsensitive)||
      filename.endsWith(".pdb.gz",Qt::CaseInsensitive)||
      filename.endsWith(".ent.gz",Qt::CaseInsensitive)){
    FileLoader::LoadPDB(filename, selection);
  }
  else{
    try{
      obj=FileLoader::TryLoadEntity(filename, io::EntityIOHandlerP(), selection);
  #if OST_IMG_ENABLED
      if (!obj)  {
        try{
          obj=FileLoader::TryLoadMap(filename);
        } catch (io::IOFileAlreadyLoadedException&) {
          return;
        }
      }
  #endif
      if (!obj)  {
        try{
          obj=FileLoader::TryLoadAlignment(filename);
        } catch (io::IOFileAlreadyLoadedException&) {
          return;
        }
      }
      if (!obj) {
        obj=FileLoader::TryLoadSurface(filename);
      }
      if (!obj) {
        try{
          obj=FileLoader::NoHandlerFound(filename);
        } catch (io::IOFileAlreadyLoadedException&) {
          return;
        }
      }
      if (!obj){
        return;
      }
      FileLoader::AddToScene(filename, obj);
    }catch (io::IOException& e) {
      FileLoader::HandleError(e,IO_LOADING,filename);
    }
  }
}

void FileLoader::AddToScene(const QString& filename, gfx::GfxObjP obj)
{
  try{
    gfx::Scene::Instance().Add(obj);
    if (gfx::Scene::Instance().GetRootNode()->GetChildCount()==1) {
      gfx::Scene::Instance().SetCenter(obj->GetCenter());
    }
  }
  catch (Message m) {
    FileLoader::HandleError(m, GFX_ADD, filename, obj);
  }
}

gfx::GfxObjP FileLoader::NoHandlerFound(const QString& filename)
{
  FileTypeDialog dialog(filename);
  try{
    if(dialog.exec()){
      if(dialog.GetEntityHandler()){
        return TryLoadEntity(filename, dialog.GetEntityHandler());
      }
      if(dialog.GetSequenceHandler()){
        return TryLoadAlignment(filename, dialog.GetSequenceHandler());
      }
      if(dialog.GetSurfaceHandler()){
        return TryLoadSurface(filename,dialog.GetSurfaceHandler());
      }
  #if OST_IMG_ENABLED
      if(dialog.GetMapHandler()){
        return TryLoadMap(filename,dialog.GetMapHandler());
      }
  #endif
    }
  }
  catch (io::IOException& e) {
    FileLoader::HandleError(e,IO_LOADING,filename);
  }
  return gfx::GfxObjP();
}

void FileLoader::LoadFrom(const QString& id, const QString& site, const QString& selection)
{
  if(!loader_manager_)
    loader_manager_ = LoaderManagerPtr(new LoaderManager());
  RemoteSiteLoader* remote_site = loader_manager_->GetRemoteSiteLoader(site);
  if(remote_site){
    remote_site->LoadById(id,selection);
  }
  else{
    remote_site = loader_manager_->GetCurrentSiteLoader();
    if(remote_site){
      remote_site->LoadById(id,selection);
    }
  }
}

LoaderManagerPtr FileLoader::GetLoaderManager()
{
  if(!loader_manager_)
    loader_manager_ = LoaderManagerPtr(new LoaderManager());
  return loader_manager_;
}

std::vector<String> FileLoader::GetSiteLoaderIdents()
{
  if(!loader_manager_)
    loader_manager_ = LoaderManagerPtr(new LoaderManager());
  return loader_manager_->GetSiteLoaderIdents();
}

void FileLoader::HandleError(Message m, ErrorType type, const QString& filename, gfx::GfxObjP obj)
{
  if(type==GFX_ADD || type==GFX_MULTIPLE_ADD){
    QMessageBox message_box(QMessageBox::Warning,
        "Error while adding Node to Scene", m._mesg.c_str());
    if(type==GFX_ADD){
      message_box.setStandardButtons( QMessageBox::Yes | QMessageBox::Cancel);
      message_box.setButtonText(QMessageBox::Yes, "Reload");
      int value = message_box.exec();
      switch(value){
      case QMessageBox::Yes:
        gfx::Scene::Instance().Remove(obj->GetName());
        gfx::Scene::Instance().Add(obj);
        break;
      }
    }
    else {
      message_box.setStandardButtons(QMessageBox::Ok);
      message_box.exec();
    }
  }
  else if(type==IO_LOADING){
    QMessageBox message_box(QMessageBox::Warning,
                "Error while loading file", m._mesg.c_str());
    message_box.setStandardButtons( QMessageBox::Yes | QMessageBox::Cancel);
    message_box.setButtonText(QMessageBox::Yes, "Chose format");
    int value = message_box.exec();
    switch(value){
    case QMessageBox::Yes:
      FileLoader::NoHandlerFound(filename);
      break;
    }
  }
  else if(type==INFO){
    QMessageBox message_box(QMessageBox::Information,
        "Information", m._mesg.c_str());
    message_box.setStandardButtons( QMessageBox::Ok);
    message_box.exec();
  }
}

gfx::GfxObjP FileLoader::TryLoadEntity(const QString& filename, io::EntityIOHandlerP handler, const QString& selection)
{
  if(!handler){
    try{
      handler = io::IOManager::Instance().FindEntityImportHandler(filename.toStdString());
    }
    catch(io::IOUnknownFormatException e){
      handler = io::EntityIOHandlerP();
    }
  }
  if(handler){
    if(dynamic_cast<io::EntityIOPDBHandler*>(handler.get())){
      FileLoader::LoadPDB(filename, selection);
    }
    else{
      QFileInfo file_info(filename);
      mol::EntityHandle eh=mol::CreateEntity();
      mol::XCSEditor xcs_lock=eh.RequestXCSEditor(mol::BUFFERED_EDIT);
      handler->Import(eh,filename.toStdString());
      if(handler->RequiresBuilder()) {
          conop::BuilderP builder = conop::Conopology::Instance().GetBuilder();
          conop::Conopology::Instance().ConnectAll(builder,eh,0);
      }
      gfx::GfxObjP obj(new gfx::Entity(file_info.baseName().toStdString(),eh,mol::Query(selection.toStdString())));
      return obj;
    }
  }
  return gfx::GfxObjP();
}

#if OST_IMG_ENABLED
gfx::GfxObjP FileLoader::TryLoadMap(const QString& filename, io::MapIOHandlerPtr handler)
{
  if(!handler){
    try{
      handler = io::IOManager::Instance().FindMapImportHandlerFile(filename.toStdString(),io::UndefinedImageFormat());
    }
    catch(io::IOUnknownFormatException e){
      handler = io::MapIOHandlerPtr();
    }
  }
  if(handler){
    img::ImageHandle map = CreateImage(img::Extent(),img::REAL,img::SPATIAL);
    handler->Import(map,filename.toStdString(),io::UndefinedImageFormat());
    ost::img::Extent ext = map.GetExtent();
    if(ext.GetSize().GetDepth()>1){
      QFileInfo file_info(filename);
      gfx::MapIsoP map_iso(new gfx::MapIso(file_info.baseName().toStdString(),
                                           map, 0.0));
      map_iso->SetLevel(map_iso->GetMean());
      return map_iso;
    }
    else if(ext.GetSize().GetDepth()==1){
      //FIXME ImageHandle should not be destroyed at the end of method
      //therefore hack with list
      loaded_images_.append(map);
      ost::img::gui::DataViewer* viewer = GostyApp::Instance()->CreateDataViewer(loaded_images_.last());
      gui::GostyApp::Instance()->GetPerspective()->GetMainArea()->AddWidget(filename,viewer);
      throw io::IOFileAlreadyLoadedException("Loaded in DataViewer");
    }
  }
  return gfx::GfxObjP();
}
#endif

gfx::GfxObjP FileLoader::TryLoadSurface(const QString& filename, io::SurfaceIOHandlerPtr handler)
{
  if(!handler){
    try{
      handler = io::IOManager::Instance().FindSurfaceImportHandler(filename.toStdString(),"auto");
    }
    catch(io::IOUnknownFormatException e){
      handler = io::SurfaceIOHandlerPtr();
    }
  }
  if(handler){
    QFileInfo fi(filename);
    QString path = fi.absolutePath().append(QDir::separator()).append(fi.completeBaseName());
    mol::SurfaceHandle sh = mol::CreateSurface();
    handler->Import(sh,filename.toStdString());
    QString pdb_path(path + ".pdb");
    if(QFile::exists(pdb_path)){
      mol::EntityHandle ent = io::LoadEntity(pdb_path.toStdString());
      sh.Attach(ent,5.0);
      gfx::SurfaceP gfx_surf(new gfx::Surface(fi.baseName().toStdString(),sh));
      return gfx_surf;
    }
    else{
      gfx::SurfaceP gfx_surf(new gfx::Surface(fi.baseName().toStdString(),sh));
      return gfx_surf;
    }
  }
  return gfx::GfxObjP();
}

gfx::GfxObjP FileLoader::TryLoadAlignment(const QString& filename, io::SequenceIOHandlerPtr handler)
{
  if(!handler){
    try{
      handler = io::IOManager::Instance().FindAlignmentImportHandler(filename.toStdString(),"auto");
    }
    catch(io::IOUnknownFormatException e){
      handler = io::SequenceIOHandlerPtr();
    }
  }
  if(handler){
    seq::SequenceList seq_list = seq::CreateSequenceList();
    handler->Import(seq_list,filename.toStdString());
    seq::AlignmentHandle alignment = seq::AlignmentFromSequenceList(seq_list);
    gui::MainArea* main_area = gui::GostyApp::Instance()->GetPerspective()->GetMainArea();
    SequenceViewer* viewer = new SequenceViewer(true,main_area);
    viewer->AddAlignment(alignment);
    main_area->AddWidget(filename,viewer);
    throw io::IOFileAlreadyLoadedException("Loaded in DataViewer");
  }
  return gfx::GfxObjP();
}

void FileLoader::RunScript(const QString& filename)
{
  PythonInterpreter& pi = PythonInterpreter::Instance();
  //HackerMode On
  //The following code lines are just temporary
  //TODO create class or function which can load any kind of files and execute scripts
  pi.RunCommand("_sys_argv_backup=sys.argv");
  pi.RunCommand("sys.argv=''");
  pi.RunCommand("_dir=os.getcwd()");
  pi.RunCommand("os.chdir('"+QFileInfo(filename).absolutePath()+"')");
  pi.RunCommand("execfile('"+QFileInfo(filename).fileName()+"')");
  pi.RunCommand("os.chdir(_dir)");
  pi.RunCommand("del(_dir)");
  pi.RunCommand("sys.argv=_sys_argv_backup");
  pi.RunCommand("del(_sys_argv_backup)");
  //HackerMode Off
}

void FileLoader::LoadPDB(const QString& filename, const QString& selection)
{
  io::PDBReader reader(filename.toStdString());
  QList<mol::EntityHandle> entities;
  conop::BuilderP builder=conop::Conopology::Instance().GetBuilder("DEFAULT");
  while (reader.HasNext()){
    mol::EntityHandle ent=mol::CreateEntity();
    reader.Import(ent);
    conop::Conopology::Instance().ConnectAll(builder,ent,0);
    entities.append(ent);
  }
  QFileInfo file_info(filename);
  if(entities.empty()){
    FileLoader::HandleError(Message(QString("No entities found in file: "+ filename).toStdString()),INFO,filename);
  }
  else if(entities.size()==1){
    gfx::EntityP gfx_ent(new gfx::Entity(file_info.baseName().toStdString(),entities.first(),mol::Query(selection.toStdString())));
    try{
      gfx::Scene::Instance().Add(gfx_ent);
    }
    catch (Message m) {
      HandleError(m, GFX_ADD, filename, gfx_ent);
    }
    if (gfx::Scene::Instance().GetRootNode()->GetChildCount()==1) {
      gfx::Scene::Instance().SetCenter(gfx_ent->GetCenter());
    }
  }
  else{
    try{
      for(int i = 0 ; i<entities.size(); i++){
        gfx::EntityP gfx_ent(new gfx::Entity(QString(file_info.baseName()+" ("+QString::number(i)+")").toStdString(),entities[i],mol::Query(selection.toStdString())));
        gfx::Scene::Instance().Add(gfx_ent);
      }
    }
    catch (Message m) {
      FileLoader::HandleError(m,GFX_MULTIPLE_ADD,filename);
    }
  }
}

FileLoader::~FileLoader(){}

} }
