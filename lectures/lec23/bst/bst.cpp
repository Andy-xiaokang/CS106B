// Sean Szumlanski
// CS106B, Autumn 2023
//
// bst.cpp
// =======
// Implementation of a few basic BST functions: traversal algorithms, insertion,
// and deletion.

#include <iostream>
#include "console.h"
#include "queue.h"
#include "simpio.h"
#include "error.h"
#include <limits>
#include "MemoryDiagnostics.h"
#include "SimpleTest.h"
using namespace std;

struct Node
{
    TRACK_ALLOCATIONS_OF(Node);
	int data;
	Node *left;
	Node *right;
	Node(int val)
	{
		data = val;
		left = right = nullptr;
	}
};

void preorderHelper(Node *root)
{
	if (root == nullptr)
	{
		return;
	}

	cout << " " << root->data;
	preorderHelper(root->left);
	preorderHelper(root->right);
}

void preorder(Node *root)
{
	if (root == nullptr)
	{
		cout << "(empty tree)" << endl;
		return;
	}

	cout << "Preorder Traversal:";
	preorderHelper(root);
	cout << endl;
}

void postorderHelper(Node *root)
{
	if (root == nullptr)
	{
		return;
	}

	postorderHelper(root->left);
	postorderHelper(root->right);
	cout << " " << root->data;
}

void postorder(Node *root)
{
	if (root == nullptr)
	{
		cout << "(empty tree)" << endl;
		return;
	}

	cout << "Postorder Traversal:";
	postorderHelper(root);
	cout << endl;
}

void inorderHelper(Node *root)
{
	if (root == nullptr)
	{
		return;
	}

	inorderHelper(root->left);
	cout << " " << root->data;
	inorderHelper(root->right);
}

void inorder(Node *root)
{
	if (root == nullptr)
	{
		cout << "(empty tree)" << endl;
		return;
	}

	cout << "Inorder Traversal:";
	inorderHelper(root);
	cout << endl;
}

void levelorder(Node *root)
{
    if (root == nullptr)
    {
        cout << "(empty tree)" << endl;
        return;
    }

	Queue<Node *> q;
	q.enqueue(root);

	cout << "Level Order Traversal:";

	while (!q.isEmpty())
	{
		Node *current = q.dequeue();

		if (current == nullptr)
		{
			continue;
		}

		cout << " " << current->data;

		q.enqueue(current->left);
		q.enqueue(current->right);
	}

	cout << endl;
}

void bstInsert(Node *&root, int data)
{
	if (root == nullptr)
	{
		Node *n = new Node(data);
		n->data = data;
		n->left = n->right = nullptr;

		root = n;
		return;
	}

	if (data <= root->data)
	{
		bstInsert(root->left, data);
	}
	else
	{
		bstInsert(root->right, data);
	}
}

/*
// WARNING: This function will segfault if passed an empty tree. This is
// written in a supremely unsafe way with the assumption that it is only
// being called from bstDelete() in the case where we're deleting a node
// with two children -- and so when we called bstFindMax(root->left) from
// that function, we were guaranteed not to be passing nullptr.
int bstFindMax(Node *root)
{
	Node *current = root;

    while (current->right != nullptr)
	{
        current = current->right;
	}

    return current->data;
}

// Deletes a single occurrence of 'data' from the given BST, if it is present.
// Recall that we pass a reference to our root pointer because we might want to
// change what this pointer is pointing to. That happens if we end up deleting
// the root node in this function call, in which case we want that pointer back
// in our calling function to be set to nullptr.
void bstDelete(Node *&root, int data)
{
    // If we encounter an empty tree, there's nothing to delete. Simply return.
    if (root == nullptr)
    {
        return;
    }

    if (data < root->data)
    {
        bstDelete(root->left, data);
    }
    else if (data > root->data)
    {
        bstDelete(root->right, data);
    }
    else
    {
        // If we get here, we've found the value to be deleted. There
        // are four cases to consider now:
        //
        //   1. The node to be deleted has no children.
        //   2(a). The node to be deleted has one child (a left child).
        //   2(b). The node to be deleted has one child (a right child).
        //   3. The node to be deleted has two children.

        if (root->left == nullptr && root->right == nullptr)
        {
            // This is the case where the node has no children. We can simply
            // remove it from the tree.
            delete root;
            root = nullptr;
        }
        else if (root->right == nullptr)
        {
            // Here, we have a single child: a left child. It might seem a bit
            // jarring not to check above whether root->left != nullptr, but we
            // know that if root->right == nullptr, root->left can't be nullptr.
            // If it were, then both children would be null, and we would have
            // triggered the if condition above and never made it to this
            // else-if condition.
            //
            // In this case, we need to delete the current node and move its
            // left child up to take its place. Note that we can't safely
            // access root->left after deleting root. The arrow operator (->)
            // dereferences our struct pointer, and we should never dereference
            // a pointer to something that has been deleted. So, we have to do
            // a delicate dance here.

            // Hold onto the left child. This will become the new root of this
            // subtree.
            Node *uhmTempOrSomething = root->left;

            // Delete the node 'root' is pointing to. (This doesn't delete the
            // local variable called 'root'. It's still a reference to the root
            // pointer passed to this function. This just deletes the
            // dynamically allocated node that 'root' was pointing to.)
            delete root;

            // Now set the root to point to the temp node -- the left child
            // that is moving up to take its parent's place.
            root = uhmTempOrSomething;
        }
        else if (root->left == nullptr)
        {
            // This is the case where we have just one child: a right child.
            // The operation is symmetric to the one above.
            Node *uhmTempOrSomething = root->right;
            delete root;
            root = uhmTempOrSomething;
        }
        else
        {
            // This is the case where we have two children. The max value in
            // the left subtree needs to move up. Note that I'm not rewiring
            // the tree here. I just leave this node in place and change the
            // value it contains.
            root->data = bstFindMax(root->left);
            bstDelete(root->left, root->data);
        }
    }
}
*/

int bstFindMax(Node* root) {
    // if (root == nullptr) {
    //     return - INFINITY;
    // }
    // Node* curr = root;
    // while(curr->right != nullptr) {
    //     curr = curr->right;
    // }
    // return curr->data;
    if (root->right == nullptr) {
        return root->data;
    }
    return bstFindMax(root->right);
}

void bstDelete(Node* & root, int data) {
    if (root == nullptr) {
        return;
    }
    if (root->data < data) {
        bstDelete(root->right, data);  // search from the right child tree
    } else if (root->data > data) {
        bstDelete(root->left, data);  // search from the left child tree
    } else {   // root->data == data
        if (root->left == nullptr && root->right == nullptr) {
            // case1: leaf node
            delete root;
            root = nullptr;
        } else if (root->left == nullptr) {
            // case2: only left or right subtree
            Node* tmp = root->right;
            delete root;
            root = tmp;
        } else if (root->right == nullptr) {
            Node* tmp = root->left;
            delete root;
            root = tmp;
        } else {
            // case3: with left subtree and right subtree
            root->data = bstFindMax(root->left);
            bstDelete(root->left, root->data);
        }
    }
}

int findMax(Node* root) {
    if (root == nullptr) {
        return numeric_limits<int>::min();
    }
    Queue<Node*> q;
    q.enqueue(root);
    int maxValue = root->data;
    while(!q.isEmpty()) {
        Node* curr = q.dequeue();
        if (curr == nullptr) {
            continue;
        }
        maxValue = max(maxValue, curr->data);
        q.enqueue(curr->left);
        q.enqueue(curr->right);
    }
    return maxValue;
}

int findMin(Node* root) {
    if (root == nullptr) {
        error("can't findMin on an empty tree");
    }
    Queue<Node*> q;
    q.enqueue(root);
    int minValue = root->data;
    while(!q.isEmpty()) {
        Node* curr = q.dequeue();
        if (curr == nullptr) {
            continue;
        }
        minValue = min(minValue, curr->data);
        q.enqueue(curr->left);
        q.enqueue(curr->right);
    }
    return minValue;
}

int bstFindMin(Node* root) {
    if (root == nullptr) {
        error("can't bstFindMin on empty tree");
    }
    if (root->left == nullptr) {
        return root->data;
    }
    return bstFindMin(root->left);
}

bool contains(Node* root, int data) {
    if (root == nullptr) {
        return false;
    }
    if (root->data == data) {
        return true;
    }
    // if (root->left == nullptr && root->right == nullptr) {
    //     return false;
    // }
    return (contains(root->left, data) || contains(root->right, data));
}

bool bstContains(Node* root, int data) {
    if (root == nullptr) {
        return false;
    }
    if (root->data == data) {
        return true;
    } else if (root->data < data) {
        return bstContains(root->right, data);
    } else {
        return bstContains(root->left, data);
    }
}

void deallocate(Node* root) {
    if (root == nullptr) {
        return;
    }
    deallocate(root->left);
    deallocate(root->right);
    delete root;
    // if (root == nullptr) {
    //     return;
    // }
    // Queue<Node*> q;
    // q.enqueue(root);
    // while (!q.isEmpty()) {
    //     Node* curr = q.dequeue();
    //     if (curr == nullptr) {
    //         continue;
    //     }
    //     q.enqueue(curr->left);
    //     q.enqueue(curr->right);
    //     delete curr;
    // }
}

int main()
{

    Node *root = nullptr;
    bstInsert(root, 50);
    // cout << "before delete root: " << root->left << endl;
    // delete root;
    // cout << "after delete root: " << root->left << endl;
    bstInsert(root, 30);
    bstInsert(root, 55);
    bstInsert(root, 40);
    bstInsert(root, 35);
    bstInsert(root, 45);

    /*
     *           50
     *         /    \
     *        30     55
     *         \
     *          40
     *         /  \
     *       35    45
     */

    // Before we had a nice bstInsert() function, we constructed our tree
    // manually, like so:
    //
    //	root = new Node(50);
    //	root->left = new Node(30);
    //	root->right = new Node(55);
    //	root->left->right = new Node(40);
    //	root->left->right->left = new Node(35);
    //	root->left->right->right = new Node(45);
    cout << "findMax: " << findMax(root) << endl;
    cout << "findMin: " << findMin(root) << endl;
    cout << "bstFindMin: " << bstFindMin(root) << endl;
    cout << "contains: 55: " << contains(root, 55) << endl;
    cout << "contsins: 100: " << contains(root, 100) << endl;
    cout << "bstContains 55: " << bstContains(root, 55) << endl;
    cout << "bstContains 100: " << bstContains(root, 100) << endl;
    preorder(root);
    postorder(root);
    inorder(root);
    levelorder(root);

    while (true)
    {
        cout << endl << "What value shall we delete? (-1 to quit) ";
        int n = getInteger();

        if (n == -1)
        {
            break;
        }

        bstDelete(root, n);

        cout << endl;
        preorder(root);
        postorder(root);
        inorder(root);
        levelorder(root);
    }

    runSimpleTests(SELECTED_TESTS);

	return 0;
}

STUDENT_TEST("test frees all dynamically allocated memory") {
    Node *root = nullptr;
    bstInsert(root, 50);
    bstInsert(root, 30);
    bstInsert(root, 55);
    bstInsert(root, 40);
    bstInsert(root, 35);
    bstInsert(root, 45);
    cout << "address of root: " << &root << endl;
    deallocate(root);
}
