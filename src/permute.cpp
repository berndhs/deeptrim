
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
#include "perm-editbox.h"
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
#include <QFontDatabase>
#include <QFont>
#include <Qsci/qscilexer.h>
#include <Qsci/qscilexercpp.h>
#include <QListWidget>
#include <QObject>
#include <QObjectList>
#include <QListWidgetItem>

using namespace deliberate;

namespace permute
{
Permute::Permute (QWidget *parent)
  :QMainWindow (parent),
   app (0),
   configEdit (this),
   hiddenBox (false),
   helpView (0),
   again (false),
   tooltiplen (40)
{
  ui.setupUi (this);
  setDockOptions (QMainWindow::AnimatedDocks
                 | QMainWindow::AllowTabbedDocks
                 | QMainWindow::ForceTabbedDocks 
                 );
  setDockNestingEnabled (false);
  hiddenBox = new PermEditBox (tr("HiddenRight"), this);
  QMainWindow::addDockWidget (Qt::RightDockWidgetArea, hiddenBox);
  hiddenBox->hide ();
  configEdit.setWindowIcon (windowIcon());
  helpView = new HelpView (this);
  helpView->setWindowIcon (windowIcon());
  Connect ();
}

void
Permute::Connect ()
{
  connect (ui.actionQuit, SIGNAL (triggered()), this, SLOT (Quit()));
  connect (ui.actionRestart, SIGNAL (triggered()), this, SLOT (Restart()));
  connect (ui.actionAbout, SIGNAL (triggered()), this, SLOT (About()));
  connect (ui.actionLicense, SIGNAL (triggered()), this, SLOT (License()));
  connect (ui.actionSettings, SIGNAL (triggered()),
           this, SLOT (EditSettings()));
  connect (ui.actionNewFile, SIGNAL (triggered()), this, SLOT (NewFile()));
  connect (ui.actionOpenFile, SIGNAL (triggered()), this, SLOT (OpenFile()));
}

void
Permute::Init (QApplication & pa)
{
  app = &pa;
  connect (app, SIGNAL (lastWindowClosed()), this, SLOT (Exiting()));
  Settings().sync();
  initDone = true;
  qDebug () << " main is " << objectName() << size();
  //ListChildren (this);
}

void
Permute::ListChildren (QObject *parent, int prefixLen)
{
  QObjectList kids = parent->children ();
  QObjectList::iterator it;
  QString prefix;
  prefix.fill ('.',prefixLen);
  for (it = kids.begin(); it!= kids.end(); it++) {
    QString name = (*it)->objectName();
    QWidget * widg = dynamic_cast <QWidget*> (*it);
    if (widg) {
      qDebug () << " child widget " << QString ("%1%2")
                                       .arg (prefix).arg(name)
                << widg->size();
    } else {
      qDebug () << " child object " << QString ("%1%2")
                                     .arg(prefix).arg (name);
    }
    ListChildren (*it, prefixLen+1);
  }
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
  restoreGeometry (Settings().value ("states/maingeometry").toByteArray());
  restoreState (Settings().value ("states/mainstate").toByteArray());
  show ();
  Settings().sync ();
  NewFile ();
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
  Settings().setValue("states/maingeometry", saveGeometry());
  Settings().setValue("states/mainstate", saveState());
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
Permute::OpenFile ()
{
  QString oldFile;
  QFileInfo info (saveFile);
  oldFile = QFileDialog::getOpenFileName (this, tr("Read File"),
                             info.path(),
                             tr ("Any file (*)"));
  if (oldFile.length() > 0) {
    PermEditBox * newEdit = new PermEditBox (tr("File Edit"),this);
    bool ok = newEdit->LoadFile (oldFile);
    if (ok) {
      newEdit->setAllowedAreas (Qt::AllDockWidgetAreas);
      newEdit->setFeatures (QDockWidget::DockWidgetClosable 
                           | QDockWidget::DockWidgetMovable
                           | QDockWidget::DockWidgetFloatable
                           );
      AddDockWidget (Qt::RightDockWidgetArea, newEdit);
      NewTag (newEdit->Title(), newEdit);
      connect (newEdit, SIGNAL (NewTitle (QString, PermEditBox *)),
               this, SLOT (NewTag (QString, PermEditBox *)));
      connect (newEdit, SIGNAL (TitleGone (PermEditBox *)),
               this, SLOT (RemoveTag (PermEditBox *)));
    } else {
      delete newEdit;
    }
  }
}

void
Permute::AddDockWidget ( Qt::DockWidgetArea area, 
                        QDockWidget * dockwidget )
{
qDebug () << " add dock widget " << dockwidget << " in area " << area;
  QMainWindow::addDockWidget (area, dockwidget);
  tabifyDockWidget (hiddenBox, dockwidget);
  dockwidget->setFocus ();
  ListChildren (dockwidget);
}

void
Permute::AddDockWidget ( Qt::DockWidgetArea area, 
                        QDockWidget * dockwidget, 
                        Qt::Orientation orientation )
{
qDebug () << " add dock widget " << dockwidget << " in area " << area 
          << " oriented " << orientation;
  QMainWindow::addDockWidget (area, dockwidget, orientation);
  tabifyDockWidget (hiddenBox, dockwidget);
  dockwidget->setFocus ();
}

void
Permute::NewFile ()
{
  PermEditBox * newEdit = new PermEditBox (tr("New-File"), this);
  NewTag (newEdit->Title(), newEdit);
  connect (newEdit, SIGNAL (NewTitle (QString, PermEditBox *)),
               this, SLOT (NewTag (QString, PermEditBox *)));
  connect (newEdit, SIGNAL (TitleGone (PermEditBox *)),
               this, SLOT (RemoveTag (PermEditBox *)));
  AddDockWidget (Qt::RightDockWidgetArea, newEdit);
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

void
Permute::NewTag (QString tag, PermEditBox * box)
{
  qDebug () << " new tag " << tag;
  QListWidgetItem *item (0);
  if (titleItems.contains (box)) {
    item = titleItems [box];
  } else {
    item = new QListWidgetItem (ui.titleList);
    item->setText (tag);
    titleItems [box] = item;
    titleBoxes [item] = box;
    ui.titleList->addItem (item);
  }
  item->setText (tag);
}

void
Permute::RemoveTag (PermEditBox *box)
{
  qDebug () << " remove tag " << box->Title();
  if (titleItems.contains (box)) {
    QListWidgetItem * item = titleItems [box];
    int row = ui.titleList->row (item);
    ui.titleList->takeItem (row);
    titleItems.remove (box);
    titleBoxes.remove (item);
    delete item;
  }
}

} // namespace

