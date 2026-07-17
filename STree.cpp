//--- This code is Copyright 2007, Tamer F. Abdelmaguid
//    Use of this code or portions of it is granted without fee for academic,
//    non-commercial purposes only provided that the above copyright statement
//    is clearly indicated
//------------------------------------------------------------------------------

#include <math.h>
#include "STree.h"
#include "SList.h"

//---------------------------------------------------------------------------
STree::SNode::SNode(STree::SNode *parent, void *data)
{
   _Parent = parent;
   _Left = _Right = _LeftMostChild = NULL;
   _data = data;
}

STree::SNode::~SNode()
{
}

STree::SNode *STree::SNode::getRightMostChild()
{
  STree::SNode *ret = _LeftMostChild;
  while(ret->_Right){ ret = ret->_Right; }
  return ret;
}

//---------------------------------------------------------------------------

STree::STree(void *data)
{
  _Data = data;
  RootNode = new STree::SNode(NULL, data);
  CurrentNode = RootNode;
  Nodes = new SList;
  __currLevel = new SList;
  __mngrsLevel = new SList;
  __mngrs = new SList;
  _currLevel = new SList;
  _mngrsLevel = new SList;
  _mngrs = new SList;

  Nodes->Add(RootNode);

  nlevels = 0;
}

STree::~STree()
{
  int i, n=Nodes->Count();
  STree::SNode *nd;

  for(i=0; i<n; i++){
    nd = (STree::SNode *)Nodes->Item(i);
    delete nd;
  }
  delete Nodes;
  delete __currLevel;         delete _currLevel;
  delete __mngrsLevel;        delete _mngrsLevel;
  delete __mngrs;             delete _mngrs;
}

bool STree::AddLeftNode(void *data)
{
  if(CurrentNode == RootNode || !CurrentNode) return false;

  STree::SNode *mngrNode = CurrentNode->_Parent;
  STree::SNode *newNode = new STree::SNode(mngrNode, data);
  STree::SNode *leftNode = CurrentNode->_Left;

  if(leftNode != NULL){
    CurrentNode->_Left = newNode;    newNode->_Right = CurrentNode;
    leftNode->_Right = newNode;      newNode->_Left = leftNode;
  }
  else{
    CurrentNode->_Left = newNode;    newNode->_Right = CurrentNode;
    mngrNode->_LeftMostChild = newNode;
  }
  Nodes->Add(newNode);
  CurrentNode = newNode;

  return true;
}
bool STree::AddRightNode(void *data)
{
  if(CurrentNode == RootNode || !CurrentNode) return false;

  STree::SNode *mngrNode = CurrentNode->_Parent;
  STree::SNode *newNode = new STree::SNode(mngrNode, data);
  STree::SNode *rightNode = CurrentNode->_Right;

  if(rightNode != NULL){
    CurrentNode->_Right = newNode;   newNode->_Left = CurrentNode;
    rightNode->_Left = newNode;      newNode->_Right = rightNode;
  }
  else{
    CurrentNode->_Right = newNode;    newNode->_Left = CurrentNode;
  }
  Nodes->Add(newNode);
  CurrentNode = newNode;

  return true;
}
bool STree::AddParent(void *data)
{
  if(CurrentNode == RootNode || !CurrentNode) return false;

  STree::SNode *mngrNode = CurrentNode->_Parent;
  STree::SNode *rightNode = CurrentNode->_Right;
  STree::SNode *leftNode = CurrentNode->_Left;
  STree::SNode *newNode = new STree::SNode(mngrNode, data);

  newNode->_LeftMostChild = CurrentNode;
  newNode->_Left = leftNode;
  if(leftNode != NULL) leftNode->_Right = newNode;
  else mngrNode->_LeftMostChild = newNode;
  newNode->_Right = rightNode;
  if(rightNode != NULL) rightNode->_Left = newNode;

  CurrentNode->_Parent = newNode;
  CurrentNode->_Left = CurrentNode->_Right = NULL;

  Nodes->Add(newNode);
  CurrentNode = newNode;

  return true;
}
bool STree::AddChild(void *data)
{
  if(!CurrentNode) return false;

  STree::SNode *newNode = new STree::SNode(CurrentNode, data);
  STree::SNode *leftmostNode = CurrentNode->_LeftMostChild;

  CurrentNode->_LeftMostChild = newNode;

  newNode->_Left = NULL;
  newNode->_Right = leftmostNode;
  if(leftmostNode) leftmostNode->_Left = newNode;

  Nodes->Add(newNode);
  CurrentNode = newNode;

  return true;
}
bool STree::RemoveNode(STree::SNode *nd)
{
  if(Nodes->IndexOf(nd)==-1 || nd==RootNode) return false;

  STree::SNode *mngrNode = nd->_Parent;
  STree::SNode *rightNode = nd->_Right;
  STree::SNode *leftNode = nd->_Left;

  STree::SNode *subNode = nd->_LeftMostChild;
  if(subNode){
    subNode->_Left = leftNode;
    if(leftNode) leftNode->_Right = subNode;
    else mngrNode->_LeftMostChild = subNode;

    while(true){
      subNode->_Parent = mngrNode;
      subNode->_Level--;
      if(!subNode->_Right) break;
      subNode = subNode->_Right;
    }
    subNode->_Right = rightNode;
    if(rightNode) rightNode->_Left = subNode;
  }
  else{
    if(leftNode){
      leftNode->_Right = rightNode;
      if(rightNode) rightNode->_Left = leftNode;
    }
    else if(rightNode){
      rightNode->_Left = NULL;
      mngrNode->_LeftMostChild = rightNode;
    }
    else mngrNode->_LeftMostChild = NULL;
  }

  Nodes->Remove(nd);
  delete nd;

  return true;
}
bool STree::RemoveCurrentNode()
{
  STree::SNode *nd = CurrentNode;
  if(CurrentNode != RootNode)
     CurrentNode = CurrentNode->_Parent;
  return RemoveNode(nd);
}
bool STree::DeleteNode(STree::SNode *nd)
{
  if(Nodes->IndexOf(nd)==-1 || nd==RootNode) return false;

  STree::SNode *mngrNode = nd->_Parent;
  STree::SNode *rightNode = nd->_Right;
  STree::SNode *leftNode = nd->_Left;

  STree::SNode *subNode = nd->_LeftMostChild;
  STree::SNode *nextNode;
  while(subNode){
    nextNode = subNode->_Right;
    DeleteNode(subNode);
    subNode = nextNode;
  }
  Nodes->Remove(nd);
  delete nd;

  if(rightNode) rightNode->_Left = leftNode;
  if(leftNode) leftNode->_Right = rightNode;
  else{
    mngrNode->_LeftMostChild = rightNode;
  }

  return true;
}
bool STree::DeleteCurrentNode()
{
  STree::SNode *nd = CurrentNode;
  if(CurrentNode != RootNode)
     CurrentNode = CurrentNode->_Parent;
  return DeleteNode(nd);
}
bool STree::ClearAll()
{
  int i, n = Nodes->Count();
  STree::SNode *nd;

  for(i=1; i<n; i++){
    nd = (STree::SNode *)Nodes->Item(i);
    delete nd;
  }
  Nodes->Clear();
  RootNode->_Left = RootNode->_Right = RootNode->_LeftMostChild = NULL;
  Nodes->Add(RootNode);
  CurrentNode = RootNode;

  return true;
}

void STree::setCurrentNode(STree::SNode *nd)
{
  CurrentNode = nd;
}
STree::SNode *STree::MoveRight()
{
  if(CurrentNode->_Right) return (CurrentNode = CurrentNode->_Right);
  return NULL;
}
STree::SNode *STree::MoveLeft()
{
  if(CurrentNode->_Left) return (CurrentNode = CurrentNode->_Left);
  return NULL;
}
STree::SNode *STree::MoveUp()
{
  if(CurrentNode->_Parent) return (CurrentNode = CurrentNode->_Parent);
  return NULL;
}
STree::SNode *STree::MoveDown()
{
  if(CurrentNode->_LeftMostChild) return (CurrentNode = CurrentNode->_LeftMostChild);
  return NULL;
}

void STree::Iterate(SNodesIterator Itr, void *data)
{
  SList *currLevel = __mngrsLevel;   currLevel->Clear();
  SList *nextLevel = __currLevel;    nextLevel->Clear();
  int i, n;
  STree::SNode *currNode, *subNode;

  currLevel->Add(RootNode);
  while((n = currLevel->Count()) != 0){
    for(i=0; i<n; i++){
      currNode = (STree::SNode *)currLevel->Item(i);
      (*Itr)(currNode, data);
      subNode = currNode->_LeftMostChild;
      while(subNode){
        nextLevel->Add(subNode);
        subNode = subNode->_Right;
      }
    }
    currLevel->Clear();
    SList *lvl = currLevel;    currLevel = nextLevel;    nextLevel = lvl;
  }
  currLevel->Clear();          nextLevel->Clear();
}

void STree::IterateForSubTree(SNodesIterator Itr, SNode *nd, void *data)
{
  SList *currLevel = __mngrsLevel;   currLevel->Clear();
  SList *nextLevel = __currLevel;    nextLevel->Clear();
  int i, n;
  STree::SNode *currNode, *subNode;

  currLevel->Add(nd);
  while((n = currLevel->Count()) != 0){
    for(i=0; i<n; i++){
      currNode = (STree::SNode *)currLevel->Item(i);
      (*Itr)(currNode, data);
      subNode = currNode->_LeftMostChild;
      while(subNode){
        nextLevel->Add(subNode);
        subNode = subNode->_Right;
      }
    }
    currLevel->Clear();
    SList *lvl = currLevel;    currLevel = nextLevel;    nextLevel = lvl;
  }
  currLevel->Clear();          nextLevel->Clear();
}

int IndexSort(void *p1, void *p2)
{
  STree::SNode *nd1 = (STree::SNode *)p1;
  STree::SNode *nd2 = (STree::SNode *)p2;
  return (nd1->Index - nd2->Index);
}

STree::SNode *STree::CommonParent(STree::SNode *nd1, STree::SNode *nd2)
{
  SList *mngrs1 = __mngrs;          mngrs1->Clear();
  STree::SNode *mngr = nd1->_Parent;
  while(mngr){  mngrs1->Add(mngr);  mngr = mngr->_Parent; }
  mngr = nd2->_Parent;
  while(mngrs1->IndexOf(mngr)==-1) mngr = mngr->_Parent;
  mngrs1->Clear();
  return mngr;
}

STree::SNode *STree::getNode(void *data)
{
  if(!Nodes) return NULL;

  int i, n=Nodes->Count();
  STree::SNode *ret = NULL;
  STree::SNode *node;

  for(i=n-1; i>=0; i--){
    node = (STree::SNode *)Nodes->Item(i);
    if(node->_data == data){
      ret = node;
      break;
    }
  }
  return ret;
}

#pragma package(smart_init)
