//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Wretch.h"
#include "MoveableType.h"
#include "CommonUtil.h"
#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TntExtCtrls"
#pragma link "TntStdCtrls"
#pragma resource "*.dfm"
TForm1 *Form1;

WideString szAppPath;   // this application's location path
AnsiString szReferer;   // the Referer of the Header

Wretch *fWretch   = NULL;
MoveableType *fMT = NULL;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
    : TTntForm(Owner)
{
    // Initialize the code mapping table
    InitCodeTable();
    
    // Get the application's path
    szAppPath = WideString(ExtractFilePath(Application->ExeName));

    // Put the progressbar in the statusbar
    ProgressBar1->Parent = StatusBar1;
    ProgressBar1->Top = 2;
    ProgressBar1->Height = StatusBar1->Height - 2;
    ProgressBar1->Left = StatusBar1->Panels->Items[0]->Width + 2;
    ProgressBar1->Width = StatusBar1->Panels->Items[1]->Width - 2;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCreate(TObject *Sender)
{
    /* Show the Version No. in Caption */
    DWORD dwVerInfoSize = 0;
    AnsiString szFile = Application->ExeName;
    dwVerInfoSize = GetFileVersionInfoSize(szFile.c_str(), &dwVerInfoSize);
    BYTE *bVerInfoBuf = new BYTE[dwVerInfoSize];
    if (GetFileVersionInfo(szFile.c_str(), 0, dwVerInfoSize, bVerInfoBuf))
    {
        VS_FIXEDFILEINFO *vsInfo;
        UINT vsInfoSize;
        if (VerQueryValue(bVerInfoBuf, "\\", (void**)&vsInfo, &vsInfoSize))
        {
            int iFileVerMajor   = HIWORD(vsInfo->dwFileVersionMS);
            int iFileVerMinor   = LOWORD(vsInfo->dwFileVersionMS);
            int iFileVerRelease = HIWORD(vsInfo->dwFileVersionLS);
            int iFileVerBuild   = LOWORD(vsInfo->dwFileVersionLS);
            Caption = Caption + IntToStr(iFileVerMajor) + "." + IntToStr(iFileVerMinor) +
            "." + IntToStr(iFileVerRelease) + "." + IntToStr(iFileVerBuild);
        }
    }
    delete bVerInfoBuf;

    /*
     * Initial OLE, thus enable the Context Menu functions
     * in the CppWebBrowser component.
     */
    OleInitialize(NULL);

    if (ParamCount() == 1)
    {
        if (ExtractFileExt(ParamStr(1)) == ".xml")
        {
            fWretch = new Wretch(WideString(ParamStr(1)), xmlDoc, tvArticle);
            tbExportAsMt->Enabled = true; // Enable the export menu
        }
        else if (ExtractFileExt(ParamStr(1)) == ".txt")
        {
            fMT = new MoveableType(WideString(ParamStr(1)), tvArticle);
            tbExportAsMt->Enabled = false; // Disable the export menu
        }
        Caption = tvArticle->Items->Item[0]->Text;
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
    ClearData(tvArticle);
    OleUninitialize();
    if (FileExists(szAppPath + "temp.html"))
        DeleteFile(szAppPath + "temp.html");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ClearData(TTntTreeView *tvTree)
{
    if (fWretch != NULL)
    {
        fWretch->ClearData(tvTree);
        delete fWretch;
        fWretch = NULL;
    }
    else if (fMT != NULL)
    {
        fMT->ClearData(tvTree);
        delete fMT;
        fMT = NULL;
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::tvArticleChange(TObject *Sender, TTreeNode *Node)
{
    if (Node == NULL || Node->Data == NULL)
        return;

    /* Show the article when clicking */
    TVariant headers;
    if (szReferer != "")
        headers = "Referer:" + szReferer;   // set Referer so that we can see the pics.
    ((tNodeData*)Node->Data)->text->AnsiStrings->SaveToFileEx(szAppPath + "temp.html", CP_UTF8);
    Browser->Navigate(WideString("file:///") + szAppPath + WideString("temp.html"),
                      NULL, NULL, NULL, &headers);
//    TMemoryStream *stm = new TMemoryStream();
//    TStreamAdapter *sa = new TStreamAdapter(stm, soReference);
//
//    ((tNodeData*)Node->Data)->text->SaveToStream(stm);
//
//    IPersistStreamInit *psi = NULL;
//    stm->Seek(0, 0);
//    if (sa == NULL)
//    {
//        delete stm;
//        return;
//    }
//    _di_IDispatch doc = Browser->Document;
//
//    if (doc == NULL)
//    {
//        TVariant url = "about:blank";
//        Browser->Navigate2(&url);
//        while (doc == NULL)
//        {
//            Application->ProcessMessages();
//            doc = Browser->Document;
//        }
//    }
//    if (doc->QueryInterface(IID_IPersistStreamInit, (void**)&psi ) == S_OK)
//    {
//        if (psi)
//        {
//            psi->Load(*sa);
//        }
//    }
//    else
//        delete sa;
//    delete stm;
//
    Caption = tvArticle->Selected->Text;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::tvArticleCollapsed(TObject *Sender,
      TTreeNode *Node)
{
    Node->ImageIndex = 0;
    Node->SelectedIndex = 0;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::tvArticleExpanded(TObject *Sender, TTreeNode *Node)
{
    Node->ImageIndex = 1;
    Node->SelectedIndex = 1;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::BrowserStatusTextChange(TObject *Sender, BSTR URL)
{
    /*
     * When the mouse cursor points to a link in the browser,
     * show the url in the statusbar.
     */
    StatusBar1->Panels->Items[0]->Text = URL;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::BrowserProgressChange(TObject *Sender,
      long Progress, long ProgressMax)
{
    /*
     * Show the download progress in the progressbar,
     * -1 means complete.
     */
    if (Progress == -1)
    {
        ProgressBar1->Position = 0;
    }
    else
    {
        ProgressBar1->Max = ProgressMax;
        ProgressBar1->Position = Progress;
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::StatusBar1Resize(TObject *Sender)
{
    ProgressBar1->Width =
        StatusBar1->Width - StatusBar1->Panels->Items[0]->Width - 2;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PopupMenu1Popup(TObject *Sender)
{
    TTntTreeNode *Node = tvArticle->Selections[0];
    /* If root node is selected */
    if (Node->Level == 0)
    {
        pmItem1->Enabled = false;
        pmItem2->Enabled = false;
    }
    /* If category nodes are selected */
    else if (Node->HasChildren)
    {
        pmItem1->Enabled = false;
        pmItem2->Enabled = true;
    }
    /* If article nodes are selected */
    else
    {
        pmItem1->Enabled = true;
        pmItem2->Enabled = false;
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::tvArticleMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    TPoint p;
    GetCursorPos(&p);
    if (Button == mbRight && tvArticle->GetNodeAt(X, Y) != NULL)
    {
        tvArticle->GetNodeAt(X, Y)->Selected = true;
        PopupMenu1->Popup(p.x, p.y);
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::pmItem1Click(TObject *Sender)
{
    SaveDialog1->Title = "將選取的文章匯出成 MT 格式";
    SaveDialog1->Filter = "MT 格式 (*.txt)|*.txt";
    SaveDialog1->FileName = "文章匯出 - " + Date().FormatString("yyyy-mm-dd");
    SaveDialog1->DefaultExt = "txt";

    if (SaveDialog1->Execute())
    {
        Screen->Cursor = crHourGlass;
        TTntStringList *MT = new TTntStringList;
        int count = tvArticle->SelectionCount, i;

        if (fWretch != NULL)
        {
            for (i = 0; i < count; ++i)
                fWretch->ExportArticle(tvArticle->Selections[i], MT);
        }
        else
        {
            for (i = 0; i < count; ++i)
                fMT->ExportArticle(tvArticle->Selections[i], MT);
        }

        //MT->AnsiStrings->SaveToFileEx(SaveDialog1->FileName, CP_UTF8);
        MT->SaveToFile(SaveDialog1->FileName);
        delete MT;
        Screen->Cursor = crDefault;
        ShowMessage("匯出完成：\n\n" + SaveDialog1->FileName);
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::pmItem2Click(TObject *Sender)
{
    SaveDialog1->Title = "將選取的分類匯出成 MT 格式";
    SaveDialog1->Filter = "MT 格式 (*.txt)|*.txt";
    SaveDialog1->FileName = "文章匯出 - " + Date().FormatString("yyyy-mm-dd");
    SaveDialog1->DefaultExt = "txt";

    if (SaveDialog1->Execute())
    {
        Screen->Cursor = crHourGlass;
        TTntStringList *MT = new TTntStringList;
        int count = tvArticle->SelectionCount, i, j, subCount;
        TTntTreeNode *Node;

        if (fWretch != NULL)
        {
            for (i = 0; i < count; ++i)
            {
                subCount = tvArticle->Selections[i]->Count;
                for (j = 0; j < subCount; ++j)
                {
                    Node = tvArticle->Selections[i]->Item[j];
                    if (Node->HasChildren)
                    {
                        for (int k = 0; k < Node->Count; ++k)
                            fWretch->ExportArticle(Node->Item[k], MT);
                    }
                    else
                        fWretch->ExportArticle(Node, MT);
                }
            }
        }
        else
        {
            for (i = 0; i < count; ++i)
            {
                subCount = tvArticle->Selections[i]->Count;
                for (j = 0; j < subCount; ++j)
                {
                    fMT->ExportArticle(tvArticle->Selections[i]->Item[j], MT);
                }
            }
        }

        //MT->AnsiStrings->SaveToFileEx(SaveDialog1->FileName, CP_UTF8);
        MT->SaveToFile(SaveDialog1->FileName);
        delete MT;
        Screen->Cursor = crDefault;
        ShowMessage("匯出完成：\n\n" + SaveDialog1->FileName);
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::tbLoadFileClick(TObject *Sender)
{
    OpenDialog1->InitialDir = szAppPath + "Data";
    OpenDialog1->Filter =
        "所有支援格式|*.xml;*.txt|無名網誌備份檔(*.xml)|*.xml|MT 格式備份檔(*.txt)|*.txt";
    if (OpenDialog1->Execute())
    {
        Screen->Cursor = crHourGlass;
        Browser->Navigate(WideString("about:blank"), NULL, NULL, NULL, NULL);
        ClearData(tvArticle);

        if (ExtractFileExt(OpenDialog1->FileName) == ".xml")
        {
            fWretch = new Wretch(OpenDialog1->FileName, xmlDoc, tvArticle);
            tbExportAsMt->Enabled = true; // Enable the export menu
        }
        else if (ExtractFileExt(OpenDialog1->FileName) == ".txt")
        {
            fMT = new MoveableType(OpenDialog1->FileName, tvArticle);
            tbExportAsMt->Enabled = false; // Disable the export menu
        }
        Screen->Cursor = crDefault;
        edtSearch->Enabled = true;
        tbSearch->Enabled = true;
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::tbExportAsMtClick(TObject *Sender)
{
    SaveDialog1->Title = "匯出成 MT 格式";
    SaveDialog1->Filter = "MT 格式 (*.txt)|*.txt";
    // SaveDialog1->FileName = szOwner + " - " + tvArticle->Items->GetFirstNode()->Text;
    SaveDialog1->FileName = Date().FormatString("yyyy-mm-dd");
    SaveDialog1->DefaultExt = "txt";
    if (SaveDialog1->Execute())
    {
        Screen->Cursor = crHourGlass;
        TTntStringList *MT = new TTntStringList;
        fWretch->ArticleToMt(MT);
        //MT->AnsiStrings->SaveToFileEx(SaveDialog1->FileName, CP_UTF8);
        MT->SaveToFile(SaveDialog1->FileName);
        delete MT;
        Screen->Cursor = crDefault;
        ShowMessage("轉換完畢");
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::tbVisitBlogClick(TObject *Sender)
{
    ShellExecute(NULL, NULL, "http://blog.nelson.csie.us/", NULL, NULL, SW_SHOW);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::comboRefererChange(TObject *Sender)
{
    szReferer = comboReferer->Text;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::edtSearchKeyPress(TObject *Sender, char &Key)
{
    if (Key == VK_RETURN)
        tbSearch->Click();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::edtSearchMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (Button == mbLeft)
    {
        edtSearch->SelectAll();
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::tbSearchClick(TObject *Sender)
{
    if (fWretch != NULL)
        fWretch->CreateTree(tvArticle, edtSearch->Text);
    else if (fMT != NULL)
        fMT->CreateTree(tvArticle, edtSearch->Text);
}
//---------------------------------------------------------------------------


