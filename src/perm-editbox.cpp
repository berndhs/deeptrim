
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
#include "lexer-chooser.h"

namespace permute
{

PermEditBox::PermEditBox (QWidget *parent)
  :QsciScintilla (parent)
{
}

void
PermEditBox::SetDefaultFont (const QFont & font, bool setNow)
{
  defaultFont = font;
  if (setNow) {
    setFont (defaultFont);
  }
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
    setText (bytes);
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
    setLexer (lex);
    currentFile = file.fileName();
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

