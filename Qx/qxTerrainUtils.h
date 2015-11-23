// qxTerrainUtils.h: 
//
// Original code: GenScape (c) 1999, Lucas Ackerman
//
// Author: Jeff Thelen
//
// Copyright 2000 Quixotic, Inc.  All Rights Reserved.
//////////////////////////////////////////////////////////////////////

#ifndef qxTerrainUtils_H
#define qxTerrainUtils_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class qxTerrainVert;
class qxTerrainPoly;

class qxPolyPool
{
public:
	qxPolyPool();
	~qxPolyPool();

	void PutPoly(qxTerrainPoly** Poly);
	qxTerrainPoly* ExtractPoly(void);

private:

	qxTerrainPoly* List_Head;

};


class qxVertPool
{

public:
	qxVertPool();
	~qxVertPool();

	void PutVert(qxTerrainVert** Vert);
	qxTerrainVert* ExtractVert();

private:

	qxTerrainVert* List_Head;

};



class qxSplitQueueNode	
{
public:

	qxSplitQueueNode(){};
	~qxSplitQueueNode(){};
	void UpdatePriority(int nNearestIsHighest=0);
	


private:
	
	qxTerrainPoly*		m_pPoly;
	qxSplitQueueNode*	m_pNext;
	qxSplitQueueNode*	m_pPrevious;
	int m_nPriority;
	
	friend class qxSplitQueue;
	friend class qxTerrainMapBase;

};



class qxSplitQueue
{
public:

	qxSplitQueue();
	~qxSplitQueue();

	void Clear();	
	void ClearPool();
	void RemoveNode(qxSplitQueueNode* pNode);
	void InsertTerrainPoly(qxTerrainPoly* pPoly);
	void UpdatePriorities(int nNearestIsHighest=0);

	qxSplitQueueNode* GetHighestPriorityNode();

private:

	qxSplitQueueNode* AllocateNode();
	void PoolNode(qxSplitQueueNode* pNode);

	// array of ptrs to doubly linked lists.  works sort of like a chained hash table
	qxSplitQueueNode** m_pPriorityList;
	qxSplitQueueNode* m_pPoolListHead;
	
	int m_nHighestPriority;

	friend class qxTerrainMapBase;


};


class qxMergeQueueNode	
{

public:

	qxMergeQueueNode(){};
	~qxMergeQueueNode(){};
	void UpdatePriority(int nNearestIsHighest=0);

private:
	
	qxTerrainPoly* m_pPoly1;
	qxTerrainPoly* m_pPoly2;
	qxMergeQueueNode* m_pNext;
	qxMergeQueueNode* m_pPrevious;

	int m_nPriority;

	friend class qxMergeQueue;
	friend class qxTerrainMapBase;

};

class qxMergeQueue
{
public:

	qxMergeQueue();
	~qxMergeQueue();

	void Clear();	
	void ClearPool();
	void RemoveNode(qxMergeQueueNode* pNode);

	// poly's bottom-neighbor is the 2nd half of the diamond
	void InsertDiamond(qxTerrainPoly* poly);
	void UpdatePriorities(int nNearestIsHighest=0);

	qxMergeQueueNode* GetLowestPriorityNode();

private:

	qxMergeQueueNode* AllocateNode();
	void PoolNode(qxMergeQueueNode* Node);

	qxMergeQueueNode** m_pPriorityList;
	qxMergeQueueNode* m_pPoolListHead;
	int m_nLowestPriority;

	friend class qxTerrainMapBase;

};






#endif