
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

#include <QtCore/QPointF>
#include <QtCore/QDebug>

#include <QtGui/QPolygonF>
#include <QtGui/QPen>
#include <QtGui/QBrush>

#include <QtGui/QPainter>
#include <QtGui/QGraphicsDropShadowEffect>
#include <QtGui/QColor>
#include <QtGui/QGraphicsRectItem>
#include <QtGui/QGraphicsSceneMouseEvent>

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "EditorRectangleItem.h"
#include "HandgripItem.h"

EditorRectangleItem::EditorRectangleItem(QGraphicsItem *parent)
    : BaseItem(parent),
    m_currentRotation(0.0),
    m_rotation(0.0),
    m_relativeRotation(0.0),
    m_currentSx(0.0),
    m_currentSy(0.0)
{
    m_type = metaObject()->className();
    m_item = new QGraphicsRectItem(this);
    m_item->setGraphicsEffect(m_dropShadowFx);

    // init with 2 dummy points
    m_line << QPointF();
    m_line << QPointF();

    // prepare handgrips
    m_handgrips << new HandgripItem(this,m_item,HandgripItem::TopLeft,HandgripItem::ROTATOR);
    m_handgrips << new HandgripItem(this,m_item,HandgripItem::TopCenter,HandgripItem::TRANSLATOR_Y);
    m_handgrips << new HandgripItem(this,m_item,HandgripItem::TopRight,HandgripItem::ROTATOR);
    m_handgrips << new HandgripItem(this,m_item,HandgripItem::MiddleLeft,HandgripItem::TRANSLATOR_X);
    m_handgrips << new HandgripItem(this,m_item,HandgripItem::MiddleRight,HandgripItem::TRANSLATOR_X);
    m_handgrips << new HandgripItem(this,m_item,HandgripItem::BottomLeft,HandgripItem::ROTATOR);
    m_handgrips << new HandgripItem(this,m_item,HandgripItem::BottomCenter,HandgripItem::TRANSLATOR_Y);
    m_handgrips << new HandgripItem(this,m_item,HandgripItem::BottomRight,HandgripItem::ROTATOR);

    updateHandgrips();
}

EditorRectangleItem::~EditorRectangleItem()
{ }

void EditorRectangleItem::setPen(const QPen &p)
{
    m_pen = p;
    m_item->setPen(p);
}

void EditorRectangleItem::setBrush(const QBrush &b)
{
    m_brush = b;
    m_item->setBrush(b);
}

QRectF EditorRectangleItem::boundingRect() const
{
    return mapFromScene( m_item->sceneBoundingRect() ).boundingRect();
}

QPainterPath EditorRectangleItem::shape() const
{
    return mapFromItem( m_item,m_item->shape() );
}

void EditorRectangleItem::mousePress ( QGraphicsSceneMouseEvent * mouseEvent )
{
    if (m_initialized) // to avoid move by post selection of item
        return;

    m_line[BEGIN] = mouseEvent->scenePos();

    m_initialized = true;
}

void EditorRectangleItem::mouseMove ( QGraphicsSceneMouseEvent * mouseEvent )
{
    m_line[END] = mouseEvent->scenePos();
    m_item->setRect( QRectF(m_line[BEGIN],m_line[END]).normalized() );
    HandgripItem::syncHandgrips( m_item->rect(), m_handgrips );
}

BaseItem::typModeEdition EditorRectangleItem::mouseRelease ( QGraphicsSceneMouseEvent * mouseEvent )
{
    Q_UNUSED(mouseEvent);
    return BaseItem::EDITION_NEXTITEM;
}

void EditorRectangleItem::updatePoint(const QPointF &pt)
{
    HandgripItem *hg = qobject_cast<HandgripItem *>( sender() );

    if (!hg)
        return;

    QRectF rect = m_item->rect();
    prepareGeometryChange();

    if (hg->getType() != HandgripItem::ROTATOR)
    {
        QRectF newRect = HandgripItem::applyTranslation(hg,rect,pt, m_item);

        m_line[BEGIN] = newRect.topLeft();
        m_line[END] = newRect.bottomRight();

        // must translate the rect
        QPointF origRef = m_item->mapToScene(rect.topLeft());
        QPointF origPre = makeRotation(m_rotation, m_item->mapToScene(rect.center())).map(origRef);

        QPointF origNew = m_item->mapToScene(newRect.topLeft());
        QPointF origPost = makeRotation(m_rotation, m_item->mapToScene(newRect.center())).map(origNew);

            qDebug() << origPost - origPre << rect << newRect << m_line;


        m_item->setRect( newRect );
        setPos(pos()+ origPost - origPre);
        rotate(0);

        HandgripItem::syncHandgrips( m_item->rect(), m_handgrips );
    }
    else
    {
        m_currentRotation = HandgripItem::applyRotation(hg,pt,m_item->sceneBoundingRect().center(),m_refRotationPoint);
        rotate(m_currentRotation);
    }
}

void EditorRectangleItem::initRotationReferencePoint(const QPointF &p)
{
    m_refRotationPoint = p;
}

void EditorRectangleItem::validRotationReferencePoint(const QPointF &p)
{
    Q_UNUSED(p);
    m_rotation += m_relativeRotation;
}

void EditorRectangleItem::rotate(double r)
{
    /*
    QTransform m;
    QPointF anchor = m_item->rect().center();

    m.translate( anchor.x(),anchor.y() );
    m.rotate(-m_rotation - r);
    m.translate( -anchor.x(),-anchor.y() );
*/
    m_item->setTransform(makeRotation(-m_rotation - r, m_item->rect()),false);

    m_relativeRotation = r;
}

void EditorRectangleItem::endEdition()
{
    // ensure rect is "normalized"
    QRectF norm = QRectF(m_line[BEGIN],m_line[END]).normalized();
    m_line[BEGIN] = norm.topLeft();
    m_line[END] = norm.bottomRight();

    // check validity
    if ( !QRectF(m_line[BEGIN],m_line[END]).isValid() )
        deleteLater();
}

void EditorRectangleItem::toXml(QDomElement &elt) const
{
    elt.setAttribute( "color", IOHelper::variantToString( m_item->pen().color() ) );
    elt.setAttribute( "bgColor", IOHelper::variantToString( m_item->brush().color() ) );
    elt.setAttribute( "rotation", QString::number( m_rotation,'f',1) );
    elt.setAttribute( "width", IOHelper::variantToString( m_item->pen().width() ) );
    if ( m_item->brush().style() == Qt::NoBrush)
        elt.setAttribute( "alpha", "0");
    else
        elt.setAttribute( "alpha", IOHelper::variantToString( m_item->brush().color().alpha() ) );
    elt.setAttribute( "rect", IOHelper::variantToString( m_item->rect() ) );
    elt.setAttribute( "pos", IOHelper::variantToString( pos() ) );
    elt.setAttribute( "style", IOHelper::variantToString( m_item->pen().style() ) );
    elt.setAttribute( "shadow", IOHelper::variantToString( isShadowEnabled() ) );
}

void EditorRectangleItem::fromXml(const QDomElement &elt)
{
    m_line.clear();
    m_initialized = true;
    bool ok = true;
    QRectF rect = IOHelper::stringToVariant(elt.attribute("rect"),QVariant::RectF,ok).toRectF();
    m_line << rect.topLeft();
    m_line << rect.bottomRight();

    setEnableShadow( IOHelper::stringToVariant(elt.attribute("shadow","true"),QVariant::Bool,ok).toBool() );
    QColor color = IOHelper::stringToVariant(elt.attribute("color"),QVariant::Color,ok).value<QColor>();
    int width = elt.attribute("width").toInt();
    int style = elt.attribute("style").toInt();
    int alpha = elt.attribute("alpha").toInt();
    m_rotation = elt.attribute("rotation").toDouble();
    QPointF pos = IOHelper::stringToVariant(elt.attribute("pos"),QVariant::PointF,ok).toPointF();
    setPos(pos);

    m_pen = QPen(color,width,(Qt::PenStyle)style);
    m_pen.setCapStyle(Qt::RoundCap);
    m_pen.setJoinStyle(Qt::RoundJoin);

    setPen(m_pen);
    color.setAlpha(alpha);
    m_brush = QBrush(color,Qt::SolidPattern); // just in case old format (no bg color)

    // read the bg if available
    if ( elt.hasAttribute("bgColor") )
    {
        QColor bgColor = IOHelper::stringToVariant(elt.attribute("bgColor"),QVariant::Color,ok).value<QColor>();
        bgColor.setAlpha(alpha);
        m_brush = QBrush(bgColor,Qt::SolidPattern);
    }

    if (alpha == 0)
        m_brush.setStyle(Qt::NoBrush);
    setBrush(m_brush);

    m_item->setRect( rect );
    HandgripItem::syncHandgrips( m_item->rect(), m_handgrips );
    rotate(0.0);
}

// copy constructor
BaseItem * EditorRectangleItem::clone()
{
    EditorRectangleItem *copiedItem = new  EditorRectangleItem();
    copyDataTo(copiedItem);
    return copiedItem;
}
