/*
 * copyright (c) 2006 Michael Niedermayer <michaelni@gmx.at>
 *
 * This file is part of BVBase.
 *
 * BVBase is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * BVBase is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with BVBase; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file
 * A tree container.
 * @author Michael Niedermayer <michaelni@gmx.at>
 */

#ifndef BVUTIL_TREE_H
#define BVUTIL_TREE_H

#include "attributes.h"
#include "version.h"

/**
 * @addtogroup lavu_tree AVTree
 * @ingroup lavu_data
 *
 * Low-complexity tree container
 *
 * Insertion, removal, finding equal, largest which is smaller than and
 * smallest which is larger than, all have O(log n) worst-case complexity.
 * @{
 */


struct BVTreeNode;
extern const int bv_tree_node_size;

/**
 * Allocate an BVTreeNode.
 */
struct BVTreeNode *bv_tree_node_alloc(void);

/**
 * Find an element.
 * @param root a pointer to the root node of the tree
 * @param next If next is not NULL, then next[0] will contain the previous
 *             element and next[1] the next element. If either does not exist,
 *             then the corresponding entry in next is unchanged.
 * @return An element with cmp(key, elem) == 0 or NULL if no such element
 *         exists in the tree.
 */
void *bv_tree_find(const struct BVTreeNode *root, void *key,
                   int (*cmp)(void *key, const void *b), void *next[2]);

/**
 * Insert or remove an element.
 *
 * If *next is NULL, then the supplied element will be removed if it exists.
 * If *next is non-NULL, then the supplied element will be inserted, unless
 * it already exists in the tree.
 *
 * @param rootp A pointer to a pointer to the root node of the tree; note that
 *              the root node can change during insertions, this is required
 *              to keep the tree balanced.
 * @param key  pointer to the element key to insert in the tree
 * @param next Used to allocate and free BVTreeNodes. For insertion the user
 *             must set it to an allocated and zeroed object of at least
 *             bv_tree_node_size bytes size. bv_tree_insert() will set it to
 *             NULL if it has been consumed.
 *             For deleting elements *next is set to NULL by the user and
 *             bv_tree_insert() will set it to the BVTreeNode which was
 *             used for the removed element.
 *             This allows the use of flat arrays, which have
 *             lower overhead compared to many malloced elements.
 *             You might want to define a function like:
 *             @code
 *             void *tree_insert(struct BVTreeNode **rootp, void *key,
 *                               int (*cmp)(void *key, const void *b),
 *                               BVTreeNode **next)
 *             {
 *                 if (!*next)
 *                     *next = bv_mallocz(bv_tree_node_size);
 *                 return bv_tree_insert(rootp, key, cmp, next);
 *             }
 *             void *tree_remove(struct BVTreeNode **rootp, void *key,
 *                               int (*cmp)(void *key, const void *b, BVTreeNode **next))
 *             {
 *                 bv_freep(next);
 *                 return bv_tree_insert(rootp, key, cmp, next);
 *             }
 *             @endcode
 * @param cmp compare function used to compare elements in the tree
 * @return If no insertion happened, the found element; if an insertion or
 *         removal happened, then either key or NULL will be returned.
 *         Which one it is depends on the tree state and the implementation. You
 *         should make no assumptions that it's one or the other in the code.
 */
void *bv_tree_insert(struct BVTreeNode **rootp, void *key,
                     int (*cmp)(void *key, const void *b),
                     struct BVTreeNode **next);

void bv_tree_destroy(struct BVTreeNode *t);

/**
 * Apply enu(opaque, &elem) to all the elements in the tree in a given range.
 *
 * @param cmp a comparison function that returns < 0 for a element below the
 *            range, > 0 for a element above the range and == 0 for a
 *            element inside the range
 *
 * @note The cmp function should use the same ordering used to construct the
 *       tree.
 */
void bv_tree_enumerate(struct BVTreeNode *t, void *opaque,
                       int (*cmp)(void *opaque, void *elem),
                       int (*enu)(void *opaque, void *elem));

/**
 * @}
 */

#endif /* BVUTIL_TREE_H */
