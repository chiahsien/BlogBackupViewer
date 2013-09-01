#include "CommonUtil.h"
#include <map>

// mCode: Store encode and decode
std::map <WideString, WideString> mCode;
typedef std::map <WideString, WideString>::value_type typeCode;

//---------------------------------------------------------------------------
void InitCodeTable()
{
    mCode.insert(typeCode(L"&nbsp;", L" "));
    mCode.insert(typeCode(L"&amp;",  L"&"));
    mCode.insert(typeCode(L"&quot;", L"\""));
    mCode.insert(typeCode(L"&apos;", L"\'"));
    mCode.insert(typeCode(L"&lt;",   L"¡Õ"));
    mCode.insert(typeCode(L"&gt;",   L"¡Ö"));

    mCode.insert(typeCode(L"&#033;", L"!"));
    mCode.insert(typeCode(L"&#035;", L"#"));
    mCode.insert(typeCode(L"&#036;", L"$"));
    mCode.insert(typeCode(L"&#037;", L"%"));
    mCode.insert(typeCode(L"&#039;", L"\'"));
}
//---------------------------------------------------------------------------
WideString ParseCode(const WideString wsInput)
{
    /*
     * translate &amp; to &, &gt; to >, and so on
     */
    std::map <WideString, WideString>::iterator Iter;
    WideString result(wsInput);
    for (Iter = mCode.begin(); Iter != mCode.end(); ++Iter)
    {
        result = WideStringReplace(result, Iter->first, Iter->second,
                                   TReplaceFlags() << rfReplaceAll);
    }
    return result;
}
//---------------------------------------------------------------------------
WideString WideStringReplace(const WideString wsInput,
        const WideString OldPattern, const WideString NewPattern,
        TReplaceFlags Flags)
{
    /* §@ªÌ: ¿½¨R Xiao Chong
    ** Email : qs.xiao@gmail.com
    ** To-Do : implement rfIgnoreCase
    */

    int pos = 0;
    int old_pattern_len = 0;
    WideString result(wsInput);


    if (0 == (pos = result.Pos(OldPattern))) //no found
        return result;

    old_pattern_len = OldPattern.Length();
    result.Delete(pos, old_pattern_len);
    result.Insert(NewPattern, pos);

    if (Flags.Contains(rfReplaceAll))
    {
        while (0 != (pos = result.Pos(OldPattern)))
        {
            result.Delete(pos, old_pattern_len);
            result.Insert(NewPattern, pos);
        }
    }

    return result;
}
//---------------------------------------------------------------------------
