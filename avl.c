/*****************************************************   
 * Author : Avia Avikasis                            *
 * Reviewer: Gal                                     *
 * 02/08/2022                                        *
 * Description : imlementation of avl tree           *
 *                                                   *
 *****************************************************/
#include <assert.h> /* assert */
#include <stdlib.h> /* malloc, free */
#include <stdio.h>

#include "avl.h"

#define MAX_HEIGHT 10

typedef enum 
{
	LEFT,
	RIGHT,
	CHILDREN_NUM
} avl_children_ty;

typedef enum
{
	LL,
	RR,
	LR,
	RL
}balance_state_ty;

struct node
{
	void *data;
	long hight;
	struct node *childrens[CHILDREN_NUM];
};

struct avl
{
	node_ty *root;
    cmp_func cmp;
    void *params;
};

typedef status_ty (*trav_func)(node_ty *, action_func, void *);
typedef node_ty* (*balance_func)(node_ty *);

static node_ty * BalanceLL(node_ty *node);
static node_ty * BalanceRR(node_ty *node);
static node_ty * BalanceLR(node_ty *node);
static node_ty * BalanceRL(node_ty *node);

balance_func balance_funcs_lut[4] = {&BalanceLL, &BalanceRR, &BalanceLR, &BalanceRL};

/* traverse functions */
static status_ty InOrder(node_ty *root, action_func action, void *params);
static status_ty PreOrder(node_ty *root, action_func action, void *params);
static status_ty PostOrder(node_ty *root, action_func action, void *params);

static long UpdateAllHights(node_ty *root);

static int HaveLeftChild(node_ty *node);
static int HaveRightChild(node_ty *node);
static int HeightsDiff(node_ty *node);
static int IsBalance(node_ty *node);
static int LeftHigherOrEqualFromRight(node_ty *node);
static int RightHigherOrEqualFromLeft(node_ty *node);
static void SwapAndRemove(node_ty *rm_node, node_ty *swap_node_father, avl_children_ty side);

trav_func travers_functions_lut[3] = {&InOrder, &PreOrder, &PostOrder};


/*--------------- setters & getters------------*/

static void *GetData(node_ty *node)
{
	assert(NULL != node);
	
	return node->data;
}

static node_ty *GetRoot(const avl_ty *avl)
{
	assert(NULL != avl);
	
	return avl->root;
}

static node_ty **GetChildren(node_ty *node)
{
	assert(NULL != node);
	
	return node->childrens;
}

static long GetHight(node_ty *node)
{
	
	if(NULL == node)
	{
		return -1;
	}
	return node->hight;
}

static cmp_func GetCmp(const avl_ty *avl)
{
	assert(NULL != avl);
	
	return avl->cmp;
}

static void *GetParams(const avl_ty *avl)
{
	assert(NULL != avl);
	
	return avl->params;
}

static void SetData(node_ty *node, void *data)
{
	assert(NULL != node);
	
	node->data = data;
}

static void SetHight(node_ty *node, long hight)
{
	assert(NULL != node);
	
	node->hight = hight;
} 


static node_ty *CreateNode(void *data , long hight)
{
	node_ty *new_node = (node_ty*)malloc(sizeof(node_ty));
	if(NULL == new_node)
	{
		return NULL;
	}

	new_node->data = data;
	new_node->hight = hight;
	new_node->childrens[LEFT] = NULL;
	new_node->childrens[RIGHT] = NULL;

	return new_node;
}


avl_ty *AvlCreate(cmp_func cmp, void *params)
{
	avl_ty *new_avl = NULL;
	node_ty *root = NULL;
	
	assert(NULL != cmp);
	 
	new_avl = (avl_ty*)malloc(sizeof(avl_ty));
	if(NULL == new_avl)
	{
		return NULL;
	}
	
	root = CreateNode(NULL, 0);
	if(NULL == root)
	{
		free(new_avl);
		return NULL;
	}
	new_avl->root = root;
	new_avl->cmp = cmp;
	new_avl->params = params;
	
	return new_avl;
}


static void RecursionDestroy(node_ty *root)
{
	if(NULL != GetChildren(root)[LEFT])
	{
		RecursionDestroy(GetChildren(root)[LEFT]);
	}	
	if(NULL != GetChildren(root)[RIGHT])
	{
		RecursionDestroy(GetChildren(root)[RIGHT]);
	}
	free(root);
	root = NULL;
}


void AvlDestroy(avl_ty *avl)
{
	assert(NULL != avl);

	RecursionDestroy(avl->root);
	free(avl);
	avl = NULL;
}

static avl_children_ty FindInsertSide(node_ty *node,
									  void *data,
									  cmp_func cmp,
									   void *params)
{
	assert(NULL != node);
	assert(NULL != cmp);
	
	return (0 > cmp(node->data, data, params)) ? RIGHT : LEFT;
}

static node_ty *SubTreeBalance(node_ty *sub_tree)
{
	
	UpdateAllHights(sub_tree);
	
	if(IsBalance(sub_tree))
	{
		return sub_tree;
	}

	
	if(HeightsDiff(sub_tree) > 1 && 
					LeftHigherOrEqualFromRight(GetChildren(sub_tree)[LEFT]))
	{
		return balance_funcs_lut[LL](sub_tree);
	}
	else if(HeightsDiff(sub_tree) > 1)
	{
		return balance_funcs_lut[LR](sub_tree);
	}
	
	else if(HeightsDiff(sub_tree) < -1 &&
				    RightHigherOrEqualFromLeft(GetChildren(sub_tree)[RIGHT]))
	{
		return balance_funcs_lut[RR](sub_tree);
	}
	else if(HeightsDiff(sub_tree) < -1)
	{
		return balance_funcs_lut[RL](sub_tree);
	}
	
	UpdateAllHights(sub_tree);
	
	return NULL;
}


static node_ty *RecursiveInsert(node_ty *root,
								 void *data,
								 cmp_func cmp,
								 void *params)
{
	node_ty *new_node = NULL;
	node_ty *sub_tree = NULL;
	
	avl_children_ty which_side = FindInsertSide(root, data, cmp, params);
	sub_tree = GetChildren(root)[which_side];
	
	if(NULL != sub_tree)
	{
		RecursiveInsert(sub_tree, data, cmp, params);
		root->childrens[which_side] = SubTreeBalance(sub_tree);
		UpdateAllHights(root);
		return root;
	}
	
	else
	{
		new_node = CreateNode(data, 0);
		if(NULL == new_node)
		{
			return NULL;
		}
		
		GetChildren(root)[which_side] = new_node;
		UpdateAllHights(root);
		return root;
	}	
}


status_ty AvlInsert(avl_ty *avl, void *data)
{
	node_ty *new_node = NULL;
	assert(NULL != avl);
	
	if(NULL == GetData(GetRoot(avl)))
	{
		SetData(GetRoot(avl), data);
		return SUCCESS;
	}
	new_node = RecursiveInsert(avl->root, data, avl->cmp, avl->params);
	if(NULL == new_node)
	{
		return FAIL;
	}
	avl->root = SubTreeBalance(GetRoot(avl));
	UpdateAllHights(GetRoot(avl));

	return SUCCESS;
}


static int HaveTwoChildrens(node_ty *node)
{
	return (NULL != GetChildren(node)[LEFT] &&
			NULL != GetChildren(node)[RIGHT]);
}

static int HaveOneChildren(node_ty *node)
{
	return ((NULL != GetChildren(node)[LEFT] &&
			NULL == GetChildren(node)[RIGHT])
			||
			(NULL == GetChildren(node)[LEFT] &&
			NULL != GetChildren(node)[RIGHT]));
}

static int IsLeaf(node_ty *node)
{
	assert(NULL != node);
	
	return (NULL == GetChildren(node)[LEFT] &&
			NULL == GetChildren(node)[RIGHT]);
}


static size_t RecursionSize(node_ty *root)
{
	if(IsLeaf(root))
	{
		return 1;
	}
	if(HaveTwoChildrens(root))
	{
		return 1 + RecursionSize(GetChildren(root)[LEFT])
							 + RecursionSize(GetChildren(root)[RIGHT]);
	}
	if(HaveOneChildren(root))
	{
		return  (NULL == GetChildren(root)[LEFT]) ? 
		1 + RecursionSize(GetChildren(root)[RIGHT]) : 
		1 + RecursionSize(GetChildren(root)[LEFT]);
	}
	return 0;
} 


size_t AvlSize(const avl_ty *avl)
{
	assert(NULL != avl);
	if(NULL == GetData(GetRoot(avl)))
	{
		return 0;
	}
	return RecursionSize(GetRoot(avl));
}

bool_ty AvlIsEmpty(const avl_ty *avl)
{
	assert(NULL != avl);
	return (NULL == GetData(GetRoot(avl)));
}


static avl_children_ty FindSearchSide(node_ty *root,
										 void *data,
										  cmp_func cmp,
										   void *params)
{
	assert(NULL != root);
	assert(NULL != cmp);
	
	if(0 > cmp(GetData(root), data, params))
	{
		return RIGHT;
	}
	return LEFT;
}


static node_ty *RecursiveFind(node_ty *root, void *data, cmp_func cmp,
																 void *params)
{
	avl_children_ty search_side = 0;
	
	assert(NULL != root);
	assert(NULL != cmp);
	
	if(0 == cmp(GetData(root), data, params))
	{
		return root;
	}
	
	search_side = FindSearchSide(root, data, cmp, params);
	
	if(NULL != GetChildren(root)[search_side])
	{
		return RecursiveFind(GetChildren(root)[search_side], data, cmp, params);
	}
	
	return NULL;
}


status_ty AvlFind(const avl_ty *avl, void *data)
{
	assert(NULL != avl);
	
	if(NULL == RecursiveFind(GetRoot(avl), data, GetCmp(avl), GetParams(avl)))
	{
		return FAIL;
	}
	return SUCCESS;
}


static status_ty InOrder(node_ty *root, action_func action, void *params)
{
	status_ty status = SUCCESS;
	assert(NULL != root);
	assert(NULL != action);
	
	if(NULL != GetChildren(root)[LEFT])
	{
		status |= InOrder(GetChildren(root)[LEFT], action, params);
	}
	
	status = action(GetData(root), params);
	
	if(NULL != GetChildren(root)[RIGHT])
	{
		status |= InOrder(GetChildren(root)[RIGHT], action, params);
	}
	
	return status;
}


static status_ty PreOrder(node_ty *root, action_func action, void *params)
{
	status_ty status = SUCCESS;
	assert(NULL != root);
	assert(NULL != action);
	
	status = action(GetData(root), params);
	
	if(NULL != GetChildren(root)[LEFT])
	{
		status |= PreOrder(GetChildren(root)[LEFT], action, params);
	}
	
	if(NULL != GetChildren(root)[RIGHT])
	{
		status |= PreOrder(GetChildren(root)[RIGHT], action, params);
	}
	
	return status;

}

static status_ty PostOrder(node_ty *root, action_func action, void *params)
{
	status_ty status = SUCCESS;
	assert(NULL != root);
	assert(NULL != action);
	
	if(NULL != GetChildren(root)[LEFT])
	{
		status |= PostOrder(GetChildren(root)[LEFT], action, params);
	}
	
	if(NULL != GetChildren(root)[RIGHT])
	{
		status |= PostOrder(GetChildren(root)[RIGHT], action, params);
	}
	
	status |= action(GetData(root), params);
	
	return status;

}

status_ty AvlForEach(avl_ty *avl, action_func action, void *params,trav_ty trav)
{
	assert(NULL != avl);
	assert(NULL != action);
	
	return travers_functions_lut[trav](GetRoot(avl), action, params);	
}


static node_ty *GetMostLeft(node_ty *node)
{
	if(!IsLeaf(node) && NULL != GetChildren(node)[LEFT])
	{
		return GetMostLeft(GetChildren(node)[LEFT]);
	}
	return node;
}


static void SwapAndRemove(node_ty *rm_node, node_ty *swap_node_father, avl_children_ty side)
{
	node_ty *swap_node = GetChildren(swap_node_father)[side];
		
	SetData(rm_node, GetData(swap_node));
	
	if(IsLeaf(swap_node))
	{
		GetChildren(swap_node_father)[side] = NULL;
		free(swap_node);
	}
	else
	{		
		GetChildren(swap_node_father)[side] = GetChildren(swap_node)[RIGHT];
		free(swap_node);
	}	
}


static int HaveLeftChildren(node_ty *node)
{
	assert(NULL != node);
	return (NULL != GetChildren(node)[LEFT]);
}

static node_ty *RecursiveRemove(node_ty *root, cmp_func cmp, void *params, void *data)
{
	avl_children_ty search_side = 0;
	node_ty *rm_position = NULL;
	node_ty *father_of_next = NULL;
	node_ty *next = NULL;
	
	assert(NULL != root);
	assert(NULL != cmp);
	
	search_side = FindSearchSide(root, data, cmp, params);
	
	rm_position = GetChildren(root)[search_side];
	
	if(0 == cmp(GetData(rm_position), data, params))
	{
		if(IsLeaf(rm_position))
		{
			free(rm_position);
			GetChildren(root)[search_side] = NULL;
		}
		else if(HaveOneChildren(rm_position))
		{
			GetChildren(root)[search_side] = 
			(NULL == GetChildren(rm_position)[LEFT]) ?
			GetChildren(rm_position)[RIGHT] :
			GetChildren(rm_position)[LEFT];
			
			free(rm_position);
		}
		
		/* rm_position have two childrens */
		else if(HaveLeftChildren(GetChildren(rm_position)[RIGHT]))
		{

			next = GetMostLeft(GetChildren(rm_position)[RIGHT]);
			SetData(rm_position, GetData(next));
			rm_position->childrens[RIGHT] = 
			RecursiveRemove(GetChildren(rm_position)[RIGHT], cmp, params, GetData(next));
			
			root->childrens[search_side] = rm_position;
		}
		else
		{
			father_of_next = rm_position;
			SwapAndRemove(rm_position, father_of_next, RIGHT);
		}
			
		UpdateAllHights(root);
		return root;
	}
	
	else
	{
		RecursiveRemove(rm_position, cmp, params, data);
		root->childrens[search_side] = SubTreeBalance(rm_position);
		UpdateAllHights(root);
		return root;
	}
}


static void RemoveRoot(avl_ty *avl)
{
	node_ty *father_of_next = NULL;
	node_ty *root = GetRoot(avl);
	node_ty *next = NULL;
	
	if(IsLeaf(root))
	{
		free(root);
		avl->root = NULL;
	}
	else if(HaveOneChildren(root))
	{
		avl->root = 
		(NULL == GetChildren(root)[LEFT]) ?
		GetChildren(root)[RIGHT] :
		GetChildren(root)[LEFT];
		
		free(root);
	}
	/* root have two childrens */
	else if(HaveLeftChildren(GetChildren(root)[RIGHT]))
	{
		next = GetMostLeft(GetChildren(root)[RIGHT]);
		SetData(root, GetData(next));
		avl->root->childrens[RIGHT] =
		RecursiveRemove(GetChildren(root)[RIGHT],
									 GetCmp(avl),
									 GetParams(avl),
									 GetData(next));
	}
	else
	{
		father_of_next = root;
		SwapAndRemove(root, father_of_next, RIGHT);
	}
	
	UpdateAllHights(root);
}


void AvlRemove(avl_ty *avl, void *data)
{
	if(0 == GetCmp(avl)(GetData(GetRoot(avl)), data, GetParams(avl)))
	{
		RemoveRoot(avl);
		avl->root = SubTreeBalance(GetRoot(avl));	
	}
	else
	{
		RecursiveRemove(GetRoot(avl), GetCmp(avl), GetParams(avl), data);
		avl->root = SubTreeBalance(GetRoot(avl));	
	}
}


static long UpdateAllHights(node_ty *root)
{
	long left_subtree_hight = 0;
	long right_subtree_hight = 0;
	assert(NULL != root);
	
	left_subtree_hight = (NULL == GetChildren(root)[LEFT]) ? 0 :
								 1 + UpdateAllHights(GetChildren(root)[LEFT]);
	
	right_subtree_hight = (NULL == GetChildren(root)[RIGHT]) ? 0 :
								 1 + UpdateAllHights(GetChildren(root)[RIGHT]);
	 
	if(left_subtree_hight >= right_subtree_hight)
	{
		SetHight(root, left_subtree_hight);
		return left_subtree_hight;
	}
	
	SetHight(root, right_subtree_hight);
	return right_subtree_hight;
}


long AvlHeight(const avl_ty *avl)
{
	assert(NULL != avl);
	return GetHight(GetRoot(avl)); 
}


static node_ty *BalanceLL(node_ty *root)
{
	node_ty *pivot = NULL;
	node_ty *save_right_of_pivot = NULL;
	
	assert(NULL != root);
	
	pivot = GetChildren(root)[LEFT];
	save_right_of_pivot = GetChildren(pivot)[RIGHT];
	pivot->childrens[RIGHT] = root;
	root->childrens[LEFT] = save_right_of_pivot;
	
	return pivot;
}


static node_ty *BalanceRR(node_ty *root)
{
	node_ty *pivot = NULL;
	node_ty *save_left_of_pivot = NULL;

	assert(NULL != root);
	
	pivot = GetChildren(root)[RIGHT];
	save_left_of_pivot = GetChildren(pivot)[LEFT];
	pivot->childrens[LEFT] = root;
	root->childrens[RIGHT] = save_left_of_pivot;
	
	return pivot;
}


static void LeftRotation(node_ty *father_of_root, node_ty *root, avl_children_ty side)
{
	node_ty *pivot = NULL;
	node_ty *save_left_of_pivot = NULL;
	
	assert(NULL != father_of_root);
	assert(NULL != root);
	
	pivot = GetChildren(root)[RIGHT];
	save_left_of_pivot = GetChildren(pivot)[LEFT];
	father_of_root->childrens[side] = pivot;
	root->childrens[RIGHT] = save_left_of_pivot;
	pivot->childrens[LEFT] = root;
}


static void RightRotation(node_ty *father_of_root, node_ty *root, avl_children_ty side)
{
	node_ty *pivot = NULL;
	node_ty *save_right_of_pivot = NULL;
	
	assert(NULL != father_of_root);
	assert(NULL != root);
	
	pivot = GetChildren(root)[LEFT];
	save_right_of_pivot = GetChildren(pivot)[RIGHT];
	father_of_root->childrens[side] = pivot;
	root->childrens[LEFT] = save_right_of_pivot;
	pivot->childrens[RIGHT] = root;
}

static node_ty *BalanceLR(node_ty *root)
{
	node_ty *pivot = NULL;

	assert(NULL != root);
	
	pivot = GetChildren(root)[LEFT];
	LeftRotation(root, pivot, LEFT);
	
	return BalanceLL(root);
}


static node_ty *BalanceRL(node_ty *root)
{
	node_ty *pivot = NULL;
	
	assert(NULL != root);
	
	pivot = GetChildren(root)[RIGHT];
	RightRotation(root, pivot, RIGHT);
	
	return BalanceRR(root);
}


static int HaveLeftChild(node_ty *node)
{
	return (NULL != GetChildren(node)[LEFT]);
}

static int HaveRightChild(node_ty *node)
{
	return (NULL != GetChildren(node)[RIGHT]);
}

static int HeightsDiff(node_ty *node)
{
	int left_subtree_hight = 0;
	int right_subtree_hight = 0;
	assert(NULL != node);
	
	left_subtree_hight = (!HaveLeftChild(node)) ? 0 :
								 GetHight(GetChildren(node)[LEFT]) + 1;
	
	right_subtree_hight = (!HaveRightChild(node)) ? 0 :
								 GetHight(GetChildren(node)[RIGHT]) + 1;
	
	return (left_subtree_hight - right_subtree_hight);
}


static int IsBalance(node_ty *node)
{
	return ((NULL == node) ||(-1 <= HeightsDiff(node) && 1 >= HeightsDiff(node)));
}



static int LeftHigherOrEqualFromRight(node_ty *node)
{
	assert(NULL != node);
	return (GetHight(GetChildren(node)[LEFT]) >= 
						GetHight(GetChildren(node)[RIGHT]));
}

static int RightHigherOrEqualFromLeft(node_ty *node)
{
	assert(NULL != node);
	return (GetHight(GetChildren(node)[RIGHT]) >= 
						GetHight(GetChildren(node)[LEFT]));
}
	

					 
static void TreePrintR(node_ty *node, int level)
{
    int i = 0;
    if (node == NULL)
    {
        return;
    }
     
    level += MAX_HEIGHT;
 
    TreePrintR(node->childrens[RIGHT], level);

    for (i = MAX_HEIGHT; i < level; i++)
    {
        printf("   ");
    }
    
    printf("Num: %d H:%ld\n",  *(int *)node->data, node->hight);
 
    TreePrintR(node->childrens[LEFT], level);
}							 
						 
void TreePrint(avl_ty *avl)
{
    printf("\n----------------------------TREE-----------------------------\n");
    TreePrintR(avl->root, 0);
    printf("\n-------------------------------------------------------------\n");
}


				 
						 
						 
						 
						 
						 
						 
