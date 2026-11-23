#include "stdafx.h"
#include "w_BuffStateValueControl.h"
#include "w_BuffScriptLoader.h"
#include "Core/Utilities/_GlobalFunctions.h"
#include "Data/GameData/BuffData/BuffDatabase.h"

#include <algorithm>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BuffStateValueControlPtr BuffStateValueControl::Make()
{
    BuffStateValueControlPtr buffstatecontrol(new BuffStateValueControl);
    return buffstatecontrol;
}

BuffStateValueControl::BuffStateValueControl() = default;

BuffStateValueControl::~BuffStateValueControl() = default;

void BuffStateValueControl::GetBuffInfoString(std::list<std::wstring>& outstr, eBuffState bufftype)
{
    const BuffInfo buffinfo = g_BuffInfo(bufftype);

    outstr = buffinfo.s_BuffDescriptlist;
    outstr.push_front(L"\n");
    outstr.push_front(buffinfo.s_BuffName);
}

namespace
{
// One "{0}" placeholder per label: "{0}" is the value itself, "{0:%}" the value as hundredths of
// a percent (7500 -> "75", 75 -> "0.75", 1050 -> "10.5"). A label without a placeholder renders
// as-is; a value without a label is not shown.
void ReplaceAll(std::wstring& text, const std::wstring& from, const std::wstring& to)
{
    size_t pos = 0;
    while ((pos = text.find(from, pos)) != std::wstring::npos)
    {
        text.replace(pos, from.size(), to);
        pos += to.size();
    }
}

std::wstring FormatPercentHundredths(int value)
{
    const bool negative = value < 0;
    const unsigned int absolute =
        static_cast<unsigned int>(negative ? -static_cast<long long>(value) : value);
    std::wstring text = std::to_wstring(absolute / 100);
    text += L'.';
    if (absolute % 100 < 10)
    {
        text += L'0';
    }
    text += std::to_wstring(absolute % 100);
    while (!text.empty() && text.back() == L'0')
    {
        text.pop_back();
    }
    if (!text.empty() && text.back() == L'.')
    {
        text.pop_back();
    }
    if (negative)
    {
        text.insert(text.begin(), L'-');
    }
    return text;
}

std::wstring FormatBuffValueLabel(const std::wstring& labelTemplate, int value)
{
    std::wstring line = labelTemplate;
    ReplaceAll(line, L"{0:%}", FormatPercentHundredths(value));
    ReplaceAll(line, L"{0}", std::to_wstring(value));
    return line;
}
} // namespace

void BuffStateValueControl::SetServerValues(eBuffState bufftype, std::span<const int> values)
{
    if (bufftype <= eBuffNone || bufftype >= eBuff_Count)
    {
        return;
    }
    if (values.empty())
    {
        m_BuffServerValues.erase(bufftype);
        return;
    }
    m_BuffServerValues[bufftype].assign(values.begin(), values.end());
}

void BuffStateValueControl::ClearServerValues(eBuffState bufftype)
{
    m_BuffServerValues.erase(bufftype);
}

void BuffStateValueControl::GetBuffValueLines(std::list<std::wstring>& outLines, eBuffState bufftype) const
{
    outLines.clear();
    const auto found = m_BuffServerValues.find(bufftype);
    if (found == m_BuffServerValues.end() || found->second.empty())
    {
        return;
    }
    const Data::Buffs::BuffDisplayTexts* texts = g_BuffDatabase.FindTexts(bufftype);
    if (texts == nullptr)
    {
        return;
    }
    const size_t count = std::min(found->second.size(), texts->valueLabelLines.size());
    for (size_t i = 0; i < count; ++i)
    {
        outLines.push_back(FormatBuffValueLabel(texts->valueLabelLines[i], found->second[i]));
    }
}
