//--- This code is Copyright 2007, Tamer F. Abdelmaguid
//    Use of this code or portions of it is granted without fee for academic,
//    non-commercial purposes only provided that the above copyright statement
//    is clearly indicated
//------------------------------------------------------------------------------

#include "SList.h"

SCompareNodes SList::_CompAlg;
SComparisonBasis SList::_CompBasis;

SList::SList()
{
  _head = _tail = NULL;
  _count = 0;
}
SList::SList(SList &lst)
{
  _head = _tail = NULL;
  _count = 0;
  Copy(lst);
}
SList::~SList()
{
  Clear();
}
SList::SNode *SList::Node(void *data)
{
  SNode *nd = _head;
  while(nd){
    if(nd->_data == data) break;
    nd = nd->_next;
  }
  return nd;
}
SList::SNode *SList::Node(int indx)
{
  int i;
  SNode *nd = _head;
  for(i=0; i<indx && nd!=NULL; i++) nd = nd->_next;
  return nd;
}
void SList::Remove(SNode *nd)
{
  if(nd == NULL) return;
  if(nd == _head && nd == _tail) _head = _tail = NULL;
  else if(nd == _tail){ _tail = nd->_prev;  _tail->_next = NULL; }
  else if(nd == _head){ _head = nd->_next;  _head->_prev = NULL; }
  else{
    SNode *p = nd->_prev, *n = nd->_next;
    p->_next = n;
    n->_prev = p;
  }
  delete nd;
  _count--;
}
void SList::Remove(SList &lst)
{
  SNode *nd = lst.head();
  while(nd){
    Remove(nd->Data());
    nd = nd->Next();
  }
}
void SList::LimitTo(SList &lst)
{
  SNode *nd = head();
  SNode *nxt;
  while(nd){
	if(lst.IndexOf(nd->Data()) == -1){
	  nxt = nd->Next();
	  Remove(nd);
	  nd = nxt;
	}
	else nd = nd->Next();
  }
}
int SList::IndexOf(void *data)
{
  int i=-1;
  SNode *nd = _head;
  if(nd){
    while(true){
      i++;
      if(nd->_data == data) break;
      nd = nd->_next;
      if(!nd){ i=-1; break; }
    }
  }
  return i;
}
int SList::OrderOf(SList::SNode *ond)
{
  int i=-1;
  SList::SNode *nd = _head;
  if(nd){
    while(true){
      i++;
      if(nd == ond) break;
      nd = nd->_next;
      if(!nd){ i=-1; break; }
    }
  }
  return i;
}
void SList::Add(void *data)
{
  SNode *nd = new SNode(data);
  if(_head == NULL) _head = _tail = nd;
  else{
    _tail->_next = nd;
    nd->_prev = _tail;
    _tail = nd;
  }
  _count++;
}
void SList::Add(SList &lst)
{
  SNode *nd = lst.head();
  while(nd){
    Add(nd->Data());
    nd = nd->Next();
  }
}
void SList::AddAfter(SNode *node, void *data)
{
  if(node == NULL) return;

  if(node == _tail) Add(data);
  else{
    SNode *nd = new SNode(data);
    SNode *nxt = node->_next;
    node->_next = nd;
    nd->_prev = node;
    nxt->_prev = nd;
    nd->_next = nxt;
    _count++;
  }
}
void SList::AddBefore(SNode *node, void *data)
{
  if(node == NULL) return;

  if(node == _head) Push(data);
  else{
    SNode *nd = new SNode(data);
    SNode *prv = node->_prev;
    prv->_next = nd;
    nd->_prev = prv;
    node->_prev = nd;
    nd->_next = node;
    _count++;
  }
}
void SList::Push(void *data)
{
  SNode *nd = new SNode(data);
  if(_head==NULL) _head = _tail = nd;
  else{
    _head->_prev = nd;
    nd->_next = _head;
    _head = nd;
  }
  _count++;
}
void SList::Clear(void)
{
  SNode *nd = _head, *next;
  while(nd){
    next = nd->_next;
    delete nd;
    nd = next;
  }
  _head = _tail = NULL;
  _count = 0;
}

void SList::Swap(int indx1, int indx2)
{
  SNode *nd1 = Node(indx1), *nd2 = Node(indx2);
  void *d = nd1->_data;
  nd1->_data = nd2->_data;
  nd2->_data = d;
}

void SList::Swap(SList::SNode *nd1, SList::SNode *nd2)
{
  void *d = nd1->_data;
  nd1->_data = nd2->_data;
  nd2->_data = d;
}

void SList::CutBefore(SNode *nd)
{
  if(nd->_prev == NULL) return;
  _head->_prev = _tail;
  _tail->_next = _head;
  _head = nd;
  _tail = nd->_prev;
  _head->_prev = NULL;
  _tail->_next = NULL;
}
void SList::CutAfter(SNode *nd)
{
  if(nd->_next == NULL) return;
  _head->_prev = _tail;
  _tail->_next = _head;
  _head = nd->_next;
  _tail = nd;
  _head->_prev = NULL;
  _tail->_next = NULL;
}
void SList::Reverse(SNode* nd1, SNode *nd2)
{
  if(nd1 == nd2) return;
  void *taildata = _tail->_data;
  bool passedbytail = false;
  if(!nd2->_prev) passedbytail = true;
  SNode *prv = (nd2->_prev)? nd2->_prev : _tail;
  while(nd2 != nd1){
    AddBefore(nd1, nd2->Data());
    Remove(nd2);
    nd2 = prv;
    if(!nd2->_prev) passedbytail = true;
    prv = (nd2->_prev)? nd2->_prev : _tail;
  }
  if(passedbytail) CutBefore(taildata);
}
void SList::EqualsReverseOf(SList &l)
{
  Clear();
  SList::SNode *nd = l.tail();
  while(nd){
    Add(nd->Data());
    nd = nd->Prev();
  }
}

int SList::Traverse(STraverseNodes trv)
{
  int i=0;
  SNode *nd = _head;
  while(nd!=NULL){
    trv(nd->_data, i);
    nd = nd->_next;
    i++;
  }
  return i;
}
void SList::QSort(SCompareNodes CompAlg, SComparisonBasis CB)
{
  _CompAlg = CompAlg;
  _CompBasis = CB;
  qs(0, Count()-1);
}
void SList::qs(int left, int right)
{
  register int i, j;
  void *x;

  if(right>left){
    i = left-1;  j = right;
    x = Item(right);

    while(true){
      while(_CompAlg(Item(++i), x)<0);
      while(_CompAlg(x, Item(--j))<0 && j>0);
      if(i >= j) break;
      Swap(i,j);
    }
    Swap(i, right);
    qs(left, i-1);
    qs(i+1, right);
  }
}

void SList::Copy(SList &lst)
{
  SNode *nd = lst._head;
  if(lst._count == _count){
    SNode *my = _head;
    while(nd!=NULL){
      my->_data = nd->_data;
      nd = nd->_next;
      my = my->_next;
    }
    return;
  }
  Clear();
  while(nd!=NULL){
    Add(nd->_data);
    nd = nd->_next;
  }
}
bool SList::operator ==(SList &lst)
{
  if(_count != lst._count) return false;
  SNode *nd = lst._head;
  SNode *my = _head;
  while(nd!=NULL){
    if(my->_data != nd->_data) return false;
    nd = nd->_next;
    my = my->_next;
  }
  return true;
}
int CompareInt(void *i1, void *i2)
{
  return (int)SList::_CompBasis * ((*(int *)i1) - (*(int *)i2));
}
int CompareReal(void *r1, void *r2)
{
  double R1 = *(double *)r1;
  double R2 = *(double *)r2;
  return ((R1>R2)? 1 : ((R1<R2)? -1 : 0)) * SList::_CompBasis;
}

// -------------------------------------- General purpose Quick sort function
void qSort(int *c, float *s, int l, int r)
{
  int i,j;
  float v;
  int tc;
  float ts;

  if(r > l){
    v = s[r]; i = l-1; j = r;
    for(;;){
      while(s[++i] < v);
      while(s[--j] > v && j > 0);
      if(i >= j) break;
      tc = c[i]; c[i] = c[j]; c[j] = tc;
      ts = s[i]; s[i] = s[j]; s[j] = ts;
    }
    tc = c[i]; c[i] = c[r]; c[r] = tc;
    ts = s[i]; s[i] = s[r]; s[r] = ts;
    qSort(c,s,l,i-1);
    qSort(c,s,i+1,r);
  }
}

int approx_psi(SList &p1, SList &p2)             // dissimilarity measure
{
  // My first attempt to evaluate the minimum number of moves needed to
  // transform one permutation to the other. This function does not produce
  // provably results; however, it can be seen as an approximation.
  //
  if(p1.Count() != p2.Count()) return -1;
  int ret = 0;

  SList l1(p1);
  SList l2(p2);

  while(true){
	SList::SNode *e1 = l1.head();
	SList::SNode *e2 = l2.head();

	if(e1 == NULL) break;

	while(e2 != NULL && e1->Data() != e2->Data()) e2 = e2->Next();

	if(e2 != NULL){
	  if(e2 != l2.head()) ret++;
	  SList::SNode *nxt1, *nxt2;
	  do{
		nxt1 = e1->Next();
		nxt2 = e2->Next();
		l1.Remove(e1);
		l2.Remove(e2);
		if(nxt2 == NULL) break;
		e1 = nxt1;
		e2 = nxt2;
	  }while(e1->Data() == e2->Data());
	}
	else return -1; // They do not contain the same elements
  }

  return ret;
}

int IntersectionSize(SList &p1, SList &p2)
{
  // for any given two permutations, this function returns the size of
  // the intersection set S = p1 \cap p2
  // it returns 0 if there is no intersection
  //
  if(p1.Count() != p2.Count()) return 0;
  int ret = 0;
  SList::SNode *h1, *h2;

  h1 = p1.head();
  while(h1){
	h2 = p2.head();
	while(h2 != NULL && h1->Data() != h2->Data()) h2 = h2->Next();
	if(h2 == NULL) return 0;  // They do not contain the same elements
	ret++;
	do{
	  h1 = h1->Next();
	  h2 = h2->Next();
	  if(h2 == NULL || h1 == NULL) break;
	}while(h1->Data() == h2->Data());
  }
  return ret;
}

int setX(SList &p1, SList &p2, SList &X)
{
  // for any given two permutations, this function returns the size of
  // the intersection set S = p1 \cap p2
  // it returns 0 if there is no intersection
  // In addition it forms the intersection set X
  //
  if(p1.Count() != p2.Count()) return 0;
  int ret = 0;
  SList::SNode *h1, *h2;
  int x1, x2;

  h1 = p1.head();
  x1 = 1;
  while(h1){
	h2 = p2.head();
	x2 = 1;
	while(h2 != NULL && h1->Data() != h2->Data()){
	  h2 = h2->Next();
	  x2++;
	}
	if(h2 == NULL){ // They do not contain the same elements
	  for(SList::SNode *subnd = X.head(); subnd; subnd = subnd->Next()){
		SSubPermutation *sp = (SSubPermutation *)subnd->Data();
		delete sp;
	  }
	  X.Clear();
	  return 0;
	}
	ret++;
	SSubPermutation *sub = new SSubPermutation;
	X.Add((void *)sub);
	sub->head1 = h1;
	sub->head2 = h2;
	sub->x1 = x1;
	sub->x2 = x2;
	sub->l = 0;
	do{
      x1++;
	  sub->l++;
	  h1 = h1->Next();
	  h2 = h2->Next();
	  if(h2 == NULL || h1 == NULL) break;
	}while(h1->Data() == h2->Data());
	sub->tail1 = (h1 == NULL)? p1.tail() : h1->Prev();
	sub->tail2 = (h2 == NULL)? p2.tail() : h2->Prev();
  }
  return ret;
}

int psi(SList &p1, SList &p2)             // dissimilarity measure
{
  // Minimum number of moves needed to transform one permutation to the other
  // If the elements in p1 are not the same as elements in p2, or if they are
  // not similar in size, it returns -1
  //
  if(p1.Count() != p2.Count()) return -1;
  int ret = 0;

  SList::SNode *i;

  SList S;        // intersection set of sub-permutations
  SList p_2(p2);  // a copy of p2 to be used and modified in the iterations
  SList p3;       // to be used in temporary comparisons after conducting moves
  SList p_star;   // a temporary copy of best found subpermutation and insertion
				  // position resultatn permutation

  while(setX(p_2, p1, S) > 1){
	ret++;
	int smin = S.Count(), snew;

	for(SList::SNode *nd = S.head(); nd; nd = nd->Next()){ // for each subpermutation
	  SSubPermutation *p = (SSubPermutation *)nd->Data();
	  for(SList::SNode *nd2 = S.head(); nd2; nd2 = nd2->Next()){ // for each insertion position
		if(nd2 == nd) continue;  // don't check the current position of the subpermutation
		SSubPermutation *sp = (SSubPermutation *)nd2->Data();
		// Construct the permutation p3
		//
		p3.Clear();
		for(SList::SNode *nd3 = S.head(); nd3; nd3 = nd3->Next()){
		  if(nd3 == nd) continue;
		  SSubPermutation *ssp = (SSubPermutation *)nd3->Data();
		  if(ssp->x1 < sp->x1){
			i = ssp->head1;
			for(int x=0; x<ssp->l; x++){
			  p3.Add(i->Data());
			  i = i->Next();
			}
		  }
		  else if(ssp->x1 == sp->x1){
			if(p->x1 < sp->x1){
			  i = ssp->head1;
			  for(int x=0; x<ssp->l; x++){
				p3.Add(i->Data());
				i = i->Next();
			  }

			  i = p->head1;
			  for(int x=0; x<p->l; x++){
				p3.Add(i->Data());
				i = i->Next();
			  }
			}
			else{
			  i = p->head1;
			  for(int x=0; x<p->l; x++){
				p3.Add(i->Data());
				i = i->Next();
			  }

			  i = ssp->head1;
			  for(int x=0; x<ssp->l; x++){
				p3.Add(i->Data());
				i = i->Next();
			  }
			}
		  }
		  else{
			i = ssp->head1;
			for(int x=0; x<ssp->l; x++){
			  p3.Add(i->Data());
			  i = i->Next();
			}
		  }
		}

		int snew = IntersectionSize(p1, p3);
		if(snew < smin){
		  smin = snew;
		  p_star.Copy(p3);
		}
	  }
	}
	if(snew == 1) break;
	p_2.Copy(p_star);

	for(SList::SNode *subnd = S.head(); subnd; subnd = subnd->Next()){
	  SSubPermutation *sp = (SSubPermutation *)subnd->Data();
	  delete sp;
	}
	S.Clear();
  }

  for(SList::SNode *subnd = S.head(); subnd; subnd = subnd->Next()){
	SSubPermutation *sp = (SSubPermutation *)subnd->Data();
	delete sp;
  }
  S.Clear();

  return ret;
}


#pragma package(smart_init)
