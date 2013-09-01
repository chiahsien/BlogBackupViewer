#ifndef CommonUtil.H
#define CommonUtil.H

#include "TntComCtrls.hpp"

/*
 * struct to store TreeNode's data
 */
typedef struct tNodeData
{
    int id;
    TTntStringList *text;
} tNodeData;

void InitCodeTable();
WideString ParseCode(const WideString wsInput);
WideString WideStringReplace(const WideString wsInput, const WideString OldPattern,
                             const WideString NewPattern, TReplaceFlags Flags);
#endif
