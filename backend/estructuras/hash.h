#ifndef HASH_H
#define HASH_H

#include <iostream>
#include <cstdint>
#include <cmath>
using namespace std;

template<typename K, typename V>
struct HashEntry {
    K key;
    V value;
    bool used;
    HashEntry() : key(), value(), used(false) {}
};


template<typename K = uint32_t, typename V = uint32_t, bool EnableRehash = true>
class HashMap {
private:
    HashEntry<K, V>* table;  
    uint32_t capacity;
    uint32_t size;
    static constexpr double MAX_CAP = 0.65;

    bool rehash(uint32_t newCapacity) {
        HashEntry<K, V>* oldTable = table;
        uint32_t oldCapacity = capacity;
        table = nullptr;
        HashEntry<K, V>* newTable = nullptr;
        try {
            newTable = new HashEntry<K, V>[newCapacity];
        } catch (bad_alloc&) {
            cerr << "HashMap rehash failed: allocation error." << endl;
            table = oldTable;
            return false;
        }
        table = newTable;
        uint32_t oldSize = size;
        capacity = newCapacity;
        size = 0;
        for (uint32_t i = 0; i < oldCapacity; ++i) {
            if (oldTable[i].used) {
                if (!insert(oldTable[i].key, oldTable[i].value)) {
                    cerr << "HashMap rehash failed: cannot insert existing entry." << endl;
                    delete[] newTable;
                    table = oldTable;
                    capacity = oldCapacity;
                    size = oldSize;
                    return false;
                }
            }
        }
        delete[] oldTable;
        return true;
    }

    bool ensureCapacityForInsert() {
        if constexpr (EnableRehash) {
            if (size + 1 > static_cast<uint32_t>(capacity * MAX_CAP)) {
                uint32_t newCapacity = capacity ? capacity * 2 : 2;
                return rehash(newCapacity);
            }
        }
        return true;
    }

public:
    HashMap(uint32_t capacity_) : table(nullptr), capacity(capacity_), size(0) {
        if (capacity_ == 0) capacity = 2;
        try {
            table = new HashEntry<K, V>[capacity];
        } catch (bad_alloc&) {
            cerr << "HashMap constructor failed: allocation error." << endl;
            table = nullptr;
            capacity = 0;
        }
    }

    ~HashMap() {
        delete[] table;
    }

    uint32_t hash(K key) const {
        uint32_t hashed = static_cast<uint32_t>(key) * 2654435769u;
        return (capacity == 0) ? 0 : (hashed % capacity);
    }

    int32_t find(K key) const {
        if (!table || capacity == 0) return -1;
        uint32_t idx = hash(key);
        uint32_t start_idx = idx;
        while (table[idx].used) {
            if (table[idx].key == key) {
                return static_cast<int32_t>(idx);
            }
            idx = (idx + 1) % capacity;
            if (idx == start_idx) break;
        }
        return -1;
    }

    bool insert(K key, V value) {
        if (!table || capacity == 0) {
            cerr << "HashMap insert failed: table not initialized." << endl;
            return false;
        }
        if (!ensureCapacityForInsert()) {
            cerr << "HashMap insert failed: rehashing failed." << endl;
            return false;
        }
        uint32_t idx = hash(key);
        uint32_t start_idx = idx;
        while (table[idx].used) {
            if (table[idx].key == key) {
                table[idx].value = value;
                return true;
            }
            idx = (idx + 1) % capacity;
            if (idx == start_idx) {
                cerr << "HashMap insert failed: table is full after resize." << endl;
                return false;
            }
        }
        table[idx].key = key;
        table[idx].value = value;
        table[idx].used = true;
        size++;
        return true;
    }

    bool remove(K key) {
        if (!table || capacity == 0) return false;
        int32_t idx = find(key);
        if (idx == -1) return false;

        table[idx].used = false;
        size--;
        idx = (idx + 1) % capacity;
        while (table[idx].used) {
            HashEntry<K, V> rehashEntry = table[idx];
            table[idx].used = false;
            size--;
            insert(rehashEntry.key, rehashEntry.value);
            idx = (idx + 1) % capacity;
        }
        return true;
    }

    V& operator[](K key) {
        if (!table || capacity == 0) {
            static V dummy = V();
            cerr << "HashMap operator[] failed: table not initialized." << endl;
            return dummy;
        }
        if (!ensureCapacityForInsert()) {
            static V dummy = V();
            cerr << "HashMap operator[] failed: rehashing failed." << endl;
            return dummy;
        }
        uint32_t idx = hash(key);
        uint32_t start_idx = idx;
        while (table[idx].used) {
            if (table[idx].key == key) {
                return table[idx].value;
            }
            idx = (idx + 1) % capacity;
            if (idx == start_idx) {
                cerr << "HashMap operator[] failed: table is full after resize." << endl;
                return table[0].value;
            }
        }
        table[idx].key = key;
        table[idx].value = V();
        table[idx].used = true;
        size++;
        return table[idx].value;
    }

    void clear() {
        if (!table) return;
        for (uint32_t i = 0; i < capacity; i++) {
            table[i].used = false;
        }
        size = 0;
    }

    uint32_t getSize() const {
        return size;
    }

    uint32_t getCapacity() const {
        return capacity;
    }

    bool getValue(K key, V& out) const {
        int32_t pos = find(key);
        if (pos == -1) return false;
        out = table[pos].value;
        return true;
    }

    const HashEntry<K, V>& entryAt(uint32_t idx) const {
        return table[idx];
    }
};

#endif 
