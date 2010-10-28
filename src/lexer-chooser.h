#ifndef LEXER_CHOOSER_H
#define LEXER_CHOOSER_H

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
#include "all-lexers.h"
#include <map>
#include <QString>

namespace permute
{


class  LexerChooser
{
public:

  QsciLexer * NewLexerByName (QWidget *parent, const QString & kind);
  QsciLexer * NewLexerBySuffix (QWidget *parent, const QString & kind);
  QsciLexer * NewLexerDialog (QWidget *parent, 
                              const QString & kind = QString ());

static LexerChooser & Ref ();

private:

  typedef std::map <QString, QsciLexer*(*)(QWidget*)>  NewFuncMapType;

  LexerChooser ();
  static LexerChooser * Instance ();

  void InitDefaultNames ();
  void InitDefaultSuffixes ();

  NewFuncMapType    newByName;
  NewFuncMapType    newBySuffix;
  
  static QsciLexer * NewLexerBash (QWidget *parent);
  static QsciLexer * NewLexerBatch (QWidget *parent);
  static QsciLexer * NewLexerCMake (QWidget *parent);
  static QsciLexer * NewLexerCPP (QWidget *parent);
  static QsciLexer * NewLexerCSharp (QWidget *parent);
  static QsciLexer * NewLexerCSS (QWidget *parent);
  static QsciLexer * NewLexerD (QWidget *parent);
  static QsciLexer * NewLexerDiff (QWidget *parent);
  static QsciLexer * NewLexerFortran (QWidget *parent);
  static QsciLexer * NewLexerFortran77 (QWidget *parent);
  static QsciLexer * NewLexerHTML (QWidget *parent);
  static QsciLexer * NewLexerIDL (QWidget *parent);
  static QsciLexer * NewLexerJava (QWidget *parent);
  static QsciLexer * NewLexerJavaScript (QWidget *parent);
  static QsciLexer * NewLexerLua (QWidget *parent);
  static QsciLexer * NewLexerMakefile (QWidget *parent);
  static QsciLexer * NewLexerPascal (QWidget *parent);
  static QsciLexer * NewLexerPerl (QWidget *parent);
  static QsciLexer * NewLexerPostScript (QWidget *parent);
  static QsciLexer * NewLexerPOV (QWidget *parent);
  static QsciLexer * NewLexerProperties (QWidget *parent);
  static QsciLexer * NewLexerPython (QWidget *parent);
  static QsciLexer * NewLexerRuby (QWidget *parent);
  static QsciLexer * NewLexerSpice (QWidget *parent);
  static QsciLexer * NewLexerSQL (QWidget *parent);
  static QsciLexer * NewLexerTCL (QWidget *parent);
  static QsciLexer * NewLexerTeX (QWidget *parent);
  static QsciLexer * NewLexerVerilog (QWidget *parent);
  static QsciLexer * NewLexerVHDL (QWidget *parent);
  static QsciLexer * NewLexerXML (QWidget *parent);
  static QsciLexer * NewLexerYAML (QWidget *parent);
  
};



} // namespace


#endif
