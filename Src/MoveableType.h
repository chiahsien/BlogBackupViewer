#ifndef MoveableTypeH
#define MoveableTypeH

#include "CommonUtil.h"
#include "TntComCtrls.hpp"
#include "TntDialogs.hpp"
#include <list>

/*
 * struct to store MT article's information
 */
typedef struct tMtArticle
{
    int id;
    WideString author;
    WideString title;
    AnsiString date;
    AnsiString status;
    AnsiString allowComments;
    AnsiString allowPings;
    AnsiString convertBreaks;
    TTntStringList *category;
    TTntStringList *text;
    TTntStringList *comment;
    TTntStringList *traceback;
} tMtArticle;

/*
 * struct to store MT article's comment
 */
typedef struct tMtComment
{
    int pid;    // parent's id
    WideString author;
    WideString date;
    WideString ip;
    WideString email;
    WideString url;
    TTntStringList *comment;
} tMtComment;

/*
 * struct to store MT article's traceback
 */
typedef struct tMtTraceback
{
    int pid;    // parent's id
    WideString title;
    WideString url;
    WideString ip;
    WideString blog;
    WideString date;
    TTntStringList *traceback;
} tMtTraceback;
//---------------------------------------------------------------------------
class MoveableType
{
public:
    MoveableType();
    MoveableType(WideString wsFileName, TTntTreeView *tvTree);
    ~MoveableType();

    void Load(WideString wsFileName, TTntTreeView *tvTree);
    void ClearData(TTntTreeView *tvTree);
    void ExportArticle(TTntTreeNode *Node, TTntStringList *slOutput);
    void CreateTree(TTntTreeView *tvTree, WideString Keyword);

    /* Compare the Article's date */
    friend bool CmpArticle(tMtArticle *A1, tMtArticle *A2);
    /* Compare the Traceback's date */
    friend bool CmpTraceback(tMtTraceback *T1, tMtTraceback *T2);
    /* Compare the Comment's date */
    friend bool CmpComment(tMtComment *C1, tMtComment *C2);

private:
    std::list <tMtArticle*>   lMtArticle;   // lMtArticle: Store MT article's information
    std::list <tMtComment*>   lMtComment;   // lMtComment: Store MT article's comment
    std::list <tMtTraceback*> lMtTraceback; // lMtComment: Store MT article's traceback

    void ParseMtArticle(TTntStringList *Text, int &Index);
    void ParseMtComment(TTntStringList *Text, int &Index,
                        TTntStringList *Comment, int pid);
    void ParseMtTraceback(TTntStringList *Text, int &Index,
                          TTntStringList *Traceback, int pid);
};

#endif
