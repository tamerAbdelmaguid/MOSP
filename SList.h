//--- This code is Copyright 2007, Tamer F. Abdelmaguid
//    Use of this code or portions of it is granted without fee for academic,
//    non-commercial purposes only provided that the above copyright statement
//    is clearly indicated
//------------------------------------------------------------------------------

//---------------------------------------------------- Lists management
//   Management of lists.
//   Prepaired and Copyrighted by Tamer F. Abdelhady, 2000.
//---------------------------------------------------------------------

#ifndef SListH
#define SListH
#include <stdlib.h>

enum SComparisonBasis { ASCENDING = 1, DESCENDING = -1};
typedef int (*SCompareNodes)(void *, void *);
typedef void (*STraverseNodes)(void *, int);

class SList {      // Ordered list of permutation of elements
  friend int psi(SList &p1, SList &p2);

  public:
	class SNode{
	  friend class SList;
	  friend class MOSP_PS;
	  friend class MOSP_OO;
	  private:
		void *_data;
		SNode *_prev, *_next;
	  public:
		SNode(void *data=NULL) : _data(data), _prev(NULL), _next(NULL){};
        SNode *Next(){ return _next; }
        SNode *Prev(){ return _prev; }
        void *Data(){ return _data; }
        void SetData(void *d){ _data = d; }
    };

  private:
    SNode *_head;
    SNode *_tail;
    int _count;

  public:
    SList();
    SList(SList &lst);
    ~SList();

    SNode *Node(void *data);
    SNode *Node(int indx);

    const int Count(void){ return _count; }
	void *Item(int indx){ return Node(indx)->_data; }
    void *operator[](int indx){ return Node(indx)->_data; }
    int IndexOf(void *data);
    int OrderOf(SNode *nd);

    void Add(void *data);
    void Add(SList &lst);
    void AddAfter(SNode *nd, void *data);
    void AddBefore(SNode *nd, void *data);
    void Push(void *data);
    void Remove(SNode *nd);
    void Remove(void *data){ Remove(Node(data)); }
    void Remove(int indx){ Remove(Node(indx)); }
	void Remove(SList &lst);
	void LimitTo(SList &lst);  // Suitable for intersections
    void Clear(void);
	void Swap(int indx1, int indx2);
	void Swap(SNode *nd1, SNode *nd2);
    int Traverse(STraverseNodes trv);

    SNode *head(){ return _head; }
    SNode *tail(){ return _tail; }

    // Some additional functions to deal with cyclic lists.
    void CutBefore(SNode *nd);
    void CutBefore(void *d){ CutBefore(Node(d)); }
    void CutAfter(SNode *nd);
    void CutAfter(void *d){ CutAfter(Node(d)); }
    void Reverse(SNode* nd1, SNode *nd2);
    void Reverse(int p1, int p2){ Reverse(Node(p1), Node(p2)); }
    void Reverse(void *d1, void *d2){ Reverse(Node(d1), Node(d2)); }
    void EqualsReverseOf(SList &l);
    // ---- end of cyclic lists functions

    static SCompareNodes _CompAlg;
    static SComparisonBasis _CompBasis;

    void QSort(SCompareNodes CompAlg, SComparisonBasis CB = ASCENDING);
    void qs(int left, int right);

    void Copy(SList &lst);
    SList &operator = (SList &lst) { Copy(lst);  return *this; }
    bool operator == (SList &lst);
};

struct SSubPermutation {
  SList::SNode *head1, *head2;   // starting nodes in given two permutations
  SList::SNode *tail1, *tail2;   // ending nodes in given two permutations
  int x1, x2;                    // psitions of starting nodes in the two permutations
  int l;                         // number of nodes in sub-permutation
};

void qSort(int *c, float *s, int l, int r);
int approx_psi(SList &p1, SList &p2);    // dissimilarity measure (approximation)

int IntersectionSize(SList &p1, SList &p2);  // returns the size of the intersetion set
int psi(SList &p1, SList &p2);    // dissimilarity measure

#endif
