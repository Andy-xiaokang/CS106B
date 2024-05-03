#include "hashtable.h"
#include <iostream>
using namespace std;

HashTable::HashTable() {
    nBuckets = INITIAL_BUCKET_COUNT;
    buckets = new Node*[nBuckets];
    for (int i = 0; i < nBuckets; i++) {
        buckets[i] = nullptr;
    }
}

HashTable::~HashTable() {
    for (int i = 0; i < nBuckets; i++) {
        Node* curr = buckets[i];
        while (curr != nullptr) {
            Node* tmp = curr;
            curr = curr->next;
            delete tmp;
        }
    }
}

int HashTable::hashCode(string s) {
    if (s == "") {
        return 0;
    } else {
        return (int(tolower(s[0]) - 'a') + 1);
    }
}

// string HashTable::get(string key) const {

// }

void HashTable::put(string key) {
    int n = hashCode(key) % nBuckets;
    if (findNode(n, key) == nullptr) {
        Node* newNode = new Node(key);
        newNode->next = buckets[n];
        buckets[n] = newNode;
    }
}

Node* HashTable::findNode(int bucket, string key) const {
    Node* curr = buckets[bucket];
    while (curr != nullptr && curr->key != key) {
        curr = curr->next;
    }
    return curr;
}

ostream& operator<< (ostream& out, HashTable& table) {
    for (int i = 0; i < table.nBuckets; i++) {
        out << "bucket[" << i << "]" << ":/";
        Node* curr = table.buckets[i];
        while (curr != nullptr) {
            if (curr->next != nullptr) {
                out << curr->key << "->";
            } else {
                out << curr->key;
            }
            curr = curr->next;
        }
        out << endl;
    }
    return out;
}































