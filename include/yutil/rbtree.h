/*
 * rbtree.h -- red-black tree
 *
 * Copyright (c) 2021, Li Zihao <yidianyiko@foxmail.com> All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of LCUI nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
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
