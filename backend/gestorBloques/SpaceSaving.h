#ifndef SPACESAVING_H
#define SPACESAVING_H

#include <cstdint>
#include <iostream>
#include "../estructuras/hash.h"
#include "../estructuras/minHeap.h"

template<typename K = uint16_t>
struct Contador {
    uint32_t topicID;
    K count;
    K error;
};

template<typename K = uint16_t>
class SpaceSaving {
private:
    

    Contador<K>* contadores; 
    uint16_t capacidad;  //max contadores = m IMPORTANTE
    uint16_t usados; //cuantos ocupados actualmente

    MinHeap<K>* heap;//para encontrar el min de (count+error)
    HashMap<uint32_t, uint16_t, false>* map; //mapa topicID -> contadores[]
    
public:
//friend class BloqueTemp; //   SOLO TEST
    SpaceSaving(uint16_t capacidad_);
    ~SpaceSaving();
    bool insertar(const uint32_t topic);
    uint16_t buscarMinimo() const;


    void mostrarResumen() const;

    uint16_t getUsados() const { return usados; }
    uint16_t getCapacidad() const { return capacidad; }
    inline const Contador<>& getContador(uint16_t j) const {return contadores[j];}
};

#endif