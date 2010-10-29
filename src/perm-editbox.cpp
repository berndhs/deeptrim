
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
#include "lexer-chooser.h"

namespace permute
{

PermEditBox::PermEditBox (const QString & title, 
               QWidget * parent, 
               Qt::WindowFlags flags)
  :QDockWidget (title, parent, flags),
   scin (0)
{
  setupUi (this);
  setWindowIcon (parent->windowIcon());
  setTitleBarWidget (0);
  topMenu = new QMenuBar (dockWidgetContents);
  fileMenu = new QMenu (tr("File..."), topMenu);
  configMenu = new QMenu (tr("Config"), topMenu);
  actionSave = new QAction (tr("Save"),this);
  actionSaveAs = new QAction (tr("Save As..."),this);
  actionFont = new QAction (tr("Font"),this);
  actionLang = new QAction (tr("File Type"),this);
  iconAction = new QAction (parent->windowIcon(), tr(""),this);
  iconAction->setVisible (false);
  iconAction->setToolTip (tr("%1 Editor")
                          .arg(QApplication::applicationName()));
  fileMenu->addAction (actionSave);
  fileMenu->addAction (actionSaveAs);
  configMenu->addAction (actionFont);
  configMenu->addAction (actionLang);
  topMenu->addAction (iconAction);
  topMenu->addAction (fileMenu->menuAction());
  topMenu->addAction (configMenu->menuAction());
  gridLayout->addWidget (topMenu,0,0,1,1);
  scin = new QsciScintilla (this);
  gridLayout->addWidget (scin, 1,0,1,1);
  Connect ();
}

void
PermEditBox::Connect ()
{
  connect (this, SIGNAL (NewTitle (QString)), 
          this, SLOT (SetTitle (QString)));
  connect (this, SIGNAL (dockLocationChanged (Qt::DockWidgetArea)),
           this, SLOT (DockMoved (Qt::DockWidgetArea)));
  connect (this, SIGNAL (topLevelChanged (bool)),
           this, SLOT (TopChanged (bool)));
  connect (actionSave, SIGNAL (triggered()),
           this, SLOT (SaveAction()));
  connect (actionSaveAs, SIGNAL (triggered()),
           this, SLOT (SaveAsAction ()));
  connect (actionFont, SIGNAL (triggered()),
           this, SLOT (FontAction ()));
  connect (actionLang, SIGNAL (triggered()),
           this, SLOT (LangAction ()));
  connect (iconAction, SIGNAL (triggered()),
           this, SLOT (IconAction ()));
}

void
PermEditBox::SetText (const QString & text)
{
  scin->setText (text);
}

void
PermEditBox::SetDefaultFont (const QFont & font, bool setNow)
{
  defaultFont = font;
  if (setNow) {
    scin->setFont (defaultFont);
  }
}

void
PermEditBox::DockMoved (Qt::DockWidgetArea area)
{
  qDebug () << "Dock Moved to " << area;
  if (isWindow()) {
    setWindowIcon (parentWidget()->windowIcon());
qDebug () << windowIcon();
  }
}

void
PermEditBox::TopChanged (bool isTop)
{
  qDebug () << " Top Changed " << isTop;
  iconAction->setVisible (isTop);
  if (isTop) {
    setWindowIcon (parentWidget()->windowIcon());
  }
}

void
PermEditBox::FontAction ()
{
  qDebug () << " Font Action called";
  QFont defont;
  if (scin->lexer()) {
    defont = scin->lexer()->defaultFont();
    qDebug () << " Lexer font " << defont; 
  } else {
    defont = scin->font();
    qDebug () << " Base font " << defont; 
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
    QsciLexer * newLex = LexerChooser::Ref().NewLexerDialog (this, scin, lang);
    if (newLex) {
      scin->setLexer (newLex);
    }
  }
}

void
PermEditBox::IconAction ()
{
  qDebug () << " IconAction ";
  setFloating (false);
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
    if (!ok) {
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
      emit NewTitle (QFileInfo (currentFile).fileName());
    } else {
      QMessageBox::warning (this, tr("Write Failed"),
                              tr("Could not write \n\"%1\"")
                                .arg (newFile));
    }
  }
}

void
PermEditBox::SetTitle (QString newTitle)
{
  setWindowTitle (newTitle);
}

QsciScintilla &
PermEditBox::TextEdit ()
{
  return *scin;
}

void
PermEditBox::resizeEvent (QResizeEvent *event)
{
  QDockWidget::resizeEvent (event);
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
    currentFile = file.fileName();
    emit NewTitle (QFileInfo (currentFile).fileName());
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

