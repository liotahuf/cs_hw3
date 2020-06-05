/* 046267 Computer Architecture - Spring 2020 - HW #3 */
/* Implementation (skeleton)  for the dataflow statistics calculator */

#include "dflow_calc.h"

#include <iostream>
#include <map>
#include <vector>

typedef struct node {
	InstInfo myInfo;
	int dep1;
	int dep2;
	int nodeDepth;
	int myLatency;
	bool hasDependent;
} node_t;

int ROB[32] = { -1 };

typedef struct Context{
	std::map<int, node_t> nodesMap;//this is the data strcuture which will hold the analysis graph.each node key will be the instruction number, and is value node
	std::vector<int> exit; // this will hold the dependencies of exit node
}Context_t;



ProgCtx analyzeProg(const unsigned int opsLatency[], const InstInfo progTrace[], unsigned int numOfInsts) {
    
	for (int i = 0; i < 32; i++)
	{
		ROB[i] = -1;
	}

	Context_t* myContext;
	myContext = new Context_t;

	
	node_t curr_node;
	for (int i = 0; i < numOfInsts; i++)
	{
		curr_node.myInfo = progTrace[i];
		curr_node.dep1 = ROB[curr_node.myInfo.src1Idx];
		curr_node.dep2 = ROB[curr_node.myInfo.src2Idx];
		int tempDepth1 = 0;
		int tempDepth2 = 0;
		
		//update current node dependencies that they have one dependent
		if (curr_node.dep1!= -1)//if depend on node that is not entry
		{
			myContext->nodesMap[curr_node.dep1].hasDependent = true;
			tempDepth1 = myContext->nodesMap[curr_node.dep1].nodeDepth + opsLatency[myContext->nodesMap[curr_node.dep1].myInfo.opcode];
		}
		if (curr_node.dep2 != -1)//if depend on node that is not entry
		{
			myContext->nodesMap[curr_node.dep2].hasDependent = true;
			tempDepth2 = myContext->nodesMap[curr_node.dep2].nodeDepth + opsLatency[myContext->nodesMap[curr_node.dep2].myInfo.opcode];
		}

		if (tempDepth1 >= tempDepth2)
		{
			curr_node.nodeDepth = tempDepth1;
		}
		else
		{
			curr_node.nodeDepth = tempDepth2;
		}

		//update ROB - the instruction i is now the last one who wrote to ROB in place myInfo.dst, used to know nexts instructions dependecies
		ROB[curr_node.myInfo.dstIdx] = i;


		//update node latency
		curr_node.myLatency = opsLatency[curr_node.myInfo.opcode];

		//update nodes map
		myContext->nodesMap[i] = curr_node;
	}

	//build exit node
	std::map<int, node_t>::iterator it;
	for (it = myContext->nodesMap.begin(); it != myContext->nodesMap.end(); it++)
	{
		if (it->second.hasDependent == false)
		{
			myContext->exit.push_back(it->first);
		}
	}

	return (void*)myContext;
		
}

void freeProgCtx(ProgCtx ctx) {

	delete (Context_t*)ctx;
}

int getInstDepth(ProgCtx ctx, unsigned int theInst) {

	if (ctx == PROG_CTX_NULL)
	{
		return -1;
	}
	Context_t* myContext = (Context_t*)ctx;
	node_t curr_node = myContext->nodesMap[theInst];
	return curr_node.nodeDepth;
}

int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) {
	
	if (ctx == PROG_CTX_NULL)
	{
		return -1;
	}

	Context_t* myContext = (Context_t*)ctx;
	node_t curr_node = myContext->nodesMap[theInst];
	*src1DepInst = curr_node.dep1;
	*src2DepInst = curr_node.dep2;

	return 0;
}

int getProgDepth(ProgCtx ctx) {
	
	if (ctx == PROG_CTX_NULL)
	{
		return -1;
	}
	
	Context_t* myContext = (Context_t*)ctx;
	std::vector<int>::iterator it;
	int tmpProgDepth = 0;
	for (it = myContext->exit.begin(); it != myContext->exit.end(); it++)
	{
		if ((myContext->nodesMap[*it].nodeDepth + myContext->nodesMap[*it].myLatency) > tmpProgDepth)
		{
			tmpProgDepth = myContext->nodesMap[*it].nodeDepth + myContext->nodesMap[*it].myLatency;
		}
	}
	return tmpProgDepth;
}


