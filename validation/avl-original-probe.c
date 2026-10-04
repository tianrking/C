#include <assert.h>
#include <limits.h>
#ifdef NDEBUG
#error Assertions must remain active
#endif
#define main avl_interactive_main
#include "data_structures/binary_trees/avl_tree.c"
#undef main

static int validate(avlNode *node, long long lower, long long upper)
{
    if (node == NULL)
    {
        return -1;
    }
    assert(lower < node->key && node->key < upper);
    int left = validate(node->left, lower, node->key);
    int right = validate(node->right, node->key, upper);
    assert(left - right >= -1 && left - right <= 1);
    int height = 1 + (left > right ? left : right);
    assert(node->height == height);
    return height;
}

static void release(avlNode *node)
{
    if (node != NULL)
    {
        release(node->left);
        release(node->right);
        free(node);
    }
}

int main(int argc, char **argv)
{
    assert(argc == 2);
    static const int values[][5] = {
        {2, 1, 4, 5, 0}, {2, 1, 4, 3, 0}, {2, 1, 4, 3, 5},
        {4, 2, 5, 1, 0}, {4, 2, 5, 3, 0}, {4, 2, 5, 1, 3}};
    int which = atoi(argv[1]);
    assert(which >= 0 && which < 6);
    size_t count = (which % 3 == 2) ? 5 : 4;
    avlNode *root = NULL;
    for (size_t i = 0; i < count; ++i)
    {
        root = insert(root, values[which][i]);
        validate(root, LLONG_MIN, LLONG_MAX);
    }
    int removed = which < 3 ? 1 : 5;
    root = delete(root, removed);
    validate(root, LLONG_MIN, LLONG_MAX);
    assert(findNode(root, removed) == NULL);
    for (size_t i = 0; i < count; ++i)
    {
        if (values[which][i] != removed)
        {
            assert(findNode(root, values[which][i]) != NULL);
        }
    }
    release(root);
    return 0;
}
