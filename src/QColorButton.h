
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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *******************************************************************************/

#ifndef _QCOLORBUTTON_H_
#define _QCOLORBUTTON_H_

#include <QtGui/QPushButton>
#include <QtGui/QWidget>

class QLabel;

class QColorButton : public QPushButton
{
    Q_OBJECT

public:

    QColorButton(QWidget *parent);
    ~QColorButton();
    QColor getColor();
    void setColor(const QColor& color);

protected:

    void updateColorLabelSize();
    virtual void resizeEvent(QResizeEvent *);

protected slots:

    void click();
    void currentColorChanged(const QColor &color);

signals:

    void colorChanged(const QColor &);

protected:

    QLabel * colorLabel;
};

#endif // _QCOLORBUTTON_H_
