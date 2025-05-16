#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "wd-paths.h"
#include "error.h"

struct _tree_node {
	char *path;
	bool is_dir;
	int wd;
	int net_balance;
	struct _tree_node *left;
	struct _tree_node *right;
};

struct paths {
	struct _tree_node *root;
	size_t size;
};

typedef struct paths *paths_t;

paths_t
paths_create(void) {
	struct paths *start = malloc(sizeof(struct paths));
	*start = (struct paths){ NULL, 0 };
	return start;
}

void
_destroy_node(struct _tree_node *node)
{
	if (node == NULL)
		return;

	_destroy_node(node->left);
	_destroy_node(node->right);

	free(node->path);
	free(node);
	node = NULL;
}

void
paths_destroy(paths_t start)
{
	_destroy_node(start->root);
	
	free(start);
	start = NULL;
}

void
_rotate_left(struct _tree_node **node_ptr)
{
	if (*node_ptr == NULL || (*node_ptr)->right == NULL)
		PANIC(-1, "These nodes should never be null");

	struct _tree_node *parent = *node_ptr;
	struct _tree_node *pivot = parent->right;
	int pivot_balance = pivot->net_balance;

	parent->right = pivot->left;
	pivot->left = parent;
	*node_ptr = pivot;

	if (pivot_balance == 0) {
		parent->net_balance = 1;
		pivot->net_balance = -1;
	} else {
		parent->net_balance = 0;
		pivot->net_balance = 0;
	}
}

void
_rotate_right(struct _tree_node **node_ptr)
{
	if (*node_ptr == NULL || (*node_ptr)->left == NULL)
		PANIC(-1, "These nodes should never be null");

	struct _tree_node *parent = *node_ptr;
	struct _tree_node *pivot = parent->left;
	int pivot_balance = pivot->net_balance;

	parent->left = pivot->right;
	pivot->right = parent;
	*node_ptr = pivot;

	if (pivot_balance == 0) {
		parent->net_balance = -1;
		pivot->net_balance = 1;
	} else {
		parent->net_balance = 0;
		pivot->net_balance = 0;
	}
}

void
_rotate_left_right(struct _tree_node **node_ptr)
{
	struct _tree_node *parent = *node_ptr;
	struct _tree_node *left_child = parent->left;
	struct _tree_node *pivot = left_child->right;
	int pivot_balance = pivot->net_balance;
	
	_rotate_left(&(*node_ptr)->left);
	_rotate_right(node_ptr);
	
	if (pivot_balance == 1) {
		parent->net_balance = 0;
		left_child->net_balance = -1;
		pivot->net_balance = 0;
	} else if (pivot_balance == -1) {
		parent->net_balance = 1;
		left_child->net_balance = 0;
		pivot->net_balance = 0;
	} else {
		parent->net_balance = 0;
		left_child->net_balance = 0;
		pivot->net_balance = 0;
	}
}

void
_rotate_right_left(struct _tree_node **node_ptr)
{
	struct _tree_node *parent = *node_ptr;
	struct _tree_node *right_child = parent->right;
	struct _tree_node *pivot = right_child->left;
	int pivot_balance = pivot->net_balance;
	
	_rotate_right(&(*node_ptr)->right);
	_rotate_left(node_ptr);
	
	if (pivot_balance == 1) {
		parent->net_balance = -1;
		right_child->net_balance = 0;
		pivot->net_balance = 0;
	} else if (pivot_balance == -1) {
		parent->net_balance = 0;
		right_child->net_balance = 1;
		pivot->net_balance = 0;
	} else {
		parent->net_balance = 0;
		right_child->net_balance = 0;
		pivot->net_balance = 0;
	}
}

int
_add_to_node(struct _tree_node **node_ptr, struct _tree_node *new)
{
	int height_increased = 0;

	if (new->wd < (*node_ptr)->wd) {
		if ((*node_ptr)->left == NULL) {
			(*node_ptr)->left = new;
			(*node_ptr)->net_balance--;

			return ((*node_ptr)->net_balance == -1);
		}

		height_increased = _add_to_node(&(*node_ptr)->left, new);
		
		if (height_increased) {
			(*node_ptr)->net_balance--;

			if ((*node_ptr)->net_balance < -1) {
				if ((*node_ptr)->left->net_balance > 0)
					_rotate_left_right(node_ptr);
				else
					_rotate_right(node_ptr);

				return 0;
			}
			
			return ((*node_ptr)->net_balance == -1);
		}
	}
	else if (new->wd > (*node_ptr)->wd) {
		if ((*node_ptr)->right == NULL) {
			(*node_ptr)->right = new;
			(*node_ptr)->net_balance++;

			return ((*node_ptr)->net_balance == 1);
		}

		height_increased = _add_to_node(&(*node_ptr)->right, new);
		
		if (height_increased) {
			(*node_ptr)->net_balance++;
			
			if ((*node_ptr)->net_balance > 1) {
				if ((*node_ptr)->right->net_balance < 0)
					_rotate_right_left(node_ptr);
				else
					_rotate_left(node_ptr);

				return 0;
			}
			
			return ((*node_ptr)->net_balance == 1);
		}
	}
	else {
		PANIC(-1, "This point shouldn't be reached");
	}
	
	return 0;
}

void
paths_add(paths_t start, int wd, char *path, bool is_dir)
{
	struct _tree_node *new = malloc(sizeof(struct _tree_node));
	*new = (struct _tree_node){ path, is_dir, wd, 0, NULL, NULL };

	if (start->size == 0)
		start->root = new;
	else
		_add_to_node(&start->root, new);

	start->size++;
}

void
paths_remove(paths_t start, int wd)
{
	//_remove_node(&start->root, wd);
	start->size--;
}

struct _tree_node **
_node_search(struct _tree_node **head, int target)
{
	if (head == NULL)
		return NULL;

	if ((*head)->wd == target)
		return head;

	if (target < (*head)->wd)
		return _node_search(&(*head)->left, target);
	else
		return _node_search(&(*head)->right, target);
}

const char *
paths_retrieve(paths_t start, int wd)
{
	struct _tree_node **found = _node_search(&(start->root), wd);
	if (found == NULL)
		return NULL;

	return (*found)->path;
}

void
_print_tree(struct _tree_node *node, size_t depth)
{
	if (node == NULL)
		return;

	_print_tree(node->right, depth + 1);

	for (size_t i = 0; i < depth; i++)
		printf("|\t");

	printf("%d: %s %d\n", node->wd, node->path, node->net_balance);

	_print_tree(node->left, depth + 1);
}

void
paths_debug(paths_t start)
{
	printf("___ START ___\n");
	_print_tree(start->root, 0);
	printf("^^^^ END ^^^^\n");
}
