#pragma once

template <typename T>
struct Node {
    T key;
    Node* left = nullptr;
    Node* right = nullptr;
    int height = 1;

    explicit Node(const T& value) : key(value) {}
};

template <typename T>
inline int h(const Node<T>* n) {
    return n ? n->height : 0;
}

inline int imax(int a, int b) {
    return (a > b) ? a : b;
}

template <typename T>
inline void fix(Node<T>* n) {
    if (n) {
        n->height = 1 + imax(h(n->left), h(n->right));
    }
}

template <typename T>
inline int bf(const Node<T>* n) {
    return n ? h(n->left) - h(n->right) : 0;
}

template <typename T>
inline Node<T>* rotRight(Node<T>* y) {
    Node<T>* x = y->left;
    Node<T>* t2 = x->right;

    x->right = y;
    y->left = t2;

    fix(y);
    fix(x);
    return x;
}

template <typename T>
inline Node<T>* rotLeft(Node<T>* x) {
    Node<T>* y = x->right;
    Node<T>* t2 = y->left;

    y->left = x;
    x->right = t2;

    fix(x);
    fix(y);
    return y;
}
