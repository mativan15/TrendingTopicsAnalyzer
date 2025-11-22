#ifndef MINHEAP_H
#define MINHEAP_H

#include <cstdint>
#include <iostream>


template<typename K = uint16_t>
class MinHeap {
private:
    uint16_t capacity;
    uint16_t size;
    uint16_t* values; // array contador indices
    K* keys;  // keys[...] = prioridad (count + error)
    int* indexInHeap; //pos en el heap

    //auxiliares
    inline int parent(int i) const { return (i - 1) / 2; }
    inline int leftChild(int i) const { return 2 * i + 1; }
    inline int rightChild(int i) const { return 2 * i + 2; }

    inline void swapNodes(int a, int b) {
        K tk = keys[a];
        keys[a] = keys[b];
        keys[b] = tk;

        uint16_t tv = values[a];
        values[a] = values[b];
        values[b] = tv;

        indexInHeap[values[a]] = a;
        indexInHeap[values[b]] = b;
    }

    void heapUp(int idx) {
        while (idx > 0 && keys[parent(idx)] > keys[idx]) {
            int p = parent(idx);
            swapNodes(p, idx);
            idx = p;
        }
    }

    void heapDown(int idx) {
        while (true) {
            int l = leftChild(idx);
            int r = rightChild(idx);
            int smallest = idx;

            if (l < size && keys[l] < keys[smallest]) smallest = l;
            if (r < size && keys[r] < keys[smallest]) smallest = r;

            if (smallest == idx) break;
            swapNodes(idx, smallest);
            idx = smallest;
        }
    }

public:
    MinHeap(uint16_t cap) : capacity(cap), size(0) {
        values = new uint16_t[capacity];
        keys = new K[capacity];
        indexInHeap = new int[capacity];
        for (int i = 0; i < capacity; i++) indexInHeap[i] = -1;
    }

    ~MinHeap() {
        delete[] values;
        delete[] keys;
        delete[] indexInHeap;
    }

    bool isEmpty() const { return size == 0; }
    bool isFull() const { return size == capacity; }

    void insert(uint16_t contadorIndex, K priority) {
        if (isFull()) return;

        int pos = size;
        values[pos] = contadorIndex;
        keys[pos] = priority;
        indexInHeap[contadorIndex] = pos;

        heapUp(pos);
        size++;
    }

    //minimo sin eliminar
    inline uint16_t getMinValue() const {
        if (isEmpty()) return 0;
        return values[0];
    }

    //remplazar minimo por otro
    void replaceMin(uint16_t contadorIndex, K priority) {
        if (isEmpty()) return;

        uint16_t oldIndex = values[0];
        indexInHeap[oldIndex] = -1;

        values[0] = contadorIndex;
        keys[0] = priority;
        indexInHeap[contadorIndex] = 0;

        heapDown(0);
    }

    //actualizar prioridad->count+err
    void updateKey(uint16_t contadorIndex, K newPriority) {
        int pos = indexInHeap[contadorIndex];
        if (pos < 0) return;

        K old = keys[pos];
        keys[pos] = newPriority;

        if (newPriority < old) heapUp(pos);
        else heapDown(pos);
    }

    void clear() {
        size = 0;
        for (int i = 0; i < capacity; i++) indexInHeap[i] = -1;
    }
};

#endif