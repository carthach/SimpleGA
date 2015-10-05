/** 
    DrumEvent.h
    Defines struct for dealing with drum events.
*/

#pragma once

#include <vector>

struct DrumEvent
{
    int N16th;      ///< note position in 16th grid
    float delta;    ///< microtiming, i.e. onset deviation from grid position
    int length;     ///< length in sixteenth notes
    float velocity; ///< velocity normalized to [0,1]
    int pitch;      ///< midi pitch
    int clipId;     ///< clipId for prototype format
    int eventId;    ///< eventId for prototype format
    int groupId;    ///< groupId for prototype format
    int soundId;    ///< soundId for prototype format
    
    /// compare equal operator based on note position
    bool operator==(DrumEvent rhs) const
    {
        return (N16th == rhs.N16th);
    }

    /// compare less operator based on note position
    bool operator<(DrumEvent rhs) const
    {
        return N16th < rhs.N16th;
    }
};

typedef std::vector<DrumEvent> DrumEvents;
typedef std::vector<DrumEvents> DrumPattern;
