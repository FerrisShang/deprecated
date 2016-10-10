#include "stdio.h"
#include "stdlib.h"
#include "rb_tree.h"

int CompFunc(const void *a,const void *b)
{
	if(*(int*)a > *(int*)b){
		return 1;
	}else if(*(int*)a < *(int*)b){
		return -1;
	}else{
		return 0;
	}
}
void desKey_cb(void *key)
{
	printf("free key(%x) @ %p\n", *(int*)key, key);
	rb_free(key);
}
void desInfo_cb(void *info)
{
	printf("free info(%x) @ %p\n", *(int*)info, info);
	rb_free(info);
}
void inorder_cb(struct rb_node *node, void *pdata)
{
	printf("0x%08x : 0x%08x\n", *(int*)node->key, *(int*)node->info);
}
void enumerate_cb(struct rb_node *node, void *pdata)
{
	printf("0x%08x : 0x%08x\n", *(int*)node->key, *(int*)node->info);
}
#define MAX_NUM  10
#define KEY_MASK 0x7
int main(int argc, char *argv[])
{
	struct rb_tree* rb_tree;
	int *key, *info;
	int total = 0;
	struct rb_node* node;
	int i;
	printf("\n");
	rb_tree = RBTreeCreate(CompFunc, desKey_cb, desInfo_cb);
	if(!rb_tree){
		printf("create tree failed\n");
		return 0;
	}
	srand(time(0));
	for(i=0;i<MAX_NUM;i++){
		key = rb_malloc(sizeof(int));
		info = rb_malloc(sizeof(int));
		if(!key || !info){
			printf("malloc key or info failed\n");
			return 0;
		}
		*key = rand()%KEY_MASK;
		*info = rand();
		node = RBExactQuery(rb_tree, key);
		if(node){
			printf("node 0x%08x exists\n", *key);
			free(key);
			free(info);
		}else{
			total++;
			RBTreeInsert(rb_tree, key, info);
		}
	}
	printf("data init done total:%d\ndump key & info :\n" ,total);
	RBInorderTree(rb_tree, inorder_cb, NULL);
	int tmp = rand()%KEY_MASK;
	node = RBExactQuery(rb_tree, &tmp);
	if(node){
		printf("0x%x is 0x%08x\n", *(int*)node->key, *(int*)node->info);
		printf("delete nodes key = %d\n", tmp);
		RBDelete(rb_tree, node);
		int high = tmp + 2; int low = tmp - 2;
		printf("dump key & info in range(%d~%d)\n", low, high);
		RBEnumerate(rb_tree, &low, &high, enumerate_cb, NULL);
	}else{
		printf("node 0x%x not found\n", tmp);
	}
	printf("destroy tree\n");
	RBTreeDestroy(rb_tree);
}
