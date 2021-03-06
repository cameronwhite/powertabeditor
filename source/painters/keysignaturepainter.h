/*
  * Copyright (C) 2011 Cameron White
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
  
#ifndef PAINTERS_KEYSIGNATUREPAINTER_H
#define PAINTERS_KEYSIGNATUREPAINTER_H

#include <memory>
#include <QFont>
#include <QGraphicsItem>
#include <painters/layoutinfo.h>
#include <score/scorelocation.h>

class ScoreLocationPubSub;

class KeySignaturePainter : public QGraphicsItem
{
public:
    KeySignaturePainter(const LayoutConstPtr &layout, const KeySignature &key,
                        const ScoreLocation &location,
                        std::shared_ptr<ScoreLocationPubSub> pubsub);

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                       QWidget *) override;
    virtual QRectF boundingRect() const override
    {
        return myBounds;
    }

private:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *) override;
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *) override;

    LayoutConstPtr myLayout;
    const KeySignature &myKeySignature;
    const ScoreLocation myLocation;
    std::shared_ptr<ScoreLocationPubSub> myPubSub;
    QFont myMusicFont;
    const QRectF myBounds;
    QVector<double> myFlatPositions;
    QVector<double> mySharpPositions;

    void adjustHeightOffset(QVector<double> &lst);
    void drawAccidentals(QVector<double> &positions, QChar accidental,
                         QPainter *painter);
    void initAccidentalPositions();
};

#endif
