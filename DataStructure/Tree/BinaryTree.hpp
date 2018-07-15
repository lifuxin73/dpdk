/*
 * BinaryTree.hpp
 *
 *  Created on: Feb 25, 2018
 *      Author: lifuxin
 */

#ifndef DPDK_DATASTRUCTURE_TREE_BINARYTREE_HPP_
#define DPDK_DATASTRUCTURE_TREE_BINARYTREE_HPP_
class Node
{
public:
    Node(int key):key(key),value(0){}
    int key;
    int value;
    Node* lnode = nullptr;
    Node* rnode = nullptr;
};

class Tree
{
private:
    Node* root = nullptr;
public:
    void insert(Node* ndoe, int key);
    Node* search(Node* node, int key);
};

void Tree::insert(Node* q, int key)
{
    Node* p = new Node(key);
    if(q == nullptr)
    {
        root = p;
        return;
    }

    while( q->lnode != p && q->rnode != p)
    {
        if(key < q->key)
        {
            if(q->lnode != nullptr)
            {
                q = q->lnode;
            }
            else
            {
                q->lnode = p;
            }
        }
        else
        {
            if(q->rnode != nullptr)
            {
                q = q->rnode;
            }
            else
            {
                q->rnode = p;
            }
        }
    }
    return;

}

Node* Tree::search(Node* q,int key)
{
    while(q!=nullptr)
    {
        if(q->key == key)
        {
            return q;
        }
        else if(q->key > key)
        {
            q = q->lnode;
        }else if(q->key < key)
        {
            q = q->rnode;
        }
    }
    return nullptr;
}
#endif /* DPDK_DATASTRUCTURE_TREE_BINARYTREE_HPP_ */
