#ifndef WretchH
#define WretchH

#include <XMLDoc.hpp>
#include <xmldom.hpp>
#include <XMLIntf.hpp>
#include "TntComCtrls.hpp"
#include "TntDialogs.hpp"
#include <list>
// #include <winsock2.h>   // in order to convert IP to A.B.C.D format

/*
 * struct to store article's information
 */
typedef struct tArticle
{
    WideString id;
    WideString category_id;
    WideString title;
    WideString date;
    WideString allow_comment;
    WideString numComments;
    TTntStringList *text;
} tArticle;

/*
 * struct to store folder's information
 */
typedef struct tFolder
{
    WideString id;
    WideString name;
    WideString order;
    WideString numCategories;
} tFolder;

/*
 * struct to store aritcle's comment
 */
typedef struct tComment
{
    WideString article_id;
    WideString date;
    WideString name;
    WideString email;
    WideString url;
    WideString ip;
    TTntStringList *text;
	WideString reply_date;
	TTntStringList *reply;
} tComment;

/*
 * struct to store folder category
 */
typedef struct tCategory
{
    WideString id;
    WideString folder_id;
    WideString name;
    WideString order;
    WideString numPosts;
} tCategory;

//---------------------------------------------------------------------------
class Wretch
{
public:
    Wretch();
    Wretch(WideString wsFileName, TXMLDocument *xmlDoc, TTntTreeView *tvTree);
    ~Wretch();

    void Load(WideString wsFileName, TXMLDocument *xmlDoc, TTntTreeView *tvTree);
    void ClearData(TTntTreeView *tvTree);
    void CreateTree(TTntTreeView *tvTree, WideString Keyword);

    void ArticleToMt(TTntStringList *MT);
    void CommentToMt(TTntStringList *MT, WideString ID);
    void ExportArticle(TTntTreeNode *Node, TTntStringList *slOutput);

    /* Compare the Article's date */
    friend bool CmpArticle(tArticle *A1, tArticle *A2);

    /* Compare the Folder's order */
    friend bool CmpFolder(tFolder *A1, tFolder *A2);

    /*
     * Compare the Category's order
     */
    friend bool CmpCategory(tCategory *A1, tCategory *A2);

private:
    _di_IXMLNode diRoot;    // the root node of the XML file
    WideString szOwner;     // the owner of this blog
    int NumPosts;           // the number of all the posts

    std::list <tArticle*>  lArticle;    // lArticle: Store article's information
    std::list <tFolder*>   lFolder;     // lFolder: Store folder's information
    std::list <tComment*>  lComment;    // lComment: Store article's information
    std::list <tCategory*> lCategory;   // lCategory: Store folder category's information

    void ParseArticle(_di_IXMLNode diNode);
    void ParseFolder(_di_IXMLNode diNode);
    void ParseComment(_di_IXMLNode diNode);
    void ParseCategory(_di_IXMLNode diNode);

    void AddFolder(TTntTreeView *tvTree, TTntTreeNode *Root, WideString Keyword);
    void AddCategory(TTntTreeView *tvTree, TTntTreeNode *Root, WideString ID, WideString Keyword);
    void AddArticle(TTntTreeView *tvTree, TTntTreeNode *Root, WideString ID, WideString Keyword);
    void AddComment(WideString ID, TTntStringList *Text);
};

#endif
