#ifndef PERM_EDITBOX_H
#define PERM_EDITBOX_H

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

#include <QDockWidget>
#include <Qsci/qsciscintilla.h>
#include <QFont>
#include "ui_edit-box.h"

class QResizeEvent;
class QMenuBar;
class QMenu;
class QAction;

namespace permute
{
class PermEditBox : public QDockWidget, public Ui_EditBox
{
Q_OBJECT

public:

  PermEditBox (const QString & title, 
               QWidget * parent = 0, 
               Qt::WindowFlags flags = 0 );
  
  void SetDefaultFont (const QFont & font, bool setNow=false);
  void SetText (const QString & text);
  QString Text ();
  QsciScintilla & TextEdit ();

  void Clear ();
  bool LoadFile (const QString & filename);
  QString FileName ();

public slots:

  void SetTitle (QString newTitle);

signals:

  void NewTitle (QString newTitle);

protected:

  void resizeEvent (QResizeEvent *event);

private:

  QString        currentFile;
  QFont          defaultFont;
  QsciScintilla *scin;
  QMenuBar      *topMenu;
  QMenu         *fileMenu;
  QAction       *actionSave;
};

} // namespace

#endif
