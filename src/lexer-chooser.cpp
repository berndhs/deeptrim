
#include "lexer-chooser.h"

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

#include <QDebug>
#include "pick-string.h"

namespace permute
{

static LexerChooser * theOnly (0);

LexerChooser & 
LexerChooser::Ref ()
{
  LexerChooser * theOne = Instance ();
  return *theOne;
}

LexerChooser *
LexerChooser::Instance ()
{
  if (theOnly == 0) {
    theOnly = new LexerChooser;
  }
  if (theOnly == 0) {
    qFatal ("Memory Allocation Failure");
  }
  return theOnly;
}

QsciLexer * 
LexerChooser::NewLexerByName (QWidget *lexParent, const QString & kind)
{
  if (newByName.find (kind) != newByName.end()) {
    return (*newByName[kind]) (lexParent);
  } else {
    return 0;
  }
}

QsciLexer *
LexerChooser::NewLexerBySuffix (QWidget *lexParent, const QString & kind)
{
  if (newBySuffix.find (kind) != newBySuffix.end()) {
    return (*newBySuffix [kind])(lexParent);
  } else {
    return 0;
  }
}

QsciLexer *
LexerChooser::NewLexerDialog (QWidget *dialogParent,
                              QWidget *lexParent, 
                              bool & chosen,
                              const QString &kind )
{
  QStringList languages;
  NewFuncMapType::iterator nit;
  for (nit = newByName.begin(); nit != newByName.end(); nit++) {
     languages << nit->first;
  }
  PickString  picker (dialogParent);
  picker.SetTitle (QObject::tr("Choose File Format"));
  int picked = picker.Pick (languages);
  if (picked) {
    QString lang = picker.Choice ();
    nit = newByName.find (lang);
    if (nit != newByName.end()) {
      QsciLexer * newLex = nit->second (lexParent);
      chosen = true;
      return newLex;
    }
  }
  chosen = false;
  return 0;
}

LexerChooser::LexerChooser ()
{
  InitDefaultNames ();
  InitDefaultSuffixes ();
}

void
LexerChooser::InitDefaultNames ()
{
  newByName [QObject::tr(" - none - ")] = LexerChooser::NewLexerNone;
  newByName ["Bash"] = LexerChooser::NewLexerBash; 
  newByName ["Batch"] = LexerChooser::NewLexerBatch; 
  newByName ["CMake"] = LexerChooser::NewLexerCMake; 
  newByName ["C++"] = LexerChooser::NewLexerCPP; 
  newByName ["C#"] = LexerChooser::NewLexerCSharp; 
  newByName ["CSS"] = LexerChooser::NewLexerCSS; 
  newByName ["D"] = LexerChooser::NewLexerD; 
  newByName ["Diff"] = LexerChooser::NewLexerDiff; 
  newByName ["Fortran"] = LexerChooser::NewLexerFortran; 
  newByName ["Fortran77"] = LexerChooser::NewLexerFortran77; 
  newByName ["HTML"] = LexerChooser::NewLexerHTML; 
  newByName ["IDL"] = LexerChooser::NewLexerIDL; 
  newByName ["Java"] = LexerChooser::NewLexerJava; 
  newByName ["JavaScript"] = LexerChooser::NewLexerJavaScript; 
  newByName ["Lua"] = LexerChooser::NewLexerLua; 
  newByName ["Makefile"] = LexerChooser::NewLexerMakefile; 
  newByName ["Pascal"] = LexerChooser::NewLexerPascal; 
  newByName ["Perl"] = LexerChooser::NewLexerPerl; 
  newByName ["PostScript"] = LexerChooser::NewLexerPostScript; 
  newByName ["POV"] = LexerChooser::NewLexerPOV; 
  newByName ["Properties"] = LexerChooser::NewLexerProperties; 
  newByName ["Python"] = LexerChooser::NewLexerPython; 
  newByName ["Ruby"] = LexerChooser::NewLexerRuby; 
  newByName ["SQL"] = LexerChooser::NewLexerSQL; 
  newByName ["TCL"] = LexerChooser::NewLexerTCL; 
  newByName ["TeX"] = LexerChooser::NewLexerTeX; 
  newByName ["VHDL"] = LexerChooser::NewLexerVHDL; 
  newByName ["XML"] = LexerChooser::NewLexerXML; 
  newByName ["YAML"] = LexerChooser::NewLexerYAML; 
#if QSCINTILLA_VERSION >= 0x020402
  newByName ["Spice"] = LexerChooser::NewLexerSpice; 
  newByName ["Verilog"] = LexerChooser::NewLexerVerilog; 
#endif
}

void
LexerChooser::InitDefaultSuffixes ()
{
  newBySuffix ["sh"] = LexerChooser::NewLexerBash;
  newBySuffix ["cpp"] = LexerChooser::NewLexerCPP;
  newBySuffix ["cxx"] = LexerChooser::NewLexerCPP;
  newBySuffix ["cc"] = LexerChooser::NewLexerCPP;
  newBySuffix ["c"] = LexerChooser::NewLexerCPP;
  newBySuffix ["desktop"] = LexerChooser::NewLexerProperties;
  newBySuffix ["ini"] = LexerChooser::NewLexerProperties;
  newBySuffix ["h"] = LexerChooser::NewLexerCPP;
  newBySuffix ["hh"] = LexerChooser::NewLexerCPP;
  newBySuffix ["hpp"] = LexerChooser::NewLexerCPP;
  newBySuffix ["hxx"] = LexerChooser::NewLexerCPP;
  newBySuffix ["html"] = LexerChooser::NewLexerHTML;
  newBySuffix ["htm"] = LexerChooser::NewLexerHTML;
  newBySuffix ["ps"] = LexerChooser::NewLexerPostScript;
  newBySuffix ["bat"] = LexerChooser::NewLexerBatch;
  newBySuffix ["css"] = LexerChooser::NewLexerCSS;
  newBySuffix ["pas"] = LexerChooser::NewLexerPascal;
  newBySuffix ["f"] = LexerChooser::NewLexerFortran;
  newBySuffix ["f77"] = LexerChooser::NewLexerFortran77;
  newBySuffix ["pl"] = LexerChooser::NewLexerPerl;
  newBySuffix ["py"] = LexerChooser::NewLexerPython;
  newBySuffix ["sql"] = LexerChooser::NewLexerSQL;
  newBySuffix ["tcl"] = LexerChooser::NewLexerTCL;
  newBySuffix ["tex"] = LexerChooser::NewLexerTeX;
  newBySuffix ["xml"] = LexerChooser::NewLexerXML;
  newBySuffix ["yaml"] = LexerChooser::NewLexerYAML;
  newBySuffix ["l"] = LexerChooser::NewLexerCPP;     // lex
  newBySuffix ["y"] = LexerChooser::NewLexerCPP;     // yacc
  // Qt stuff
  newBySuffix ["pro"] = LexerChooser::NewLexerMakefile; 
  newBySuffix ["pri"] = LexerChooser::NewLexerMakefile;
  newBySuffix ["ui"] = LexerChooser::NewLexerXML;
  newBySuffix ["qrc"] = LexerChooser::NewLexerXML;
}

QsciLexer *
LexerChooser::NewLexerNone (QWidget * parent)
{
  Q_UNUSED (parent)
  return 0;
}

QsciLexer *
LexerChooser::NewLexerBash (QWidget * parent)
{
  return new QsciLexerBash (parent);
}



QsciLexer *
LexerChooser::NewLexerBatch (QWidget * parent)
{
  return new QsciLexerBatch (parent);
}



QsciLexer *
LexerChooser::NewLexerCMake (QWidget * parent)
{
  return new QsciLexerCMake (parent);
}



QsciLexer *
LexerChooser::NewLexerCPP (QWidget * parent)
{
  return new QsciLexerCPP (parent);
}



QsciLexer *
LexerChooser::NewLexerCSharp (QWidget * parent)
{
  return new QsciLexerCSharp (parent);
}



QsciLexer *
LexerChooser::NewLexerCSS (QWidget * parent)
{
  return new QsciLexerCSS (parent);
}


QsciLexer *
LexerChooser::NewLexerD (QWidget * parent)
{
  return new QsciLexerD (parent);
}



QsciLexer *
LexerChooser::NewLexerDiff (QWidget * parent)
{
  return new QsciLexerDiff (parent);
}



QsciLexer *
LexerChooser::NewLexerFortran (QWidget * parent)
{
  return new QsciLexerFortran (parent);
}



QsciLexer *
LexerChooser::NewLexerFortran77 (QWidget * parent)
{
  return new QsciLexerFortran77 (parent);
}



QsciLexer *
LexerChooser::NewLexerHTML (QWidget * parent)
{
  return new QsciLexerHTML (parent);
}



QsciLexer *
LexerChooser::NewLexerIDL (QWidget * parent)
{
  return new QsciLexerIDL (parent);
}



QsciLexer *
LexerChooser::NewLexerJava (QWidget * parent)
{
  return new QsciLexerJava (parent);
}



QsciLexer *
LexerChooser::NewLexerJavaScript (QWidget * parent)
{
  return new QsciLexerJavaScript (parent);
}



QsciLexer *
LexerChooser::NewLexerLua (QWidget * parent)
{
  return new QsciLexerLua (parent);
}



QsciLexer *
LexerChooser::NewLexerMakefile (QWidget * parent)
{
  return new QsciLexerMakefile (parent);
}



QsciLexer *
LexerChooser::NewLexerPascal (QWidget * parent)
{
  return new QsciLexerPascal (parent);
}



QsciLexer *
LexerChooser::NewLexerPerl (QWidget * parent)
{
  return new QsciLexerPerl (parent);
}



QsciLexer *
LexerChooser::NewLexerPostScript (QWidget * parent)
{
  return new QsciLexerPostScript (parent);
}



QsciLexer *
LexerChooser::NewLexerPOV (QWidget * parent)
{
  return new QsciLexerPOV (parent);
}



QsciLexer *
LexerChooser::NewLexerProperties (QWidget * parent)
{
  return new QsciLexerProperties (parent);
}



QsciLexer *
LexerChooser::NewLexerPython (QWidget * parent)
{
  return new QsciLexerPython (parent);
}



QsciLexer *
LexerChooser::NewLexerRuby (QWidget * parent)
{
  return new QsciLexerRuby (parent);
}



QsciLexer *
LexerChooser::NewLexerSQL (QWidget * parent)
{
  return new QsciLexerSQL (parent);
}



QsciLexer *
LexerChooser::NewLexerTCL (QWidget * parent)
{
  return new QsciLexerTCL (parent);
}



QsciLexer *
LexerChooser::NewLexerTeX (QWidget * parent)
{
  return new QsciLexerTeX (parent);
}


#if QSCINTILLA_VERSION >= 0x020402
QsciLexer *
LexerChooser::NewLexerSpice (QWidget * parent)
{
  return new QsciLexerSpice (parent);
}



QsciLexer *
LexerChooser::NewLexerVerilog (QWidget * parent)
{
  return new QsciLexerVerilog (parent);
}
#endif



QsciLexer *
LexerChooser::NewLexerVHDL (QWidget * parent)
{
  return new QsciLexerVHDL (parent);
}



QsciLexer *
LexerChooser::NewLexerXML (QWidget * parent)
{
  return new QsciLexerXML (parent);
}



QsciLexer *
LexerChooser::NewLexerYAML (QWidget * parent)
{
  return new QsciLexerYAML (parent);
}




} // namespace

