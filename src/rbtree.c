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

static void rbtree_insert_rebalance(rbtree_t *tree, rbtree_node_t *node)
{
	rbtree_node_t *gparent;
	rbtree_node_t *uncle;
	while (node != tree->root && rbtree_is_red(node->parent)) {
		gparent = node->parent->parent;
		if (node->parent == gparent->left) {
			uncle = gparent->right;

			if (rbtree_is_red(uncle)) {
				rbtree_black(node->parent);
				rbtree_black(uncle);
				rbtree_red(gparent);
				node = gparent;

			} else {
				if (node == node->parent->right) {
					node = node->parent;
					rbtree_left_rotate(tree, node);
					gparent = node->parent->parent;
				}

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

	rbtree_black(tree->root);
}

void rbtree_insert(rbtree_t *tree, rbtree_node_t *node)
{
	rbtree_node_t *sentinel;

	/* a binary tree insert */

	sentinel = tree->sentinel;

	if (tree->root == sentinel) {
		node->parent = NULL;
		node->left = sentinel;
		node->right = sentinel;
		rbtree_black(node);
		tree->root = node;

		return;
	}

	tree->insert(tree, node);

	/* re-balance tree */
	rbtree_insert_rebalance(tree, node);
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

static void rbtree_delete_rebalance(rbtree_t *tree, rbtree_node_t *temp)
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

	rbtree_black(temp);
}

void rbtree_delete(rbtree_t *tree, rbtree_node_t *node)
{
	int red;
	rbtree_node_t *sentinel, *subst, *temp;

	/* a binary tree delete */

	sentinel = tree->sentinel;

	if (node->left == sentinel) {
		temp = node->right;
		subst = node;

	} else if (node->right == sentinel) {
		temp = node->left;
		subst = node;

	} else {
		subst = rbtree_min(tree, node->right);
		temp = subst->right;
	}

	if (subst == tree->root) {
		tree->root = temp;
		rbtree_black(temp);

		/* DEBUG stuff */
		node->left = NULL;
		node->right = NULL;
		node->parent = NULL;
		node->key = 0;

		return;
	}

	red = rbtree_is_red(subst);

	if (subst == subst->parent->left) {
		subst->parent->left = temp;

	} else {
		subst->parent->right = temp;
	}

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
		rbtree_copy_color(subst, node);

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

	/* DEBUG stuff */
	node->left = NULL;
	node->right = NULL;
	node->parent = NULL;
	node->key = 0;

	if (red) {
		return;
	}

	/* a delete fixup */
	rbtree_delete_rebalance(tree, temp);
}

rbtree_node_t *rbtree_next(rbtree_t *tree, rbtree_node_t *node)
{
	rbtree_node_t *root, *sentinel, *parent;

	sentinel = tree->sentinel;

	if (node->right != sentinel) {
		return rbtree_min(tree, node->right);
	}

	root = tree->root;

	for (;;) {
		parent = node->parent;

		if (node == root) {
			return NULL;
		}

		if (node == parent->left) {
			return parent;
		}

		node = parent;
	}
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