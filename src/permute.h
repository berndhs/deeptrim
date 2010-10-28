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

class QListWidgetItem;
class QCloseEvent;
class QIODevice;
class QAction;
class QTimer;
class QsciScintilla;

using namespace deliberate;

namespace permute
{
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


private slots:

  void License ();
  void Exiting ();
  void Restart ();
  void StorePara ();
  void NewPara ();
  void EditSettings ();
  void ClickedHeader (QListWidgetItem* item);
  void OpenItemPara (const QListWidgetItem* item);
  void Save ();
  void SaveText ();
  void SaveHtml ();
  void Load ();
  void ImportPara ();
  void CleanTrash ();

private:

  void  Connect ();
  void  CloseCleanup ();
  void  WriteDom (QIODevice * dest);
  void  WriteText (QIODevice * dest);
  void  ReadDom (QIODevice * source);
  QAction*  CellMenu (const QListWidgetItem *cell,
                  const QList <QAction *> extraActions = QList<QAction*>());

  bool           initDone;
  QApplication  *app;

  Ui_PermuteWin  ui;
  QsciScintilla *textEdit;
  ConfigEdit     configEdit;

  QStringList      configMessages;
  HelpView        *helpView;

  QStringList    args;
  bool           again;
  int            nextPara;
  int            tooltiplen;
  QString        saveFile;

  QMap <QString, QString>  paragraphs;

  QList <QListWidgetItem *>  deadItems;
  QTimer                    *trashCollect;
};

} // namespace

#endif
