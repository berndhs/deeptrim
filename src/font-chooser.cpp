
#include "font-chooser.h"

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
#include "deliberate.h"

using namespace deliberate;

namespace permute
{

FontChooser* FontChooser::theOnly (0);

FontChooser::FontChooser ()
{
}

FontChooser &
FontChooser::Ref ()
{
  if (theOnly == 0) {
    theOnly = new FontChooser();
    theOnly->Init ();
    if (theOnly == 0) {
      qFatal ("Memory Allocation Failure for FontChooser");
    }
  }
  return *theOnly;
}

void
FontChooser::StoreFont (const QString & key, const QFont & font)
{
  fontMap [key] = font;
  Settings().setValue (QString ("customfonts/%1").arg(key), font.toString());
  Settings().sync ();
  qDebug () << " set font for " << key;
}

void
FontChooser::Init ()
{
}

bool
FontChooser::LookupFont (const QString & key, QFont & font)
{
  if (fontMap.contains (key)) {
    font = fontMap [key];
    return true;
  }
  QString settKey (QString("customfonts/%1").arg(key));
  if (Settings().contains (settKey)) {
    QString fontString = Settings().value (settKey).toString ();
    font.fromString (fontString);
    fontMap [key] = font;
    return true;
  }
  return false;
}

} // namespace

