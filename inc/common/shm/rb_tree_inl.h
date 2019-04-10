#include <map>
#include <stdio.h>

#ifdef WIN32
    #pragma warning(push)
    #pragma warning(disable:4312)
    #pragma warning(disable:4244)
#endif

#define RB_RED		(0)
#define RB_BLACK	(1)

#define RB_GET_PARENT(__node__)		((RB_TREE_NODE<N>*)((__node__)->uColor & ~3))
#define RB_COLOR(__node__)			(((__node__)->uColor) & 1)
#define RB_IS_RED(__node__)			(!RB_COLOR(__node__))
#define RB_IS_BLACK(__node__)		RB_COLOR(__node__)

#define RB_SET_RED(__node__)													\
	do																			\
	{																			\
		(__node__)->uColor &= ~1;												\
	} while (0)

#define RB_SET_BLACK(__node__)													\
	do																			\
	{																			\
		(__node__)->uColor |= 1;												\
	} while (0)

#define RB_SET_PARENT(__node__, __parent__)										\
	do																			\
	{																			\
		(__node__)->uColor = ((__node__)->uColor & 3) | (uint64_t)(__parent__);	\
	} while (0)

#define RB_SET_COLOR(__node__, __color__)										\
	do																			\
	{																			\
		(__node__)->uColor = ((__node__)->uColor & ~1) | __color__;				\
	} while (0)

template<class N>
static void rb_rotate_left(RB_TREE_NODE<N>* pDataNode, RB_TREE_NODE<N>** ppRootNode)
{
	RB_TREE_NODE<N>* pRight = pDataNode->pRight;
	RB_TREE_NODE<N>* pParent = RB_GET_PARENT(pDataNode);

	if ((pDataNode->pRight = pRight->pLeft))
		RB_SET_PARENT(pRight->pLeft, pDataNode);
	pRight->pLeft = pDataNode;

	RB_SET_PARENT(pRight, pParent);

	if (pParent)
	{
		if (pDataNode == pParent->pLeft)
			pParent->pLeft = pRight;
		else
			pParent->pRight = pRight;
	}
	else
		*ppRootNode = pRight;
	RB_SET_PARENT(pDataNode, pRight);
}

template<class N>
static void rb_rotate_right(RB_TREE_NODE<N>* pDataNode, RB_TREE_NODE<N>** ppRootNode)
{
	RB_TREE_NODE<N>* pLeft = pDataNode->pLeft;
	RB_TREE_NODE<N>* pParent = RB_GET_PARENT(pDataNode);

	if ((pDataNode->pLeft = pLeft->pRight))
		RB_SET_PARENT(pLeft->pRight, pDataNode);
	pLeft->pRight = pDataNode;

	RB_SET_PARENT(pLeft, pParent);

	if (pParent)
	{
		if (pDataNode == pParent->pRight)
			pParent->pRight = pLeft;
		else
			pParent->pLeft = pLeft;
	}
	else
		*ppRootNode = pLeft;
	RB_SET_PARENT(pDataNode, pLeft);
}

template<class N>
static void rb_erase_from_parent(RB_TREE_NODE<N>* pDataNode, RB_TREE_NODE<N>* pParent, RB_TREE_NODE<N>** ppRootNode)
{
	RB_TREE_NODE<N>* pOther;

	while ((!pDataNode || RB_IS_BLACK(pDataNode)) && pDataNode != *ppRootNode)
	{
		if (pParent->pLeft == pDataNode)
		{
			pOther = pParent->pRight;
			if (RB_IS_RED(pOther))
			{
				RB_SET_BLACK(pOther);
				RB_SET_RED(pParent);
				rb_rotate_left(pParent, ppRootNode);
				pOther = pParent->pRight;
			}
			if ((!pOther->pLeft || RB_IS_BLACK(pOther->pLeft)) &&
				(!pOther->pRight || RB_IS_BLACK(pOther->pRight)))
			{
				RB_SET_RED(pOther);
				pDataNode = pParent;
				pParent = RB_GET_PARENT(pDataNode);
			}
			else
			{
				if (!pOther->pRight || RB_IS_BLACK(pOther->pRight))
				{
					RB_SET_BLACK(pOther->pLeft);
					RB_SET_RED(pOther);
					rb_rotate_right(pOther, ppRootNode);
					pOther = pParent->pRight;
				}
				RB_SET_COLOR(pOther, RB_COLOR(pParent));
				RB_SET_BLACK(pParent);
				RB_SET_BLACK(pOther->pRight);
				rb_rotate_left(pParent, ppRootNode);
				pDataNode = *ppRootNode;
				break;
			}
		}
		else
		{
			pOther = pParent->pLeft;
			if (RB_IS_RED(pOther))
			{
				RB_SET_BLACK(pOther);
				RB_SET_RED(pParent);
				rb_rotate_right(pParent, ppRootNode);
				pOther = pParent->pLeft;
			}
			if ((!pOther->pLeft || RB_IS_BLACK(pOther->pLeft)) &&
				(!pOther->pRight || RB_IS_BLACK(pOther->pRight)))
			{
				RB_SET_RED(pOther);
				pDataNode = pParent;
				pParent = RB_GET_PARENT(pDataNode);
			}
			else
			{
				if (!pOther->pLeft || RB_IS_BLACK(pOther->pLeft))
				{
					RB_SET_BLACK(pOther->pRight);
					RB_SET_RED(pOther);
					rb_rotate_left(pOther, ppRootNode);
					pOther = pParent->pLeft;
				}
				RB_SET_COLOR(pOther, RB_COLOR(pParent));
				RB_SET_BLACK(pParent);
				RB_SET_BLACK(pOther->pLeft);
				rb_rotate_right(pParent, ppRootNode);
				pDataNode = *ppRootNode;
				break;
			}
		}
	}

	if (pDataNode)
		RB_SET_BLACK(pDataNode);
}

template<class N>
static void rb_insert_color(RB_TREE_NODE<N>* pDataNode, RB_TREE_NODE<N>** ppRootNode)
{
	RB_TREE_NODE<N>* pParent;
	RB_TREE_NODE<N>* pGrandParent;

	while ((pParent = RB_GET_PARENT(pDataNode)) && RB_IS_RED(pParent))
	{
		pGrandParent = RB_GET_PARENT(pParent);

		if (pParent == pGrandParent->pLeft)
		{
			register RB_TREE_NODE<N>* pUncle = pGrandParent->pRight;
			if (pUncle && RB_IS_RED(pUncle))
			{
				RB_SET_BLACK(pUncle);
				RB_SET_BLACK(pParent);
				RB_SET_RED(pGrandParent);
				pDataNode = pGrandParent;
				continue;
			}

			if (pParent->pRight == pDataNode)
			{
				register RB_TREE_NODE<N>* pTemp;
				rb_rotate_left(pParent, ppRootNode);
				pTemp = pParent;
				pParent = pDataNode;
				pDataNode = pTemp;
			}

			RB_SET_BLACK(pParent);
			RB_SET_RED(pGrandParent);
			rb_rotate_right(pGrandParent, ppRootNode);
		}
		else
		{
			register RB_TREE_NODE<N>* pUncle = pGrandParent->pLeft;
			if (pUncle && RB_IS_RED(pUncle))
			{
				RB_SET_BLACK(pUncle);
				RB_SET_BLACK(pParent);
				RB_SET_RED(pGrandParent);
				pDataNode = pGrandParent;
				continue;
			}

			if (pParent->pLeft == pDataNode)
			{
				register RB_TREE_NODE<N>* pTemp;
				rb_rotate_right(pParent, ppRootNode);
				pTemp = pParent;
				pParent = pDataNode;
				pDataNode = pTemp;
			}

			RB_SET_BLACK(pParent);
			RB_SET_RED(pGrandParent);
			rb_rotate_left(pGrandParent, ppRootNode);
		}
	}

	RB_SET_BLACK(*ppRootNode);
}

template<class N>
BOOL rb_insert(RB_TREE_NODE<N>* pDataNode, RB_TREE_NODE<N>** ppRootNode)
{
	RB_TREE_NODE<N>** ppInsertPos = ppRootNode;
	RB_TREE_NODE<N>* pParent = NULL;

	if (ppRootNode == NULL)
		return FALSE;

	while (*ppInsertPos)
	{
		pParent = *ppInsertPos;

		if (pDataNode->ID < (*ppInsertPos)->ID)
			ppInsertPos = &((*ppInsertPos)->pLeft);
		else if (pDataNode->ID > (*ppInsertPos)->ID)
			ppInsertPos = &((*ppInsertPos)->pRight);
		else
			return FALSE;
	}

	pDataNode->pNext = pParent;
	pDataNode->pLeft = pDataNode->pRight = NULL;
	*ppInsertPos = pDataNode;

	rb_insert_color(pDataNode, ppRootNode);

	return TRUE;
}

template<class N>
void rb_erase(RB_TREE_NODE<N>* pDataNode, RB_TREE_NODE<N>** ppRootNode)
{
	RB_TREE_NODE<N>* pChild;
	RB_TREE_NODE<N>* pParent;
	int32_t nColor;

	if (ppRootNode == NULL)
		return;

	if (pDataNode->pLeft == NULL)
		pChild = pDataNode->pRight;
	else if (pDataNode->pRight == NULL)
		pChild = pDataNode->pLeft;
	else
	{
		RB_TREE_NODE<N>* pOld = pDataNode;
		RB_TREE_NODE<N>* pLeft;

		pDataNode = pDataNode->pRight;
		while ((pLeft = pDataNode->pLeft) != NULL)
			pDataNode = pLeft;
		pChild = pDataNode->pRight;
		pParent = RB_GET_PARENT(pDataNode);
		nColor = RB_COLOR(pDataNode);

		if (pChild)
			RB_SET_PARENT(pChild, pParent);
		if (pParent == pOld)
		{
			pParent->pRight = pChild;
			pParent = pDataNode;
		}
		else
			pParent->pLeft = pChild;

		pDataNode->pNext = pOld->pNext;
		pDataNode->pRight = pOld->pRight;
		pDataNode->pLeft = pOld->pLeft;

		if (RB_GET_PARENT(pOld))
		{
			if (RB_GET_PARENT(pOld)->pLeft == pOld)
				RB_GET_PARENT(pOld)->pLeft = pDataNode;
			else
				RB_GET_PARENT(pOld)->pRight = pDataNode;
		}
		else
			*ppRootNode = pDataNode;

		RB_SET_PARENT(pOld->pLeft, pDataNode);
		if (pOld->pRight)
			RB_SET_PARENT(pOld->pRight, pDataNode);
		goto color;
	}

	pParent = RB_GET_PARENT(pDataNode);
	nColor = RB_COLOR(pDataNode);

	if (pChild)
		RB_SET_PARENT(pChild, pParent);
	if (pParent)
	{
		if (pParent->pLeft == pDataNode)
			pParent->pLeft = pChild;
		else
			pParent->pRight = pChild;
	}
	else
		*ppRootNode = pChild;

color:
	if (nColor == RB_BLACK)
		rb_erase_from_parent(pChild, pParent, ppRootNode);
}

template<class N>
RB_TREE_NODE<N>* rb_search(N ID, RB_TREE_NODE<N>* pRootNode)
{
	RB_TREE_NODE<N>* pNode = pRootNode;

	while (pNode)
	{
		if (ID < pNode->ID)
			pNode = pNode->pLeft;
		else if (ID > pNode->ID)
			pNode = pNode->pRight;
		else
			return pNode;
	}
	return NULL;
}

template<class N>
RB_TREE_NODE<N>* rb_first(RB_TREE_NODE<N>* pRootNode)
{
	RB_TREE_NODE<N>* pNode;

	pNode = pRootNode;
	if (!pNode)
		return NULL;
	while (pNode->pLeft)
		pNode = pNode->pLeft;
	return pNode;
}

template<class N>
RB_TREE_NODE<N>* rb_last(RB_TREE_NODE<N>* pRootNode)
{
	RB_TREE_NODE<N>* pNode;

	pNode = pRootNode;
	if (!pNode)
		return NULL;
	while (pNode->pRight)
		pNode = pNode->pRight;
	return pNode;
}

template<class N>
RB_TREE_NODE<N>* rb_next(RB_TREE_NODE<N>* pDataNode)
{
	RB_TREE_NODE<N>* pParent;

	if (pDataNode == NULL)
		return NULL;

	if (RB_GET_PARENT(pDataNode) == pDataNode)
		return NULL;

	if (pDataNode->pRight)
	{
		pDataNode = pDataNode->pRight;
		while (pDataNode->pLeft)
			pDataNode = pDataNode->pLeft;
		return pDataNode;
	}

	while ((pParent = RB_GET_PARENT(pDataNode)) && pDataNode == pParent->pRight)
		pDataNode = pParent;

	return pParent;
}

template<class N>
RB_TREE_NODE<N>* rb_prev(RB_TREE_NODE<N>* pDataNode)
{
	RB_TREE_NODE<N>* pParent;

	if (pDataNode == NULL)
		return NULL;

	if (RB_GET_PARENT(pDataNode) == pDataNode)
		return NULL;

	if (pDataNode->pLeft)
	{
		pDataNode = pDataNode->pLeft;
		while (pDataNode->pRight)
			pDataNode = pDataNode->pRight;
		return pDataNode;
	}

	while ((pParent = RB_GET_PARENT(pDataNode)) && pDataNode == pParent->pLeft)
		pDataNode = pParent;

	return pParent;
}

#ifdef WIN32
    #pragma warning(pop)
#endif
