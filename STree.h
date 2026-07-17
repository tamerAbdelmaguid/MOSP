//--- This code is Copyright 2007, Tamer F. Abdelmaguid
//    Use of this code or portions of it is granted without fee for academic,
//    non-commercial purposes only provided that the above copyright statement
//    is clearly indicated
//------------------------------------------------------------------------------

#ifndef STreeH
#define STreeH

#include <vcl.h>
#pragma hdrstop
#include "SList.h"
//---------------------------------------------------------------------------

class STree {
  public:
    class SNode{
        friend class STree;
        friend int IndexSort(void *p1, void *p2);
      private:
        void *_data;
        SNode *_Parent;
        SNode *_Left;
        SNode *_Right;
        SNode *_LeftMostChild;

        int _Index;
        int _Level, _nSubs;

      public:
        SNode(SNode *parent, void *data=NULL);
        ~SNode();

        SNode *getRightMostChild();
        bool HasChildren(){ return (_LeftMostChild)? true : false; }

        __property SNode *Parent = {read = _Parent};
        __property SNode *Left = {read = _Left};
        __property SNode *Right = {read = _Right};
        __property SNode *LeftMostChild = {read = _LeftMostChild};
        __property SNode *RightMostChild = {read = getRightMostChild};

        __property int Index = {read = _Index};
        __property int level = {read = _Level};
        __property int nSubs = {read = _nSubs};

        void *Data(){ return _data; }
    };

  private:
    void *_Data;
    SNode *RootNode, *CurrentNode;
    SList *Nodes;
    SList *__currLevel, *__mngrsLevel, *__mngrs;
    SList *_currLevel, *_mngrsLevel, *_mngrs;

    int  nlevels;

  public:
    STree(void *data=NULL);
    ~STree();

    bool AddLeftNode(void *data);
    bool AddRightNode(void *data);
    bool AddParent(void *data);
    bool AddChild(void *data);

    bool AddLeftNodeTo(SNode *nd, void *data)
            { CurrentNode=nd; return AddLeftNode(data);}
    bool AddRightNodeTo(SNode *nd, void *data)
            { CurrentNode=nd; return AddRightNode(data);}
    bool AddParentTo(SNode *nd, void *data)
            { CurrentNode=nd; return AddParent(data);}
    bool AddChildTo(SNode *nd, void *data)
            { CurrentNode=nd; return AddChild(data);}
    bool RemoveNode(SNode *nd);  // just delete the node and put its child nodes
    bool RemoveCurrentNode();    // in its current level.
    bool DeleteNode(SNode *nd); // delete the node and all
    bool DeleteCurrentNode();   // its child nodes.
    bool ClearAll();       // delete all nodes

    void setCurrentNode(SNode *nd);
    SNode *MoveRight();
    SNode *MoveLeft();
    SNode *MoveUp();
    SNode *MoveDown();

    typedef void(*SNodesIterator)(STree::SNode *, void *);
    void Iterate(SNodesIterator, void *);
    void IterateForSubTree(SNodesIterator, SNode *nd, void *);

    SNode *CommonParent(SNode *nd1, SNode *nd2);

    int GetCurrNodeLevel(){ return CurrentNode->level; }

    STree::SNode *getNode(void *data);

    SNode *GetCurrentNode(){ return CurrentNode; }
    void *GetCurrentNodeData(){ return CurrentNode->Data(); }

    __property void * Data = {read=_Data};
};

#endif
