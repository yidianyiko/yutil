#include <stdio.h>
#include "../include/yutil/rbtree.h"

static inline void rbtree_clear(rbtree_node_t *node)
{
	node->left = NULL;
	node->right = NULL;
	node->parent = NULL;
	node->data = NULL;
	node->key = 0;
}
/** 红黑树查找结点 */
static rbtree_node_t *rbtree_search_(rbtree_t *tree, int key,
				     const void *keydata)
{
	rbtree_node_t *node = tree->root;
	int ret;

	while (node != tree->sentinel) {
		if (tree->compare && keydata) {
			ret = tree->compare(node->data, keydata);
			if (ret == 0) {
				return node;
			}
			node = (ret < 0) ? node->right : node->left;
			continue;
		}
		if (key == node->key) {
			return node;
		}

		node = (key < node->key) ? node->left : node->right;
	}
	return NULL;
}

// repalce old_node in the tree by new_node
static inline void rbtree_replace_(rbtree_t *tree, rbtree_node_t *old_node,
				   rbtree_node_t *new_node)
{
	new_node->parent = old_node->parent;

	// repalce
	if (old_node == tree->root) {
		tree->root = new_node;
	} else if (old_node == old_node->parent->left) {
		old_node->parent->left = new_node;
	} else {
		old_node->parent->right = new_node;
	}

	old_node->parent = new_node;
}

/**
 * left rotate
 *
 *      node              temp
 *      / \       ==>     /  \
 *     a  temp         node  y
 *    / \              / \
 *   b   y            a   b
 */
static inline void rbtree_left_rotate(rbtree_t *tree, rbtree_node_t *node)
{
	rbtree_node_t *temp = node->right;

	// replace temp with left[temp]
	node->right = temp->left;
	// if left[temp] is not NIL, then it has a parent
	if (temp->left != tree->sentinel) {
		temp->left->parent = node;
	}

	// replace node with temp
	rbtree_replace_(tree, node, temp);
	temp->left = node;
}

/**
 * right rotate
 *
 *        node           temp
 *        /  \           /  \
 *      temp  y   ==>   a   node
 *      / \            / \
 *     a   b          b   y
 */
static inline void rbtree_right_rotate(rbtree_t *tree, rbtree_node_t *node)
{
	rbtree_node_t *temp = node->left;

	node->left = temp->right;
	if (temp->right != tree->sentinel) {
		temp->right->parent = node;
	}

	rbtree_replace_(tree, node, temp);
	temp->right = node;
}

static void rbtree_insert_rebalance(rbtree_t *tree, rbtree_node_t *node)
{
	rbtree_node_t *gparent;
	rbtree_node_t *uncle;
	while (node != tree->root && rbtree_is_red(node->parent)) {
		gparent = node->parent->parent;
		if (node->parent == gparent->left) {
			uncle = gparent->right;
			// case 1: uncle is red, then update node to gparent
			// case 2: uncle is balck or NIL
			if (rbtree_is_red(uncle)) {
				rbtree_black(node->parent);
				rbtree_black(uncle);
				rbtree_red(gparent);
				node = gparent;

			} else {
				// ensue node is the left kid, make gparent,
				// parent, node in one line
				if (node == node->parent->right) {
					node = node->parent;
					rbtree_left_rotate(tree, node);
					gparent = node->parent->parent;
				}
				// case 2 -> case 1
				rbtree_black(node->parent);
				rbtree_red(gparent);
				rbtree_right_rotate(tree, gparent);
			}
		} else {
			uncle = node->parent->parent->left;

			if (rbtree_is_red(uncle)) {
				rbtree_black(node->parent);
				rbtree_black(uncle);
				rbtree_red(gparent);
				node = gparent;
			} else {
				if (node == node->parent->left) {
					node = node->parent;
					rbtree_right_rotate(tree, node);
					gparent = node->parent->parent;
				}

				rbtree_black(node->parent);
				rbtree_red(gparent);
				rbtree_left_rotate(tree, gparent);
			}
		}
	}

	// ensure the root being black, useful when new node is root
	rbtree_black(tree->root);
}

static void rbtree_delete_rebalance(rbtree_t *tree, rbtree_node_t *node)
{
	rbtree_node_t *brother;
	while (node != tree->root && rbtree_is_black(node)) {
		if (node == node->parent->left) {
			brother = node->parent->right;

			if (rbtree_is_red(brother)) {
				rbtree_black(brother);
				rbtree_red(node->parent);
				rbtree_left_rotate(tree, node->parent);
				brother = node->parent->right;
			}

			if (rbtree_is_black(brother->left) &&
			    rbtree_is_black(brother->right)) {
				rbtree_red(brother);
				node = node->parent;

			} else {
				if (rbtree_is_black(brother->right)) {
					rbtree_black(brother->left);
					rbtree_red(brother);
					rbtree_right_rotate(tree, brother);
					brother = node->parent->right;
				}
				brother->color = node->parent->color;
				rbtree_black(node->parent);
				rbtree_black(brother->right);
				rbtree_left_rotate(tree, node->parent);
				node = tree->root;
			}

		} else {
			brother = node->parent->left;

			if (rbtree_is_red(brother)) {
				rbtree_black(brother);
				rbtree_red(node->parent);
				rbtree_right_rotate(tree, node->parent);
				brother = node->parent->left;
			}

			if (rbtree_is_black(brother->left) &&
			    rbtree_is_black(brother->right)) {
				rbtree_red(brother);
				node = node->parent;

			} else {
				if (rbtree_is_black(brother->left)) {
					rbtree_black(brother->right);
					rbtree_red(brother);
					rbtree_left_rotate(tree, brother);
					brother = node->parent->left;
				}

				brother->color = node->parent->color;
				rbtree_black(node->parent);
				rbtree_black(brother->left);
				rbtree_right_rotate(tree, node->parent);
				node = tree->root;
			}
		}
	}

	rbtree_black(node);
}

void rbtree_insert_value(rbtree_t *tree, rbtree_node_t *node)
{
	rbtree_node_t **temp = &tree->root;
	rbtree_node_t *parent = NULL;

	while (*temp != tree->sentinel) {
		parent = *temp;
		temp =
		    (node->key < parent->key) ? &parent->left : &parent->right;
	}

	*temp = node;
	node->parent = parent;
	node->left = tree->sentinel;
	node->right = tree->sentinel;
	rbtree_red(node);
}

void rbtree_init(rbtree_t *tree, rbtree_node_t *sentinel)
{
	rbtree_sentinel_init(sentinel);
	tree->root = sentinel;
	tree->sentinel = sentinel;
	tree->compare = NULL;
	tree->destroy = NULL;
}

void rbtree_insert(rbtree_t *tree, rbtree_node_t *node)
{
	rbtree_node_t *sentinel = tree->sentinel;
	rbtree_node_t *temp = rbtree_search_(tree, node->key, node->data);

	if (temp)
		return;

	// if tree is empty
	if (rbtree_is_empty(tree)) {
		node->parent = NULL;
		node->left = sentinel;
		node->right = sentinel;
		rbtree_black(node);
		tree->root = node;

		return;
	}

	rbtree_insert_value(tree, node);

	/* re-balance tree */
	rbtree_insert_rebalance(tree, node);
}

void rbtree_delete(rbtree_t *tree, rbtree_node_t *node)
{
	rbtree_node_t *subst;
	rbtree_node_t *temp;
	rbtree_node_t *sentinel = tree->sentinel;
	int red;
	/* a binary tree delete */
	if (rbtree_is_empty(tree) || node == sentinel) {
		return;
	}

	if (node->left == sentinel) {
		temp = node->right;
		subst = node;

	} else if (node->right == sentinel) {
		temp = node->left;
		subst = node;

	} else {
		subst = rbtree_get_min(tree, node->right);
		temp = subst->right;
	}

	if (subst == tree->root) {
		tree->root = temp;
		rbtree_black(temp);
		if (tree->destroy && node->data) {
			tree->destroy(node->data);
		}
		rbtree_clear(node);

		return;
	}

	red = rbtree_is_red(subst);

	if (subst == subst->parent->left) {
		subst->parent->left = temp;

	} else {
		subst->parent->right = temp;
	}

	// if subst is the node to delete
	if (subst == node) {
		temp->parent = subst->parent;

	} else {
		if (subst->parent == node) {
			temp->parent = subst;

		} else {
			temp->parent = subst->parent;
		}

		subst->left = node->left;
		subst->right = node->right;
		subst->parent = node->parent;
		subst->color = node->color;

		if (node == tree->root) {
			tree->root = subst;

		} else {
			if (node == node->parent->left) {
				node->parent->left = subst;
			} else {
				node->parent->right = subst;
			}
		}

		if (subst->left != sentinel) {
			subst->left->parent = subst;
		}

		if (subst->right != sentinel) {
			subst->right->parent = subst;
		}
	}

	if (tree->destroy && node->data) {
		tree->destroy(node->data);
	}
	rbtree_clear(node);

	if (!red) {
		rbtree_delete_rebalance(tree, temp);
	}

	return;
}

rbtree_node_t *rbtree_next(rbtree_t *tree, rbtree_node_t *node)
{
	rbtree_node_t *parent;
	if (node == tree->root) {
		return NULL;
	}
	if (node->right != tree->sentinel) {
		return rbtree_get_min(tree, node->right);
	}
	while ((parent = node->parent) && node == parent->right) {
		node = parent;
	}
	return parent;
}

static i = 0;
static void rbtree_node_destroy(rbtree_t *tree, rbtree_node_t *node)
{
	if (node == tree->sentinel) {
		return;
	}
	rbtree_node_destroy(tree, node->left);
	rbtree_node_destroy(tree, node->right);
	if (tree->destroy && node->data) {
		tree->destroy(node->data);
	}
	if (node->parent) {
		if (node->parent->left == node) {
			node->parent->left = tree->sentinel;
		} else {
			node->parent->right = tree->sentinel;
		}
	}
	rbtree_clear(node);
	// somthing wrong
	// free(node)
}

void rbtree_destory(rbtree_t *tree)
{
	rbtree_node_destroy(tree, tree->root);
	tree->root = NULL;
}
rbtree_node_t *rbtree_get_min(rbtree_t *tree, rbtree_node_t *node)
{
	while (node->left != tree->sentinel) {
		node = node->left;
	}

	return node;
}

rbtree_node_t *rbtree_search_by_key(rbtree_t *tree, int key)
{
	return rbtree_search_(tree, key, NULL);
}
rbtree_node_t *rbtree_search_by_data(rbtree_t *tree, const void *keydata)
{
	return rbtree_search_(tree, 0, keydata);
}

void *rbtree_get_data_by_data(rbtree_t *tree, const void *keydata)
{
	rbtree_node_t *node;
	node = rbtree_search_by_data(tree, keydata, tree->compare);
	if (node) {
		return node->data;
	}
	return NULL;
}

void *rbtree_get_data_by_key(rbtree_t *tree, int key)
{
	rbtree_node_t *node;
	node = rbtree_search_by_key(tree, key);
	if (node) {
		return node->data;
	}
	return NULL;
}