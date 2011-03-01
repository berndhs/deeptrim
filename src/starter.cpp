
#include "starter.h"

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

#include <QTimer>
#include <QFileInfo>

namespace permute 
{
PermuteStarter::PermuteStarter (QApplication & app,
                               const QStringList & configMessages,
                               const QStringList & argv,
                               QObject * parent)
  :QObject (parent),
   theApp (app),
   theConfigMessages (configMessages),
   theArgv (argv),
   permuteMain (0),
   remote (0),
   server (0),
   adapt (0),
   dbusConn (QDBusConnection::sessionBus ())
{
}

PermuteStarter::~PermuteStarter ()
{
  if (remote) {
    delete remote;
  }
}

void
PermuteStarter::Run (bool useDBus)
{
  if (useDBus) {
    remote = new local::BerndStramm::permute::IpcServer
                 (PERMUTE_DBUS_SERVICE,
                  "/Server",
                  QDBusConnection::sessionBus(),
                  this);
    if (remote->isValid ()) {  
      // found server, hand off the files and quit
      for (int a=0; a<theArgv.size(); a++) {
        QFileInfo info (theArgv.at(a));
        remote->TakeFile (info.absoluteFilePath());
      }
      QTimer::singleShot (100, this, SLOT (Quit ()));
    } else {
      // no server, have to do it myself
      RunPermute (useDBus);
    }
  } else {
    RunPermute (useDBus);
  }
}

void
PermuteStarter::Quit ()
{
  theApp.quit ();
}

void
PermuteStarter::RunPermute (bool useDBus)
{ 
  permuteMain = new Permute;
  permuteMain->setWindowTitle ("DeepTrim");
  theApp.setWindowIcon (permuteMain->windowIcon());
  permuteMain->Init (theApp);
  permuteMain->AddConfigMessages (theConfigMessages);
  permuteMain->CommandArgs (theArgv);

  if (useDBus) {
    server = new IpcServer (permuteMain, this);
    adapt = new IpcServerAdaptor (server);
    dbusConn.registerObject ("/Server",server);
    dbusConn.registerService (PERMUTE_DBUS_SERVICE);
  }
  permuteMain->Run ();
}

} // permute 

