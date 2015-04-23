
/******************************************************************************
   HotShots: Screenshot utility
   Copyright(C) 2011-2014  xbee@xbee.net

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *******************************************************************************/

#include "WindowGrabber.h"

#if defined(Q_WS_MAC)

#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QxtWindowSystem>

WindowGrabber::WindowGrabber()
{
}

WindowGrabber::~WindowGrabber()
{
}

// no code yet !!! but if you want to help ...

QPixmap WindowGrabber::grabCurrent( bool includeDecorations, QRect & )
{
    Q_UNUSED(includeDecorations)
    WId id = QxtWindowSystem::activeWindow();
    QRect winGeom = QxtWindowSystem::windowGeometry(id);
    QPixmap pixmap = QPixmap::grabWindow( QApplication::desktop()->winId(), winGeom.x(), winGeom.y(), winGeom.width(),winGeom.height() );

    return pixmap;
}

QPixmap WindowGrabber::grabCursor(QPoint &hotSpot)
{
    QPixmap pm;
    // TODO !!
    return pm;
}

#endif // Q_WS_MAC
