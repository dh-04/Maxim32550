/*******************************************************************************
 *  (C) Copyright 2009 Molisys Solutions Co., Ltd. , All rights reserved       *
 *                                                                             *
 *  This source code and any compilation or derivative thereof is the sole     *
 *  property of Molisys Solutions Co., Ltd. and is provided pursuant to a      *
 *  Software License Agreement.  This code is the proprietary information      *
 *  of Molisys Solutions Co., Ltd and is confidential in nature.  Its use and  *
 *  dissemination by any party other than Molisys Solutions Co., Ltd is        *
 *  strictly limited by the confidential information provisions of the         *
 *  Agreement referenced above.                                                *
 ******************************************************************************/

/**
 * @defgroup node.c	node.c
 * @brief node.c
 *
 * Add more details about module
 * @{
 */

/**
 * @file	node.c
 *
 * @date 	13 Feb 2018
 * @author	stylvn008
 */


/********** Include section ***************************************************/
#include <string.h>
#include "../mlsOsal/inc/mlsOsal.h"
#include "../inc/node.h"
/********** Local Constant and compile switch definition section **************/
#define NODE_BUFFER0_SIZE	50
#define NODE_BUFFER1_SIZE	1024
#define NODE_BUFFER2_SIZE	1024*5
/********** Local Type definition section *************************************/

/********** Local Macro definition section ************************************/

/********** Global variable definition section ********************************/
static UInt8 gNodeBuffer0[NODE_BUFFER0_SIZE];
static UInt8 gNodeBuffer1[NODE_BUFFER0_SIZE];
static UInt8 gNodeBuffer2[NODE_BUFFER1_SIZE];
static UInt8 gNodeBuffer3[NODE_BUFFER2_SIZE];

static struct NodeData gNodeData[MAX_STATIC_NODE] =
{
	{
		.data = gNodeBuffer0,
	},

	{
		.data = gNodeBuffer1,
	},

	{
		.data = gNodeBuffer2,
	},

	{
		.data = gNodeBuffer3,
	},
};

struct Node gNode[MAX_STATIC_NODE] =
{
	{
		.data = &gNodeData[0],
		.next = Null,
	},
	{
		.data = &gNodeData[1],
		.next = Null,
	},
	{
		.data = &gNodeData[2],
		.next = Null,
	},
	{
		.data = &gNodeData[3],
		.next = Null,
	},
};

UInt8 gNodeStatus[MAX_STATIC_NODE] = {0, 0, 0};

static UInt16 gNodeLen[MAX_STATIC_NODE] =
{
	NODE_BUFFER0_SIZE,
	NODE_BUFFER0_SIZE,
	NODE_BUFFER1_SIZE,
	NODE_BUFFER2_SIZE
};

/********** Local (static) variable definition section ************************/

/********** Local (static) function declaration section ***********************/

/********** Local function definition section *********************************/

/********** Global function definition section ********************************/

struct Node * NodeCreate(void *data, UInt16 dataLen)
{
#if (USE_STATIC_BUFFER == 1)
	struct Node *node = Null;
	UInt32 idx;

	for (idx = 0; idx < MAX_STATIC_NODE; idx ++)
	{
		if ((0 == gNodeStatus[idx]) && (dataLen < gNodeLen[idx]))
		{
			// Mark that this node is allocated
			gNodeStatus[idx] = 1;

			// Set the length
			((struct NodeData *) gNode[idx].data)->len = dataLen;

			// Copy data to the node
			memcpy(((struct NodeData *) gNode[idx].data)->data, data, dataLen);

			// Pointer to the node
			node = &gNode[idx];

			break;
		}
	}

	return node;
#else
	struct Node *node = pvPortMalloc(sizeof(struct Node));

	if (node == Null)
	{
		return node;
	}
	else
	{
		node->data = (struct NodeData *) pvPortMalloc(sizeof(struct NodeData));
		((struct NodeData *) node->data)->data = pvPortMalloc(dataLen);

		if ((node->data == Null) ||
				(((struct NodeData *) node->data)->data == Null))
		{
			vPortFree(node);
		}
		else
		{
			((struct NodeData *) node->data)->len = dataLen;
			memcpy(((struct NodeData *) node->data)->data, data, dataLen);
		}
	}

	return node;
#endif
}

/**@}*/
