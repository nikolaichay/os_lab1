#pragma once

#include <atomic>
#include <iostream>

class Queue {
    struct Node {
        int m_data;
        Node* m_next;
    };

    std::atomic<Node*> m_head{};
    std::atomic<Node*> m_tail{};

public:
    void enqueue(const int& item) {
        Node* node = new Node();
        node->m_data = item;

        if (empty()) {
            m_head.store(node);
            m_tail.store(node);
            return;
        }

        Node* temp = m_tail.exchange(node);
        if (!temp) m_head.store(node);
        else temp->m_next = node;
    }

    bool dequeue(int& item) {
        if (empty())
            return false;

        Node* head = m_head.load();
        item = head->m_data;
        while (!m_head.compare_exchange_weak(head, head->m_next));

        delete head;
        return true;
    }

    bool empty() {
        return !m_head.load();
    }
};
