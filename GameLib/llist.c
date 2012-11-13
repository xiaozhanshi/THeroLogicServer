#include <stdlib.h>
#include <string.h>
#include "llist.h"

// 创建链表
LLIST* llist_creat(void)
{
	LLIST* handle = NULL;
	
	handle = (LLIST*)malloc(sizeof(LLIST));
	if(handle == NULL)
	{
		return NULL;
	}
	
	handle->head.prev = &handle->head;
	handle->head.next = &handle->head;
	handle->num = 0;

	return handle;
}
//后插，插在链表的最后
int llist_append(LLIST* handle, ELE* pELE)
{
	struct lnode* newnode = NULL;
	
	newnode = (struct lnode*)malloc(sizeof(struct lnode));
	if(newnode == NULL)
	{
		return -1;
	}
	newnode->element = (ELE*)malloc(sizeof(ELE));
	if(newnode->element == NULL)
	{
		free(newnode);
		return -1;
	}
	newnode->prev = handle->head.prev;
	newnode->next = &handle->head;
	handle->head.prev->next = newnode;
	handle->head.prev = newnode;
    
	memcpy(newnode->element, pELE, sizeof(ELE));
	handle->num += 1;

	return 0;	
}
//前插,插在头结点的后面的第一个结点
int llist_preappend(LLIST* handle, ELE* pELE)
{
	struct lnode* newnode = NULL;
	
	newnode = (struct lnode*)malloc(sizeof(struct lnode));
	if(newnode == NULL)
	{
		return -1;
	}
	newnode->element = (ELE*)malloc(sizeof(ELE));
	if(newnode->element == NULL)
	{
		free(newnode);
		return -1;
	}

	newnode->prev = &handle->head;
	newnode->next = handle->head.next;
	handle->head.next->prev = newnode;
	handle->head.next = newnode;

	memcpy(newnode->element, pELE, sizeof(ELE));
	handle->num += 1;

	return 0;
}
// 链表节点数
int llist_size(LLIST* handle)
{
	return handle->num;
}
// 清空链表
void llist_destory(LLIST* handle)
{
	struct lnode* tmp = NULL, *next = NULL;
	
	for(tmp = handle->head.next; tmp != &handle->head; tmp = next)
	{
		next = tmp->next;
		free(tmp->element);
		free(tmp);
	}
	free(handle);
}

// 删除Key内容的节点
int llist_delete(LLIST* handle, list_find_p find, void* key)
{
	struct lnode* tmp = NULL, *next = NULL;
	int count = 0;

	for(tmp = handle->head.next; tmp != &handle->head; tmp = next)
	{
		next = tmp->next;
		if(find(tmp->element, key) == 1)
		{
			tmp->prev->next = tmp->next;
			tmp->next->prev = tmp->prev;
			free(tmp->element);
			free(tmp);
			count += 1;
            handle->num -= 1;

			break; // 提高效率，ygh
		}
	}

	return count;
}
// 删除一个节点
int  llist_delete_quick(LLIST* handle,struct lnode * pNode)
{
	if(handle == NULL || pNode == NULL)
		return 0;
	if(pNode->next  == NULL || pNode->prev == NULL)
		return 0;
	pNode->prev->next = pNode->next;
	pNode->next->prev = pNode->prev;
	free(pNode->element);
	free(pNode);

	return 1;
}
// 安全轮询链表
void llist_travel(LLIST* handle, list_travel_p trav, void* arg)
{
	struct lnode* tmp = NULL;
	struct lnode* tmp2= NULL;
	for(tmp = handle->head.next; tmp != &handle->head;) 
	{
		tmp2 = tmp->next;	// 安全轮询 ygh
		trav(tmp->element, arg);
		tmp = tmp2;
	}
}
