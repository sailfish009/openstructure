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
#include <QDir>
#include <QVBoxLayout>
#include <QApplication>

#include <ost/platform.hh>

#include <ost/gui/widget_registry.hh>
#include <ost/gui/gosty_app.hh>

#include "info_widget.hh"

namespace ost { namespace gui {

class InfoWidgetFactory: public WidgetFactory {
public:
  InfoWidgetFactory() :
    WidgetFactory("ost::gui::InfoWidget", "Info Widget") {
  }

  virtual Widget* Create(QWidget* parent) {
    return GostyApp::Instance()->GetInfoWidget();
  }
};

OST_REGISTER_WIDGET(InfoWidget, InfoWidgetFactory);

InfoWidget::InfoWidget(QWidget* parent) : Widget(NULL, parent), model_(new QStandardItemModel(this)), view_(new QListView(this)) {
  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->setMargin(0);
  layout->setSpacing(0);
  view_->setAttribute(Qt::WA_MacShowFocusRect, false);
  view_->setAttribute(Qt::WA_MacSmallSize, true);
  view_->setModel(model_);
  view_->setSelectionBehavior(QAbstractItemView::SelectRows);
  view_->setDragEnabled(true);

  layout->addWidget(view_);


  QDir icon_path(GetSharedDataPath().c_str());
  icon_path.cd("gui");
  icon_path.cd("icons");

  QAction* clear_action = new QAction(this);
  clear_action->setToolTip("Clear info panel");
  clear_action->setIcon(QIcon(icon_path.absolutePath()+QDir::separator()+QString("delete_icon.png")));
  connect(clear_action,SIGNAL(triggered(bool)), this, SLOT(Clear()));
  this->actions_.append(clear_action);
}

void InfoWidget::Update() {
  view_->viewport()->update();
}

void InfoWidget::LogMessage(const QString& message, QMessageBox::Icon icon){
  QPixmap pix_icon = this->GetIcon(icon,this);
  QStandardItem* item = new QStandardItem();
  item->setText(message);
  item->setIcon(QIcon(pix_icon));
  this->model_->appendRow(item);
}

void InfoWidget::LogMessage(QStandardItem* item){
  this->model_->appendRow(item);
}

void InfoWidget::LogMessage(const QString& message, QIcon icon){
  QStandardItem* item = new QStandardItem();
  item->setText(message);
  item->setIcon(icon);
  this->model_->appendRow(item);
}

QPixmap InfoWidget::GetIcon(QMessageBox::Icon icon, QWidget* widget)
{
    QStyle *style = widget ? widget->style() : QApplication::style();
    int icon_size = style->pixelMetric(QStyle::PM_MessageBoxIconSize, 0, widget);
    QIcon tmp_icon;
    switch (icon) {
    case QMessageBox::Information:
        tmp_icon = style->standardIcon(QStyle::SP_MessageBoxInformation, 0, widget);
        break;
    case QMessageBox::Warning:
        tmp_icon = style->standardIcon(QStyle::SP_MessageBoxWarning, 0, widget);
        break;
    case QMessageBox::Critical:
        tmp_icon = style->standardIcon(QStyle::SP_MessageBoxCritical, 0, widget);
        break;
    case QMessageBox::Question:
        tmp_icon = style->standardIcon(QStyle::SP_MessageBoxQuestion, 0, widget);
    default:
        break;
    }
    if (!tmp_icon.isNull())
        return tmp_icon.pixmap(icon_size, icon_size);
    return QPixmap();
}

void InfoWidget::Clear(){
  this->model_->clear();
}

void InfoWidget::RemoveSelected(){
  QItemSelectionModel* selection_model = this->view_->selectionModel();
  const QItemSelection& item_selection =  selection_model->selection();
  const QModelIndexList& model_indexes = item_selection.indexes();
  for(int i=0;i<model_indexes.size();i++){
    this->model_->removeRow(model_indexes[i].row());
  }
}

ActionList InfoWidget::GetActions(){
  return this->actions_;
}

InfoWidget::~InfoWidget() {
}

}
} // ns
