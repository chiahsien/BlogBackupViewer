#include "Wretch.h"
#include "CommonUtil.h"

/*
 * Compare the Article's date
 */
bool CmpArticle(tArticle *A1, tArticle *A2)
{
    if (CompareText(AnsiString(A1->date), AnsiString(A2->date)) < 0)
        return false;
    else
        return true;
}
//---------------------------------------------------------------------------
/*
 * Compare the Folder's order
 */
bool CmpFolder(tFolder *A1, tFolder *A2)
{
    return (StrToInt(A1->order) < StrToInt(A2->order));
}
//---------------------------------------------------------------------------
/*
 * Compare the Category's order
 */
bool CmpCategory(tCategory *A1, tCategory *A2)
{
    return (StrToInt(A1->order) < StrToInt(A2->order));
}
//---------------------------------------------------------------------------
Wretch::Wretch()
{
}
Wretch::Wretch(WideString wsFileName, TXMLDocument *xmlDoc, TTntTreeView *tvTree)
{
    Load(wsFileName, xmlDoc, tvTree);
}
//---------------------------------------------------------------------------
Wretch::~Wretch()
{
}
//---------------------------------------------------------------------------
void Wretch::Load(WideString wsFileName, TXMLDocument *xmlDoc, TTntTreeView *tvTree)
{
    /*
     * Load the XML file first, and check if there are invalid characters.
     * If there are some, replace them with white-space (0x20).
     */
    TTntStringList *sl = new TTntStringList;
    sl->AnsiStrings->LoadFromFileEx(wsFileName, CP_UTF8);
    WideString ws = sl->Text;
    for (int i = 1; i <= ws.Length(); ++i)
    {
        if (ws[i] < 0x20)
           ws[i] = 0x20;
    }
    /*
     * Try to fix Wretch's stupid bug.
     */
    if (ws.Pos("<?xml version") != 1)
        ws.Delete(1, ws.Pos("<?xml version")-1);
    if ((ws.Pos("</blog_backup>") + 15) != ws.Length())
        ws.Delete((ws.Pos("</blog_backup>") + 14), ws.Length()-(ws.Pos("</blog_backup>") + 15));
    delete sl;

    xmlDoc->LoadFromXML(ws);    // Loading the fixed one.
//        xmlDoc->LoadFromFile(OpenDialog1->FileName);
    if (xmlDoc->IsEmptyDoc())
    {
        ShowMessage("載入失敗，可能是檔案內容或編碼有誤。");
        return;
    }

    /*
     * Clear previous datas store in TreeNode, list, and map.
     */
    ClearData(tvTree);

    AnsiString articles = "blog_articles_";
    xmlDoc->Active = true;
    diRoot = xmlDoc->ChildNodes->FindNode(L"blog_backup");
    szOwner =
        diRoot->ChildNodes->FindNode(L"blog_blogs")->ChildNodes->FindNode("id")->Text;
    NumPosts =
        AnsiString(diRoot->ChildNodes->FindNode(L"blog_blogs")->ChildNodes->FindNode("NumPosts")->Text).ToInt();
    articles.Insert(szOwner, 15);
    articles = articles.LowerCase().SubString(0, 15);

    ParseArticle(diRoot->ChildNodes->FindNode((WideString)articles));
    ParseFolder(diRoot->ChildNodes->FindNode(L"blog_category_folders"));
    ParseComment(diRoot->ChildNodes->FindNode(L"blog_articles_comments"));
    ParseCategory(diRoot->ChildNodes->FindNode(L"blog_articles_categories"));

    CreateTree(tvTree, "");
}
//---------------------------------------------------------------------------
void Wretch::ClearData(TTntTreeView *tvTree)
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
     * Delete data in lArticle
     */
    std::list<tArticle*>::iterator aIter;
    for (aIter = lArticle.begin(); aIter != lArticle.end(); ++aIter)
    {
        delete (*aIter)->text;
        delete (*aIter);
        *aIter = NULL;
    }
    lArticle.clear();

    /*
     * Delete data in lFolder
     */
    std::list<tFolder*>::iterator fIter;
    for (fIter = lFolder.begin(); fIter != lFolder.end(); ++fIter)
    {
        delete (*fIter);
        *fIter = NULL;
    }
    lFolder.clear();

    /*
     * Delete data in lComment
     */
    std::list<tComment*>::iterator cIter;
    for (cIter = lComment.begin(); cIter != lComment.end(); ++cIter)
    {
        delete (*cIter)->text;
		delete (*cIter)->reply;
        delete (*cIter);
        *cIter = NULL;
    }
    lComment.clear();

    /*
     * Delete data in lCategory
     */
    std::list<tCategory*>::iterator gIter;
    for (gIter = lCategory.begin(); gIter != lCategory.end(); ++gIter)
    {
        delete (*gIter);
        *gIter = NULL;
    }
    lCategory.clear();
}
//---------------------------------------------------------------------------
void Wretch::ParseArticle(_di_IXMLNode diNode)
{
    _di_IXMLNodeList diNodeList; // the nodes taht belong to the <article> node

    for (int i = 0; i < diNode->ChildNodes->Count; ++i)
    {
        diNodeList = diNode->ChildNodes->GetNode(i)->ChildNodes;

        // create a tArticle struct to store article's info
        tArticle *tData    = new tArticle;
        tData->text        = new TTntStringList;
        tData->id          = diNodeList->FindNode(L"id")->Text;
        tData->category_id = diNodeList->FindNode(L"category_id")->Text;
        tData->date        = diNodeList->FindNode(L"date")->Text;
        tData->allow_comment = diNodeList->FindNode(L"allow_comment")->Text;
        tData->numComments = diNodeList->FindNode(L"NumComments")->Text;
        tData->title       = diNodeList->FindNode(L"title")->Text;
        if (tData->title == NULL)
            tData->title = WideString("＜無標題＞");
        tData->text->Add(diNodeList->FindNode(L"text")->Text);

        lArticle.push_back(tData);
    }
    lArticle.sort(CmpArticle);
}
//---------------------------------------------------------------------------
void Wretch::ParseFolder(_di_IXMLNode diNode)
{
    _di_IXMLNodeList diNodeList; // the nodes that belong to the <ategory_folder> node

    for (int i = 0; i < diNode->ChildNodes->Count; ++i)
    {
        diNodeList = diNode->ChildNodes->GetNode(i)->ChildNodes;

        tFolder *tData       = new tFolder;
        tData->id            = diNodeList->FindNode(L"id")->Text;
        tData->name          = diNodeList->FindNode(L"name")->Text;
        tData->order         = diNodeList->FindNode(L"order")->Text;
        tData->numCategories = diNodeList->FindNode(L"NumCategories")->Text;
        lFolder.push_back(tData);
    }
    lFolder.sort(CmpFolder);
}
//---------------------------------------------------------------------------
void Wretch::ParseComment(_di_IXMLNode diNode)
{
    _di_IXMLNodeList diNodeList; // the nodes that belong to the <article_comment> node

    for (int i = 0; i < diNode->ChildNodes->Count; ++i)
    {
        diNodeList = diNode->ChildNodes->GetNode(i)->ChildNodes;

        // create a tComment struct to store article's comments' info
        tComment *tData    = new tComment;
        tData->text        = new TTntStringList;
		tData->reply	   = new TTntStringList;
        tData->article_id  = diNodeList->FindNode(L"article_id")->Text;
        tData->date        = diNodeList->FindNode(L"date")->Text;
        tData->name        = diNodeList->FindNode(L"name")->Text;
        tData->email       = diNodeList->FindNode(L"email")->Text;
        tData->url         = diNodeList->FindNode(L"url")->Text;
        tData->ip          = diNodeList->FindNode(L"ip")->Text;
        tData->text->Add(diNodeList->FindNode(L"text")->Text);
		tData->reply->Add(diNodeList->FindNode(L"reply")->Text);
		tData->reply_date  = diNodeList->FindNode(L"reply_date")->Text;
        if (tData->name == NULL)
            tData->name = " ";
        if (tData->url != NULL)
        {
            /*
             * if the url is not starting with "http://",
             * we add "http://www.wretch.cc/user/" as its prefix.
             */
            if (tData->url.Pos("http://") == 0) // not start with http://
                tData->url.Insert("http://www.wretch.cc/user/", 1);
        }
        if (tData->ip != NULL)
        {
            wchar_t *endptr;
            struct in_addr ipnum;
            /*
             * Use wcstoul to convert WideString to unsigned long,
             * and use ntohl to convert network order to host order.
             */
            ipnum.s_addr = ntohl(wcstoul(tData->ip, &endptr, 10));
            /*
             * Use inet_ntoa to convert IP to A.B.C.D format.
             */
            tData->ip = inet_ntoa(ipnum);
        }
        lComment.push_back(tData);
    }
}
//---------------------------------------------------------------------------
void Wretch::ParseCategory(_di_IXMLNode diNode)
{
    _di_IXMLNodeList diNodeList; // the nodes belong to the <category> node

    for (int i = 0; i < diNode->ChildNodes->Count; ++i)
    {
        diNodeList = diNode->ChildNodes->GetNode(i)->ChildNodes;

        // create a tCategory struct to store folder's category
        tCategory *tData = new tCategory;
        tData->id        = diNodeList->FindNode(L"id")->Text;
        tData->folder_id = diNodeList->FindNode(L"folder_id")->Text;
        tData->name      = diNodeList->FindNode(L"name")->Text;
        tData->order     = diNodeList->FindNode(L"order")->Text;
        tData->numPosts  = diNodeList->FindNode(L"NumPosts")->Text;
        NumPosts = NumPosts - AnsiString(tData->numPosts).ToInt();
        lCategory.push_back(tData);
    }
    lCategory.sort(CmpCategory);

    /*
     * manually add the NOT CLASSIFIED category
     */
    tCategory *tData = new tCategory;
    tData->id = "0";
    tData->folder_id = "0";
    tData->name = WideString("未分類文章");
    tData->numPosts = WideString(NumPosts);
    lCategory.push_back(tData);
}
//---------------------------------------------------------------------------
void Wretch::CreateTree(TTntTreeView *tvTree, WideString Keyword)
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

    TTntTreeNode *Root =
        tvTree->Items->Add(NULL, diRoot->ChildNodes->FindNode(L"blog_blogs")->ChildNodes->FindNode(L"name")->Text);
    Root->ImageIndex = 0;
    Root->SelectedIndex = 0;

    AddFolder(tvTree, Root, Keyword);
    AddCategory(tvTree, Root, "0", Keyword);
    Root->Selected = true;
    Root->Expand(false);

    TTntTreeNode *Root2 = tvTree->Items->Add(NULL, L"所有文章 - 由新到舊");
    Root2->ImageIndex = 0;
    Root2->SelectedIndex = 0;
    AddArticle(tvTree, Root2, WideString("-1"), Keyword);
    Root2->Text = Root2->Text + " (" + IntToStr(Root2->Count) + ")";

    tvTree->Items->EndUpdate();
}
//---------------------------------------------------------------------------
void Wretch::AddFolder(TTntTreeView *tvTree, TTntTreeNode *Root, WideString Keyword)
{
    std::list<tFolder*>::iterator Iter;
    TTntTreeNode *Node;

    for (Iter = lFolder.begin(); Iter != lFolder.end(); ++Iter)
    {
        Node = tvTree->Items->AddChild(Root, ParseCode((*Iter)->name));
        Node->ImageIndex = 0;
        Node->SelectedIndex = 0;
        AddCategory(tvTree, Node, (*Iter)->id, Keyword);
        Node->Text = Node->Text + " (" + IntToStr(Node->Count) + ")";
    }
}
//---------------------------------------------------------------------------
void Wretch::AddCategory(TTntTreeView *tvTree, TTntTreeNode *Root, WideString ID, WideString Keyword)
{
    std::list<tCategory*>::iterator Iter;
    TTntTreeNode *Node;
    for (Iter = lCategory.begin(); Iter != lCategory.end(); ++Iter)
    {
        if ((*Iter)->folder_id == ID)
        {
            Node = tvTree->Items->AddChild(Root, ParseCode((*Iter)->name));
            Node->ImageIndex = 0;
            Node->SelectedIndex = 0;
            AddArticle(tvTree, Node, (*Iter)->id, Keyword);
            Node->Text = Node->Text + " (" + IntToStr(Node->Count) + ")";
        }
    }
}
//---------------------------------------------------------------------------
void Wretch::AddArticle(TTntTreeView *tvTree, TTntTreeNode *Root, WideString ID, WideString Keyword)
{
    std::list<tArticle*>::iterator Iter;
    TTreeNode *Node;
    for (Iter = lArticle.begin(); Iter != lArticle.end(); ++Iter)
    {
        if (Keyword == WideString("") || (*Iter)->title.Pos(Keyword) > 0 || (*Iter)->text->Text.Pos(Keyword) > 0)
        {
            /* -1 means not to be categorized */
            if ((*Iter)->category_id == ID || ID == WideString("-1"))
            {
                WideString url = "http://www.wretch.cc/blog/" + szOwner + "/" + (*Iter)->id;
                tNodeData *Data = new tNodeData;
                Data->text = new TTntStringList;
                Node = tvTree->Items->AddChildObject(Root, ParseCode((*Iter)->title), Data);
                Node->ImageIndex = 2;
                Node->SelectedIndex = 2;

                Data->id = StrToInt((*Iter)->id);
                Data->text->Add(
                    "<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\" />\
                    <link rel=\"stylesheet\" href=\"default.css\" type=\"text/css\" />\
                    <title></title></head><body>");
                Data->text->Add("<div class=\"header\"><a href=\"" + url + "\">");
                Data->text->Add((*Iter)->title + "</a></div><div class=\"content\">");
                Data->text->AddStrings((*Iter)->text);
                Data->text->Add("<div class=\"content-date\">於 ");
                Data->text->Add((*Iter)->date);
                Data->text->Add(" 發表</div></div><div id=\"comment\"><div class=\"comment-header\">回應</div>");

                if ((*Iter)->numComments != WideString("0"))
                    AddComment((*Iter)->id, Data->text);

                Data->text->Add("</div></body></html>");
            }
        }
    }
}
//---------------------------------------------------------------------------
void Wretch::AddComment(WideString ID, TTntStringList *Text)
{
    std::list<tComment*>::iterator Iter;
    for (Iter = lComment.begin(); Iter != lComment.end(); ++Iter)
    {
        if ((*Iter)->article_id == ID)
        {
            Text->Add("<div class=\"comment-body\">");
            Text->AddStrings((*Iter)->text);
            Text->Add("</div>");
            if ((*Iter)->url != NULL)
            {
                Text->Add("<div class=\"comment-foot\"><a href=\"" +
                    (*Iter)->url + "\">" + (*Iter)->name + "</a> 於 " +
                    (*Iter)->date + " 回應");
            }
            else
            {
                Text->Add("<div class=\"comment-foot\">" + (*Iter)->name +
                    " 於 " + (*Iter)->date + " 回應");
            }
            if ((*Iter)->email != NULL)
                Text->Add(" | <a href=\"mailto:" + (*Iter)->email + "\">EMail</a>");
            if ((*Iter)->ip != NULL)
                Text->Add("<br />IP: " + (*Iter)->ip);
            Text->Add("</div>");

			/*
			 * The reply part
			 */
			if ((*Iter)->reply_date != NULL)
			{
				Text->Add("<div class=\"comment-reply-body\">");
				Text->AddStrings((*Iter)->reply);
				Text->Add("</div>");
				Text->Add("<div class=\"comment-reply-foot\">" + szOwner + " 於 " +
					(*Iter)->reply_date + " 回應</div>");
			}
        }
    }
}
//---------------------------------------------------------------------------
/*
 * MT Format: http://www.sixapart.com/movabletype/docs/mtimport
 */
void Wretch::ArticleToMt(TTntStringList *MT)
{
    WideString yyyy, mm, dd, time;
    std::list<tArticle*>::iterator Iter;
    std::list<tCategory*>::iterator cIter;

    for (Iter = lArticle.begin(); Iter != lArticle.end(); ++Iter)
    {
        MT->Add(WideString("AUTHOR: ") + szOwner);
        MT->Add(WideString("TITLE: ") + (*Iter)->title);
        yyyy = (*Iter)->date.SubString(1, 4);
        mm   = (*Iter)->date.SubString(6, 2);
        dd   = (*Iter)->date.SubString(9, 2);
        time = (*Iter)->date.SubString(12, 8);
        MT->Add("DATE: " + mm + "/" + dd + "/" + yyyy + " " + time);
        MT->Add("STATUS: publish");
        if ((*Iter)->allow_comment == WideString("1"))
            MT->Add("ALLOW COMMENTS: 1");
		else
		    MT->Add("ALLOW COMMENTS: 0");

        for (cIter = lCategory.begin(); cIter != lCategory.end(); ++cIter)
        {
            if ((*cIter)->id == (*Iter)->category_id)
            {
                MT->Add(WideString("PRIMARY CATEGORY: ") + ((*cIter)->name));
                break;  // break the for loop
            }
        }
        MT->Add("-----");

        MT->Add("BODY:");
        MT->AddStrings((*Iter)->text);
        MT->Add("-----");

        if ((*Iter)->numComments != WideString("0")) // has comments
            CommentToMt(MT, (*Iter)->id);

        MT->Add("--------");
    }
}
//---------------------------------------------------------------------------
void Wretch::CommentToMt(TTntStringList *MT, WideString ID)
{
    WideString yyyy, mm, dd, time;
    std::list<tComment*>::iterator Iter;
    for (Iter = lComment.begin(); Iter != lComment.end(); ++Iter)
    {
        if ((*Iter)->article_id == ID)
        {
            MT->Add("COMMENT:");
            MT->Add(WideString("AUTHOR: ") + (*Iter)->name);
            yyyy = (*Iter)->date.SubString(1, 4);
            mm   = (*Iter)->date.SubString(6, 2);
            dd   = (*Iter)->date.SubString(9, 2);
            time = (*Iter)->date.SubString(12, 8);
            MT->Add("DATE: " + mm + "/" + dd + "/" + yyyy + " " + time);
            if ((*Iter)->url != NULL)
                MT->Add("URL: " + (*Iter)->url);
            if ((*Iter)->email != NULL)
                MT->Add("EMAIL: " + (*Iter)->email);
            if ((*Iter)->ip != NULL)
                MT->Add("IP: " + (*Iter)->ip);
            MT->AddStrings((*Iter)->text);
            MT->Add("-----");

			/*
			 * The reply part
			 */
			if ((*Iter)->reply_date != NULL)
			{
	            MT->Add("COMMENT:");
	            MT->Add(WideString("AUTHOR: ") + szOwner);
	            yyyy = (*Iter)->reply_date.SubString(1, 4);
	            mm   = (*Iter)->reply_date.SubString(6, 2);
	            dd   = (*Iter)->reply_date.SubString(9, 2);
	            time = (*Iter)->reply_date.SubString(12, 8);
	            MT->Add("DATE: " + mm + "/" + dd + "/" + yyyy + " " + time);
	            MT->AddStrings((*Iter)->reply);
	            MT->Add("-----");
			}
        }
    }
}
//---------------------------------------------------------------------------
void Wretch::ExportArticle(TTntTreeNode *Node, TTntStringList *slOutput)
{
    tArticle *tData;
    tNodeData *data = (tNodeData*)Node->Data;

    WideString yyyy, mm, dd, time;
    std::list<tCategory*>::iterator cIter;
    std::list<tArticle*>::iterator Iter;

    for (Iter = lArticle.begin(); Iter != lArticle.end(); ++Iter)
    {
        if (StrToInt((*Iter)->id) == data->id)
        {
            slOutput->Add(WideString("AUTHOR: ") + szOwner);
            slOutput->Add(WideString("TITLE: ") + (*Iter)->title);
            yyyy = (*Iter)->date.SubString(1, 4);
            mm   = (*Iter)->date.SubString(6, 2);
            dd   = (*Iter)->date.SubString(9, 2);
            time = (*Iter)->date.SubString(12, 8);
            slOutput->Add("DATE: " + mm + "/" + dd + "/" + yyyy + " " + time);
            slOutput->Add("STATUS: publish");
            if ((*Iter)->allow_comment == WideString("1"))
                slOutput->Add("ALLOW COMMENTS: 1");
            else
                slOutput->Add("ALLOW COMMENTS: 0");

            for (cIter = lCategory.begin(); cIter != lCategory.end(); ++cIter)
            {
                if ((*cIter)->id == (*Iter)->category_id)
                {
                    slOutput->Add(WideString("PRIMARY CATEGORY: ") + ((*cIter)->name));
                    break;  // break the for loop
                }
            }
            slOutput->Add("-----");

            slOutput->Add("BODY:");
            slOutput->AddStrings((*Iter)->text);
            slOutput->Add("-----");

            if ((*Iter)->numComments != WideString("0")) // has comments
                CommentToMt(slOutput, (*Iter)->id);

            slOutput->Add("--------");
        }
    }
}
//---------------------------------------------------------------------------

