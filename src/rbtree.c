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

static inline void rbtree_insert_rebalance(rbtree_t *tree, rbtree_node_t *node)
{
	rbtree_node_t *gparent = node->parent->parent;
	while (rbtree_is_red(node->parent)) {
		if (node->parent == gparent->left) {
			// case one: the bro of node's parent is red
			if (rbtree_is_red(gparent->right)) {
				rbtree_black(node->parent);
				rbtree_black(gparent->right);
				rbtree_red(gparent);
				node = gparent;

			}
			// case two: node's uncle is black
			else {
				if (node == node->parent->right) {
					node = node->parent;
					rbtree_left_rotate(tree, node);
				}
				// case 2 -> case 1
				rbtree_black(node->parent);
				rbtree_red(gparent);
				rbtree_right_rotate(tree, gparent);
			}
		} else {
			if (rbtree_is_red(gparent->left)) {
				rbtree_black(node->parent);
				rbtree_black(gparent->left);
				rbtree_red(gparent);
				node = gparent;

			} else {
				if (node == node->parent->left) {
					node = node->parent;
					rbtree_left_rotate(tree, node);
				}
				rbtree_black(node->parent);
				rbtree_red(gparent);
				rbtree_left_rotate(tree, gparent);
			}
		}
	}
	rbtree_black(tree->root);
}

static inline void rbtree_delete_rebalance(rbtree_t *tree, rbtree_node_t *temp)
{
	rbtree_node_t *brother;
	while (temp != tree->root && rbtree_is_black(temp)) {
		if (temp == temp->parent->left) {
			brother = temp->parent->right;

			if (rbtree_is_red(brother)) {
				rbtree_black(brother);
				rbtree_red(temp->parent);
				rbtree_left_rotate(tree, temp->parent);
				brother = temp->parent->right;
			}

			if (rbtree_is_black(brother->left) &&
			    rbtree_is_black(brother->right)) {
				rbtree_red(brother);
				temp = temp->parent;

			} else {
				if (rbtree_is_black(brother->right)) {
					rbtree_black(brother->left);
					rbtree_red(brother);
					rbtree_right_rotate(tree, brother);
					brother = temp->parent->right;
				}

				rbtree_copy_color(brother, temp->parent);
				rbtree_black(temp->parent);
				rbtree_black(brother->right);
				rbtree_left_rotate(tree, temp->parent);
				temp = tree->root;
			}

		} else {
			brother = temp->parent->left;

			if (rbtree_is_red(brother)) {
				rbtree_black(brother);
				rbtree_red(temp->parent);
				rbtree_right_rotate(tree, temp->parent);
				brother = temp->parent->left;
			}

			if (rbtree_is_black(brother->left) &&
			    rbtree_is_black(brother->right)) {
				rbtree_red(brother);
				temp = temp->parent;

			} else {
				if (rbtree_is_black(brother->left)) {
					rbtree_black(brother->right);
					rbtree_red(brother);
					rbtree_left_rotate(tree, brother);
					brother = temp->parent->left;
				}

				rbtree_copy_color(brother, temp->parent);
				rbtree_black(temp->parent);
				rbtree_black(brother->left);
				rbtree_right_rotate(tree, temp->parent);
				temp = tree->root;
			}
		}
	}
}

void rbtree_init(rbtree_t *tree, rbtree_node_t *s)
{
	rbtree_sentinel_init(s);
	(tree)->root = s;
	(tree)->sentinel = s;
	(tree)->insert = NULL;
}

static void rbtree_insert_value(rbtree_t *tree, rbtree_node_t *node)
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

void rbtree_insert(rbtree_t *tree, rbtree_node_t *node)
{
	rbtree_node_t *sentinel = tree->sentinel;

	if ((tree)->root == sentinel) {
		tree->root = node;
		node->parent = sentinel;
		node->right = sentinel;
		node->left = sentinel;
		rbtree_black(node);

		return;
	}
	rbtree_insert_value(tree, node);
	rbtree_insert_rebalance(tree, node);
}
void rbtree_delete(rbtree_t *tree, rbtree_node_t *node)
{
	rbtree_node_t *sentinel = tree->sentinel;
	rbtree_node_t *cover = NULL;
	rbtree_node_t *temp = NULL;
	int red = rbtree_is_red(node);
	int is_root = (node == tree->root);
	if (node->left == sentinel) {
		temp = node->right;
		cover = node;

	} else if (node->right == sentinel) {
		temp = node->left;
		cover = node;

	} else {
		cover = rbtree_get_min(tree, node->right);
		temp = cover->right;
		is_root = 0;
		red = rbtree_is_red(node);
	}

	if (is_root) {
		tree->root = temp;
		rbtree_black(temp);

		/* DEBUG stuff */
		node->left = NULL;
		node->right = NULL;
		node->parent = NULL;
		node->key = 0;
		node->data = NULL;
		return;
	}

	if (cover == cover->parent->left) {
		cover->parent->left = temp;
	} else {
		cover->parent->right = temp;
	}

	if (cover == node) {
		temp->parent = cover->parent;
	} else {
		if (cover->parent == node) {
			temp->parent = cover;
		} else {
			temp->parent = cover->parent;
		}
		cover->parent = node->parent;
		cover->left = node->left;
		cover->right = node->right;
		rbtree_copy_color(cover, node);

		if (node == tree->root) {
			tree->root = cover;
		} else {
			if (node == node->parent->left) {
				node->parent->left = cover;
			} else {
				node->parent->right = cover;
			}
		}
		if (cover->left != sentinel) {
			cover->left->parent = cover;
		}
		if (cover->right != sentinel) {
			cover->right->parent = cover;
		}
	}

	node->key = 0;
	node->parent = NULL;
	node->left = NULL;
	node->right = NULL;
	node->data = NULL;

	if (red) {
		return;
	}

	/* a delete fixup */
	rbtree_delete_rebalance(tree, temp);
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