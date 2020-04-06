/*
**************************************************
* 文件名：LinkStack.h
* 描  述：
* 作  者：Neil
* 日  期：2010-9-9 21:10:30
**************************************************
*/

#ifndef _LINK_STACK_H
#define _LINK_STACK_H

//typedef int ElementType;

template<typename ElementType>
class CLinkStack
{
public:
	CLinkStack(void);
	CLinkStack(const CLinkStack & OrigStack);
	~CLinkStack(void);
	const CLinkStack & operator=(const CLinkStack & RightStack);

	bool IsEmpty(void) const;
	void Push(const ElementType & Value);
	void Pop(void);
	ElementType Peek(int nPosition = 1) const;
	int GetNodeCount(void) const;
	bool IsValueExisting(const ElementType & Value) const;
	void Erase(void);
private:
	struct Node
	{
		ElementType Data;
		Node * pNextNode;
	} * pTop;
	int nNodeCount;
};


template<typename ElementType>
CLinkStack<ElementType>::CLinkStack(void):
											 pTop(0)
											,nNodeCount(0)
{
}

template<typename ElementType>
CLinkStack<ElementType>::CLinkStack(const CLinkStack & OrigStack):
											 pTop(0)
											,nNodeCount(OrigStack.nNodeCount)
{
	if(!OrigStack.IsEmpty())
	{
		this->pTop = new CLinkStack::Node;
		this->pTop->Data = OrigStack.pTop->Data; //first node
		this->pTop->pNextNode = 0;

		CLinkStack::Node * pLastNode = this->pTop;
		CLinkStack::Node * pOrigNode = OrigStack.pTop->pNextNode;
		while(0 != pOrigNode)
		{
			pLastNode->pNextNode = new CLinkStack::Node; //create new node;
			pLastNode->pNextNode->Data = pOrigNode->Data; //copy data
			pLastNode->pNextNode->pNextNode = 0; //point to null

			pLastNode = pLastNode->pNextNode; //move to next point
			pOrigNode = pOrigNode->pNextNode;
		}
	}
}

template<typename ElementType>
CLinkStack<ElementType>::~CLinkStack(void)
{
	CLinkStack::Node * pCurrentNode = this->pTop;
	CLinkStack::Node * pNextNode;
	while(0 != pCurrentNode)
	{
		pNextNode = pCurrentNode->pNextNode;
		delete pCurrentNode;
		pCurrentNode = pNextNode;
		this->nNodeCount --;
	}
#ifdef _DEBUG
	ASSERT(0 == this->nNodeCount);
#else
	VERIFY(0 == this->nNodeCount);
#endif
	this->pTop = NULL;
}

template<typename ElementType>
const CLinkStack<ElementType> & CLinkStack<ElementType>::operator=(const CLinkStack & RightStack)
{
	if(this != &RightStack)
	{
		this->~CLinkStack();
		this->nNodeCount = 0;
		if(RightStack.IsEmpty())
			this->pTop = 0;
		else
		{
			this->pTop = new CLinkStack::Node;
			this->pTop->Data = RightStack.pTop->Data; //first node
			this->pTop->pNextNode = 0;

			CLinkStack::Node * pLastNode;
			CLinkStack::Node * pRightStackNode = RightStack.pTop->pNextNode;
			while(0 != pRightStackNode)
			{
				pLastNode->pNextNode = new CLinkStack::Node; //create new node
				pLastNode->pNextNode->Data = pRightStackNode->Data; //copy data
				pLastNode->pNextNode->pNextNode = 0; //point to null

				pLastNode = pLastNode->pNextNode; //point to next node
				pRightStackNode = pRightStackNode->pNextNode; //point to next node
			}
		}
	}
	return * this;
}

template<typename ElementType>
bool CLinkStack<ElementType>::IsEmpty(void) const
{
	return (0 == this->pTop);
}

template<typename ElementType>
void CLinkStack<ElementType>::Push(const ElementType & Value)
{
	CLinkStack::Node * pTmpNode = this->pTop; //save to top node pointer
	this->pTop = new CLinkStack::Node; //create new node
	this->pTop->Data = Value;
	this->pTop->pNextNode = pTmpNode; //new node point to old top node
	nNodeCount ++;
}

template<typename ElementType>
void CLinkStack<ElementType>::Pop(void)
{
	if(!this->IsEmpty())
	{
		CLinkStack::Node * pTmpNode = this->pTop;
		this->pTop = this->pTop->pNextNode;
		delete pTmpNode;
		nNodeCount -- ;
	}
	else
		AfxMessageBox("The stack is empty!!");
}

template<typename ElementType>
ElementType CLinkStack<ElementType>::Peek(int nPosition) const
{
#ifdef _DEBUG
	ASSERT(!this->IsEmpty());
	ASSERT(nPosition >= 1);
	ASSERT(nPosition <= this->nNodeCount);
#else
	VERIFY(!this->IsEmpty());
	VERIFY(nPosition >= 1);
	VERIFY(nPosition <= this->nNodeCount);
#endif

	Node * pTmp = this->pTop;
	while((--nPosition) > 0)
		pTmp = pTmp->pNextNode;
	return pTmp->Data;
}

template<typename ElementType>
int CLinkStack<ElementType>::GetNodeCount(void) const
{
	return this->nNodeCount;
}

template<typename ElementType>
bool CLinkStack<ElementType>::IsValueExisting(const ElementType & Value) const
{
	for(int n = 1; n <= this->nNodeCount; ++ n)
		if(this->Peek(n) == Value)
			return true;
	return false;
}

template<typename ElementType>
void CLinkStack<ElementType>::Erase(void)
{
	this->~CLinkStack();
}
#endif