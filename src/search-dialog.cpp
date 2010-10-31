
#include "search-dialog.h"

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
#include <Qsci/qsciscintilla.h>
#include <QTimer>

namespace permute
{

SearchDialog * SearchDialog::theOnly (0);

SearchDialog::SearchDialog (QWidget * parent)
  :QDialog (parent),
   isFirst (true)
{
  ui.setupUi (this);
  theOnly = this;
  Connect ();
}

void
SearchDialog::Connect ()
{
  connect (ui.cancelButton, SIGNAL (clicked()),
           this, SLOT (Cancel()));
  connect (ui.searchButton, SIGNAL (clicked()),
           this, SLOT (RunSearch ()));
  connect (ui.replaceButton, SIGNAL (clicked()),
           this, SLOT (RunReplace ()));
  connect (ui.replaceAllButton, SIGNAL (clicked()),
           this, SLOT (RunReplaceAll ()));
qDebug () << " Search Dialog done connecting";
}

SearchDialog &
SearchDialog::Ref ()
{
  if (theOnly == 0) {
    theOnly = new SearchDialog;
  }
  if (theOnly == 0) {
    qFatal ("Memory Allocation Error for SearchDialog");
  }
  return *theOnly;
}

void
SearchDialog::DoSearch (QsciScintilla * scin)
{
  if (scin == 0) {
    return;
  }
  latestScin = scin;
  QString needle;
  if (scin->hasSelectedText()) {
    needle = scin->selectedText();
    ui.needleEdit->setText (needle);
  }
  ui.replaceEdit->hide();
  ui.replaceLabel->hide();
  ui.replaceButton->hide();
  ui.replaceAllButton->hide();
  isFirst = true;
  show ();
}

void
SearchDialog::DoReplace (QsciScintilla * scin)
{
  if (scin == 0) {
    return;
  }
  latestScin = scin;
  QString needle;
  if (scin->hasSelectedText()) {
    needle = scin->selectedText();
    ui.needleEdit->setText (needle);
  }
  ui.replaceEdit->show();
  ui.replaceLabel->show();
  ui.replaceButton->show();
  ui.replaceAllButton->show();
  isFirst = true;
  show ();
}

void
SearchDialog::Cancel ()
{
  qDebug () << " Search cancelled";
  reject ();
}

void
SearchDialog::RunSearch ()
{
  qDebug () << " Run Search";
  FirstSearch ();
}

bool
SearchDialog::FirstSearch ()
{
  QString expr = ui.needleEdit->text();
  bool isReg = ui.checkRegular->isChecked ();
  bool isWrap = ui.checkWrap->isChecked ();
  bool isWord = ui.checkWord->isChecked ();
  bool isSense = ui.checkCase->isChecked ();
  isFirst = false;
  return latestScin->findFirst (expr, isReg, isSense, 
                         isWord, isWrap);
}

void
SearchDialog::RunReplace ()
{
  qDebug () << " Run Replace";
  latestScin->replace (ui.replaceEdit->text());
}

void
SearchDialog::RunReplaceAll ()
{
  qDebug () << " Run Replace All";
  bool found (true);
  if (isFirst) {
    found = FirstSearch ();
  }
  int row, col;
  latestScin->getCursorPosition (&row, &col);
  int nextRow (row), nextCol (col);
  int wrapped (0);
  while (found && wrapped < 2) {
    latestScin->replace (ui.replaceEdit->text());
    found = latestScin->findNext ();
    if (found) {
      latestScin->getCursorPosition (&nextRow, &nextCol);
      if (Less (nextRow, nextCol, row, col)) {
        wrapped ++;
      }
      row = nextRow; col = nextCol;
    }
  }
}

bool
SearchDialog::Less (int x1, int y1, int x2, int y2)
{
  if (x1 < x2) {
    return true;
  }
  if (x1 == x2 && y1 < y2) {
    return true;
  }
  return false;
}

} // namespace
