//
// Created by Chad Russell on 6/22/18.
//

#ifndef CPI_CONTAINER_H
#define CPI_CONTAINER_H

#include <string>
#include "util.h"

using namespace std;

// ==========================
//          HASH
// ==========================
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
struct hash_t<Key, Value> *hash_init(int32_t bc) {
    auto ht = (hash_t<Key, Value> *) calloc(1, sizeof(struct hash_t<Key, Value>));
    ht->bucket_count = bc;
    ht->size = 0;
    ht->buckets = static_cast<bucket_t<Key, Value> **>(calloc(static_cast<size_t>(bc), sizeof(bucket_t<Key, Value> *)));
    return ht;
}

template<typename Key, typename Value>
void hash_insert(struct hash_t<Key, Value> *ht, Key key, Value value) {
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
void hash_erase(hash_t<Key, Value> *ht, Key key) {
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
Value *hash_get(struct hash_t<Key, Value> *ht, Key key) {
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


// ==========================
//          VECTOR
// ==========================
template<typename T>
struct vector_t {
    unsigned long length;
    unsigned long capacity;

    T *items;
};

// to debug:
// (T(*)[128]) v->items

template<typename T>
vector_t<T> vector_init(unsigned long initial_capacity) {
    vector_t<T> vec;
    vec.length = 0;
    vec.capacity = initial_capacity;
    vec.items = (T *) calloc((size_t) initial_capacity, sizeof(T));
    return vec;
}

template<typename T>
void vector_grow(vector_t<T> &vector) {
    unsigned long new_capacity = vector.capacity * 2;

    auto new_items = (T *) calloc((size_t) new_capacity, sizeof(T));
//    auto old_items = vector.items;

    memcpy(new_items, vector.items, vector.capacity * sizeof(T));

    vector.capacity = new_capacity;
    vector.items = new_items;

    // todo(chad): why might this fail??
//    free(old_items);
}

template<typename T>
void vector_append(vector_t<T> &vector, T item) {
    if (vector.length == vector.capacity) {
        vector_grow(vector);
    }

    memcpy(vector.items + vector.length, &item, sizeof(T));
    vector.length = vector.length + 1;
}

template<typename T>
T vector_at(vector_t<T> vector, unsigned long index) {
    assert(index >= 0 && index < vector.length);

    return vector.items[index];
}

template<typename T>
void vector_set_at(vector_t<T> vector, unsigned long index, T item) {
    memcpy(vector.items + index, &item, sizeof(T));
}

template<typename T>
auto begin(vector_t<T> &v) {
    return &v.items[0];
}

template<typename T>
auto end(vector_t<T> &v) {
    return &v.items[v.length];
}

#endif //CPI_CONTAINER_H
