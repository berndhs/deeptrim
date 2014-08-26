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
#include <Qsci/qsciscintilla.h>

using namespace deliberate;
 
namespace permute
{
Permute::Permute (QWidget *parent)
  :QMainWindow (parent),
   app (0),
   configEdit (this),
   hiddenBox (0),
   helpView (0),
   searcher (this),
   again (false),
   tooltiplen (40),
   emphedBox (0),
   useless (42)
{
  ui.setupUi (this);
  setDockOptions (QMainWindow::AnimatedDocks
                 | QMainWindow::AllowTabbedDocks
                 | QMainWindow::ForceTabbedDocks 
                 );
  setDockNestingEnabled (false);
  normalSpacerSize = ui.rightSpace->sizeHint ();
  smallSpacerSize = normalSpacerSize;
  smallSpacerSize.setWidth (1);
  hiddenBox = new PermEditBox (tr("HiddenRight"), this);
  QMainWindow::addDockWidget (Qt::RightDockWidgetArea, hiddenBox);
  hiddenBox->hide ();
  configEdit.setWindowIcon (windowIcon());
  helpView = new HelpView (this);
  helpView->setWindowIcon (windowIcon());
  Connect ();
  setStyleSheet (
       " QDockWidget { "
       "     border-top: 3px solid blue;"
       "     border-left: 2px solid lightblue;"
       "     border-right: 2px solid lightblue;"
       "     border-bottom: 1px solid lightgray;"
       " }"
       " QDockWidget::title { "
       "   text-align: left; "
       "   font-style: italic; "
       "   background: white; "
       "   padding-left: 20px; "
       " } "
       );
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
  connect (ui.titleList, SIGNAL (itemClicked (QListWidgetItem*)),
           this, SLOT (TitleItemClicked (QListWidgetItem*)));
}

void
Permute::Init (QApplication & pa)
{
  app = &pa;
  connect (app, SIGNAL (lastWindowClosed()), this, SLOT (Exiting()));
  QString normalStyle ("");
  QString emphStyle ("background-color: lightgreen");
  normalStyle = Settings().value ("editbox/normalstyle",normalStyle).toString();
  emphStyle = Settings().value ("editbox/emphstyle",emphStyle).toString();
  Settings().setValue ("editbox/normalstyle",normalStyle);
  Settings().setValue ("editbox/emphstyle",emphStyle);
  QString mainStyle = Settings().value ("mainbox/stylesheet",
                         styleSheet()).toString();
  Settings().setValue ("mainbox/stylesheet",mainStyle);
  setStyleSheet (mainStyle);
  Settings().sync();
  initDone = true;
  qDebug () << " main window style " << styleSheet();
}

void
Permute::AdjustSpace ()
{
  if (titleItems.size() < 1) {
    ui.rightSpace->changeSize (smallSpacerSize.width (),
                               smallSpacerSize.height (),
                               QSizePolicy::Fixed,
                               QSizePolicy::Minimum);
  } else {
    ui.rightSpace->changeSize (smallSpacerSize.width (),
                               smallSpacerSize.height (),
                               QSizePolicy::Expanding,
                               QSizePolicy::Minimum);
  }
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
  if (args.size() > 0) {
    QStringList::iterator sit;
    for (sit = args.begin(); sit != args.end(); sit++) {
      OpenFile (*sit);
    }
  }
  AdjustSpace ();
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
  QStringList files;
  QFileInfo info (saveFile);
  files = QFileDialog::getOpenFileNames (this, tr("Open File"),
                             info.path(),
                             tr ("Any file (*)"));
  if (files.size() > 0) {
    for (int i=0; i<files.size(); i++) {
      OpenFile (files[i]);
    }
  }
}

void
Permute::OpenFile (const QString & filename)
{
  PermEditBox * newEdit = new PermEditBox (tr("File Edit"),this);
  bool ok = newEdit->LoadFile (filename);
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
    qDebug () << " new Edit stylesheet " << newEdit->styleSheet ();
    newEdit->raise ();
    newEdit->setFocus ();
  } else {
    delete newEdit;
  }
  AdjustSpace ();
}

void
Permute::AddDockWidget ( Qt::DockWidgetArea area, 
                        QDockWidget * dockWidget )
{
qDebug () << " add dock widget " << dockWidget << " in area " << area;
  QMainWindow::addDockWidget (area, dockWidget);
  tabifyDockWidget (hiddenBox, dockWidget);
  dockWidget->setFocus ();
  QTimer::singleShot (150, dockWidget, SLOT (raise()));
}

void
Permute::AddDockWidget ( Qt::DockWidgetArea area, 
                        QDockWidget * dockWidget, 
                        Qt::Orientation orientation )
{
qDebug () << " add dock widget " << dockWidget << " in area " << area 
          << " oriented " << orientation;
  QMainWindow::addDockWidget (area, dockWidget, orientation);
  tabifyDockWidget (hiddenBox, dockWidget);
  dockWidget->setFocus ();
  QTimer::singleShot (150, dockWidget, SLOT (raise()));
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
  AdjustSpace ();
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
  if (titleItems.contains (box)) {
    QListWidgetItem * item = titleItems [box];
    int row = ui.titleList->row (item);
    ui.titleList->takeItem (row);
    titleItems.remove (box);
    titleBoxes.remove (item);
    if (box == emphedBox) {
      box->SetEmphasis (false);
      emphedBox = 0;
    }
    delete item;
    AdjustSpace ();
  }
}

void
Permute::TitleItemClicked (QListWidgetItem *item)
{
  if (item) {
    if (titleBoxes.contains (item)) {
      PermEditBox * box = titleBoxes [item];
      box->raise ();
      box->setFocus ();
      if (emphedBox && emphedBox != box) {
        emphedBox->SetEmphasis (false);
        emphedBox->lower ();
      }
      emphedBox = box;
      emphedBox->SetEmphasis (true);
    }
  }
}

} // namespace

