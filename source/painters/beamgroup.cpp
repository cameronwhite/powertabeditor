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
  
#include "beamgroup.h"

#include <cmath>
#include <painters/layoutinfo.h>
#include <painters/musicfont.h>
#include <QFontMetricsF>
#include <QGraphicsItem>
#include <QPainterPath>
#include <QPen>

static const double FRACTIONAL_BEAM_WIDTH = 5.0;

static int getNumExtraBeams(const NoteStem &stem)
{
    // 16th note gets 1 extra beam, 32nd gets two, etc.
    // Calculate log_2 of the note duration, and subtract three (so log_2(16) - 3 = 1).
    return std::log(static_cast<double>(stem.getDurationType())) /
               std::log(2.0) - 3;
}

BeamGroup::BeamGroup(NoteStem::StemType direction, size_t start, size_t end)
    : myStemDirection(direction), myStartIndex(start), myEndIndex(end)
{
}

void BeamGroup::drawStems(QGraphicsItem *parent,
                          const std::vector<NoteStem> &stems,
                          const QFont &musicFont, const QFontMetricsF &fm,
                          const LayoutInfo &layout) const
{
    QList<QGraphicsItem *> symbols;
    QPainterPath stemPath;

    auto begin = stems.begin() + myStartIndex;
    auto end = stems.begin() + myEndIndex;
    const NoteStem &firstStem = *begin;
    const NoteStem &lastStem = *(end - 1);
    auto numStems = std::distance(begin, end);

    // Draw each stem.
    for (auto it = begin; it != end; ++it)
    {
        const NoteStem &stem = *it;

        stemPath.moveTo(stem.getX(), stem.getTop());
        stemPath.lineTo(stem.getX(), stem.getBottom());

        // Draw any symbols that use information about the stem, like staccato,
        // fermata, etc.
        if (stem.isStaccato())
            symbols << createStaccato(stem, musicFont);

        if (stem.hasFermata())
            symbols << createFermata(stem, musicFont, layout);

        if (stem.hasSforzando() || stem.hasMarcato())
            symbols << createAccent(stem, musicFont, layout);

        for (QGraphicsItem *symbol : symbols)
            symbol->setParentItem(parent);

        symbols.clear();
    }

    auto stemPathItem = new QGraphicsPathItem(stemPath);
    stemPathItem->setParentItem(parent);

    QPainterPath beamPath;

    // Draw connecting line.
    if (numStems > 1)
    {
        const double connectorHeight = firstStem.getStemEdge();

        beamPath.moveTo(firstStem.getX() + 0.5, connectorHeight);
        beamPath.lineTo(lastStem.getX(), connectorHeight);
    }

    drawExtraBeams(beamPath, begin, end);

    auto beams = new QGraphicsPathItem(beamPath);
    beams->setPen(QPen(Qt::black, 2.0, Qt::SolidLine, Qt::RoundCap));
    beams->setParentItem(parent);

    // Draw a note flag for single notes (eighth notes or less) or grace notes.
    if (numStems == 1 && NoteStem::canHaveFlag(firstStem))
    {
        QGraphicsItem *flag = createNoteFlag(firstStem, musicFont, fm);
        flag->setParentItem(parent);
    }
}

void BeamGroup::drawExtraBeams(QPainterPath &path,
                               std::vector<NoteStem>::const_iterator begin,
                               std::vector<NoteStem>::const_iterator end) const
{
    for (auto stem = begin; stem != end; ++stem)
    {
        std::vector<NoteStem>::const_iterator prevStem;
        if (stem != begin)
			prevStem = boost::prior(stem);

        auto nextStem = boost::next(stem);
        const Position::DurationType duration = stem->getDurationType();
        const Position::DurationType prevDuration = (stem != begin) ?
                    prevStem->getDurationType() : Position::SixtyFourthNote;
        const Position::DurationType nextDuration = (nextStem != end) ?
                    nextStem->getDurationType() : Position::SixtyFourthNote;

        const int extraBeams = getNumExtraBeams(*stem);

        const bool hasFullBeaming = stem != begin;
        const bool hasFractionalLeft =
            (duration > prevDuration) &&
            (nextStem == end || duration > nextDuration);
        const bool hasFractionalRight = !hasFractionalLeft && duration > nextDuration;

        if (hasFullBeaming || hasFractionalLeft || hasFractionalRight)
        {
            for (int i = 1; i <= extraBeams; i++)
            {
                double y = stem->getStemEdge();
                y += i * 3 * ((myStemDirection == NoteStem::StemUp) ? 1 : -1);

                double xStart = 0, xEnd = 0;

                if (hasFullBeaming && i <= getNumExtraBeams(*prevStem))
                {
                    xStart = prevStem->getX() + 0.5;
                    xEnd = stem->getX() - 1;
                }
                else if (hasFractionalRight)
                {
                    xStart = stem->getX() + 0.5;
                    xEnd = xStart + FRACTIONAL_BEAM_WIDTH;
                }
                else if (hasFractionalLeft)
                {
                    xEnd = stem->getX();
                    xStart = xEnd - FRACTIONAL_BEAM_WIDTH;
                }

                path.moveTo(xStart, y);
                path.lineTo(xEnd, y);
            }
        }
    }

}

QGraphicsItem *BeamGroup::createStaccato(const NoteStem &stem,
                                         const QFont &musicFont)
{
    // Draw the dot near either the top or bottom note of the position,
    // depending on stem direction.
    const double yPos = (stem.getStemType() == NoteStem::StemUp) ?
                stem.getBottom() - 25 : stem.getTop() - 43;

    const double xPos = (stem.getStemType() == NoteStem::StemUp) ?
                stem.getX() - 8 : stem.getX() - 2;

    QGraphicsTextItem *dot = new QGraphicsTextItem(QChar(MusicFont::Dot));
    dot->setFont(musicFont);
    dot->setPos(xPos, yPos);
    return dot;
}

QGraphicsItem *BeamGroup::createFermata(const NoteStem &stem,
                                        const QFont &musicFont,
                                        const LayoutInfo &layout)
{
    double y = 0;

    // Position the fermata directly above/below the staff if possible, unless
    // the note stem extends beyond the standard notation staff.
    // After positioning, offset the height due to the way that
    // QGraphicsTextItem positions text.
    if (stem.getStemType() == NoteStem::StemUp)
    {
        y = std::min(stem.getTop(), layout.getTopStdNotationLine());
        y -= 33;
    }
    else
    {
        y = std::max(stem.getBottom(), layout.getBottomStdNotationLine());
        y -= 25;
    }

    const QChar symbol = (stem.getStemType() == NoteStem::StemUp) ?
                MusicFont::FermataUp : MusicFont::FermataDown;
    QGraphicsSimpleTextItem *fermata = new QGraphicsSimpleTextItem(symbol);
    fermata->setFont(musicFont);
    fermata->setPos(stem.getX(), y);

    return fermata;
}

QGraphicsItem *BeamGroup::createAccent(const NoteStem &stem,
                                       const QFont &musicFont,
                                       const LayoutInfo &layout)
{
    double y = 0;

    // Position the accent directly above/below the staff if possible, unless
    // the note stem extends beyond the std. notation staff.
    // - It should be positioned opposite to the fermata symbols.
    // After positioning, offset the height due to the way that
    // QGraphicsTextItem positions text.
    if (stem.getStemType() == NoteStem::StemDown)
    {
        y = std::min(stem.getTop(), layout.getTopStdNotationLine());
        y -= 38;
    }
    else
    {
        y = std::max(stem.getBottom(), layout.getBottomStdNotationLine());
        y -= 20;
    }

    QChar symbol;
    if (stem.hasMarcato())
        symbol = MusicFont::Marcato;
    else if (stem.hasSforzando())
    {
        symbol = MusicFont::Sforzando;
        y += 3;
    }

    if (stem.isStaccato())
        y += (stem.getStemType() == NoteStem::StemUp) ? 7 : -7;

    QGraphicsSimpleTextItem *accent = new QGraphicsSimpleTextItem(symbol);
    accent->setFont(musicFont);
    accent->setPos(stem.getX(), y);

    return accent;
}

QGraphicsItem *BeamGroup::createNoteFlag(const NoteStem &stem,
                                         const QFont &musicFont,
                                         const QFontMetricsF &fm)
{
    Q_ASSERT(NoteStem::canHaveFlag(stem));

    QChar symbol = 0;

    // Choose the flag symbol, depending on duration and stem direction.
    if (stem.getStemType() == NoteStem::StemUp)
    {
        switch (stem.getDurationType())
        {
        case Position::EighthNote:
            symbol = MusicFont::FlagUp1;
            break;
        case Position::SixteenthNote:
            symbol = MusicFont::FlagUp2;
            break;
        case Position::ThirtySecondNote:
            symbol = MusicFont::FlagUp3;
            break;
        case Position::SixtyFourthNote:
            symbol = MusicFont::FlagUp4;
            break;
        default:
            break;
        }

        if (stem.isGraceNote())
            symbol = MusicFont::FlagUp1;
    }
    else
    {
        switch (stem.getDurationType())
        {
        case Position::EighthNote:
            symbol = MusicFont::FlagDown1;
            break;
        case Position::SixteenthNote:
            symbol = MusicFont::FlagDown2;
            break;
        case Position::ThirtySecondNote:
            symbol = MusicFont::FlagDown3;
            break;
        case Position::SixtyFourthNote:
            symbol = MusicFont::FlagDown4;
            break;
        default:
            break;
        }

        if (stem.isGraceNote())
            symbol = MusicFont::FlagDown1;
    }

    // Draw the symbol.
    const double y = stem.getStemEdge() - fm.ascent() - 5;
    QGraphicsTextItem *flag = new QGraphicsTextItem(symbol);
    flag->setFont(musicFont);
    flag->setPos(stem.getX() - 3, y);

    return flag;
}
