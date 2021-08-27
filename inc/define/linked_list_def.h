#ifndef _LINKED_LIST_DEF_H_
#define _LINKED_LIST_DEF_H_

struct LINK_HEAD
{
	uint64_t	qwHeadObjMid;
	uint64_t	qwTailObjMid;
};

struct LINK_NODE
{
	uint64_t	qwPrevObjMid;
	uint64_t	qwNextObjMid;
};


#endif 