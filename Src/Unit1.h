//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
#include "SHDocVw_OCX.h"
#include "TntComCtrls.hpp"
#include "TntDialogs.hpp"
#include "TntMenus.hpp"
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>
#include <Menus.hpp>
#include <msxmldom.hpp>
#include <OleCtrls.hpp>
#include <XMLDoc.hpp>
#include <xmldom.hpp>
#include <XMLIntf.hpp>
#include "TntExtCtrls.hpp"
#include "TntStdCtrls.hpp"
#include <StdCtrls.hpp>
#include <ToolWin.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TTntForm
{
__published:	// IDE-managed Components
    TXMLDocument *xmlDoc;
    TCppWebBrowser *Browser;
    TSplitter *Splitter1;
    TPanel *Panel1;
    TImageList *imgTreeView;
    TTntTreeView *tvArticle;
    TTntOpenDialog *OpenDialog1;
    TTntSaveDialog *SaveDialog1;
    TTntStatusBar *StatusBar1;
    TTntPopupMenu *PopupMenu1;
    TTntMenuItem *pmItem1;
    TTntMenuItem *pmItem2;
    TTntProgressBar *ProgressBar1;
    TTntToolBar *TntToolBar1;
    TTntToolButton *tbLoadFile;
    TTntToolButton *tbExportAsMt;
    TTntToolButton *tbVisitBlog;
    TTntToolButton *tbSep1;
    TTntEdit *edtSearch;
    TTntToolButton *tbSearch;
    TTntToolButton *tbSep2;
    TTntComboBox *comboReferer;
    TImageList *imgToolBar;
    TTntLabel *TntLabel1;
    TTntLabel *TntLabel2;
    void __fastcall tvArticleChange(TObject *Sender, TTreeNode *Node);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall tvArticleCollapsed(TObject *Sender, TTreeNode *Node);
    void __fastcall BrowserStatusTextChange(TObject *Sender, BSTR URL);
    void __fastcall BrowserProgressChange(TObject *Sender, long Progress,
          long ProgressMax);
    void __fastcall StatusBar1Resize(TObject *Sender);
    void __fastcall tvArticleExpanded(TObject *Sender, TTreeNode *Node);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall PopupMenu1Popup(TObject *Sender);
    void __fastcall tvArticleMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall pmItem1Click(TObject *Sender);
    void __fastcall pmItem2Click(TObject *Sender);
    void __fastcall tbLoadFileClick(TObject *Sender);
    void __fastcall tbExportAsMtClick(TObject *Sender);
    void __fastcall tbVisitBlogClick(TObject *Sender);
    void __fastcall comboRefererChange(TObject *Sender);
    void __fastcall edtSearchKeyPress(TObject *Sender, char &Key);
    void __fastcall tbSearchClick(TObject *Sender);
    void __fastcall edtSearchMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
private:	// User declarations
    void __fastcall ClearData(TTntTreeView *tvTree);
public:		// User declarations
    __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
