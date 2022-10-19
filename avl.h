/*****************************************************   
 * Author : Avia Avikasis                            *
 * Reviewer: Gal                                     *
 * 02/08/2022                                        *
 * Description : imlementation of avl tree           *
 *                                                   *
 *****************************************************/
#ifndef __ILRD_OL127_128_AVL_TREE_H__
#define __ILRD_OL127_128_AVL_TREE_H__

#include <stddef.h> /* size_t */

typedef enum 
{
    INORDER     = 0,
    PREORDER    = 1,
    POST_ORDER  = 2
}trav_ty;

typedef enum
{
	SUCCESS = 0,
	FAIL    = 1
}status_ty;

typedef enum
{
	FALSE = 0,
	TRUE    = 1
}bool_ty;

typedef struct avl avl_ty;
typedef struct node node_ty;

typedef int(*cmp_func)(const void *avl_data,
                       const void *user_data,
                       void *params);

typedef int(*action_func)(void *data, void *params);

/*
DESCRIPTION : create a new avl tree
PARAMETERS : pointer compare function,
and params to compare function.
RETURN : pointer to the new avl tree.
COMPLEXITY : time - O(1), space - O(1) 
*/
avl_ty *AvlCreate(cmp_func cmp, void *params);

/*
DESCRIPTION : destroy exist avl tree
PARAMETERS : pointer to avl
RETURN : void
COMPLEXITY : time - O(n), space - O(1) 
*/
void AvlDestroy(avl_ty *avl);

/*
DESCRIPTION : insert new element to avl
PARAMETERS : pointer to avl, pointer to data
RETURN : SUCCESS or FAIL
COMPLEXITY : time - O(logn), space - O(1) 
*/
status_ty AvlInsert(avl_ty *avl, void *data);

/*
DESCRIPTION : remove element from avl
PARAMETERS : pointer to avl, pointer
data of the element
RETURN : void
COMPLEXITY : time - O(logn), space - O(1) 
*/
void AvlRemove(avl_ty *avl, void *data);

/*
DESCRIPTION : return the hight of avl tree
PARAMETERS : pointer to avl.
RETURN : the hight(long)
COMPLEXITY : time - O(n), space - O(1) 
*/
long AvlHeight(const avl_ty *avl);

/*
DESCRIPTION : return the num of elements in avl tree
PARAMETERS : pointer to avl.
RETURN : num of elements(size_t)
COMPLEXITY : time - O(n), space - O(1) 
*/	
size_t AvlSize(const avl_ty *avl);

/*
DESCRIPTION : check if avl is empty
PARAMETERS : pointer to avl.
RETURN : TRUE or FALSE
COMPLEXITY : time - O(1), space - O(1) 
*/
bool_ty AvlIsEmpty(const avl_ty *avl);

/*
DESCRIPTION : check if data exist in avl tree
PARAMETERS : pointer to avl, pointer to data.
RETURN : SUCCESS if found, else FAIL.
COMPLEXITY : time - O(log(n)), space - O(1) 
*/
status_ty AvlFind(const avl_ty *avl, void *data);

/*
DESCRIPTION : executes a function on each
element in avl tree.
PARAMETERS : pointer to avl, pointer to action function,
pointer to params of action function and traversal order.
RETURN : SUCCESS or FAIL.
COMPLEXITY : time - O(n), space - O(1) 
*/
status_ty AvlForEach(avl_ty *avl, action_func action,
						 void *params, trav_ty trav);
						 
void TreePrint(avl_ty *avl);

#endif /* __ILRD_OL127_128_AVLTREE_H__ */
