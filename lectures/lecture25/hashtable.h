#ifndef HASHTABLE_H
#define HASHTABLE_H
#include <string>
#include "node.h"

class HashTable
{
public:
    HashTable();
    ~HashTable();
    // std::string get(std::string key) const;
    void put(std::string key);
static int hashCode(std::string s);
private:
    static const int INITIAL_BUCKET_COUNT = 11;
    Node** buckets;
    int nBuckets;
    Node* findNode(int bucket, std::string key) const;


friend std::ostream& operator<< (std::ostream& out, HashTable& table);
};


#endif // HASHTABLE_H
