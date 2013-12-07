/*
  * Copyright (C) 2013 Cameron White
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

#include "voiceutils.h"

#include <boost/range/adaptor/reversed.hpp>

namespace VoiceUtils
{

FilteredPositionConstIterator getPositionsInRange(const Voice &voice, int left,
                                                  int right)
{
    auto range = voice.getPositions();
    return boost::adaptors::filter(range, InPositionRange(left, right));
}

InPositionRange::InPositionRange(int left, int right)
    : myLeft(left), myRight(right)
{
}

bool InPositionRange::operator()(const Position &pos) const
{
    return pos.getPosition() >= myLeft && pos.getPosition() <= myRight;
}

const Position *getNextPosition(const Voice &voice, int position)
{
    for (const Position &pos : voice.getPositions())
    {
        if (pos.getPosition() > position)
            return &pos;
    }

    return nullptr;
}

const Note *getNextNote(const Voice &voice, int position, int string)
{
    const Position *nextPos = getNextPosition(voice, position);
    return nextPos ? Utils::findByString(*nextPos, string) : nullptr;
}

const Note *getPreviousNote(const Voice &voice, int position, int string)
{
    const Note *note = nullptr;

    for (const Position &pos : boost::adaptors::reverse(voice.getPositions()))
    {
        if (pos.getPosition() < position)
        {
            note = Utils::findByString(pos, string);
            break;
        }
    }

    return note;
}

bool canTieNote(const Voice &voice, int position, const Note &note)
{
    const Note *prevNote = getPreviousNote(voice, position, note.getString());
    return prevNote && prevNote->getFretNumber() == note.getFretNumber();
}

bool canHammerOnOrPullOff(const Voice &voice, int position, const Note &note)
{
    const Note *nextNote = getNextNote(voice, position, note.getString());
    return nextNote && nextNote->getFretNumber() != note.getFretNumber();
}

bool hasNoteWithHammerOn(const Voice &voice, const Position &pos)
{
    for (const Note &note : pos.getNotes())
    {
        if (note.hasProperty(Note::HammerOnOrPullOff))
        {
            const Note *nextNote =
                getNextNote(voice, pos.getPosition(), note.getString());
            return nextNote && nextNote->getFretNumber() > note.getFretNumber();
        }
    }

    return false;
}
}