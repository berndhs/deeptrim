
#include "main.h"

/****************************************************************
 * This file is distributed under the following license:
 *
 * Copyright (C) 2010,2011 Bernd Stramm
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


#include <QApplication>
#include <Qsci/qsciglobal.h>
#include "deliberate.h"
#include "version.h"
#include "cmdoptions.h"
#include "starter.h"


int
main (int argc, char *argv[])
{
  QCoreApplication::setOrganizationName ("BerndStramm");
  QCoreApplication::setOrganizationDomain ("bernd-stramm.com");
  QCoreApplication::setApplicationName ("deeptrim");
  deliberate::ProgramVersion pv ("DeepTrim");
  QCoreApplication::setApplicationVersion (pv.Version());
  QSettings  settings;
  deliberate::InitSettings ();
  deliberate::SetSettings (settings);
  settings.setValue ("program",pv.MyName());


  QStringList  configMessages;

  deliberate::CmdOptions  opts ("deeptrim");
  opts.AddSoloOption ("debug","D",
                       QObject::tr("show Debug log window"));
  opts.AddStringOption ("logdebug","L",
                       QObject::tr("write Debug log to file"));
  opts.AddSoloOption ("alone","A",
                       QObject::tr("run sepate from other instances"));

  deliberate::UseMyOwnMessageHandler ();

  bool optsOk = opts.Parse (argc, argv);
  if (!optsOk) {
    opts.Usage ();
    exit(1);
  }
  if (opts.WantHelp ()) {
    opts.Usage ();
    exit (0);
  }
  configMessages.append (QObject::tr("Build with Qt %1").arg(QT_VERSION_STR));
  configMessages.append (QObject::tr("Running with Qt %1").arg(qVersion()));
  configMessages.append (QObject::tr("Using QScintilla %1")
                            .arg(QSCINTILLA_VERSION_STR));
  if (opts.WantVersion ()) {
    pv.CLIVersion ();
    for (int cm=0; cm<configMessages.size(); cm++) {
      deliberate::StdOut () << configMessages[cm] << endl;
    }
    exit (0);
  }
  bool showDebug = opts.SeenOpt ("debug");
  int result;
  QApplication  app (argc, argv);

#if DELIBERATE_DEBUG
  deliberate::StartDebugLog (showDebug);
  bool logDebug = opts.SeenOpt ("logdebug");
  if (logDebug) {
    QString logfile ("/dev/null");
    opts.SetStringOpt ("logdebug",logfile);
    deliberate::StartFileLog (logfile);
  }
#endif
  permute::PermuteStarter Starter (app, configMessages, opts.Arguments());
  Starter.Run (!opts.SeenOpt ("alone"));
  result = app.exec ();
  qDebug () << " QApplication exec finished " << result;
  return result;
}
