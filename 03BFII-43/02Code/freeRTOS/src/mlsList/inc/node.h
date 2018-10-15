#ifndef NODE_H_
#define NODE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "mlsInclude.h"

#define USE_STATIC_BUFFER	1
#define MAX_STATIC_NODE		4

typedef struct Node
{
	void *data;
	struct Node *next;
}Node_t;

typedef struct Node2
{
	void *data;
	struct Node *next;
	struct Node *prev;
}Node2_t;

typedef struct NodeData
{
	UInt16 len;
	UInt8 *data;
}NodeData_t;

extern UInt8 gNodeStatus[MAX_STATIC_NODE];
extern struct Node gNode[MAX_STATIC_NODE];

struct Node * NodeCreate(void *data, UInt16 dataLen);

#ifdef __cplusplus
}
#endif

#endif /* NODE_H_ */
