#ifndef SEARCH_DIALOG_H
#define SEARCH_DIALOG_H

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

#include <QDialog>
#include "ui_search.h"

class QsciScintilla;

namespace permute
{

class Permute;

class SearchDialog : public QDialog
{
Q_OBJECT
public:

  static SearchDialog & Ref ();
  
  void DoSearch (QsciScintilla * scin);
  void DoReplace (QsciScintilla * scin);

public slots:

  void Cancel ();

private slots:

  void RunSearch ();
  void RunReplace ();
  void RunReplaceAll ();

private:

  SearchDialog (QWidget *parent=0);

  bool FirstSearch ();

  void Connect ();

  bool Less (int x1, int y1, int x2, int y2);

  static SearchDialog * theOnly;

  friend class Permute;

  Ui_SearchDialog    ui;

  bool    isFirst;
  QsciScintilla  * latestScin;

}; 

} // namespace
#endif