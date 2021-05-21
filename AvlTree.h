#ifndef AVLTREE_H
#define AVLTREE_H

#include <iostream>
#include "exceptions.h"

namespace wet1
{

    int max(int x, int y) {
        return ( x > y ) ? x : y;
    }

    template<typename T>
    class AvlTreeNode {
        T data;
        AvlTreeNode* parent;
        AvlTreeNode* left;
        AvlTreeNode* right;
        int height;

    public:
        AvlTreeNode(const T& data) : data(data) , parent(nullptr),left(nullptr), right(nullptr), height(0) {}
        AvlTreeNode(const T& data , AvlTreeNode<T>* father ) : data(data) , parent(father), left(nullptr),
                                                                right(nullptr), height(0) {}
        AvlTreeNode* buildATree(T* arr, int max, int min) {
            if ((max-min) < 0 ) return nullptr;
            int mid = (max+min)/2;
            AvlTreeNode* node = new AvlTreeNode<T>(arr[mid]);

            node->left = buildATree(arr, mid-1,min);
            if (node->left) node->left->parent = node;
            int left_height = node->left ? node->left->height : -1 ;
            node->right = buildATree(arr, max,mid+1);
            if (node->right) node->right->parent = node;
            int right_height = node->right ? node->right->height : -1 ;
            node->height = right_height > left_height ? right_height : left_height;
            node->height++;
            return node;
        }
        AvlTreeNode<T>* get_left() {
            return this->left ? this->left : nullptr ;
        }
        void set_left(AvlTreeNode<T>* node) {
            this->left = node;
        }
        AvlTreeNode<T>* get_right() {
            return this->right ? this->right : nullptr;
        }
        void set_right(AvlTreeNode<T>* node) {
            this->right = node;
        }
        AvlTreeNode<T>* get_parent() {
            return this->parent ? this->parent : nullptr ;
        }
        void set_parent(AvlTreeNode<T>* node) {
            this->parent = node;
        }
        int get_height(){
            return this->height ? this->height : -1;
        }
        int get_left_height(){
            return left ? left->height : -1;
        }
        int get_right_height(){
            return right ? right->height : -1;
        }
        void set_height(int new_height){
            this->height = new_height;
        }
        T& get_data() {
            return this->data;
        }
        T& get_left_data() {
            return left->get_data();
        }
        T& get_right_data() {
            return right->get_data();
        }
        void set_data( T& new_data) {
            this->data = new_data;
        }
        void destroyTree(AvlTreeNode<T>* root) {
            if (root == nullptr ) return;
            destroyTree(root->left);
            destroyTree(root->right);
            delete root;
        }
    };

    template<typename T, typename Comp>
    class AvlTree {
        AvlTreeNode<T>* root;
        Comp compFunc;
        AvlTreeNode<T>* youngest;
        AvlTreeNode<T>* oldest;

    public:
        AvlTree() : root(nullptr),compFunc(), youngest(nullptr), oldest(nullptr) {}
        AvlTree(T* arr, int max , int min) : root(nullptr),compFunc(), youngest(nullptr) {
            root = root->buildATree(arr,max,min);
            youngest = get_younget_child(root);
            oldest = get_oldest_child(root);
        }
        ~AvlTree() {
            root->destroyTree(root);
        };

        AvlTreeNode<T>* getRoot() {
            return this->root;
        }
        void set_root(AvlTreeNode<T>* new_root) {
            root = new_root;
        }

    private:

        AvlTreeNode<T>* find_in_tree(AvlTreeNode<T>* node , const T& data_to_find ) {
            if (!node) return nullptr;
            if (compFunc(data_to_find, node->get_data()))
                return find_in_tree(node->get_left(),data_to_find);
            else if (compFunc(node->get_data(),data_to_find))
                return find_in_tree(node->get_right(),data_to_find);
            else return node;
            return nullptr; //should never get here
        }


        AvlTreeNode<T>* rightRotate(AvlTreeNode<T>* y)
        {
            AvlTreeNode<T>* x = y->get_left();
            AvlTreeNode<T>* z = x ? x->get_right() : nullptr ;
            x->set_right(y);
            x->set_parent(y->get_parent());
            y->set_parent(x);
            y->set_left(z);
            if (z) z->set_parent(y);
            y->set_height(1 + max(y->get_left_height(),y->get_right_height()));
            x->set_height(1 + max(x->get_left_height(),x->get_right_height()));

            return x;
        }


        AvlTreeNode<T>* leftRotate(AvlTreeNode<T>* x)
        {
            AvlTreeNode<T>* y = x->get_right();
            AvlTreeNode<T>* z = y ? y->get_left() : nullptr;
            y->set_left(x);
            y->set_parent(x->get_parent());
            x->set_parent(y);
            x->set_right(z);
            if (z) z->set_parent(x);
            y->set_height(1 + max(y->get_left_height(),y->get_right_height()));
            x->set_height(1 + max(x->get_left_height(),x->get_right_height()));

            return y;
        }

        AvlTreeNode<T>* get_younget_child(AvlTreeNode<T>* node) {
            if ( node->get_left() == nullptr )
                return node;
            else return get_younget_child(node->get_left());
        }

        AvlTreeNode<T>* get_oldest_child(AvlTreeNode<T>* node) {
            if ( node->get_right() == nullptr )
                return node;
            else return get_oldest_child(node->get_right());
        }


        AvlTreeNode<T>* insert( const T& data , AvlTreeNode<T>* node , AvlTreeNode<T>* parent_node ) {
            if (node == nullptr)
                return new AvlTreeNode<T>(data,parent_node);
            if ( compFunc(data,node->get_data()))
                node->set_left(insert(data,node->get_left(),node));
            else
                node->set_right(insert(data,node->get_right(),node));

            node->set_height(1 + max(node->get_left_height(),node->get_right_height()));

            int balance = node->get_left_height() - node->get_right_height();

            // LL
            if (balance > 1 && compFunc(data,node->get_left_data()) )
                return rightRotate(node);
            // RR
            if (balance < -1 && compFunc(node->get_right_data(),data))
                return leftRotate(node);
            // LR
            if (balance > 1 && compFunc(node->get_left_data(),data))
            {
                node->set_left(leftRotate(node->get_left()));
                return rightRotate(node);
            }
            // RL
            if (balance < -1 && compFunc(data,node->get_right_data()))
            {
                node->set_right(rightRotate(node->get_right()));
                return leftRotate(node);
            }
            return node;

        }


        AvlTreeNode<T>* deleteNode(AvlTreeNode<T>* node, T& data)
        {

            if (node == NULL)
                return node;

            if ( compFunc(data,node->get_data()))
                node->set_left(deleteNode(node->get_left(),data));
            else if( compFunc(node->get_data(),data) )
                node->set_right(deleteNode(node->get_right(),data));
            else
            {
                // node with max one child
                if( (node->get_left() == nullptr ) || ( node->get_right() == nullptr) )
                {
                    AvlTreeNode<T>* temp = node->get_left() ? node->get_left() : node->get_right() ;

                    // No child case
                    if (temp == NULL)
                    {
                        temp = node;
                        node = NULL;
                    }
                    else // One child case
                        *node = *temp;

                    delete temp; // or a delete function
                }
                else
                {
                    // node with two children
                    AvlTreeNode<T>* temp = get_younget_child(node->get_right());
                    node->set_data(temp->get_data());
                    node->set_right(deleteNode(node->get_right(),temp->get_data()));
                }
            }

            if (node == nullptr)
                return node;

            // STEP 2: UPDATE HEIGHT OF THE CURRENT NODE
            node->set_height(1+ max(node->get_left_height(),node->get_right_height()));

            int balance = node->get_left_height() - node->get_right_height();

            // LL
            if (balance > 1 && compFunc(data,node->get_left_data()) )
                return rightRotate(node);
            // RR
            if (balance < -1 && compFunc(node->get_right_data(),data))
                return leftRotate(node);
            // LR
            if (balance > 1 && compFunc(node->get_left_data(),data))
            {
                node->set_left(leftRotate(node->get_left()));
                return rightRotate(node);
            }
            // RL
            if (balance < -1 && compFunc(data,node->get_right_data()))
            {
                node->set_right(rightRotate(node->get_right()));
                return leftRotate(node);
            }
            return node;
        }

    public:

        T& find(const T& data) {
            AvlTreeNode<T>* node = find_in_tree(root,data);
            if(!node)
                throw NotFound();
            return node->get_data();
        }

        void deleteElement(T& data) {
            root = deleteNode(root,data);
            youngest = get_younget_child(root);
            oldest = get_oldest_child(root);
        }

        void addElement(T& data) {
            root = insert(data, this->root, this->root);
            youngest = get_younget_child(root);
            oldest = get_oldest_child(root);
        }

        T& getOldestData()
        {
            if(!oldest)
                throw EmptyTree();
            return oldest->get_data();
        }

        T& getYoungestData()
        {
            if(!youngest)
                throw EmptyTree();
            return youngest->get_data();
        }

        AvlTreeNode<T>* getYoungestNode()
        {
            return youngest;
        }
    };
}
#endif //AVLTREE_H
