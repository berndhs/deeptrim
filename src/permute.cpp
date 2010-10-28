
#include "permute.h"

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

#include "deliberate.h"
#include "version.h"
#include "delib-debug.h"
#include <QMessageBox>
#include <QTimer>
#include <QFile>
#include <QFileDialog>
#include <QDomDocument>
#include <QDomElement>
#include <QProcess>
#include <QMenu>
#include <QAction>
#include <QClipboard>
#include <QUrl>
#include <QDesktopServices>
#include <QTimer>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexer.h>
#include <Qsci/qscilexercpp.h>
#include <QFontDatabase>

using namespace deliberate;

namespace permute
{
Permute::Permute (QWidget *parent)
  :QMainWindow (parent),
   app (0),
   configEdit (this),
   again (false),
   nextPara (1),
   tooltiplen (40),
   trashCollect (0)
{
  ui.setupUi (this);
  configEdit.setWindowIcon (windowIcon());
  QsciLexer * lex = new QsciLexerCPP (this);
  textEdit = new QsciScintilla (this);
  QSizePolicy sp = ui.headerList->sizePolicy();
  int horizontal = sp.horizontalStretch ();
  sp.setHorizontalStretch (horizontal * 3);
  textEdit->setSizePolicy (sp);
  ui.editSplitter->addWidget (textEdit);
  textEdit->setLexer (lex);
  Connect ();
  trashCollect = new QTimer (this);
  connect (trashCollect, SIGNAL (timeout()), this, SLOT (CleanTrash()));
  trashCollect->start (2*60*1000);
}

void
Permute::Connect ()
{
  connect (ui.actionQuit, SIGNAL (triggered()), this, SLOT (Quit()));
  connect (ui.actionRestart, SIGNAL (triggered()), this, SLOT (Restart()));
  connect (ui.storeButton, SIGNAL (clicked()), this, SLOT (StorePara()));
  connect (ui.newButton, SIGNAL (clicked()), this, SLOT (NewPara ()));
  connect (ui.actionAbout, SIGNAL (triggered()), this, SLOT (About()));
  connect (ui.actionSettings, SIGNAL (triggered()),
           this, SLOT (EditSettings()));

  connect (ui.actionSave, SIGNAL (triggered()), this, SLOT (Save()));
  connect (ui.actionOpen, SIGNAL (triggered()), this, SLOT (Load()));
  connect (ui.actionWriteTxt, SIGNAL (triggered()), this, SLOT (SaveText()));
  connect (ui.actionWriteHtml, SIGNAL (triggered()),
           this, SLOT (SaveHtml()));
  connect (ui.actionImportPara, SIGNAL (triggered()),
           this, SLOT (ImportPara()));

  connect (ui.headerList, SIGNAL (itemClicked (QListWidgetItem*)),
           this, SLOT (ClickedHeader (QListWidgetItem*)));
}

void
Permute::Init (QApplication & pa)
{
  app = &pa;
  connect (app, SIGNAL (lastWindowClosed()), this, SLOT (Exiting()));
  Settings().sync();
  initDone = true;
}

void
Permute::AddConfigMessages (const QStringList & msgs)
{
  configMessages = msgs;
}

void
Permute::CommandArgs (const QStringList & asl)
{
  args = asl;
}

void
Permute::Run ()
{
  qDebug () << " Permute Run";
  QSize defaultSize = size();
  QSize newsize = Settings().value ("sizes/main", defaultSize).toSize();
  resize (newsize);
  Settings().setValue ("sizes/main",newsize);
  saveFile = Settings().value ("files/lastfile",saveFile).toString();
  Settings().setValue ("files/lastfile",saveFile);
  tooltiplen = Settings().value ("styles/tooltiplen",tooltiplen).toInt();
  Settings().setValue ("styles/tooltiplen",tooltiplen);
  show ();
  Settings().sync ();
  QFontDatabase fdb;
  QStringList fontlist;
  fontlist << "Available Fonts:";
  fontlist +=  fdb.families ();
  textEdit->setText (fontlist.join("\n"));
}

void
Permute::Quit ()
{
  qDebug () << " Permute Quit";
  CloseCleanup ();
  if (app) {
    app->quit ();
  }
}

void
Permute::closeEvent (QCloseEvent *event)
{
  qDebug () << " close event " << event;
  Quit ();
}

void
Permute::Restart ()
{
  again = true;
  qDebug () << " called restart";
  Quit ();
}

bool
Permute::Again ()
{
  bool doAgain = again;
  again = false;
  return doAgain;
}

void
Permute::CloseCleanup ()
{
  QSize currentSize = size();
  Settings().setValue ("sizes/main",currentSize);
  Settings().setValue ("files/lastfile",saveFile);
  Settings().sync();
}


void
Permute::About ()
{
  QString version (deliberate::ProgramVersion::Version());
  QStringList messages;
  messages.append (version);
  messages.append (configMessages);

  QMessageBox  box;
  box.setText (version);
  box.setDetailedText (messages.join ("\n"));
  QTimer::singleShot (30000, &box, SLOT (accept()));
  box.exec ();
}

void
Permute::EditSettings ()
{
  configEdit.Exec ();
}

void
Permute::StorePara ()
{
  QString pname = ui.textName->text();
  QString body = textEdit->text ();
  if (pname.length() == 0) {
    pname = tr ("Paragraph %1").arg (nextPara);
    ui.textName->setText (pname);
  }
  if (!paragraphs.contains(pname)) {
    QListWidgetItem *newItem = new QListWidgetItem (pname);
    newItem->setToolTip (body.left (tooltiplen));
    ui.headerList->addItem (newItem);
  }
  paragraphs [pname] = body;
}

void
Permute::NewPara ()
{
  if (textEdit->text().trimmed ().length() > 0) {
    StorePara();
  }
  textEdit->clear ();
  QString pname = tr("Paragraph %1").arg (nextPara);
  ui.textName->setText (pname);
  nextPara++;
}

void
Permute::ClickedHeader (QListWidgetItem *item)
{
  CellMenu (item);
}

void
Permute::OpenItemPara (const QListWidgetItem *item)
{
  QString oldbody = textEdit->text().trimmed();
  if (oldbody.length() > 0) {
    StorePara ();
  }
  if (item) {
    QString head = item->text ();
    if (paragraphs.contains (head)) {
      QString body = paragraphs [head];
      textEdit->setText (body);
      ui.textName->setText (head);
    }
  }
}

QAction *
Permute::CellMenu (const QListWidgetItem *cell,
                   const QList<QAction *> extraActions)
{
  if (cell == 0) {
    return 0;
  }
  QMenu menu (this);
  QAction * openAction = new QAction (tr("Open Paragraph"),this);
  QAction * copyAction = new QAction (tr("Copy Paragraph"),this);
  QAction * deleteAction = new QAction (tr("Delete Paragraph"),this);
  menu.addAction (openAction);
  menu.addAction (copyAction);
  menu.addSeparator ();
  menu.addAction (deleteAction);
  if (extraActions.size() > 0) {
    menu.addSeparator ();
  }
  for (int a=0; a < extraActions.size(); a++) {
    menu.addAction (extraActions.at (a));
  }
  
  QAction * select = menu.exec (QCursor::pos());
  if (select == openAction) {
    OpenItemPara (cell);
    return 0;
  } else if (select == copyAction) {
    QClipboard * clip = QApplication::clipboard ();
    if (clip) {
      clip->setText (paragraphs [cell->text()]);
    }
    return 0;
  } else if (select == deleteAction) {
    paragraphs.remove (cell->text()); 
    int row = ui.headerList->row (cell);
    QListWidgetItem * gone = ui.headerList->takeItem (row);
    deadItems.append (gone);
    return 0;
  } else {
    return select;
  }
}

void
Permute::Save ()
{
  QString newFile;
  newFile = QFileDialog::getSaveFileName (this, tr("Save in File"),
                                saveFile,
                                tr ("XML file (*.xml);; Any file (*)"));
  if (newFile.length() > 0) {
    QFile file (newFile);
    file.open (QFile::WriteOnly);
    WriteDom (&file);
    file.close ();
    saveFile = newFile;
  }
}

void
Permute::SaveText ()
{
  QString newFile;
  QFileInfo  info (newFile);
  
  newFile = QFileDialog::getSaveFileName (this, tr("Write to File"),
                                info.path(),
                                tr ("Text file (*.txt);; Any file (*)"));
  if (newFile.length() > 0) {
    QFile file (newFile);
    file.open (QFile::WriteOnly);
    WriteText (&file);
    file.close ();
    saveFile = newFile;
  }
}

void
Permute::SaveHtml ()
{
  QString newFile;
  QFileInfo  info (newFile);
  
  newFile = QFileDialog::getSaveFileName (this, tr("Write to File"),
                                info.path(),
                                tr ("Html file (*.html);; Any file (*)"));
  if (newFile.length() > 0) {
    QString tmpname (newFile);
    tmpname.append (".txt");
    QFile tmpfile (tmpname);
    tmpfile.open (QFile::WriteOnly);
    WriteText (&tmpfile);
    tmpfile.close ();
    QProcess *convert = new QProcess (this);
    QString convertProg ("asciidoc");
    QStringList convArgs;
    convArgs << "-o" << newFile << tmpname;
    qDebug () << " calling process " << convertProg << " with " << convArgs;
    convert->start (convertProg, convArgs);
  }
}

void
Permute::ImportPara ()
{
  QString newFile;
  QFileInfo  info (newFile);
  
  newFile = QFileDialog::getOpenFileName (this, tr("Read from File"),
                                info.filePath());
  if (newFile.length() > 0) {
    QFile file (newFile);
    file.open (QFile::ReadOnly);
    QByteArray body = file.readAll();
    if (body.size() > 0) {
      NewPara ();
      textEdit->setText (body);
    }
  }
}


void
Permute::Load ()
{
  QString oldFile;
  oldFile = QFileDialog::getOpenFileName (this, tr("Load Document from"),
                             saveFile,
                             tr ("XML files (*.xml);; Any file (*)"));
  if (oldFile.length() > 0) {
    QFile file (oldFile);
    file.open (QFile::ReadOnly);
    ReadDom (&file);
    file.close ();
    saveFile = oldFile;
  }
}

void
Permute::WriteDom (QIODevice * dest)
{
  QDomDocument doc ("Story");
  QDomElement main = doc.createElement ("Main");
  doc.appendChild (main);
  int nr = ui.headerList->count();
  for (int r=0; r<nr; r++) {
    QString head = ui.headerList->item(r)->text();
    QString body;
    if (paragraphs.contains (head)) {
      body = paragraphs [head];
    }
    QDomElement section = doc.createElement ("section");
    section.setAttribute ("title", head);
    QDomText text = doc.createTextNode (body);
    section.appendChild (text);
    main.appendChild (section);
  }
  dest->write (doc.toString().toUtf8());
}

void
Permute::ReadDom (QIODevice * source)
{
  QDomDocument  doc;
  doc.setContent (source);
  ui.headerList->clear ();
  textEdit->clear ();
  ui.textName->clear ();
  paragraphs.clear ();
  QDomElement main = doc.documentElement ();
  QDomElement section = main.firstChildElement ();
  while (!section.isNull()) {
     QString head = section.attribute ("title");
     QString body = section.text ();
     paragraphs [head] = body;
     QListWidgetItem *newItem = new QListWidgetItem (head);
     newItem->setToolTip (body.left (tooltiplen));
     ui.headerList->addItem (newItem);
     section = section.nextSiblingElement();
  }
}

void
Permute::WriteText (QIODevice * dest)
{
  QByteArray eol ("\n");
  dest->write (tr ("The Story").toUtf8());
  dest->write (eol);
  dest->write (QByteArray ("========="));
  dest->write (eol);
  dest->write (eol);
  int nr = ui.headerList->count();
  for (int r=0; r<nr; r++) {
    QString head = ui.headerList->item(r)->text();
    QString body;
    if (paragraphs.contains (head)) {
      body = paragraphs [head];
    }
    dest->write (QString("== %1 ==").arg (head).toUtf8());
    dest->write (eol);
    dest->write (eol);
    dest->write (body.toUtf8());
    dest->write (eol);
    dest->write (eol);
    dest->write (eol);
  }
}

void
Permute::CleanTrash ()
{
  QList<QListWidgetItem*>::iterator  dit;
  for (dit = deadItems.begin(); dit != deadItems.end(); dit++) {
    delete *dit;
  }
  deadItems.clear ();
}

void
Permute::Exiting ()
{
  CloseCleanup ();
}

void
Permute::License ()
{
  if (helpView) {
    helpView->Show ("qrc:/help/LICENSE.txt");
  }
}

} // namespace

