// qxTerrainMgr.cpp: implementation of the qxTerrainMgr class.
//
// Original code: GenScape (c) 1999, Lucas Ackerman
//
// Author: Jeff Thelen
//
// Copyright 2000 Quixotic, Inc.  All Rights Reserved.
//////////////////////////////////////////////////////////////////////

#include "..\\RabidFramework.h"
#include "QxUser.h"
#include "qxTerrainUtils.h"
#include "qxTerrainPoly.h"
#include "qxTerrainVert.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// qxPolyPool
//////////////////////////////////////////////////////////////////////
qxPolyPool::qxPolyPool()
: List_Head(NULL)
{
}


qxPolyPool::~qxPolyPool()
{
	qxTerrainPoly* cursor;

	cursor = List_Head;

	while(cursor != NULL)
	{
		List_Head = List_Head->m_pNext;
		delete cursor;
		cursor = List_Head;
	}

	QXASSERT(List_Head == NULL);
	QXASSERT(cursor == NULL);
	return;
}


void qxPolyPool::PutPoly(qxTerrainPoly** Poly)
{
	QXASSERT(Poly != NULL); // Poly must point to a valid pointer
	QXASSERT((*Poly) != NULL);	// the actual poly the * points to must exist

	(*Poly)->m_pNext = List_Head;

	List_Head = *Poly;

	// the pointer should no longer point to the poly, once its in the pool
	*Poly = (qxTerrainPoly *)NULL;
}


qxTerrainPoly* qxPolyPool::ExtractPoly()
{

	if(List_Head == NULL)
		return (qxTerrainPoly *)NULL;

	qxTerrainPoly* out = List_Head;
	List_Head = List_Head->m_pNext;
	return out;
}

//////////////////////////////////////////////////////////////////////
// qxVertPool
//////////////////////////////////////////////////////////////////////

qxVertPool::qxVertPool()
: List_Head(0)
{
}


qxVertPool::~qxVertPool()
{
	qxTerrainVert* cursor;

	cursor = List_Head;

	while(cursor != NULL)
	{
		List_Head = List_Head->m_pNext;
		delete cursor;
		cursor = List_Head;
	}

	QXASSERT(List_Head == NULL);
	QXASSERT(cursor == NULL);
}


void qxVertPool::PutVert(qxTerrainVert** Vert)
{
	QXASSERT(Vert != NULL);
	QXASSERT((*Vert) != NULL);

	(*Vert)->m_pNext = List_Head;

	List_Head = *Vert;

	// the pointer should no longer point to the vert, once its in the pool
	*Vert = (qxTerrainVert *)NULL;
}


qxTerrainVert* qxVertPool::ExtractVert()
{

	if(List_Head == NULL)
		return (qxTerrainVert *)NULL;

	// else
	qxTerrainVert* out = List_Head;
	List_Head = List_Head->m_pNext;
	return out;
}


//////////////////////////////////////////////////////////////////////
// qxSplitQueue
//////////////////////////////////////////////////////////////////////

qxSplitQueue::qxSplitQueue()
:m_pPoolListHead(0)
,m_nHighestPriority(0)
{
	m_pPriorityList = new qxSplitQueueNode*[PRIORITIES];

	// list is all nulls to start
	for(int i = 0; i < PRIORITIES; i++)
	{
		m_pPriorityList[i] = 0;
	}
}


qxSplitQueue::~qxSplitQueue()
{
	Clear();
	ClearPool();
	delete [] m_pPriorityList;
}


void qxSplitQueue::Clear()
{
	qxSplitQueueNode* p;

	for(int i = 0; i < PRIORITIES; i++)
	{
		while(m_pPriorityList[i] != NULL)
		{
			p = m_pPriorityList[i]->m_pNext;
			PoolNode(m_pPriorityList[i]);
			m_pPriorityList[i] = p;
		}
	}
}


void qxSplitQueue::ClearPool()
{
	qxSplitQueueNode* p = m_pPoolListHead;

	while(m_pPoolListHead != NULL)
	{
		p = m_pPoolListHead->m_pNext;
		delete m_pPoolListHead;
		m_pPoolListHead = p;
	}
}


void qxSplitQueue::InsertTerrainPoly(qxTerrainPoly* poly)
{
	QXASSERT(poly != NULL);

	qxSplitQueueNode* pNewNode = AllocateNode();

	//Double link between poly and Node
	pNewNode->m_pPoly = poly;
	poly->m_pSplitQueueNode = pNewNode;

	pNewNode->UpdatePriority();

	pNewNode->m_pNext = m_pPriorityList[pNewNode->m_nPriority];
	pNewNode->m_pPrevious = 0;

	if(pNewNode->m_pNext != NULL)
		pNewNode->m_pNext->m_pPrevious = pNewNode;

	m_pPriorityList[pNewNode->m_nPriority] = pNewNode;

	// update Highest_Priority
	if(pNewNode->m_nPriority > m_nHighestPriority)
		m_nHighestPriority = pNewNode->m_nPriority;
}


void qxSplitQueue::RemoveNode(qxSplitQueueNode* Node)
{
	if(m_pPriorityList[Node->m_nPriority] == Node)
		m_pPriorityList[Node->m_nPriority] = Node->m_pNext;

	// relink list
	if(Node->m_pPrevious != NULL)
		Node->m_pPrevious->m_pNext = Node->m_pNext;
	if(Node->m_pNext != NULL)
		Node->m_pNext->m_pPrevious = Node->m_pPrevious;

	PoolNode(Node);
}


void qxSplitQueue::UpdatePriorities(int nNearestIsHighest)
{
	qxSplitQueueNode* pNode;

	// make a new list to copy into, cuz we cant go thru the old list sequentially,
	// as priorities get swapped around. must work into new list.
	qxSplitQueueNode** pNewPriorityList = new qxSplitQueueNode*[PRIORITIES];

	int i;

	// new list must be all NULL to start
	for(i = 0; i < PRIORITIES; i++)
	{
		pNewPriorityList[i] = 0;
	}

	for(i = 0; i < PRIORITIES; i++)
	{
		while(m_pPriorityList[i] != NULL)
		{
			// remove cursor from m_pPriorityList
			pNode = m_pPriorityList[i];
			// shift list down
			m_pPriorityList[i] = pNode->m_pNext;
			// get new priority
			pNode->UpdatePriority(nNearestIsHighest);

			// put cursor in new list
			pNode->m_pNext = pNewPriorityList[pNode->m_nPriority];
			pNode->m_pPrevious = 0;

			pNewPriorityList[pNode->m_nPriority] = pNode;

			if(pNode->m_pNext != NULL)
				pNode->m_pNext->m_pPrevious = pNode;
		}
	}

	// del pointer & switch to new list
	delete [] m_pPriorityList;
	m_pPriorityList = pNewPriorityList;

	// get new highest priority
	m_nHighestPriority = 0;

	for(i = PRIORITIES - 1; i != 0; i--)
	{
		if(m_pPriorityList[i] != NULL)
		{
			m_nHighestPriority = i;
			break;
		}
	}
}


qxSplitQueueNode* qxSplitQueue::GetHighestPriorityNode()
{
	// if this node was removed, new highest is lower
	if(m_pPriorityList[m_nHighestPriority] == NULL)
	{
		for(int i = m_nHighestPriority; i >= 0; i--)	//PRIORITIES - 1
		{
			if(m_pPriorityList[i] != NULL)
			{
				m_nHighestPriority = i;
				break;
			}
		}
	}

	//else, Highest_Priority is still valid. return it
	QXASSERT(m_pPriorityList[m_nHighestPriority] != NULL);

	return m_pPriorityList[m_nHighestPriority];
}


qxSplitQueueNode* qxSplitQueue::AllocateNode()
{
	qxSplitQueueNode* NewNode;

	if(m_pPoolListHead != NULL)
	{
		NewNode = m_pPoolListHead;
		m_pPoolListHead = m_pPoolListHead->m_pNext;
	}
	else
	{
		NewNode = new qxSplitQueueNode;
	}

	return NewNode;
}


void qxSplitQueue::PoolNode(qxSplitQueueNode* Node)
{
	QXASSERT(Node != NULL);

	Node->m_pNext = m_pPoolListHead;
	m_pPoolListHead = Node;

	return;
}


//////////////////////////////////////////////////////////////////////
// qxMergeQueue
//////////////////////////////////////////////////////////////////////

qxMergeQueue::qxMergeQueue()
:m_pPoolListHead(0)
,m_nLowestPriority(0)
{
	m_pPriorityList = new qxMergeQueueNode*[PRIORITIES];

	// list is all nulls to start
	for(int i = 0; i < PRIORITIES; i++)
	{
		m_pPriorityList[i] = 0;
	}
}


qxMergeQueue::~qxMergeQueue()
{
	Clear();
	ClearPool();

	delete [] m_pPriorityList;
}


void qxMergeQueue::Clear()
{
	qxMergeQueueNode* pNode;

	for(int i = 0; i < PRIORITIES; i++)
	{
		while(m_pPriorityList[i] != NULL)
		{
			pNode = m_pPriorityList[i]->m_pNext;
			PoolNode(m_pPriorityList[i]);
			m_pPriorityList[i] = pNode;
		}
	}
}


void qxMergeQueue::ClearPool(void)
{
	qxMergeQueueNode* pNode = m_pPoolListHead;
	while(m_pPoolListHead != NULL)
	{
		pNode = m_pPoolListHead->m_pNext;
		delete m_pPoolListHead;
		m_pPoolListHead = pNode;
	}
}


void qxMergeQueue::InsertDiamond(qxTerrainPoly* poly)
{
	qxMergeQueueNode* NewNode = AllocateNode();

	NewNode->m_pPoly1 = poly;
	NewNode->m_pPoly2 = poly->m_pBottomNeighbor;	// 2nd half of diamond
	//double link between poly and this node
	poly->m_pMergeQueueNode = NewNode;

	if(NewNode->m_pPoly2 != NULL)
		NewNode->m_pPoly2->m_pMergeQueueNode = NewNode;

	NewNode->UpdatePriority();

	NewNode->m_pNext = m_pPriorityList[NewNode->m_nPriority];
	NewNode->m_pPrevious = 0;

	if(NewNode->m_pNext != NULL)
		NewNode->m_pNext->m_pPrevious = NewNode;

	m_pPriorityList[NewNode->m_nPriority] = NewNode;

	// update m_nLowestPriority
	if(NewNode->m_nPriority < m_nLowestPriority)
		m_nLowestPriority = NewNode->m_nPriority;
}


void qxMergeQueue::RemoveNode(qxMergeQueueNode* Node)
{
	if(m_pPriorityList[Node->m_nPriority] == Node)
		m_pPriorityList[Node->m_nPriority] = Node->m_pNext;

	// relink list
	if(Node->m_pPrevious != NULL)
		Node->m_pPrevious->m_pNext = Node->m_pNext;
	if(Node->m_pNext != NULL)
		Node->m_pNext->m_pPrevious = Node->m_pPrevious;

	PoolNode(Node);
}


void qxMergeQueue::UpdatePriorities(int nNearestIsHighest)
{
	qxMergeQueueNode* pNode;

	// make a new list to copy into, cuz we cant go thru the old list sequentially,
	// as priorities get swapped around. must work into new list.
	qxMergeQueueNode** NewPriorityList = new qxMergeQueueNode*[PRIORITIES];

	int i;

	// new list must be all NULL to start
	for(i = 0; i < PRIORITIES; i++)
	{
		NewPriorityList[i] = 0;
	}

	for(i = 0; i < PRIORITIES; i++)
	{

		while(m_pPriorityList[i] != NULL)
		{
			// remove cursor from m_pPriorityList
			pNode = m_pPriorityList[i];

			m_pPriorityList[i] = pNode->m_pNext;

			// get new priority
			pNode->UpdatePriority(nNearestIsHighest);

			// put cursor in new list
			pNode->m_pNext = NewPriorityList[pNode->m_nPriority];
			pNode->m_pPrevious = 0;

			NewPriorityList[pNode->m_nPriority] = pNode;

			if(pNode->m_pNext != NULL)
				pNode->m_pNext->m_pPrevious = pNode;
		}
	}

	// del pointer & switch to new list
	delete [] m_pPriorityList;
	m_pPriorityList = NewPriorityList;

	// get new lowest priority
	m_nLowestPriority = 0;

	for(i = 0; i < PRIORITIES; i++)
	{
		if(m_pPriorityList[i] != NULL)
		{
			m_nLowestPriority = i;
			break;
		}
	}
}


qxMergeQueueNode* qxMergeQueue::GetLowestPriorityNode()
{
	// if this node was removed, new lowest is higher
	// NOTE: search here happens if MergeQueue is empty.  no big deal
	if(m_pPriorityList[m_nLowestPriority] == NULL)
	{
		for(int i = m_nLowestPriority; i < PRIORITIES; i++)
		{
			if(m_pPriorityList[i] != NULL)
			{
				m_nLowestPriority = i;
				break;
			}
		}
	}

	//else, Lowest Priority is still valid. return it
	return m_pPriorityList[m_nLowestPriority];
}


qxMergeQueueNode* qxMergeQueue::AllocateNode()
{
	qxMergeQueueNode* NewNode;

	if(m_pPoolListHead != NULL)
	{
		NewNode = m_pPoolListHead;
		m_pPoolListHead = m_pPoolListHead->m_pNext;
	}
	else
	{
		NewNode = new qxMergeQueueNode;
	}

	return NewNode;
}


void qxMergeQueue::PoolNode(qxMergeQueueNode* Node)
{
	QXASSERT(Node != NULL);

	Node->m_pPoly1->m_pMergeQueueNode = 0;

	if(Node->m_pPoly2)
		Node->m_pPoly2->m_pMergeQueueNode = 0;

	Node->m_pNext = m_pPoolListHead;
	m_pPoolListHead = Node;
}


inline void qxSplitQueueNode::UpdatePriority(int nNearestIsHighest)
{
	m_nPriority = m_pPoly->CalcPriority(nNearestIsHighest);
}


void qxMergeQueueNode::UpdatePriority(int nNearestIsHighest)
{
	if(m_pPoly2)
	{
		int var1 = m_pPoly1->CalcPriority(nNearestIsHighest);
		int var2 = m_pPoly2->CalcPriority(nNearestIsHighest);

		// priority is whichever is greater.
		m_nPriority = (var1 > var2)? var1 : var2;
	}
	else
	{
		m_nPriority = m_pPoly1->CalcPriority(nNearestIsHighest);
	}
}

