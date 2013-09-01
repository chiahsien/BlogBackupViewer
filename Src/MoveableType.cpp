#include "MoveableType.h"
#include "CommonUtil.h"

/*
 * Compare the Article's date
 */
bool CmpArticle(tMtArticle *A1, tMtArticle *A2)
{
    /*
     * According to http://www.sixapart.com/movabletype/docs/mtimport
     * the date format should be in the format MM/DD/YYYY hh:mm:ss AM|PM.
     * AM|PM is optional, and if not present the hh will be intrepreted as being in military time.
     */
    TDateTime dt1 = StrToDateTime(A1->date),
              dt2 = StrToDateTime(A2->date);
    AnsiString szDateTimeFormat = "yyyy-mm-dd hh:nn:ss";

    if (CompareText(dt1.FormatString(szDateTimeFormat), dt2.FormatString(szDateTimeFormat)) < 0)
        return false;
    else
        return true;
}
/*
 * Compare the Traceback's date
 */
bool CmpTraceback(tMtTraceback *T1, tMtTraceback *T2)
{
    TDateTime dt1 = StrToDateTime(T1->date),
              dt2 = StrToDateTime(T2->date);
    AnsiString szDateTimeFormat = "yyyy-mm-dd hh:nn:ss";

    if (CompareText(dt1.FormatString(szDateTimeFormat), dt2.FormatString(szDateTimeFormat)) < 0)
        return true;
    else
        return false;
}
/*
 * Compare the Comment's date
 */
bool CmpComment(tMtComment *C1, tMtComment *C2)
{
    TDateTime dt1 = StrToDateTime(C1->date),
              dt2 = StrToDateTime(C2->date);
    AnsiString szDateTimeFormat = "yyyy-mm-dd hh:nn:ss";

    if (CompareText(dt1.FormatString(szDateTimeFormat), dt2.FormatString(szDateTimeFormat)) < 0)
        return true;
    else
        return false;
}
//---------------------------------------------------------------------------
MoveableType::MoveableType()
{
    ShortDateFormat = "mm/dd/yyyy";
    ShortTimeFormat = "hh:nn:ss AM/PM";
}
MoveableType::MoveableType(WideString wsFileName, TTntTreeView *tvTree)
{
    ShortDateFormat = "mm/dd/yyyy";
    ShortTimeFormat = "hh:nn:ss AM/PM";
    Load(wsFileName, tvTree);
}
//---------------------------------------------------------------------------
MoveableType::~MoveableType()
{
}
//---------------------------------------------------------------------------
void MoveableType::Load(WideString wsFileName, TTntTreeView *tvTree)
{
    int Index;
    TTntStringList *sl = new TTntStringList;

    /*
     * Clear previous datas store in TreeNode.
     */
    ClearData(tvTree);

    sl->LoadFromFile(wsFileName);
    if (sl->LastFileCharSet == csAnsi)
        sl->AnsiStrings->LoadFromFileEx(wsFileName, CP_UTF8);

    for (Index = 0; Index < sl->Count; ++Index)
        ParseMtArticle(sl, Index);
    lMtArticle.sort(CmpArticle);
    lMtTraceback.sort(CmpTraceback);
    lMtComment.sort(CmpComment);
    CreateTree(tvTree, "");

    delete sl; 
}
//---------------------------------------------------------------------------
void MoveableType::ClearData(TTntTreeView *tvTree)
{
    /*
     * Delete TreeNode's data
     */
    tNodeData *data = NULL;
    TTntTreeNode *Node = tvTree->Items->GetFirstNode();
    while (Node != NULL)
    {
        data = (tNodeData*)Node->Data;
        if (data)
        {
            if (data->text)
                delete data->text;
            delete data;
            Node->Data = NULL;
        }
        Node = Node->GetNext();
    }
    tvTree->Items->Clear();

    /*
     * Delete data in lMtArticle
     */
    std::list<tMtArticle*>::iterator aIter;
    for (aIter = lMtArticle.begin(); aIter != lMtArticle.end(); ++aIter)
    {
        delete (*aIter)->text;
        delete (*aIter)->category;
        delete (*aIter)->comment;
        delete (*aIter);
        *aIter = NULL;
    }
    lMtArticle.clear();

    /*
     * Delete data in lMtTraceback
     */
    std::list<tMtTraceback*>::iterator tIter;
    for (tIter = lMtTraceback.begin(); tIter != lMtTraceback.end(); ++tIter)
    {
        delete (*tIter)->traceback;
        delete (*tIter);
        *tIter = NULL;
    }
    lMtTraceback.clear();

    /*
     * Delete data in lMtComment
     */
    std::list<tMtComment*>::iterator cIter;
    for (cIter = lMtComment.begin(); cIter != lMtComment.end(); ++cIter)
    {
        delete (*cIter)->comment;
        delete (*cIter);
        *cIter = NULL;
    }
    lMtComment.clear();
}
//---------------------------------------------------------------------------
/*
 * MT Format: http://www.sixapart.com/movabletype/docs/mtimport
 */
void MoveableType::ParseMtArticle(TTntStringList *Text, int &Index)
{
    tMtArticle *tData = new tMtArticle;
    tData->text       = new TTntStringList;
    tData->category   = new TTntStringList;
    tData->comment    = new TTntStringList;
    tData->traceback  = new TTntStringList;

    int i;
    WideString wsTmp;
    tData->id = lMtArticle.size();
    for (; Index < Text->Count; ++Index)
    {
        wsTmp = Text->Strings[Index];
        if (wsTmp == WideString("-----"))           // begin of a multi-line field
            continue;
        else if (wsTmp == WideString("--------"))   // end of a section
            break;
        else if ((i = wsTmp.Pos(":")) > 0)
        {
            AnsiString szPrefix = AnsiString(wsTmp.SubString(0, i)).UpperCase();
            if (szPrefix == "AUTHOR:")
                tData->author = wsTmp.SubString(i + 1, wsTmp.Length());
            else if (szPrefix == "TITLE:")
                tData->title = wsTmp.SubString(i + 1, wsTmp.Length());
            else if (szPrefix == "DATE:")
                tData->date = wsTmp.SubString(i + 1, wsTmp.Length());
            else if (szPrefix == "PRIMARY CATEGORY:" ||
                     szPrefix == "CATEGORY:")
                tData->category->Add(wsTmp.SubString(i + 1, wsTmp.Length()));
            else if (szPrefix == "STATUS:")
                tData->status = wsTmp.SubString(i + 1, wsTmp.Length());
            else if (szPrefix == "ALLOW COMMENTS:")
                tData->allowComments = wsTmp.SubString(i + 1, wsTmp.Length());
            else if (szPrefix == "ALLOW PINGS:")
                tData->allowPings = wsTmp.SubString(i + 1, wsTmp.Length());
            else if (szPrefix == "CONVERT BREAKS:")
                tData->convertBreaks = wsTmp.SubString(i + 1, wsTmp.Length());
            else if (szPrefix == "BODY:" || szPrefix == "EXTENDED BODY:")
                continue;
            else if (szPrefix == "PING:")
                ParseMtTraceback(Text, ++Index, tData->traceback, tData->id);
            else if (szPrefix == "COMMENT:")
                ParseMtComment(Text, ++Index, tData->comment, tData->id);
            else
                tData->text->Add(wsTmp);
        }
        else
            tData->text->Add(wsTmp);
    }
    lMtArticle.push_back(tData);
}
//---------------------------------------------------------------------------
void MoveableType::ParseMtTraceback(TTntStringList *Text, int &Index,
                                  TTntStringList *slTraceback, int pid)
{
    tMtTraceback *tData = new tMtTraceback;
    tData->traceback    = new TTntStringList;
    tData->pid = pid;

    int i;
    WideString wsTmp;
    TDateTime dt;
    AnsiString szDateTimeFormat = "yyyy-mm-dd hh:nn:ss";

    slTraceback->Add("<div class=\"traceback-body\">");

    for (; Index < Text->Count; ++Index)
    {
        wsTmp = Text->Strings[Index];

        if (wsTmp == WideString("-----"))
            break;
        else if ((i = wsTmp.Pos(":")) > 0)
        {
            AnsiString szPrefix = AnsiString(wsTmp.SubString(0, i)).UpperCase();
            if (szPrefix == "TITLE:")
                tData->title = wsTmp.SubString(i + 1, wsTmp.Length());
            else if (szPrefix == "URL:")
                tData->url = wsTmp.SubString(i + 1, wsTmp.Length());
            else if (szPrefix == "IP:")
                tData->ip = wsTmp.SubString(i + 1, wsTmp.Length());
            else if (szPrefix == "BLOG NAME:")
                tData->blog = wsTmp.SubString(i + 1, wsTmp.Length());
            else if (szPrefix == "DATE:")
                tData->date = wsTmp.SubString(i + 1, wsTmp.Length());
            else
            {
                tData->traceback->Add(wsTmp);
                slTraceback->Add(wsTmp + "<br />");
            }
        }
        else
        {
            tData->traceback->Add(wsTmp);
            slTraceback->Add(wsTmp + "<br />");
        }
    }
    lMtTraceback.push_back(tData);
    slTraceback->Add("</div>");

    dt = StrToDateTime(AnsiString(tData->date));
    slTraceback->Add("<div class=\"traceback-foot\"><a href=\"" + tData->url + "\">" +
                     tData->title + " - " + tData->blog + "</a> 於 " + dt.FormatString(szDateTimeFormat) + " 引用");
    slTraceback->Add("</div>");
}
//---------------------------------------------------------------------------
void MoveableType::ParseMtComment(TTntStringList *Text, int &Index,
                                  TTntStringList *slComment, int pid)
{
    tMtComment *tData = new tMtComment;
    tData->comment    = new TTntStringList;
    tData->pid = pid;

    int i;
    WideString wsTmp;
    TDateTime dt;
    AnsiString szDateTimeFormat = "yyyy-mm-dd hh:nn:ss";

    slComment->Add("<div class=\"comment-body\">");

    for (; Index < Text->Count; ++Index)
    {
        wsTmp = Text->Strings[Index];

        if (wsTmp == WideString("-----"))
            break;
        else if ((i = wsTmp.Pos(":")) > 0)
        {
            AnsiString szPrefix = AnsiString(wsTmp.SubString(0, i)).UpperCase();
            if (szPrefix == "AUTHOR:")
                tData->author = wsTmp.SubString(i + 1, wsTmp.Length());
            else if (szPrefix == "DATE:")
                tData->date = wsTmp.SubString(i + 1, wsTmp.Length());
            else if (szPrefix == "IP:")
                tData->ip = wsTmp.SubString(i + 1, wsTmp.Length());
            else if (szPrefix == "EMAIL:")
                tData->email = wsTmp.SubString(i + 1, wsTmp.Length());
            else if (szPrefix == "URL:")
                tData->url = wsTmp.SubString(i + 1, wsTmp.Length());
            else
            {
                tData->comment->Add(wsTmp);
                slComment->Add(wsTmp + "<br />");
            }
        }
        else
        {
            tData->comment->Add(wsTmp);
            slComment->Add(wsTmp + "<br />");
        }
    }
    lMtComment.push_back(tData);
    slComment->Add("</div>");

    dt = StrToDateTime(AnsiString(tData->date));
    if ((tData->url = AnsiString(tData->url).Trim()) != WideString(""))
    {
        slComment->Add("<div class=\"comment-foot\"><a href=\"" + tData->url + "\">" +
                       tData->author + "</a> 於 " + dt.FormatString(szDateTimeFormat) + " 回應");
    }
    else
    {
        slComment->Add("<div class=\"comment-foot\">" + tData->author +
                       " 於 " + dt.FormatString(szDateTimeFormat) + " 回應");
    }
    if ((tData->email = AnsiString(tData->email).Trim()) != WideString(""))
        slComment->Add(" | <a href=\"mailto:" + tData->email + "\">EMail</a>");
    if (tData->ip != NULL)
        slComment->Add("<br />IP: " + tData->ip);
    slComment->Add("</div>");
}
//---------------------------------------------------------------------------
void MoveableType::CreateTree(TTntTreeView *tvTree, WideString Keyword)
{
    /*
     * Delete TreeNode's data
     */
    tNodeData *data = NULL;
    TTntTreeNode *Node = tvTree->Items->GetFirstNode();
    while (Node != NULL)
    {
        data = (tNodeData*)Node->Data;
        if (data)
        {
            if (data->text)
                delete data->text;
            delete data;
            Node->Data = NULL;
        }
        Node = Node->GetNext();
    }
    tvTree->Items->Clear();

    tvTree->Items->BeginUpdate();

    TTntTreeNode *Root1 = tvTree->Items->Add(NULL, WideString("文章分類"));
    Root1->ImageIndex = 0;
    Root1->SelectedIndex = 0;

    TTntTreeNode *Root2 = tvTree->Items->Add(NULL, WideString("所有文章 - 由新到舊"));
    Root2->ImageIndex = 0;
    Root2->SelectedIndex = 0;

    TTntTreeNode *Node1, *Node2;

    std::list<tMtArticle*>::iterator Iter;
    for (Iter = lMtArticle.begin(); Iter != lMtArticle.end(); ++Iter)
    {
        if (Keyword == WideString("") || (*Iter)->title.Pos(Keyword) > 0 || (*Iter)->text->Text.Pos(Keyword) > 0)
        {
            tNodeData *Data  = new tNodeData;
            Data->text       = new TTntStringList;

            TDateTime dt = StrToDateTime((*Iter)->date);
            AnsiString szDateTimeFormat = "yyyy-mm-dd hh:nn:ss";

            Data->id = (*Iter)->id;
            Data->text->Add(
                "<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\" />\
                <link rel=\"stylesheet\" href=\"default.css\" type=\"text/css\" />\
                <title></title></head><body>");
            Data->text->Add("<div class=\"header\">");
            Data->text->Add((*Iter)->title + "</a></div><div class=\"content\">");
            Data->text->AddStrings((*Iter)->text);
            Data->text->Add("<div class=\"content-date\">" + (*Iter)->author + " 於 ");
            Data->text->Add(dt.FormatString(szDateTimeFormat));
            Data->text->Add(" 發表</div></div><div id=\"traceback\"><div class=\"traceback-header\">引用列表</div>");
            Data->text->AddStrings((*Iter)->traceback);
            Data->text->Add("</div><br /><div id=\"comment\"><div class=\"comment-header\">回應</div>");
            Data->text->AddStrings((*Iter)->comment);
            Data->text->Add("</div></body></html>");

            /*
             * Create categories and put articles to categories.
             * Each article can be assigned to multiple categories.
             */
            for (int i = 0; i < (*Iter)->category->Count; ++i)
            {
                int j;
                tNodeData *newData = new tNodeData;
                newData->text = new TTntStringList;
                newData->text->Assign(Data->text);
                newData->id = (*Iter)->id;
                for (j = 0; j < Root1->Count; ++j)
                    if (ParseCode((*Iter)->category->Strings[i]) == Root1->Item[j]->Text)
                        break;
                if (j < Root1->Count)
                {
                    Node1 = tvTree->Items->AddChildObject(Root1->Item[j], ParseCode((*Iter)->title), newData);
                }
                else
                {
                    Node1 = tvTree->Items->AddChild(Root1, ParseCode((*Iter)->category->Strings[i]));
                    Node1->ImageIndex = 0;
                    Node1->SelectedIndex = 0;
                    Node1 = tvTree->Items->AddChildObject(Node1, ParseCode((*Iter)->title), newData);
                }
                Node1->ImageIndex = 2;
                Node1->SelectedIndex = 2;
            }

            Node2 = tvTree->Items->AddChildObject(Root2, ParseCode((*Iter)->title), Data);
            Node2->ImageIndex = 2;
            Node2->SelectedIndex = 2;
        }
    }

    for (int i = 0; i < Root1->Count; ++i)
    {
        Root1->Item[i]->Text =
            Root1->Item[i]->Text + " (" + IntToStr(Root1->Item[i]->Count) + ")";
    }
    Root1->Text = Root1->Text + " (" + IntToStr(Root1->Count) + ")";
    Root2->Text = Root2->Text + " (" + IntToStr(Root2->Count) + ")";

    Root1->Selected = true;
    Root1->Expand(false);

    tvTree->Items->EndUpdate();
}
//---------------------------------------------------------------------------
void MoveableType::ExportArticle(TTntTreeNode *Node, TTntStringList *slOutput)
{
    tMtArticle *tData;
    std::list<tMtArticle*>::iterator aIter;
    std::list<tMtTraceback*>::iterator tIter;
    std::list<tMtComment*>::iterator cIter;

    tNodeData *data = (tNodeData*)Node->Data;
    for (aIter = lMtArticle.begin(); aIter != lMtArticle.end(); ++aIter)
    {
        if ((*aIter)->id == data->id)
        {
            slOutput->Add("AUTHOR: " + (*aIter)->author);
            slOutput->Add("TITLE: " + (*aIter)->title);
            slOutput->Add("DATE: " + (*aIter)->date);
            slOutput->Add("STATUS: " + (*aIter)->status);
            slOutput->Add("ALLOW COMMENTS: " + (*aIter)->allowComments);
            slOutput->Add("ALLOW PINGS: " + (*aIter)->allowPings);
            slOutput->Add("CONVERT BREAKS: " + (*aIter)->convertBreaks);
            for (int j = 0; j < (*aIter)->category->Count; ++j)
                slOutput->Add(WideString("CATEGORY: ") + (*aIter)->category->Strings[j]);
            slOutput->Add("-----");

            slOutput->Add("BODY:");
            slOutput->AddStrings((*aIter)->text);
            slOutput->Add("-----");

            for (cIter = lMtComment.begin(); cIter != lMtComment.end(); ++cIter)
            {
                if ((*cIter)->pid == data->id)
                {
                    slOutput->Add("COMMENT:");
                    slOutput->Add(WideString("AUTHOR: ") + (*cIter)->author);
                    slOutput->Add(WideString("DATE: ") + (*cIter)->date);
                    slOutput->Add(WideString("IP: ") + (*cIter)->ip);
                    slOutput->Add(WideString("EMAIL: ") + (*cIter)->email);
                    slOutput->Add(WideString("URL: ") + (*cIter)->url);
                    slOutput->AddStrings((*cIter)->comment);
                    slOutput->Add("-----");
                }
            }

            for (tIter = lMtTraceback.begin(); tIter != lMtTraceback.end(); ++tIter)
            {
                if ((*tIter)->pid == data->id)
                {
                    slOutput->Add("PING:");
                    slOutput->Add(WideString("TITLE: ") + (*tIter)->title);
                    slOutput->Add(WideString("URL: ") + (*tIter)->url);
                    slOutput->Add(WideString("IP: ") + (*tIter)->ip);
                    slOutput->Add(WideString("BLOG NAME: ") + (*tIter)->blog);
                    slOutput->Add(WideString("DATE: ") + (*tIter)->date);
                    slOutput->AddStrings((*tIter)->traceback);
                    slOutput->Add("-----");
                }
            }

            slOutput->Add("--------");
            break;
        }
    }
}
//---------------------------------------------------------------------------

