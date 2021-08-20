#ifndef UTIL_RBTREE_H
#define UTIL_RBTREE_H

// the structure of rbtree node
typedef struct rbtree_node_t_ rbtree_node_t;
// the sturcture of rbtree
typedef struct rbtree_t_ rbtree_t;

struct rbtree_node_t_ {
	int key;
	rbtree_node_t *left;
	rbtree_node_t *right;
	rbtree_node_t *parent;
	unsigned char color;
	void *data;
};

// insert function pointer
typedef void (*rbtree_insert_p)(rbtree_t *root, rbtree_node_t *node);

struct rbtree_t_ {
	rbtree_node_t *root;        // the pointer of the root of the tree
	rbtree_node_t *sentinel;    // sentinel as black leaf node
	rbtree_insert_p insert;
};

/* macros */
#define rbtree_set_insert_func(tree, func) (tree)->insert = func

#define rbtree_red(node) ((node)->color = 1)
#define rbtree_black(node) ((node)->color = 0)
#define rbtree_is_red(node) ((node)->color)
#define rbtree_is_black(node) (!((node)->color))

// copy color of n2 to n1
#define rbtree_copy_color(n1, n2) (n1->color = n2->color)
// sentinel for black leaf node
#define rbtree_sentinel_init(node) rbtree_black(node)

#define rbtree_init_(tree, s, i) \
	rbtree_sentinel_init(s); \
	(tree)->root = s;        \
	(tree)->sentinel = s;    \
	(tree)->insert = i

// public methods
void rbtree_init(rbtree_t *tree, rbtree_node_t *s);
void rbtree_insert_value(rbtree_t *tree, rbtree_node_t *node);
void rbtree_insert(rbtree_t *tree, rbtree_node_t *node);
void rbtree_delete(rbtree_t *tree, rbtree_node_t *node);
rbtree_node_t *rbtree_find(rbtree_t *tree, int key);

/* get the minimum key of node in a subtree of the rbtree */
static inline rbtree_node_t *rbtree_get_min(rbtree_t *tree, rbtree_node_t *node)
{
	while (node->left != tree->sentinel) {
		node = node->left;
	}

	return node;
}

#endif
