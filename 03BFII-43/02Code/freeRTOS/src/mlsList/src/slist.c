#include <stdio.h>
#include <string.h>
#include "../inc/slist.h"
#include "../inc/node.h"
#include "../mlsOsal/inc/mlsOsal.h"

mlsMutexHandle_t gSListMutex;

SListError	slist_Init(struct SList *slist)
{
	if(MACRO_CHECK_NULL(slist))
	{
		return SLIST_INVALID_PARAM;
	}

	slist->head = Null;

	mlsOsalMutexCreate(&gSListMutex, "SList mutex");

	return SLIST_ERROR_NONE;
}

Bool	slist_IsEmpty(struct SList *slist)
{
	if(MACRO_CHECK_NULL(slist))
	{
		return False;
	}

	return (slist->head == Null);
}

UInt32	slist_GetNumberElement(struct SList *slist)
{
	UInt32 numberOfElement = 0;
	struct Node *node;

	node = slist->head;

	while (node != Null)
	{
		numberOfElement ++;
		node = node->next;
	}

	return numberOfElement;
}

SListError	slist_InsertFirst(struct SList *slist, struct Node *node)
{
	mlsOsalMutexLock(&gSListMutex, MLS_OSAL_MAX_DELAY);

	if(MACRO_CHECK_NULL(slist) || MACRO_CHECK_NULL(node))
	{
		mlsOsalMutexUnlock(&gSListMutex);
		return SLIST_INVALID_PARAM;
	}

	node->next = slist->head;
	slist->head = node;

	mlsOsalMutexUnlock(&gSListMutex);

	return SLIST_ERROR_NONE;
}

SListError	slist_InsertLast(struct SList *slist, struct Node *node)
{
//	mlsOsalMutexLock(&gSListMutex, MLS_OSAL_MAX_DELAY);

	struct Node *currNode;

	if(MACRO_CHECK_NULL(slist) || MACRO_CHECK_NULL(node))
	{
//		mlsOsalMutexUnlock(&gSListMutex);
		return SLIST_INVALID_PARAM;
	}

	if (slist->head == Null)
	{
		slist->head = node;
		slist->head->next = Null;
	}
	else
	{
		currNode = slist->head;

		while (currNode->next != Null)
		{
			currNode = currNode->next;
		}

		currNode->next = node;
		node->next = Null;
	}

//	mlsOsalMutexUnlock(&gSListMutex);

	return SLIST_ERROR_NONE;
}

SListError	slist_InsertAt(struct SList *slist, UInt32 pos, struct Node *node)
{
	mlsOsalMutexLock(&gSListMutex, MLS_OSAL_MAX_DELAY);

	struct Node *currNode;
	UInt32 idx = 0;

	if(MACRO_CHECK_NULL(slist) || MACRO_CHECK_NULL(node))
	{
		mlsOsalMutexUnlock(&gSListMutex);
		return SLIST_INVALID_PARAM;
	}

	if (pos == 0)
	{
		node->next = slist->head;
		slist->head = node;
	}
	else
	{
		currNode = slist->head;
		while (currNode != Null)
		{
			idx ++;

			if (idx >= pos)
			{
				break;
			}

			currNode = currNode->next;
		}

		if (idx == pos)
		{
			node->next = currNode->next;
			currNode->next = node;
		}
		else
		{
			mlsOsalMutexUnlock(&gSListMutex);
			return SLIST_EXECUTE_AT_OVER_POSITION;
		}
	}

	mlsOsalMutexUnlock(&gSListMutex);

	return SLIST_ERROR_NONE;
}

SListError	slist_RemoveFirst(struct SList *slist, Bool freeNode)
{
	struct Node *node;

	if(MACRO_CHECK_NULL(slist))
	{
		return SLIST_INVALID_PARAM;
	}

	// Back up old head to node
	node = slist->head;

	// Assign head to next one
	slist->head = slist->head->next;

	// Free node
	if (freeNode)
	{
		vPortFree(node->data);
		vPortFree(node);
	}

	return SLIST_ERROR_NONE; 
}

SListError	slist_RemoveLast(struct SList *slist, Bool freeNode)
{
	struct Node *currNode;

	if(MACRO_CHECK_NULL(slist))
	{
		return SLIST_INVALID_PARAM;
	}

	if (slist->head == Null)
	{
		// Empty list, nothing to remove
		return SLIST_REMOVE_ITEM_OF_EMPTY_LIST;
	}

	if (slist->head->next == Null)
	{
		// There is only item of list
		if (freeNode)
		{
			vPortFree(slist->head);
		}
		else
		{
			slist->head = Null;
		}

		return SLIST_ERROR_NONE;
	}

	currNode = slist->head;

	while (currNode != Null)
	{
		if (currNode->next->next == Null)
		{
			break;
		}
		else
		{
			currNode = currNode->next;
		}
	}

	// Free current node
	if (freeNode)
	{
		vPortFree(currNode->next->data);
		vPortFree(currNode->next);
	}

	// Assign curr node to Null
	currNode->next = Null;

	return SLIST_ERROR_NONE;
}

SListError	slist_RemoveAt(struct SList *slist, UInt32 pos, Bool freeNode)
{
	UInt32 idx = 0;
	struct Node *currNode;
	struct Node *oldNode;

	if(MACRO_CHECK_NULL(slist))
	{
		return SLIST_INVALID_PARAM;
	}

	currNode = oldNode = slist->head;

	if (currNode == Null)
	{
		// Empty list, nothing to remove
		return SLIST_REMOVE_ITEM_OF_EMPTY_LIST;
	}

	if (pos == 0)
	{
		return slist_RemoveFirst(slist, freeNode);
	}

	while (currNode != Null)
	{
		oldNode = currNode;
		currNode = currNode->next;

		idx ++;

		if (idx >= pos)
		{
			break;
		}
	}

	if ((idx < pos) || (currNode == Null))
	{
		return SLIST_EXECUTE_AT_OVER_POSITION;
	}

	oldNode->next = currNode->next;

	// Free current node
	if (freeNode)
	{
		vPortFree(currNode->data);
		vPortFree(currNode);
	}

	return SLIST_ERROR_NONE; 
}

SListError	slist_RemoveNode(struct SList *slist, struct Node *node)
{
	mlsOsalMutexLock(&gSListMutex, MLS_OSAL_MAX_DELAY);

	struct Node **currNode;

	currNode = &(slist->head);
	while(*(currNode) != node)
	{
		currNode = &((*currNode)->next);
	}

	*currNode = (*currNode)->next;

#if (USE_STATIC_BUFFER == 1)
	UInt32 idx;

	for (idx = 0; idx < MAX_STATIC_NODE; idx ++)
	{
		if (node == &gNode[idx])
		{
			gNodeStatus[idx] = 0;
			break;
		}
	}

	node = Null;

#else
	vPortFree(((struct NodeData *) node->data)->data);
	vPortFree(node->data);
	vPortFree(node);
#endif

	mlsOsalMutexUnlock(&gSListMutex);

	return SLIST_ERROR_NONE;
}

struct Node *slist_SearchNode(struct SList *slist, void *data, UInt16 dataLen)
{
	struct Node *node = Null;
	struct NodeData *nodeDataCurr;
	struct Node *currNode;

	currNode = slist->head;

	while (currNode != Null)
	{
		nodeDataCurr = (struct NodeData *)currNode->data;

		if (dataLen <= nodeDataCurr->len)
		{
			if (0 == memcmp(data, nodeDataCurr->data, dataLen))
			{
				node = currNode;
				break;
			}
		}

		currNode = currNode->next;
	}

	return node;
}
