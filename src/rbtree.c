#include <stdio.h>
#include "../include/yutil/rbtree.h"

// repalce old_node in the tree by new_node
static inline void rbtree_replace_(rbtree_t *tree, rbtree_node_t *old_node,
				   rbtree_node_t *new_node)
{
	new_node->parent = old_node->parent;

	// case 1: only one node
	if (old_node == tree->root) {
		tree->root = new_node;
	} else if (old_node == old_node->parent->left) {
		old_node->parent->left = new_node;
	} else {
		old_node->parent->right = new_node;
	}

	old_node->parent = new_node;
}

static inline void rbtree_left_rotate(rbtree_t *tree, rbtree_node_t *node)
{
	rbtree_node_t *temp = node->right;

	node->right = temp->left;
	if (temp->left != tree->sentinel) {
		temp->left->parent = node;
	}

	rbtree_replace_(tree, node, temp);
	temp->left = node;
}

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

static rbtree_insert_rebalance(rbtree_t *tree, rbtree_node_t *node)
{
	while (rbtree_is_red(node->parent)) {
		if (node->parent == node->parent->parent->left) {
			// case one: the bro of node's parent is red
			if (rbtree_is_red(node->parent->parent->right)) {
				rbtree_black(node->parent);
				rbtree_black(node->parent->parent->right);
				rbtree_red(node->parent->parent);
				node = node->parent->parent;

				/* Then we can consider the whole subtree */
				/* which is represented by the new "node" as the
				 * "node" before */
				/* and keep looping till "node" become the root.
				 */

			}
			// case two: node's uncle is black
			else {
				if (node == node->parent->right) {
					node = node->parent;
					rbtree_left_rotate(tree, node);
				}
				// case 2 -> case 1
				rbtree_black(node->parent);
				rbtree_red(node->parent->parent);
				rbtree_right_rotate(tree, node->parent->parent);
			}
		} else {
			if (rbtree_is_red(node->parent->parent->left)) {
				rbtree_black(node->parent);
				rbtree_black(node->parent->parent->left);
				rbtree_red(node->parent->parent);
				node = node->parent->parent;

			} else {
				if (node == node->parent->left) {
					node = node->parent;
					rbtree_left_rotate(tree, node);
				}
				rbtree_black(node->parent);
				rbtree_red(node->parent->parent);
				rbtree_left_rotate(tree, node->parent->parent);
			}
		}
	}
	rbtree_black(tree->root);
}

static void rbtree_delete_rebalance(rbtree_t *tree, rbtree_node_t *node)
{
	rbtree_node_t *brother = NULL;
	rbtree_node_t *gparent = NULL;
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
				rbtree_copy_color(brother, node->parent);
				rbtree_black(node->parent);
				rbtree_black(brother->right);
				rbtree_right_rotate(tree, node->parent);
				node = tree->root;
			}
		} else {
			brother = node->parent->left;
			if (rbtree_is_red(brother)) {
				rbtree_black(brother);
				rbtree_red(node->parent);
				rbtree_left_rotate(tree, node->parent);
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
					rbtree_right_rotate(tree, brother);
					brother = node->parent->left;
				}
				rbtree_copy_color(brother, node->parent);
				rbtree_black(node->parent);
				rbtree_black(brother->left);
				rbtree_left_rotate(tree, node->parent);
				node = tree->root;
			}
		}
	}
	rbtree_black(node);
}

void rbtree_init(rbtree_t *tree, rbtree_node_t *s)
{
	rbtree_sentinel_init(s);
	(tree)->root = s;
	(tree)->sentinel = s;
	(tree)->insert = NULL;
}

void rbtree_insert_value(rbtree_t *tree, rbtree_node_t *node)
{
	rbtree_node_t **temp = &tree->root;
	rbtree_node_t *parent;

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

void rbtree_insert(rbtree_t *tree, rbtree_node_t *node)
{
	rbtree_node_t *sentinel = tree->sentinel;

	if (rbtree_is_empty(tree)) {
		tree.node = node;
		node->parent = sentinel;
		node->right = sentinel;
		node->left = sentinel;
		rbtree_black(node);

		return;
	}
	tree->insert(tree, node);
	rbtree_insert_rebalance(tree, node);
}
void rbtree_delete(rbtree_t *tree, rbtree_node_t *node)
{
	rbtree_node_t *sentinel = tree->sentinel;
	rbtree_node_t **ptr_to_node = NULL;
	rbtree_node_t *cover = NULL;
	int loss_red = rbtree_is_red(node);
	int is_root = (node == tree->root);

	if (node->left == sentinel) {
		cover = node->right;
	} else if (node->right == sentinel) {
		cover = node->left;
	} else {
		cover = rbtree_subtree_min(node->right, sentinel);
		node->key = cover->key;
		node->data = cover->data;
		node = cover;
		loss_red = rbtree_is_red(node);
		is_root = 0;
		cover = node->right;
	}

	if (is_root) {
		tree->root = cover;
	} else {
		if (node == node->parent->left) {
			node->parent->left = cover;
		} else {
			node->parent->right = cover;
		}
	}
	cover->parent = node->parent;
	if (cover != sentinel) {
		cover->left = node->left;
		cover->right = node->right;
		rbtree_copy_color(cover, node);
	}

	node->key = -1;
	node->parent = NULL;
	node->left = NULL;
	node->right = NULL;
	node->data = NULL;

	if (loss_red) {
		return;
	}

	rbtree_delete_rebalance(tree, cover);
}
rbtree_node_t *rbtree_find(rbtree_t *tree, int key)
{
	rbtree_node_t *temp = tree->root;
	int step_cnt = 0;

	while (temp != tree->sentinel) {
		if (key == temp->key) {
			return temp;
		}

		temp = (key < temp->key) ? temp->left : temp->right;
	}

	return NULL;
}