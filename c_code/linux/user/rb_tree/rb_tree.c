#include "stdio.h"
#include "stdlib.h"
#include "rb_tree.h"

/* Compare(a,b) should return 1 if *a > *b, -1 if *a < *b, and 0 otherwise */
/* Destroy(a) takes a pointer to whatever key might be and frees it accordingly */
struct rb_tree {
  int (*Compare)(const void* a, const void* b);
  void (*DestroyKey)(void* key);
  void (*DestroyInfo)(void* info);
  /*  A sentinel is used for root and for nil.  These sentinels are */
  /*  created when RBTreeCreate is caled.  root->left should always */
  /*  point to the node which is the root of the tree.  nil points to a */
  /*  node which should always be black but has aribtrary children and */
  /*  parent and no key or info.  The point of using these sentinels is so */
  /*  that the root and nil nodes do not require special cases in the code */
  struct rb_node* root;
  struct rb_node* nil;
};

/***********************************************************************/
/*  FUNCTION:  RBTreeCreate */
/**/
/*  INPUTS:  All the inputs are names of functions.  CompFunc takes to */
/*  void pointers to keys and returns 1 if the first arguement is */
/*  "greater than" the second.   DestroyKeyFunc takes a pointer to a key and */
/*  destroys it in the appropriate manner when the node containing that */
/*  key is deleted.  DestroyInfoFunc is similiar to DestroyKeyFunc except it */
/*  recieves a pointer to the info of a node and destroys it. */
/**/
/*  OUTPUT:  This function returns a pointer to the newly created */
/*  red-black tree. */
/**/
/*  Modifies Input: none */
/***********************************************************************/

struct rb_tree* RBTreeCreate( int (*CompFunc) (const void*,const void*),
		void (*DestroyKeyFunc) (void*), void (*DestroyInfoFunc) (void*))
{
	struct rb_tree* newTree;
	struct rb_node* temp;

	newTree=(struct rb_tree*) rb_malloc(sizeof(struct rb_tree));
	if(!newTree){
		goto malloc_tree_failed;
	}
	newTree->Compare=  CompFunc;
	newTree->DestroyKey= DestroyKeyFunc;
	newTree->DestroyInfo= DestroyInfoFunc;

	/*  see the comment in the rb_tree structure for information on nil and root */
	temp=newTree->nil= (struct rb_node*) rb_malloc(sizeof(struct rb_node));
	if(!temp){
		goto malloc_nil_failed;
	}
	temp->parent=temp->left=temp->right=temp;
	temp->red=0;
	temp->key=0;
	temp=newTree->root= (struct rb_node*) rb_malloc(sizeof(struct rb_node));
	if(!temp){
		goto malloc_root_failed;
	}
	temp->parent=temp->left=temp->right=newTree->nil;
	temp->key=0;
	temp->red=0;
	return(newTree);
malloc_root_failed :
	rb_free(newTree->nil);
malloc_nil_failed :
	rb_free(newTree);
malloc_tree_failed :
	return NULL;
}

/***********************************************************************/
/*  FUNCTION:  LeftRotate */
/**/
/*  INPUTS:  This takes a tree so that it can access the appropriate */
/*           root and nil pointers, and the node to rotate on. */
/**/
/*  OUTPUT:  None */
/**/
/*  Modifies Input: tree, x */
/**/
/*  EFFECTS:  Rotates as described in _Introduction_To_Algorithms by */
/*            Cormen, Leiserson, Rivest (Chapter 14).  Basically this */
/*            makes the parent of x be to the left of x, x the parent of */
/*            its parent before the rotation and fixes other pointers */
/*            accordingly. */
/***********************************************************************/

static void LeftRotate(struct rb_tree* tree, struct rb_node* x)
{
	struct rb_node* y;
	struct rb_node* nil=tree->nil;

	/*  I originally wrote this function to use the sentinel for */
	/*  nil to avoid checking for nil.  However this introduces a */
	/*  very subtle bug because sometimes this function modifies */
	/*  the parent pointer of nil.  This can be a problem if a */
	/*  function which calls LeftRotate also uses the nil sentinel */
	/*  and expects the nil sentinel's parent pointer to be unchanged */
	/*  after calling this function.  For example, when RBDeleteFixUP */
	/*  calls LeftRotate it expects the parent pointer of nil to be */
	/*  unchanged. */

	y=x->right;
	x->right=y->left;

	if (y->left != nil) y->left->parent=x; /* used to use sentinel here */
	/* and do an unconditional assignment instead of testing for nil */

	y->parent=x->parent;

	/* instead of checking if x->parent is the root as in the book, we */
	/* count on the root sentinel to implicitly take care of this case */
	if( x == x->parent->left) {
		x->parent->left=y;
	} else {
		x->parent->right=y;
	}
	y->left=x;
	x->parent=y;
}


/***********************************************************************/
/*  FUNCTION:  RighttRotate */
/**/
/*  INPUTS:  This takes a tree so that it can access the appropriate */
/*           root and nil pointers, and the node to rotate on. */
/**/
/*  OUTPUT:  None */
/**/
/*  Modifies Input?: tree, y */
/**/
/*  EFFECTS:  Rotates as described in _Introduction_To_Algorithms by */
/*            Cormen, Leiserson, Rivest (Chapter 14).  Basically this */
/*            makes the parent of x be to the left of x, x the parent of */
/*            its parent before the rotation and fixes other pointers */
/*            accordingly. */
/***********************************************************************/

static void RightRotate(struct rb_tree* tree, struct rb_node* y)
{
	struct rb_node* x;
	struct rb_node* nil=tree->nil;

	/*  I originally wrote this function to use the sentinel for */
	/*  nil to avoid checking for nil.  However this introduces a */
	/*  very subtle bug because sometimes this function modifies */
	/*  the parent pointer of nil.  This can be a problem if a */
	/*  function which calls LeftRotate also uses the nil sentinel */
	/*  and expects the nil sentinel's parent pointer to be unchanged */
	/*  after calling this function.  For example, when RBDeleteFixUP */
	/*  calls LeftRotate it expects the parent pointer of nil to be */
	/*  unchanged. */

	x=y->left;
	y->left=x->right;

	if (nil != x->right)  x->right->parent=y; /*used to use sentinel here */
	/* and do an unconditional assignment instead of testing for nil */

	/* instead of checking if x->parent is the root as in the book, we */
	/* count on the root sentinel to implicitly take care of this case */
	x->parent=y->parent;
	if( y == y->parent->left) {
		y->parent->left=x;
	} else {
		y->parent->right=x;
	}
	x->right=y;
	y->parent=x;
}

/***********************************************************************/
/*  FUNCTION:  TreeInsertHelp  */
/**/
/*  INPUTS:  tree is the tree to insert into and z is the node to insert */
/**/
/*  OUTPUT:  none */
/**/
/*  Modifies Input:  tree, z */
/**/
/*  EFFECTS:  Inserts z into the tree as if it were a regular binary tree */
/*            using the algorithm described in _Introduction_To_Algorithms_ */
/*            by Cormen et al.  This funciton is only intended to be called */
/*            by the RBTreeInsert function and not by the user */
/***********************************************************************/

static void TreeInsertHelp(struct rb_tree* tree, struct rb_node* z)
{
	/*  This function should only be called by InsertRBTree (see above) */
	struct rb_node* x;
	struct rb_node* y;
	struct rb_node* nil=tree->nil;

	z->left=z->right=nil;
	y=tree->root;
	x=tree->root->left;
	while( x != nil) {
		y=x;
		if (1 == tree->Compare(x->key,z->key)) { /* x.key > z.key */
			x=x->left;
		} else { /* x,key <= z.key */
			x=x->right;
		}
	}
	z->parent=y;
	if ( (y == tree->root) ||
			(1 == tree->Compare(y->key,z->key))) { /* y.key > z.key */
		y->left=z;
	} else {
		y->right=z;
	}
}

/*  Before calling Insert RBTree the node x should have its key set */

/***********************************************************************/
/*  FUNCTION:  RBTreeInsert */
/**/
/*  INPUTS:  tree is the red-black tree to insert a node which has a key */
/*           pointed to by key and info pointed to by info.  */
/**/
/*  OUTPUT:  This function returns a pointer to the newly inserted node */
/*           which is guarunteed to be valid until this node is deleted. */
/*           What this means is if another data structure stores this */
/*           pointer then the tree does not need to be searched when this */
/*           is to be deleted. */
/**/
/*  Modifies Input: tree */
/**/
/*  EFFECTS:  Creates a node node which contains the appropriate key and */
/*            info pointers and inserts it into the tree. */
/***********************************************************************/

struct rb_node * RBTreeInsert(struct rb_tree* tree, void* key, void* info)
{
	struct rb_node * y;
	struct rb_node * x;
	struct rb_node * newNode;

	x=(struct rb_node*) rb_malloc(sizeof(struct rb_node));
	if(!x){
		return NULL;
	}
	x->key=key;
	x->info=info;

	TreeInsertHelp(tree,x);
	newNode=x;
	x->red=1;
	while(x->parent->red) { /* use sentinel instead of checking for root */
		if (x->parent == x->parent->parent->left) {
			y=x->parent->parent->right;
			if (y->red) {
				x->parent->red=0;
				y->red=0;
				x->parent->parent->red=1;
				x=x->parent->parent;
			} else {
				if (x == x->parent->right) {
					x=x->parent;
					LeftRotate(tree,x);
				}
				x->parent->red=0;
				x->parent->parent->red=1;
				RightRotate(tree,x->parent->parent);
			}
		} else { /* case for x->parent == x->parent->parent->right */
			y=x->parent->parent->left;
			if (y->red) {
				x->parent->red=0;
				y->red=0;
				x->parent->parent->red=1;
				x=x->parent->parent;
			} else {
				if (x == x->parent->left) {
					x=x->parent;
					RightRotate(tree,x);
				}
				x->parent->red=0;
				x->parent->parent->red=1;
				LeftRotate(tree,x->parent->parent);
			}
		}
	}
	tree->root->left->red=0;
	return(newNode);
}

/***********************************************************************/
/*  FUNCTION:  TreeSuccessor  */
/**/
/*    INPUTS:  tree is the tree in question, and x is the node we want the */
/*             the successor of. */
/**/
/*    OUTPUT:  This function returns the successor of x or NULL if no */
/*             successor exists. */
/**/
/*    Modifies Input: none */
/**/
/*    Note:  uses the algorithm in _Introduction_To_Algorithms_ */
/***********************************************************************/

static struct rb_node* TreeSuccessor(struct rb_tree* tree,struct rb_node* x)
{
	struct rb_node* y;
	struct rb_node* nil=tree->nil;
	struct rb_node* root=tree->root;

	if (nil != (y = x->right)) { /* assignment to y is intentional */
		while(y->left != nil) { /* returns the minium of the right subtree of x */
			y=y->left;
		}
		return(y);
	} else {
		y=x->parent;
		while(x == y->right) { /* sentinel used instead of checking for nil */
			x=y;
			y=y->parent;
		}
		if (y == root) return(nil);
		return(y);
	}
}

/***********************************************************************/
/*  FUNCTION:  Treepredecessor  */
/**/
/*    INPUTS:  tree is the tree in question, and x is the node we want the */
/*             the predecessor of. */
/**/
/*    OUTPUT:  This function returns the predecessor of x or NULL if no */
/*             predecessor exists. */
/**/
/*    Modifies Input: none */
/**/
/*    Note:  uses the algorithm in _Introduction_To_Algorithms_ */
/***********************************************************************/

static struct rb_node* TreePredecessor(struct rb_tree* tree, struct rb_node* x)
{
	struct rb_node* y;
	struct rb_node* nil=tree->nil;
	struct rb_node* root=tree->root;

	if (nil != (y = x->left)) { /* assignment to y is intentional */
		while(y->right != nil) { /* returns the maximum of the left subtree of x */
			y=y->right;
		}
		return(y);
	} else {
		y=x->parent;
		while(x == y->left) {
			if (y == root) return(nil);
			x=y;
			y=y->parent;
		}
		return(y);
	}
}

/***********************************************************************/
/*  FUNCTION:  InorderTreeHelp */
/**/
/*    INPUTS:  tree is the tree to inorder and x is the current inorder node */
/**/
/*    OUTPUT:  none  */
/**/
/*    Modifies Input: none */
/**/
/*    Note:    This function should only be called from RBInorderTree */
/***********************************************************************/

static void InorderTreeHelp(struct rb_tree* tree, struct rb_node* x,
		void (*callback)(struct rb_node *node, void *pdata),
		void *pdata)
{
	struct rb_node* nil=tree->nil;
	struct rb_node* root=tree->root;
	if (x != tree->nil) {
		InorderTreeHelp(tree,x->left, callback, pdata);
		callback(x, pdata);
		InorderTreeHelp(tree,x->right, callback, pdata);
	}
}

void RBInorderTree(struct rb_tree* tree,
		void (*callback)(struct rb_node *node, void *pdata), void *pdata)
{
	InorderTreeHelp(tree,tree->root->left, callback, pdata);
}

/***********************************************************************/
/*  FUNCTION:  TreeDestHelper */
/**/
/*    INPUTS:  tree is the tree to destroy and x is the current node */
/**/
/*    OUTPUT:  none  */
/**/
/*    EFFECTS:  This function recursively destroys the nodes of the tree */
/*              postorder using the DestroyKey and DestroyInfo functions. */
/**/
/*    Modifies Input: tree, x */
/**/
/*    Note:    This function should only be called by RBTreeDestroy */
/***********************************************************************/

static void TreeDestHelper(struct rb_tree* tree, struct rb_node* x)
{
	struct rb_node* nil=tree->nil;
	if (x != nil) {
		TreeDestHelper(tree,x->left);
		TreeDestHelper(tree,x->right);
		tree->DestroyKey(x->key);
		tree->DestroyInfo(x->info);
		rb_free(x);
	}
}


/***********************************************************************/
/*  FUNCTION:  RBTreeDestroy */
/**/
/*    INPUTS:  tree is the tree to destroy */
/**/
/*    OUTPUT:  none */
/**/
/*    EFFECT:  Destroys the key and frees memory */
/**/
/*    Modifies Input: tree */
/**/
/***********************************************************************/

void RBTreeDestroy(struct rb_tree* tree)
{
	TreeDestHelper(tree,tree->root->left);
	rb_free(tree->root);
	rb_free(tree->nil);
	rb_free(tree);
}

/***********************************************************************/
/*  FUNCTION:  RBExactQuery */
/**/
/*    INPUTS:  tree is the tree to print and key is a pointer to the key */
/*             we are searching for */
/**/
/*    OUTPUT:  returns the a node with key equal to key.  If there are */
/*             multiple nodes with key equal to key this function returns */
/*             the one highest in the tree */
/**/
/*    Modifies Input: none */
/**/
/***********************************************************************/

struct rb_node* RBExactQuery(struct rb_tree* tree, void* key)
{
	struct rb_node* x=tree->root->left;
	struct rb_node* nil=tree->nil;
	int compVal;
	if (x == nil) return(0);
	compVal=tree->Compare(x->key, key);
	while(0 != compVal) {/*assignemnt*/
		if (1 == compVal) { /* x->key > key */
			x=x->left;
		} else {
			x=x->right;
		}
		if ( x == nil) return(0);
		compVal=tree->Compare(x->key, key);
	}
	return(x);
}


/***********************************************************************/
/*  FUNCTION:  RBDeleteFixUp */
/**/
/*    INPUTS:  tree is the tree to fix and x is the child of the spliced */
/*             out node in RBTreeDelete. */
/**/
/*    OUTPUT:  none */
/**/
/*    EFFECT:  Performs rotations and changes colors to restore red-black */
/*             properties after a node is deleted */
/**/
/*    Modifies Input: tree, x */
/**/
/*    The algorithm from this function is from _Introduction_To_Algorithms_ */
/***********************************************************************/

static void RBDeleteFixUp(struct rb_tree* tree, struct rb_node* x)
{
	struct rb_node* root=tree->root->left;
	struct rb_node* w;

	while( (!x->red) && (root != x)) {
		if (x == x->parent->left) {
			w=x->parent->right;
			if (w->red) {
				w->red=0;
				x->parent->red=1;
				LeftRotate(tree,x->parent);
				w=x->parent->right;
			}
			if ( (!w->right->red) && (!w->left->red) ) {
				w->red=1;
				x=x->parent;
			} else {
				if (!w->right->red) {
					w->left->red=0;
					w->red=1;
					RightRotate(tree,w);
					w=x->parent->right;
				}
				w->red=x->parent->red;
				x->parent->red=0;
				w->right->red=0;
				LeftRotate(tree,x->parent);
				x=root; /* this is to exit while loop */
			}
		} else { /* the code below is has left and right switched from above */
			w=x->parent->left;
			if (w->red) {
				w->red=0;
				x->parent->red=1;
				RightRotate(tree,x->parent);
				w=x->parent->left;
			}
			if ( (!w->right->red) && (!w->left->red) ) {
				w->red=1;
				x=x->parent;
			} else {
				if (!w->left->red) {
					w->right->red=0;
					w->red=1;
					LeftRotate(tree,w);
					w=x->parent->left;
				}
				w->red=x->parent->red;
				x->parent->red=0;
				w->left->red=0;
				RightRotate(tree,x->parent);
				x=root; /* this is to exit while loop */
			}
		}
	}
	x->red=0;
}


/***********************************************************************/
/*  FUNCTION:  RBDelete */
/**/
/*    INPUTS:  tree is the tree to delete node z from */
/**/
/*    OUTPUT:  none */
/**/
/*    EFFECT:  Deletes z from tree and frees the key and info of z */
/*             using DestoryKey and DestoryInfo.  Then calls */
/*             RBDeleteFixUp to restore red-black properties */
/**/
/*    Modifies Input: tree, z */
/**/
/*    The algorithm from this function is from _Introduction_To_Algorithms_ */
/***********************************************************************/

void RBDelete(struct rb_tree* tree, struct rb_node* z)
{
	struct rb_node* y;
	struct rb_node* x;
	struct rb_node* nil=tree->nil;
	struct rb_node* root=tree->root;

	y= ((z->left == nil) || (z->right == nil)) ? z : TreeSuccessor(tree,z);
	x= (y->left == nil) ? y->right : y->left;
	if (root == (x->parent = y->parent)) {/* assignment of y->p to x->p is intentional */
		root->left=x;
	} else {
		if (y == y->parent->left) {
			y->parent->left=x;
		} else {
			y->parent->right=x;
		}
	}
	if (y != z) { /* y should not be nil in this case */
		/* y is the node to splice out and x is its child */

		if (!(y->red)) RBDeleteFixUp(tree,x);

		tree->DestroyKey(z->key);
		tree->DestroyInfo(z->info);
		y->left=z->left;
		y->right=z->right;
		y->parent=z->parent;
		y->red=z->red;
		z->left->parent=z->right->parent=y;
		if (z == z->parent->left) {
			z->parent->left=y;
		} else {
			z->parent->right=y;
		}
		rb_free(z);
	} else {
		tree->DestroyKey(y->key);
		tree->DestroyInfo(y->info);
		if (!(y->red)) RBDeleteFixUp(tree,x);
		rb_free(y);
	}
}


/***********************************************************************/
/*  FUNCTION:  RBDEnumerate */
/**/
/*    INPUTS:  tree is the tree to look for keys >= low */
/*             and <= high with respect to the Compare function */
/**/
/*    OUTPUT:  stack containing pointers to the nodes between [low,high] */
/**/
/*    Modifies Input: none */
/***********************************************************************/

void RBEnumerate(struct rb_tree* tree, void* low, void* high,
		void (*callback)(struct rb_node *node, void *pdata), void *pdata)
{
	struct rb_node* nil=tree->nil;
	struct rb_node* x=tree->root->left;
	struct rb_node* lastBest=nil;
	while(nil != x) {
		if ( 1 == (tree->Compare(x->key,high)) ) { /* x->key > high */
			x=x->left;
		} else {
			lastBest=x;
			x=x->right;
		}
	}
	while ((lastBest != nil) && (1 != tree->Compare(low,lastBest->key))) {
		callback(lastBest, pdata);
		lastBest=TreePredecessor(tree,lastBest);
	}
	return NULL;
}
