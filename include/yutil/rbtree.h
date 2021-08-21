#ifndef UTIL_RBTREE_H
#define UTIL_RBTREE_H

typedef struct rbtree_node_t_ rbtree_node_t;
typedef struct rbtree_t_ rbtree_t;

struct rbtree_node_t_ {
	int key;
	unsigned char color;
	void *data;
	rbtree_node_t *left;
	rbtree_node_t *right;
	rbtree_node_t *parent;
};

struct rbtree_t_ {
	rbtree_node_t *root;        // the pointer of the root of the tree
	rbtree_node_t *sentinel;    // sentinel as black leaf node
	int (*compare)(void *, const void *);
	void (*destroy)(void *);
};

/* macros */
#define rbtree_set_compare_func(tree, func) (tree)->compare = func
#define rbtree_set_destroy_func(tree, func) (tree)->destroy = func

#define rbtree_red(node) ((node)->color = 1)
#define rbtree_black(node) ((node)->color = 0)
#define rbtree_is_red(node) ((node)->color)
#define rbtree_is_black(node) (!((node)->color))
#define rbtree_is_empty(tree) ((tree)->root == (tree)->sentinel)
// sentinel for black leaf node
#define rbtree_sentinel_init(node) rbtree_black(node)

// public methods
void rbtree_init(rbtree_t *tree, rbtree_node_t *sentinel);
void rbtree_insert_value(rbtree_t *tree, rbtree_node_t *node);
void rbtree_insert(rbtree_t *tree, rbtree_node_t *node);
void rbtree_delete(rbtree_t *tree, rbtree_node_t *node);
rbtree_node_t *rbtree_search_by_key(rbtree_t *tree, int key);
rbtree_node_t *rbtree_next(rbtree_t *tree, rbtree_node_t *node);
rbtree_node_t *rbtree_search_by_data(rbtree_t *tree, const void *keydata);
void *rbtree_get_data_by_data(rbtree_t *tree, const void *keydata);
void *rbtree_get_data_by_key(rbtree_t *tree, int key);
void rbtree_destory(rbtree_t *tree);

/* get the minimum key of node in a subtree of the rbtree */
rbtree_node_t *rbtree_get_min(rbtree_t *tree, rbtree_node_t *node);

#endif
