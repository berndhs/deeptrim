
#include "perm-editbox.h"

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

#include <Qsci/qscilexer.h>
#include <Qsci/qscilexercpp.h>
#include <QFile>
#include <QFileInfo>
#include <QResizeEvent>
#include <QAction>
#include <QMenuBar>
#include <QFontComboBox>
#include <QDebug>
#include <QIcon>
#include <QMessageBox>
#include <QFileDialog>
#include <QFontDialog>
#include "lexer-chooser.h"
#include "deliberate.h"
#include "font-chooser.h"

using namespace deliberate;

namespace permute
{

int PermEditBox::boxCounter (1);

PermEditBox::PermEditBox (const QString & title, 
               QWidget * parent, 
               Qt::WindowFlags flags)
  :QDockWidget (title, parent, flags),
   wasModified (false),
   normalStyle (""),
   emphStyle ("background-color: lightgreen")
{
qDebug () << " new box with name " << title;
  QString oldName = objectName();
  setObjectName (QString ("PermEditBox-%1")
                         .arg(boxCounter));
  setWindowIcon (parentWidget()->windowIcon());
  setupUi (this);
  gridLayout->removeItem (buttonLayout);
  gridLayout->removeWidget (scin);
  setWindowTitle (title);
  setWindowIcon (parent->windowIcon());
  setTitleBarWidget (0);
  SetupMenus ();
  SetupShortcuts ();
  SetupIcons ();
  windowMenu->setVisible (false);
  gridLayout->addWidget (topMenu,0,0,1,1);
  gridLayout->addItem (buttonLayout, 0,1,1,1);
  scin->setObjectName (QString ("Scin-%1").arg (boxCounter));
  gridLayout->addWidget (scin, 1,0,2,2);
  Connect ();
  emit NewPermEditBox (objectName());
  boxCounter++;
  normalStyle = Settings().value ("editbox/normalstyle",normalStyle).toString();
  emphStyle = Settings().value ("editbox/emphstyle",emphStyle).toString();
  setStyleSheet (QString (" %1 ").arg (normalStyle));
  TopChanged (isFloating());
}

void
PermEditBox::SetupMenus ()
{
  topMenu = new QMenuBar (dockWidgetContents);
  windowMenu = new QMenu (tr(""), topMenu);
  fileMenu = new QMenu (tr("File..."), topMenu);
  configMenu = new QMenu (tr("Config"), topMenu);

  actionDock = new QAction (tr("Dock"), this);
  actionFloat = new QAction (tr("Float"), this);
  actionMaxi = new QAction (tr("Maximize"), this);
  actionNorm = new QAction (tr("Normalize"), this);
  actionCloseWin = new QAction (tr("Close"), this);

  actionSave = new QAction (tr("&Save"),this);
  actionSaveAs = new QAction (tr("Save As..."),this);
  actionLoad = new QAction (tr("Open File"),this);
  actionInsertFile = new QAction (tr("Insert File"), this);

  actionFontLocal = new QAction (tr("Font for file"),this);
  actionFontGlobal = new QAction (tr("Font for file type"),this);
  actionLang = new QAction (tr("File Type"),this);
  actionClose = new QAction (tr("Close"), this);

  windowMenu->addAction (actionDock);
  windowMenu->addAction (actionFloat);
  windowMenu->addAction (actionMaxi);
  windowMenu->addAction (actionNorm);
  windowMenu->addAction (actionCloseWin);

  fileMenu->addAction (actionSave);
  fileMenu->addAction (actionSaveAs);
  fileMenu->addAction (actionLoad);
  fileMenu->addAction (actionInsertFile);
  fileMenu->addAction (actionClose);

  configMenu->addAction (actionFontLocal);
  configMenu->addAction (actionFontGlobal);
  configMenu->addAction (actionLang);

  topMenu->addAction (windowMenu->menuAction ());
  topMenu->addAction (fileMenu->menuAction());
  topMenu->addAction (configMenu->menuAction());
  topMenu->setSizePolicy (QSizePolicy (QSizePolicy::Expanding, 
                                       QSizePolicy::Fixed));
}

void
PermEditBox::SetupIcons ()
{
  windowMenu->menuAction()->setIconVisibleInMenu(true);
  windowMenu->setIcon (QIcon(":/deeptrim.png"));
  undoButton->setIcon (QIcon (":/icons/edit-undo.png"));
  redoButton->setIcon (QIcon (":/icons/edit-redo.png"));

  actionDock->setIconVisibleInMenu (true);
  actionDock->setIcon (QIcon (":/icons/minimize.png"));
  actionFloat->setIconVisibleInMenu (true);
  actionFloat->setIcon (QIcon (":/icons/float.png"));
  actionMaxi->setIconVisibleInMenu (true);
  actionMaxi->setIcon (QIcon (":/icons/maximize.png"));
  actionNorm->setIconVisibleInMenu (true);
  actionNorm->setIcon (QIcon (":/icons/normalize.png"));
  actionCloseWin->setIconVisibleInMenu (true);
  actionCloseWin->setIcon (QIcon (":/icons/document-close.png"));

  actionSave->setIconVisibleInMenu (true);
  actionSave->setIcon (QIcon (":/icons/document-save.png"));
  actionSaveAs->setIconVisibleInMenu (true);
  actionSaveAs->setIcon (QIcon (":/icons/document-save-as.png"));
  actionLoad->setIconVisibleInMenu (true);
  actionLoad->setIcon (QIcon (":/icons/document-open.png"));
  actionInsertFile->setIconVisibleInMenu (true);
  actionInsertFile->setIcon (QIcon (":/icons/insert-text.png"));
  actionClose->setIconVisibleInMenu (true);
  actionClose->setIcon (QIcon (":/icons/document-close.png"));

  actionFontLocal->setIconVisibleInMenu (true);
  actionFontLocal->setIcon (QIcon (":/icons/font-this.png"));
  actionFontGlobal->setIconVisibleInMenu (true);
  actionFontGlobal->setIcon (QIcon (":/icons/font-all.png"));
}

void
PermEditBox::SetupShortcuts ()
{
  actionSave->setShortcut (QKeySequence::Save);
  actionSaveAs->setShortcut (QKeySequence::SaveAs);
  actionLoad->setShortcut (QKeySequence::Open);
  actionClose->setShortcut (QKeySequence::Close);
}

void
PermEditBox::Connect ()
{
  connect (this, SIGNAL (dockLocationChanged (Qt::DockWidgetArea)),
           this, SLOT (DockMoved (Qt::DockWidgetArea)));
  connect (this, SIGNAL (topLevelChanged (bool)),
           this, SLOT (TopChanged (bool)));
  connect (actionDock, SIGNAL (triggered()),
           this, SLOT (MakeDock()));
  connect (actionFloat, SIGNAL (triggered()),
           this, SLOT (MakeFloat()));
  connect (actionMaxi, SIGNAL (triggered()),
           this, SLOT (showMaximized()));
  connect (actionNorm, SIGNAL (triggered()),
           this, SLOT (showNormal ()));
  connect (actionCloseWin, SIGNAL (triggered()),
           this, SLOT (CloseAction ()));
  connect (actionSave, SIGNAL (triggered()),
           this, SLOT (SaveAction()));
  connect (actionSaveAs, SIGNAL (triggered()),
           this, SLOT (SaveAsAction ()));
  connect (actionLoad, SIGNAL (triggered()),
           this, SLOT (LoadAction ()));
  connect (actionInsertFile, SIGNAL (triggered()),
           this, SLOT (LoadInsertAction ()));
  connect (actionFontLocal, SIGNAL (triggered()),
           this, SLOT (FontLocalAction ()));
  connect (actionFontGlobal, SIGNAL (triggered()),
           this, SLOT (FontGlobalAction ()));
  connect (actionLang, SIGNAL (triggered()),
           this, SLOT (LangAction ()));
  connect (actionClose, SIGNAL (triggered()),
           this, SLOT (CloseAction ()));
  connect (scin, SIGNAL (cursorPositionChanged (int, int)),
           this, SLOT (CursorChange (int, int)));
  connect (scin, SIGNAL (modificationChanged(bool)),
           this, SLOT (ModifyChanged (bool)));
  connect (saveButton, SIGNAL (clicked()), this, SLOT (SaveAction()));
  connect (saveAsButton, SIGNAL (clicked()), this, SLOT (SaveAsAction()));
  connect (undoButton, SIGNAL (clicked()), scin, SLOT (undo()));
  connect (redoButton, SIGNAL (clicked()), scin, SLOT (redo()));
}

void
PermEditBox::SetText (const QString & text)
{
  scin->setText (text);
}

void
PermEditBox::ModifyChanged (bool isModified)
{
  if (isModified != wasModified) {
    wasModified = isModified;
    QString title = windowTitle();
    if (isModified) {
      title.prepend ('*');
      TitleChange (title);
    } else {
      if (title.startsWith('*')) {
        title.remove (0,1);
        TitleChange (title);
      }
    }
  }
}

void
PermEditBox::SetEmphasis (bool high)
{
  if (high) {
    setStyleSheet (QString (" %1 ").arg(emphStyle));
  } else {
    setStyleSheet (QString (" %1 ").arg(normalStyle));
  }
  update ();
  qDebug () << objectName () << windowTitle() << styleSheet ();
}

void
PermEditBox::SetDefaultFont (const QFont & font, bool setNow)
{
  defaultFont = font;
  if (setNow) {
    if (scin->lexer()) {
      scin->lexer()->setFont (defaultFont);
    } else {
      scin->setFont (defaultFont);
    }
  }
}

void
PermEditBox::DockMoved (Qt::DockWidgetArea area)
{
  qDebug () << "Dock Moved to " << area;
  if (isWindow()) {
    setWindowIcon (parentWidget()->windowIcon());
  }
}

void
PermEditBox::TopChanged (bool isTop)
{
  qDebug () << " Top Changed " << isTop;
qDebug () << " CHANGED window flags " << windowFlags ();
qDebug () << " full " << isFullScreen() << " max " << isMaximized();
  actionMaxi->setEnabled (isTop);
  actionNorm->setEnabled (isTop);
  actionDock->setEnabled (isTop);
  actionFloat->setEnabled (!isTop);
}

void
PermEditBox::FontLocalAction ()
{
  qDebug () << " Font Local Action called";
  QFont defont; bool ok;
  bool useLexer = (scin->lexer() != 0);
  if (useLexer) {
    defont = scin->lexer()->defaultFont();
  } else {
    defont = scin->font();
  }
  QFont newFont = QFontDialog::getFont(
                  &ok, defont, this, tr("Choose Font"));
  if (ok) {
      // the user clicked OK and font is set to the font the user selected
    qDebug () << " old font was " << defont;
    qDebug () << " they want new font " << newFont;
    if (useLexer) {
      scin->lexer()->setFont (newFont);
    } else {
      scin->setFont (newFont);
    }
  } 
}

void
PermEditBox::FontGlobalAction ()
{
  qDebug () << " Font Global Action called";
  QFont defont; bool ok;
  bool useLexer = (scin->lexer() != 0);
  if (useLexer) {
    defont = scin->lexer()->defaultFont();
  } else {
    defont = scin->font();
  }
  QFont newFont = QFontDialog::getFont(
                  &ok, defont, this, tr("Choose Font"));
  if (ok) {
      // the user clicked OK and font is set to the font the user selected
    qDebug () << " old font was " << defont;
    qDebug () << " they want new font " << newFont;
    if (useLexer) {
      scin->lexer()->setFont (newFont);
    } else {
      scin->setFont (newFont);
    }
    FontChooser::Ref().StoreFont (currentType, newFont);
  } 
}

void
PermEditBox::LangAction ()
{
  qDebug () << " Lang Action called";
  QString lang (tr("none"));
  if (scin->lexer()) {
    lang = scin->lexer()->language();
  }
  int ans = QMessageBox::question (this, 
                            tr("File Format"),
                            tr("Format\n\"%1\"\nChange ?").arg(lang),
                              QMessageBox::Yes 
                            | QMessageBox::No 
                            | QMessageBox::Cancel);
  if (ans & QMessageBox::Yes) {
    bool pickedOne (false);
    QsciLexer * newLex = LexerChooser::Ref().NewLexerDialog (this, 
                                                    scin, pickedOne, lang);
    if (pickedOne) {
      scin->setLexer (newLex);
      currentType = QString (newLex->language());
    }
  }
}

void
PermEditBox::MakeDock ()
{
  qDebug () << " MakeDock ";
  setFloating (false);
}

void
PermEditBox::MakeFloat ()
{
  setFloating (true);
}

void
PermEditBox::CloseAction ()
{
  if (scin->isModified()) {
    AskSave ();
  }
  emit TitleGone (this);
  disconnect ();
  close ();
}

void
PermEditBox::LoadAction ()
{
  if (scin->isModified()) {
    AskSave ();
  }
  QString oldFile;
  QFileInfo info (currentFile);
  oldFile = QFileDialog::getOpenFileName (this, tr("Read File"),
                             info.path());
  if (oldFile.length() > 0) {
    LoadFile (oldFile);
  }
}

void
PermEditBox::LoadInsertAction ()
{
  QString oldFile;
  QFileInfo info (currentFile);
  oldFile = QFileDialog::getOpenFileName (this, tr("Read File"),
                             info.path());
  if (oldFile.length() > 0) {
    QFile file (oldFile);
    bool ok = file.open (QFile::ReadOnly);
    if (ok) {
      QByteArray bytes = file.readAll ();
      int row, col;
      scin->getCursorPosition (&row, &col);
      scin->insertAt (QString::fromUtf8(bytes.data()), row, col);
    }
  }
}


void
PermEditBox::SaveAction ()
{
  qDebug () << "Save Action called";
  if (currentFile.length() < 1) {
    SaveAsAction ();
  } else {
    QFile file (currentFile);
    bool ok = file.open (QFile::WriteOnly);
    if (ok) {
      qint64 bytesWritten = file.write (scin->text().toUtf8());
      ok = (bytesWritten == scin->text().toUtf8().size());
    }
    if (ok) {
      scin->setModified (false);
    } else {
      QMessageBox::warning (this, tr("Write Failed"),
                              tr("Could not write \n\"%1\"")
                                .arg (currentFile));
    }
  }
}

void
PermEditBox::SaveAsAction ()
{
  qDebug () << "Save As Action called";
  QString newFile;
  newFile = QFileDialog::getSaveFileName (this, tr("Save in File"),
                                currentFile);
  if (newFile.length() > 0) {
    QFile file (newFile);
    bool ok = file.open (QFile::WriteOnly);
    if (ok) {
      qint64 bytesWritten = file.write (scin->text().toUtf8());
      ok = (bytesWritten == scin->text().toUtf8().size());
    }
    if (ok) {
      currentFile = newFile;
      TitleChange (QFileInfo (currentFile).fileName());
      scin->setModified (false);
    } else {
      QMessageBox::warning (this, tr("Write Failed"),
                              tr("Could not write \n\"%1\"")
                                .arg (newFile));
    }
  }
}

void
PermEditBox::TitleChange (const QString & newTitle)
{
  setWindowTitle (newTitle);
  emit NewTitle (newTitle, this);
}

void
PermEditBox::AskSave ()
{
  QString msg (tr("File\n\"%1\"\nhas been modified\nSave ?").arg(currentFile));
  int reply = QMessageBox::question (this,
                              tr ("File Modified !"),
                             msg,
                              QMessageBox::Yes 
                            | QMessageBox::No );
  if (reply & QMessageBox::Yes) {
    SaveAction ();
  }
}

void
PermEditBox::SetTitle (QString newTitle)
{
  setWindowTitle (newTitle);
}

QString
PermEditBox::Title ()
{
  return windowTitle();
}

QsciScintilla &
PermEditBox::TextEdit ()
{
  return *scin;
}

void
PermEditBox::CursorChange (int line, int col)
{
  linePosLabel->setText (tr("Line %1 Col %2").arg(line).arg(col));
}

void
PermEditBox::closeEvent (QCloseEvent *event)
{
  qDebug () << " PermEditBox closed " << event->spontaneous ();
  qDebug () << "      obj name " << objectName();
  if (scin->isModified()) {
    AskSave ();
  }
  emit TitleGone (this);
  QDockWidget::closeEvent (event);
}

QString
PermEditBox::Text ()
{
  return scin->text();
}

void
PermEditBox::Clear ()
{
  scin->clear ();
  scin->setLexer (0);
  currentType = QString ("none");
  currentFile.clear ();
}

bool
PermEditBox::LoadFile (const QString & filename)
{
  QFileInfo  info (filename);
  QString suf = info.suffix ();
  QFile file (filename);
  bool ok = file.open (QFile::ReadOnly);
  if (ok) {
    quint64 size = file.size();
    QByteArray bytes = file.readAll();
    if (bytes.size() == 0 && size != 0) {
      file.close ();
      return false;
    }
    scin->setText (bytes);
    scin->setModified (false);
    QsciLexer * lex (0);
    if (suf.length() > 0) {
      lex = LexerChooser::Ref().NewLexerBySuffix (this,suf.toLower());
    } else if (info.fileName().toLower () == "makefile") {
      lex = LexerChooser::Ref().NewLexerByName (this,"Makefile");
    } else {
      if (bytes.startsWith ('#')) {
        lex = LexerChooser::Ref().NewLexerByName (this,"Bash");
      } else if (bytes.startsWith ("<?xml ") ||
                 bytes.startsWith ("<!DOCTYPE")) {
        lex = LexerChooser::Ref().NewLexerByName (this, "XML");
      }
    }
    scin->setLexer (lex);
    if (lex) {
      currentType = QString (lex->language());
      QFont font;
      bool customFont = FontChooser::Ref().LookupFont (currentType, font);
      if (customFont) {
        lex->setFont (font);
      }
    } else {
      currentType = QString ("none");
      QFont font;
      bool customFont = FontChooser::Ref().LookupFont (currentType, font);
      if (customFont) {
        scin->setFont (font);
      }
    }
    currentFile = file.fileName();
    TitleChange (QFileInfo (currentFile).fileName());
    return true;
  }
  return ok;
}

QString
PermEditBox::FileName ()
{
  return currentFile;
}

} // namespace

