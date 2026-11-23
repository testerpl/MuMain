#pragma once

#include <map>
#include <span>
#include <vector>

SmartPointer(BuffStateValueControl);

// The buff strip's tooltip texts of a buff. The values and durations of the buffs are in Data/Buffs/Buffs.json
// (GameLogic/Buffs/BuffValues.h).
class BuffStateValueControl
{
public:
    static BuffStateValueControlPtr Make();
    virtual ~BuffStateValueControl();

public:
    void GetBuffInfoString(std::list<std::wstring>& outstr, eBuffState bufftype);

    // The live values the server sent for a buff (C1 F3 09), in the order of the buff's valueLabels.
    // An empty set clears them: the tooltip falls back to the static description.
    void SetServerValues(eBuffState bufftype, std::span<const int> values);
    // Drops the live values of a buff, e.g. when the buff ends.
    void ClearServerValues(eBuffState bufftype);
    // The tooltip's live-value lines ("Defense: +26%"), one per value label; empty when the server
    // sent no values or the buff has no labels.
    void GetBuffValueLines(std::list<std::wstring>& outLines, eBuffState bufftype) const;

private:
    BuffStateValueControl();
    // The live values the server sent, by buff; only the hero's buffs ever get values, and only
    // while the buff is on (UnRegisterBuff clears them).
    std::map<eBuffState, std::vector<int>> m_BuffServerValues;
};
