#ifndef _LIST_HPP_
#define _LIST_HPP_

template<typename T>
class List { 
public:
    void addLast(T* item) {
        Node* node = new Node(item);
        if (tail) tail->next = node;
        else head = node;
        tail = node;
    }

    T* removeFirst() {
        if (!head) return nullptr;
        Node* node = head;
        T* item = node->data;
        head = head->next;
        if (!head) tail = nullptr;
        delete node;
        return item;
    }

    bool isEmpty() const { return head == nullptr; }
private:
    struct Node {
        T* data;
        Node* next;

        Node(T* d) : data(d), next(nullptr) {}
    };

    Node* head = nullptr;
    Node* tail = nullptr;
};

#endif