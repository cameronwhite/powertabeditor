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

#ifndef APP_CARET_H
#define APP_CARET_H

#include <boost/signals2/signal.hpp>
#include <score/scorelocation.h>

class ScoreLocationPubSub;

/// Tracks the current location within the score.
class Caret
{
public:
    Caret(Score &score);

    ScoreLocation &getLocation();
    const ScoreLocation &getLocation() const;

    /// Returns whether the caret is in playback mode.
    bool isInPlaybackMode() const;
    /// Sets whether the caret is in playback mode.
    void setIsInPlaybackMode(bool set);

    /// Moves the caret left or right by the specified offset.
    void moveHorizontal(int offset);

    /// Moves the caret up or down to the next string, and wraps around
    /// if necessary.
    void moveVertical(int offset);

    /// Moves the caret to the first position in the staff.
    void moveToStartPosition();

    /// Moves the caret to the last position in the staff.
    void moveToEndPosition();

    /// Move to the given position index.
    void moveToPosition(int position);

    /// Moves the caret up/down by the given number of systems.
    void moveSystem(int offset);

    /// Moves the caret to the first system in the score.
    void moveToFirstSystem();

    /// Moves the caret to the last system in the score.
    void moveToLastSystem();

    /// Move to the given system index, optionally keeping the same staff index.
    bool moveToSystem(int system, bool keepStaff);

    /// Moves up or down by the given number of staves.
    void moveStaff(int offset);

    /// Move to the specified staff.
    void moveToStaff(int staff);

    /// Moves the caret to the next bar after the current position, or to the
    /// next system if necessary.
    bool moveToNextBar();

    /// Moves the caret to the bar before the current position, or to the
    /// previous system if necessary.
    void moveToPrevBar();

    typedef boost::signals2::signal<void ()> LocationChangedSlot;
    boost::signals2::connection subscribeToChanges(
            const LocationChangedSlot::slot_type &subscriber) const;

    std::shared_ptr<ScoreLocationPubSub> getSelectionPubSub() const;

private:
    /// Returns the last valid position in the system.
    int getLastPosition() const;
    /// Returns the last valid system index in the score.
    int getLastSystemIndex() const;

    void handleSelectionChanged(const ScoreLocation &location);

    ScoreLocation myLocation;
    bool myInPlaybackMode;

    /// Send out signals to subscribers whenever the location changes.
    mutable LocationChangedSlot onLocationChanged;

    /// Used to listen for signals about the selection / position changing.
    std::shared_ptr<ScoreLocationPubSub> mySelectionPubSub;
};

#endif
