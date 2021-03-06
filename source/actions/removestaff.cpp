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
  
#include "removestaff.h"

#include <app/caret.h>
#include <score/system.h>

RemoveStaff::RemoveStaff(const ScoreLocation &location, Caret &caret)
    : QUndoCommand(QObject::tr("Remove Staff")),
      myLocation(location),
      myCaret(caret),
      myOriginalStaff(location.getStaff()),
      myIndex(location.getStaffIndex())
{
}

void RemoveStaff::redo()
{
    myLocation.getSystem().removeStaff(myIndex);
    // Ensure the caret is in a valid staff.
    myCaret.moveToStaff(
        std::min<int>(myIndex, myLocation.getSystem().getStaves().size() - 1));
}

void RemoveStaff::undo()
{
    myLocation.getSystem().insertStaff(myOriginalStaff, myIndex);
    myCaret.moveToStaff(myIndex);
}
