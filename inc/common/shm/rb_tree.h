#ifndef _RB_TREE_
#define _RB_TREE_

#pragma pack(push, 1)
template<class N>
struct RB_TREE_NODE
{
	N               ID;
	union
	{
		RB_TREE_NODE*	pNext;
		uint64_t		uColor;
	};
	RB_TREE_NODE*	pLeft;
	RB_TREE_NODE*	pRight;
};
#pragma  pack(pop)

template<class N>
BOOL rb_insert(RB_TREE_NODE<N>* pDataNode, RB_TREE_NODE<N>** ppRootNode);
template<class N>
void rb_erase(RB_TREE_NODE<N>* pDataNode, RB_TREE_NODE<N>** ppRootNode);

template<class N>
RB_TREE_NODE<N>* rb_search(N ID, RB_TREE_NODE<N>* pRootNode);

template<class N>
RB_TREE_NODE<N>* rb_first(RB_TREE_NODE<N>* pRootNode);
template<class N>
RB_TREE_NODE<N>* rb_last(RB_TREE_NODE<N>* pRootNode);
template<class N>
RB_TREE_NODE<N>* rb_next(RB_TREE_NODE<N>* pDataNode);
template<class N>
RB_TREE_NODE<N>* rb_prev(RB_TREE_NODE<N>* pDataNode);

#include "rb_tree_inl.h"

#endif	// _RB_TREE_
