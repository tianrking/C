#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

struct AVLnode
{
    int key;
    struct AVLnode *left;
    struct AVLnode *right;
    int height;
};
typedef struct AVLnode avlNode;

int max(int a, int b) { return (a > b) ? a : b; }

avlNode *newNode(int key)
{
    avlNode *node = (avlNode *)malloc(sizeof(avlNode));

    if (node == NULL)
        printf("!! Out of Space !!\n");
    else
    {
        node->key = key;
        node->left = NULL;
        node->right = NULL;
        node->height = 0;
    }

    return node;
}

int nodeHeight(avlNode *node)
{
    if (node == NULL)
        return -1;
    else
        return (node->height);
}

int heightDiff(avlNode *node)
{
    if (node == NULL)
        return 0;
    else
        return (nodeHeight(node->left) - nodeHeight(node->right));
}

/* Returns the node with min key in the left subtree*/
avlNode *minNode(avlNode *node)
{
    avlNode *temp = node;

    while (temp->left != NULL) temp = temp->left;

    return temp;
}

void printAVL(avlNode *node, int level)
{
    int i;
    if (node != NULL)
    {
        printAVL(node->right, level + 1);
        printf("\n\n");

        for (i = 0; i < level; i++) printf("\t");

        printf("%d", node->key);

        printAVL(node->left, level + 1);
    }
}

avlNode *rightRotate(avlNode *z)
{
    avlNode *y = z->left;
    avlNode *T3 = y->right;

    y->right = z;
    z->left = T3;

    z->height = (max(nodeHeight(z->left), nodeHeight(z->right)) + 1);
    y->height = (max(nodeHeight(y->left), nodeHeight(y->right)) + 1);

    return y;
}

avlNode *leftRotate(avlNode *z)
{
    avlNode *y = z->right;
    avlNode *T3 = y->left;

    y->left = z;
    z->right = T3;

    z->height = (max(nodeHeight(z->left), nodeHeight(z->right)) + 1);
    y->height = (max(nodeHeight(y->left), nodeHeight(y->right)) + 1);

    return y;
}

avlNode *LeftRightRotate(avlNode *z)
{
    z->left = leftRotate(z->left);

    return (rightRotate(z));
}

avlNode *RightLeftRotate(avlNode *z)
{
    z->right = rightRotate(z->right);

    return (leftRotate(z));
}

avlNode *insert(avlNode *node, int key)
{
    if (node == NULL)
        return (newNode(key));

    /*Binary Search Tree insertion*/

    if (key < node->key)
        node->left =
            insert(node->left, key); /*Recursive insertion in L subtree*/
    else if (key > node->key)
        node->right =
            insert(node->right, key); /*Recursive insertion in R subtree*/

    /* Node  Height as per the AVL formula*/
    node->height = (max(nodeHeight(node->left), nodeHeight(node->right)) + 1);

    /*Checking for the balance condition*/
    int balance = heightDiff(node);

    /*Left Left */
    if (balance > 1 && key < (node->left->key))
        return rightRotate(node);

    /*Right Right */
    if (balance < -1 && key > (node->right->key))
        return leftRotate(node);

    /*Left Right */
    if (balance > 1 && key > (node->left->key))
    {
        node = LeftRightRotate(node);
    }

    /*Right Left */
    if (balance < -1 && key < (node->right->key))
    {
        node = RightLeftRotate(node);
    }

    return node;
}

avlNode *delete(avlNode *node, int queryNum)
{
    if (node == NULL)
        return node;

    if (queryNum < node->key)
        node->left =
            delete (node->left, queryNum); /*Recursive deletion in L subtree*/
    else if (queryNum > node->key)
        node->right =
            delete (node->right, queryNum); /*Recursive deletion in R subtree*/
    else
    {
        /*Single or No Children*/
        if ((node->left == NULL) || (node->right == NULL))
        {
            avlNode *temp = node->left ? node->left : node->right;

            /* No Children*/
            if (temp == NULL)
            {
                temp = node;
                node = NULL;
            }
            else /*Single Child : copy data to the parent*/
                *node = *temp;

            free(temp);
        }
        else
        {
            /*Two Children*/

            /*Get the smallest key in the R subtree*/
            avlNode *temp = minNode(node->right);
            node->key = temp->key; /*Copy that to the root*/
            node->right =
                delete (node->right,
                        temp->key); /*Delete the smallest in the R subtree.*/
        }
    }

    /*single node in tree*/
    if (node == NULL)
        return node;

    /*Update height*/
    node->height = (max(nodeHeight(node->left), nodeHeight(node->right)) + 1);

    int balance = heightDiff(node);

    /*Left Left */
    if ((balance > 1) && (heightDiff(node->left) >= 0))
        return rightRotate(node);

    /*Left Right */
    if ((balance > 1) && (heightDiff(node->left) < 0))
    {
        node = LeftRightRotate(node);
    }

    /*Right Right */
    if ((balance < -1) && (heightDiff(node->right) <= 0))
        return leftRotate(node);

    /*Right Left */
    if ((balance < -1) && (heightDiff(node->right) > 0))
    {
        node = RightLeftRotate(node);
    }

    return node;
}

avlNode *findNode(avlNode *node, int queryNum)
{
    if (node != NULL)
    {
        if (queryNum < node->key)
            node = findNode(node->left, queryNum);
        else if (queryNum > node->key)
            node = findNode(node->right, queryNum);
    }

    return node;
}

void printPreOrder(avlNode *node)
{
    if (node == NULL)
        return;

    printf("  %d  ", (node->key));
    printPreOrder(node->left);
    printPreOrder(node->right);
}

void printInOrder(avlNode *node)
{
    if (node == NULL)
        return;
    printInOrder(node->left);
    printf("  %d  ", (node->key));
    printInOrder(node->right);
}

void printPostOrder(avlNode *node)
{
    if (node == NULL)
        return;
    printPostOrder(node->left);
    printPostOrder(node->right);
    printf("  %d  ", (node->key));
}

/** Verify ordering, stored heights, and balance independently of tree helpers.
 */
static int assert_avl_invariants(avlNode *node, long long lower,
                                 long long upper, size_t *count)
{
    if (node == NULL)
    {
        return -1;
    }
    assert(lower < node->key && node->key < upper);
    int left = assert_avl_invariants(node->left, lower, node->key, count);
    int right = assert_avl_invariants(node->right, node->key, upper, count);
    assert(left - right >= -1 && left - right <= 1);
    int height = 1 + (left > right ? left : right);
    assert(node->height == height);
    ++*count;
    return height;
}

/** Compare every key and the node count against an independent set model. */
static void assert_avl_contents(avlNode *root, const int present[32])
{
    size_t actual_count = 0;
    size_t expected_count = 0;
    assert_avl_invariants(root, LLONG_MIN, LLONG_MAX, &actual_count);
    for (int key = 0; key < 32; ++key)
    {
        avlNode *found = findNode(root, key);
        assert((found != NULL) == (present[key] != 0));
        if (found != NULL)
        {
            assert(found->key == key);
            ++expected_count;
        }
    }
    assert(actual_count == expected_count);
}

/** Exercise both deletion directions with child balance -1, 0, and +1. */
static void test_deletion_rotations(void)
{
    const struct
    {
        int keys[5];
        size_t length;
        int removed;
        int expected_root;
    } cases[] = {{{2, 1, 4, 5}, 4, 1, 4},    {{2, 1, 4, 3}, 4, 1, 3},
                 {{2, 1, 4, 3, 5}, 5, 1, 4}, {{4, 2, 5, 1}, 4, 5, 2},
                 {{4, 2, 5, 3}, 4, 5, 3},    {{4, 2, 5, 1, 3}, 5, 5, 2}};
    for (size_t c = 0; c < sizeof(cases) / sizeof(cases[0]); ++c)
    {
        avlNode *root = NULL;
        int present[32] = {0};
        for (size_t i = 0; i < cases[c].length; ++i)
        {
            int key = cases[c].keys[i];
            root = insert(root, key);
            present[key] = 1;
            assert_avl_contents(root, present);
        }
        root = delete (root, cases[c].removed);
        present[cases[c].removed] = 0;
        assert_avl_contents(root, present);
        assert(root->key == cases[c].expected_root);
        for (size_t i = 0; i < cases[c].length; ++i)
        {
            int key = cases[c].keys[i];
            root = delete (root, key);
            present[key] = 0;
            assert_avl_contents(root, present);
        }
        assert(root == NULL);
    }
}

/** Deterministically permute insertion and deletion orders without rand(). */
static void shuffle_keys(int keys[32], unsigned int *state)
{
    for (size_t i = 31; i > 0; --i)
    {
        *state = *state * 1664525u + 1013904223u;
        size_t j = *state % (i + 1);
        int temp = keys[i];
        keys[i] = keys[j];
        keys[j] = temp;
    }
}

/** Check each mutation, including duplicate insertions and missing deletions.
 */
static void test_deletion_sequences(void)
{
    unsigned int state = 1;
    for (int round = 0; round < 16; ++round)
    {
        avlNode *root = NULL;
        int present[32] = {0};
        int keys[32];
        for (int key = 0; key < 32; ++key)
        {
            keys[key] = key;
        }
        shuffle_keys(keys, &state);
        for (size_t i = 0; i < 32; ++i)
        {
            int key = keys[i];
            root = insert(root, key);
            present[key] = 1;
            assert_avl_contents(root, present);
            root = insert(root, key);
            assert_avl_contents(root, present);
        }
        root = delete (root, 32);
        assert_avl_contents(root, present);
        shuffle_keys(keys, &state);
        for (size_t i = 0; i < 32; ++i)
        {
            int key = keys[i];
            root = delete (root, key);
            present[key] = 0;
            assert_avl_contents(root, present);
            root = delete (root, key);
            assert_avl_contents(root, present);
        }
        assert(root == NULL);
    }
}

/** Cover signed key boundaries and deletion of a node with two children. */
static void test_deletion_extrema(void)
{
    const int keys[] = {0, INT_MIN, INT_MAX, -1, 1};
    avlNode *root = NULL;
    for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); ++i)
    {
        root = insert(root, keys[i]);
        size_t count = 0;
        assert_avl_invariants(root, LLONG_MIN, LLONG_MAX, &count);
        assert(count == i + 1);
        assert(findNode(root, keys[i])->key == keys[i]);
    }
    for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); ++i)
    {
        root = delete (root, keys[i]);
        size_t count = 0;
        assert_avl_invariants(root, LLONG_MIN, LLONG_MAX, &count);
        assert(count == sizeof(keys) / sizeof(keys[0]) - i - 1);
        for (size_t j = 0; j < sizeof(keys) / sizeof(keys[0]); ++j)
        {
            avlNode *found = findNode(root, keys[j]);
            assert((found != NULL) == (j > i));
            if (found != NULL)
            {
                assert(found->key == keys[j]);
            }
        }
    }
    assert(root == NULL);
}

int main()
{
    test_deletion_rotations();
    test_deletion_sequences();
    test_deletion_extrema();
    int choice;
    int flag = 1;
    int insertNum;
    int queryNum;

    avlNode *root = NULL;
    avlNode *tempNode;

    while (flag == 1)
    {
        printf("\n\nEnter the Step to Run : \n");

        printf("\t1: Insert a node into AVL tree\n");
        printf("\t2: Delete a node in AVL tree\n");
        printf("\t3: Search a node into AVL tree\n");
        printf("\t4: printPreOrder (Ro L R) Tree\n");
        printf("\t5: printInOrder (L Ro R) Tree\n");
        printf("\t6: printPostOrder (L R Ro) Tree\n");
        printf("\t7: printAVL Tree\n");

        printf("\t0: EXIT\n");
        scanf("%d", &choice);

        switch (choice)
        {
        case 0:
        {
            flag = 0;
            printf("\n\t\tExiting, Thank You !!\n");
            break;
        }

        case 1:
        {
            printf("\n\tEnter the Number to insert: ");
            scanf("%d", &insertNum);

            tempNode = findNode(root, insertNum);

            if (tempNode != NULL)
                printf("\n\t %d Already exists in the tree\n", insertNum);
            else
            {
                printf("\n\tPrinting AVL Tree\n");
                printAVL(root, 1);
                printf("\n");

                root = insert(root, insertNum);
                printf("\n\tPrinting AVL Tree\n");
                printAVL(root, 1);
                printf("\n");
            }

            break;
        }

        case 2:
        {
            printf("\n\tEnter the Number to Delete: ");
            scanf("%d", &queryNum);

            tempNode = findNode(root, queryNum);

            if (tempNode == NULL)
                printf("\n\t %d Does not exist in the tree\n", queryNum);
            else
            {
                printf("\n\tPrinting AVL Tree\n");
                printAVL(root, 1);
                printf("\n");
                root = delete (root, queryNum);

                printf("\n\tPrinting AVL Tree\n");
                printAVL(root, 1);
                printf("\n");
            }

            break;
        }

        case 3:
        {
            printf("\n\tEnter the Number to Search: ");
            scanf("%d", &queryNum);

            tempNode = findNode(root, queryNum);

            if (tempNode == NULL)
                printf("\n\t %d : Not Found\n", queryNum);
            else
            {
                printf("\n\t %d : Found at height %d \n", queryNum,
                       tempNode->height);

                printf("\n\tPrinting AVL Tree\n");
                printAVL(root, 1);
                printf("\n");
            }

            break;
        }

        case 4:
        {
            printf("\nPrinting Tree preOrder\n");
            printPreOrder(root);

            break;
        }

        case 5:
        {
            printf("\nPrinting Tree inOrder\n");
            printInOrder(root);

            break;
        }

        case 6:
        {
            printf("\nPrinting Tree PostOrder\n");
            printPostOrder(root);

            break;
        }

        case 7:
        {
            printf("\nPrinting AVL Tree\n");
            printAVL(root, 1);

            break;
        }

        default:
        {
            flag = 0;
            printf("\n\t\tExiting, Thank You !!\n");
            break;
        }
        }
    }

    return 0;
}
