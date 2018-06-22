//
// Created by Chad Russell on 6/21/18.
//

#ifndef CPI_HASH_H
#define CPI_HASH_H

#include <string>

using namespace std;

template<typename Key, typename Value>
struct bucket_t {
    Key key;
    Value value;
    bucket_t<Key, Value> *next = nullptr;
};

template<typename Key, typename Value>
struct hash_t {
    int32_t bucket_count;
    int64_t size;
    bucket_t<Key, Value> **buckets;
};

template<typename Key, typename Value>
struct hash_t<Key, Value> *hash_t_init(int32_t bc) {
    auto ht = (hash_t<Key, Value> *) calloc(1, sizeof(struct hash_t<Key, Value>));
    ht->bucket_count = bc;
    ht->size = 0;
    ht->buckets = static_cast<bucket_t<Key, Value> **>(calloc(static_cast<size_t>(bc), sizeof(bucket_t<Key, Value> *)));
    return ht;
}

template<typename Key, typename Value>
void hash_t_insert(struct hash_t<Key, Value> *ht, Key key, Value value) {
    hash<Key> hasher;
    auto hash = hasher(key) % ht->bucket_count;

    if (ht->buckets[hash] != nullptr) {
        auto lastBucket = ht->buckets[hash];
        if (lastBucket->key == key) {
            lastBucket->value = value;
            return;
        }

        while (lastBucket->next != nullptr) {
            lastBucket = lastBucket->next;

            if (lastBucket->key == key) {
                lastBucket->value = value;
                return;
            }
        }

        auto bucket = new bucket_t<Key, Value>;
        bucket->key = key;
        bucket->value = value;

        lastBucket->next = bucket;
        ht->size += 1;
    }
    else {
        auto bucket = new bucket_t<Key, Value>;
        bucket->key = key;
        bucket->value = value;

        ht->buckets[hash] = bucket;
        ht->size += 1;
    }
}

template<typename Key, typename Value>
void hash_t_erase(hash_t<Key, Value> *ht, Key key) {
    hash<Key> hasher;
    auto hash = hasher(key) % ht->bucket_count;

    bucket_t<Key, Value> *prev = nullptr;
    if (ht->buckets[hash] != nullptr) {
        auto lastBucket = ht->buckets[hash];
        if (lastBucket->key == key) {
            ht->buckets[hash] = ht->buckets[hash]->next;
            ht->size -= 1;
            return;
        }

        while (lastBucket->next != nullptr) {
            if (lastBucket->next->key == key) {
                prev->next = lastBucket->next->next;
                ht->size -= 1;
                return;
            }

            prev = lastBucket;
            lastBucket = lastBucket->next;
        }
    }
}

template<typename Key, typename Value>
Value *hash_t_get(struct hash_t<Key, Value> *ht, Key key) {
    hash<Key> hasher;
    auto hash = hasher(key) % ht->bucket_count;

    if (ht->buckets[hash] != nullptr) {
        auto lastBucket = ht->buckets[hash];
        if (lastBucket->key == key) {
            return &lastBucket->value;
        }

        while (lastBucket->next != nullptr) {
            lastBucket = lastBucket->next;
            if (lastBucket->key == key) {
                return &lastBucket->value;
            }
        }
    }

    return nullptr;
}

#endif //CPI_HASH_H
