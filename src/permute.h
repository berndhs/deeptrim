#ifndef PERMUTE_H
#define PERMUTE_H

/****************************************************************
 * This file is distributed under the following license:
 *
 * Copyright (C) 2010, Bernd Stramm
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, 
 *  Boston, MA  02110-1301, USA.
 ****************************************************************/
#include <QMainWindow>
#include <QApplication>
#include <QStringList>
#include <QList>

#include <QMap>

#include "ui_permute.h"
#include "config-edit.h"
#include "deliberate.h"
#include "helpview.h"
#include "search-dialog.h"

class QListWidgetItem;
class QCloseEvent;
class QIODevice;
class QAction;
class QTimer;

using namespace deliberate;

namespace permute
{

class PermEditBox;

class Permute : public QMainWindow 
{
Q_OBJECT

public:

  Permute (QWidget *parent=0);

  void Init (QApplication & app);
  void AddConfigMessages (const QStringList & msgs);
  void CommandArgs (const QStringList & args);

  void Run ();

  bool Again ();

protected:

  void closeEvent (QCloseEvent * event);

public slots:

  void Quit ();
  void About ();
  void NewTag (QString tag, PermEditBox *box);
  void RemoveTag (PermEditBox *box);


private slots:

  void License ();
  void Exiting ();
  void Restart ();
  void EditSettings ();
  void OpenFile ();
  void NewFile ();
  void TitleItemClicked (QListWidgetItem *item);

private:

  void  Connect ();
  void  AdjustSpace ();
  void  CloseCleanup ();
  void  OpenFile (const QString & filename);
  void  AddDockWidget ( Qt::DockWidgetArea area, 
                        QDockWidget * dockwidget );
  void  AddDockWidget ( Qt::DockWidgetArea area, 
                        QDockWidget * dockwidget, 
                        Qt::Orientation orientation );
  void  ListChildren (QObject * parent, int prefixLen = 1);

  bool           initDone;
  QApplication  *app;

  Ui_PermuteWin  ui;
  ConfigEdit     configEdit;
  PermEditBox   *hiddenBox;
  QStringList    configMessages;
  HelpView      *helpView;
  SearchDialog   searcher;

  QStringList    args;
  bool           again;
  int            tooltiplen;
  QString        saveFile;

  QMap <PermEditBox *, QListWidgetItem *>  titleItems;
  QMap <QListWidgetItem *, PermEditBox *>  titleBoxes;
  
  PermEditBox   *emphedBox;
  QSize          normalSpacerSize;
  QSize          smallSpacerSize;

};

} // namespace

#endif
