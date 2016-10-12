#define rb_malloc malloc
#define rb_free   free

/* Compare(a,b) should return 1 if *a > *b, -1 if *a < *b, and 0 otherwise */
/* Destroy(a) takes a pointer to whatever key might be and frees it accordingly */

/* warning : never call RBTreeInsert & RBDelete in callback */

struct rb_tree;

struct rb_node {
  void* key;
  void* info;
  int red; /* if red=0 then the node is black */
  struct rb_node* left;
  struct rb_node* right;
  struct rb_node* parent;
};
struct rb_tree* RBTreeCreate( int (*CompFunc)(const void *a,const void *b),
		void (*DestroyKeyFunc)(void *key), void (*DestroyInfoFunc)(void *info));
struct rb_node* RBTreeInsert(struct rb_tree*, void* key, void* info);
void RBDelete(struct rb_tree* , struct rb_node* );
struct rb_node* RBExactQuery(struct rb_tree*, void *key);
void RBInorderTree(struct rb_tree* tree,
		void (*callback)(struct rb_node *node, void *pdata), void *pdata);
void RBEnumerate(struct rb_tree* tree, void* low, void* high,
		void (*callback)(struct rb_node *node, void *pdata), void *pdata);
void RBTreeDestroy(struct rb_tree*);
