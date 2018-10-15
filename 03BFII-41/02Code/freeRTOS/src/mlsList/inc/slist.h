#ifndef SLIST_H_
#define SLIST_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "mlsInclude.h"

#define DEBUG_LOG			1
#define ASSERT_CHECK		0
#define SLIST_ERROR_BASE	4000
#define MACRO_CHECK_NULL(x)						(x == Null)

typedef Int32 SListError;

typedef enum SListErrorCode
{
	SLIST_ERROR_NONE				= 0,
	SLIST_INVALID_PARAM				= SLIST_ERROR_BASE,
	SLIST_EXECUTE_AT_OVER_POSITION	= SLIST_ERROR_BASE + 1,
	SLIST_REMOVE_ITEM_OF_EMPTY_LIST	= SLIST_ERROR_BASE + 2,
	SLIST_REMOVE_NOT_FOUND_ITEM		= SLIST_ERROR_BASE + 3,
}SListErrorCode_t;

typedef struct SList
{
	struct Node *head;
}SList_t;

SListError	slist_Init(struct SList *slist);
Bool	slist_IsEmpty(struct SList *slist);
UInt32	slist_GetNumberElement(struct SList *slist);
SListError	slist_InsertFirst(struct SList *slist, struct Node *node);
SListError	slist_InsertLast(struct SList *slist, struct Node *node);
SListError	slist_InsertAt(struct SList *slist, UInt32 pos, struct Node *node);
SListError	slist_RemoveFirst(struct SList *slist, Bool freeNode);
SListError	slist_RemoveLast(struct SList *slist, Bool freeNode);
SListError	slist_RemoveAt(struct SList *slist, UInt32 pos, Bool freeNode);
SListError	slist_RemoveNode(struct SList *slist, struct Node *node);
struct Node *slist_SearchNode(struct SList *slist, void *data, UInt16 dataLen);
mlsErrorCode_t slistTest();

#ifdef __cplusplus
}
#endif

#endif /* SLIST_H_ */
