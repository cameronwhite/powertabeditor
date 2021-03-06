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

#include "settings.h"

#include <QDataStream>
#include <QString>
#include <QVector>
#include <score/generalmidi.h>

namespace Settings
{
    const char *APP_PREVIOUS_DIRECTORY = "app/previousDirectory";
    const char *APP_RECENT_FILES = "app/recentFiles";
    const char *APP_WINDOW_STATE = "app/windowState";

    const char *MIDI_PREFERRED_API = "midi/preferredApi";
    const int MIDI_PREFERRED_API_DEFAULT = 0;

    const char *MIDI_PREFERRED_PORT = "midi/preferredPort";
    const int MIDI_PREFERRED_PORT_DEFAULT = 0;

    const char *MIDI_VIBRATO_LEVEL = "midi/vibrato";
    const int MIDI_VIBRATO_LEVEL_DEFAULT = 85;

    const char *MIDI_WIDE_VIBRATO_LEVEL = "midi/wideVibrato";
    const int MIDI_WIDE_VIBRATO_LEVEL_DEFAULT = 127;

    const char *MIDI_METRONOME_ENABLED = "midi/metronomeEnabled";
    const bool MIDI_METRONOME_ENABLED_DEFAULT = true;

    const char *MIDI_METRONOME_PRESET = "midi/metronomePreset";
    const int MIDI_METRONOME_PRESET_DEFAULT =
        Midi::MIDI_PERCUSSION_PRESET_HI_WOOD_BLOCK;

    const char *MIDI_METRONOME_STRONG_ACCENT = "midi/metronomeStrongAccent";
    const int MIDI_METRONOME_STRONG_ACCENT_DEFAULT = 127;

    const char *MIDI_METRONOME_WEAK_ACCENT = "midi/metronomeWeakAccent";
    const int MIDI_METRONOME_WEAK_ACCENT_DEFAULT = 80;

    const char *MIDI_METRONOME_ENABLE_COUNTIN = "midi/metronomeEnableCountIn";
    const bool MIDI_METRONOME_ENABLE_COUNTIN_DEFAULT = true;

    const char *MIDI_METRONOME_COUNTIN_PRESET = "midi/metronomeCountInPreset";
    const int MIDI_METRONOME_COUNTIN_PRESET_DEFAULT =
        Midi::MIDI_PERCUSSION_PRESET_RIDE_CYMBAL2;

    const char *MIDI_METRONOME_COUNTIN_VOLUME = "midi/metronomeCountInVolume";
    const int MIDI_METRONOME_COUNTIN_VOLUME_DEFAULT = 127;

    const char *GENERAL_OPEN_IN_NEW_WINDOW = "general/openFilesInNewWindow";
    const bool GENERAL_OPEN_IN_NEW_WINDOW_DEFAULT = false;

    const char *DEFAULT_INSTRUMENT_NAME = "app/defaultInstrumentName";
    const char *DEFAULT_INSTRUMENT_NAME_DEFAULT = "Untitled";

    const char *DEFAULT_INSTRUMENT_PRESET = "app/defaultInstrumentPreset";
    const int DEFAULT_INSTRUMENT_PRESET_DEFAULT =
        Midi::MIDI_PRESET_ACOUSTIC_GUITAR_STEEL;

    const char *DEFAULT_INSTRUMENT_TUNING = "app/defaultInstrumentTuning";
    const Tuning DEFAULT_INSTRUMENT_TUNING_DEFAULT = Tuning();
}

QDataStream &operator<<(QDataStream &out, const Tuning &tuning)
{
    out << QString::fromStdString(tuning.getName());
    out << tuning.getMusicNotationOffset();
    out << tuning.usesSharps();
    out << QVector<uint8_t>::fromStdVector(tuning.getNotes());
    return out;
}

QDataStream &operator>>(QDataStream &in, Tuning &tuning)
{
    QString name;
    int8_t offset = 0;
    bool sharps = false;
    QVector<uint8_t> notes;

    in >> name >> offset >> sharps >> notes;
    tuning.setName(name.toStdString());
    tuning.setMusicNotationOffset(offset);
    tuning.setSharps(sharps);
    tuning.setNotes(notes.toStdVector());

    return in;
}

// Register the above stream operators so that Tuning objects
// can be used with QSettings.
static struct RegisterMetaTypes
{
    RegisterMetaTypes()
    {
        qRegisterMetaTypeStreamOperators<Tuning>("Tuning");
    }
} RegisterMetaTypesAtStart;
