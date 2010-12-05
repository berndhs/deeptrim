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
#include <Qt>
#include <QDockWidget>
#include <Qsci/qsciscintilla.h>
#include <QFont>
#include <QEvent>
#include <QKeyEvent>
#include "ui_edit-box.h"

class QResizeEvent; 
class QMenuBar;
class QMenu;
class QAction; 
class QShortcut;
class QLabel; 
 
namespace permute
{

class EditKeyFilter;
class SpinEventFilter;

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
  QsciScintilla * TextEdit ();

  void Clear ();
  bool LoadFile (const QString & filename);
  QString FileName ();

  QString Title ();
 
  void SetEmphasis (bool high=true);
  void SpecialKeyEvent (QKeyEvent * kevt);

public slots:

  void SetTitle (QString newTitle);
  void DoSearch ();
  void DoReplace ();
  void JumpClicked ();
  void HideJump ();

private slots:

  void ModifyChanged (bool isModified);
  void DockMoved (Qt::DockWidgetArea area);
  void TopChanged (bool isTop);
  void FontLocalAction ();
  void FontGlobalAction ();
  void LangAction ();
  void MakeDock ();
  void MakeFloat ();
  void LoadAction ();
  void LoadInsertAction ();
  void ReloadAction ();
  void SaveAction ();
  void SaveAsAction ();
  void CloseAction ();
  void AskSave ();
  void CursorChange (int line, int col);
  void LineJumpMenu ();
  void JumpLine (bool button=false);
  void LinesChanged ();
  void ShowJump ();

signals:

  void NewTitle (QString newTitle, PermEditBox * box);
  void TitleGone (PermEditBox * box);
  void NewPermEditBox (QString newName);

protected:

  void closeEvent  (QCloseEvent *event);
  void resizeEvent (QResizeEvent *event);

private:

  void SetupCustom ();
  void SetupMenus ();
  void SetupIcons ();
  void SetupShortcuts ();
  void Connect ();
  void TitleChange (const QString & newTitle);
  void MoveJump (const QPoint & bottomRight);

  bool              wasModified;
  QString           currentFile;
  QString           currentType;
  QFont             defaultFont;
  QMenuBar         *topMenu;
  QMenu            *fileMenu;
  QMenu            *configMenu;
  QMenu            *windowMenu;
  QAction          *actionDock;
  QAction          *actionFloat;
  QAction          *actionMaxi;
  QAction          *actionNorm;
  QAction          *actionCloseWin;
  QAction          *actionSave;
  QAction          *actionSaveAs;
  QAction          *actionFontLocal;
  QAction          *actionFontGlobal;
  QAction          *actionLang;
  QAction          *actionLoad;
  QAction          *actionReload;
  QAction          *actionInsertFile;
  QAction          *actionClose;

  QString           normalStyle;
  QString           emphStyle;
  int               jumpOrigin;

  EditKeyFilter    *scinFilter;
  SpinEventFilter  *spinFilter;


  static int boxCounter;
};

} // namespace

#endif
