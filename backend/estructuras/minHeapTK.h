#ifndef MINHEAPTK_H
#define MINHEAPTK_H

#include <cstdint>

struct HeapNode {
    uint32_t topic;
    uint32_t freq;
};

class MinHeapTopK {
private:
    HeapNode* arr;
    uint16_t size;
    uint16_t capacity;

    inline int left(int i) const { return 2*i + 1; }
    inline int right(int i) const { return 2*i + 2; }
    inline int parent(int i) const { return (i-1)/2; }

    void heapDown(int i) {
        while (true) {
            int l = left(i);
            int r = right(i);
            int smallest = i;

            if (l < size && arr[l].freq < arr[smallest].freq) smallest = l;
            if (r < size && arr[r].freq < arr[smallest].freq) smallest = r;

            if (smallest == i) break;

            HeapNode tmp = arr[i];
            arr[i] = arr[smallest];
            arr[smallest] = tmp;

            i = smallest;
        }
    }

    void heapUp(int i) {
        while (i > 0 && arr[parent(i)].freq > arr[i].freq) {
            HeapNode tmp = arr[i];
            arr[i] = arr[parent(i)];
            arr[parent(i)] = tmp;
            i = parent(i);
        }
    }

public:

    MinHeapTopK(uint16_t k) : size(0), capacity(k) {
        arr = new HeapNode[k];
    }

    ~MinHeapTopK() {
        delete[] arr;
    }

    bool isFull() const { return size == capacity; }

    void push(uint32_t topic, uint32_t freq) {
        if (!isFull()) {
            arr[size] = {topic, freq};
            heapUp(size);
            size++;
        } else {
            if (freq > arr[0].freq) {
                arr[0] = {topic, freq};
                heapDown(0);
            }
        }
    }

    uint16_t getSize() const { return size; }
    HeapNode operator[](int i) const { return arr[i]; }
};
#endif // 