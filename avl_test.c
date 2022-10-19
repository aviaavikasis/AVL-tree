#include <assert.h> /* assert */
#include <stdio.h> /* printf */
#include "avl.h"

#define MAX_HEIGHT 10


void AvlCreateTest(void);
void AvlInsertTest(void);
void AvlRemoveTest(void);
void AvlHeightTest(void);
void AvlIsEmptyTest(void);
void AvlFindTest(void);
void AvlForEachTest(void);
void GeneralTest(void);
void BugsTest(void);

int CompareInts(const void *bst_data, const void *user_data, void *params);
int MultInts(void *data, void *params);

void BigTree(void);



int main(void)
{

	AvlCreateTest();
	AvlInsertTest();
	AvlRemoveTest();
	GeneralTest();
	
	AvlIsEmptyTest();
	AvlFindTest();
	AvlForEachTest();

	printf("\n->->->->->-> success!! <-<-<-<-<-<-\n\n");	

	return 0;
}


void AvlCreateTest(void)
{
	cmp_func cmp_function = &CompareInts;
	avl_ty *avl = AvlCreate(cmp_function , NULL);
	assert(NULL != avl);
	AvlDestroy(avl);
}

void AvlInsertTest(void)
{
	int arr[20] = {8,2,11,9,5,6,7,1,4,17,3,12,15,14,13,16,10,18,19,20};   
	int i = 0;
	
	cmp_func cmp_function = &CompareInts;
	avl_ty *avl = AvlCreate(cmp_function , NULL);

	for(; i < 20 ; ++i)
	{
		AvlInsert(avl, arr + i);
	}
	
	TreePrint(avl);
	AvlDestroy(avl);
}


void AvlIsEmptyTest(void)
{
	int x = 1;
	cmp_func cmp_function = &CompareInts;
	avl_ty *avl = AvlCreate(cmp_function , NULL);

	assert(1 == AvlIsEmpty(avl));
	
	AvlInsert(avl, &x);
	assert(0 == AvlIsEmpty(avl));
	
	AvlDestroy(avl);
}

void AvlFindTest(void)
{
	int arr[10] = {7,5,8,3,1,2,6,9,4,10};
	int i = 0;
	int not_exist1 = 11;
	int not_exist2 = 0;
	cmp_func cmp_function = &CompareInts;
	avl_ty *avl = AvlCreate(cmp_function , NULL);

	for(; i < 10 ; ++i)
	{
		AvlInsert(avl, arr + i);
	}
	
	for(i = 0 ; i < 10 ; ++i)
	{
		assert(SUCCESS == AvlFind(avl, arr + i));
	}
	
	assert(FAIL == AvlFind(avl, &not_exist1));
	assert(FAIL == AvlFind(avl, &not_exist2));
	
	AvlDestroy(avl);

}


void AvlForEachTest(void)
{
	int arrays_lut[3][10] = {
							{7,5,8,3,1,2,6,9,4,10},
							{7,5,8,3,1,2,6,9,4,10},
							{7,5,8,3,1,2,6,9,4,10}
												 };
	
	int mult_arr[10] = {14,10,16,6,2,4,12,18,8,20};
	int i = 0;
	cmp_func cmp_function = &CompareInts;
	action_func action = &MultInts;
	avl_ty *avl = NULL;
	trav_ty trav = INORDER; 
	
	for( ; trav <= POST_ORDER ; ++trav)
	{
		avl = AvlCreate(cmp_function , NULL);

		for(i = 0; i < 10 ; ++i)
		{
			AvlInsert(avl, arrays_lut[trav] + i);
		}
		
		assert(SUCCESS == AvlForEach(avl, action, NULL, trav));
		
		for(i = 0 ; i < 10 ; ++i)
		{
			assert(SUCCESS == AvlFind(avl, mult_arr + i));
		}
		
		AvlDestroy(avl);	
	}	
}


void AvlRemoveTest(void)
{
	int arr[10] = {7,5,8,3,1,2,6,9,4,10};
	int i = 0;
	int delete1 = 3;
	int delete2 = 9;
	int delete3 = 5;
	int delete4 = 7;
	
	cmp_func cmp_function = &CompareInts;
	avl_ty *avl = AvlCreate(cmp_function , NULL);

	for(; i < 10 ; ++i)
	{
		AvlInsert(avl, arr + i);
	}
	
	TreePrint(avl);
	
	AvlRemove(avl, &delete1);
	TreePrint(avl);
	AvlRemove(avl, &delete2);
	TreePrint(avl);
	AvlRemove(avl, &delete3);
	TreePrint(avl);
	AvlRemove(avl, &delete4);
	
	assert(FAIL == AvlFind(avl, &delete1));
	assert(FAIL == AvlFind(avl, &delete2));

	AvlDestroy(avl);	
}




void GeneralTest(void)
{
	int arr[10] = {1,2,3,4,5,6,7,8,9,10};
	cmp_func cmp_function = &CompareInts;
	avl_ty *avl = AvlCreate(cmp_function , NULL);


	AvlInsert(avl, arr + 0);
	AvlInsert(avl, arr + 1);
	AvlInsert(avl, arr + 2);
	AvlInsert(avl, arr + 3);
	AvlInsert(avl, arr + 4);
	AvlInsert(avl, arr + 5);
	AvlInsert(avl, arr + 6);
	AvlInsert(avl, arr + 7);
	
	TreePrint(avl);
	
	assert(8 == AvlSize(avl));
	
	AvlDestroy(avl);

}





int CompareInts(const void *avl_data, const void *user_data, void *params)
{
	(void)params;
	assert(NULL != avl_data);
	assert(NULL != user_data);
	return (*(int*)avl_data - *(int*)user_data);
}

int MultInts(void *data, void *params)
{
	(void)params;
	assert(NULL != data);
	*(int*)data *= 2;
	return 0;
}



int SumTree(void *data, void *params)
{
	*(int *)params += *(int *)data;
	return (0);
}

int AvlPrint(void *data, void *params)
{
	(void)params;
	printf("%d " , *(int *)data);
	return (0);
}













