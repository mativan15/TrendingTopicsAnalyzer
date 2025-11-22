#ifndef BLOQUETEMP_H
#define BLOQUETEMP_H

#include <iostream>
#include <cmath>
#include <cstdlib>
#include "SpaceSaving.h"


class BloqueTemp {
private:
    uint16_t idBloque;
    SpaceSaving<uint16_t> algoritmo;

public:
    BloqueTemp(uint16_t id, uint16_t capacidad_max);
    //~BloqueTemp();

    bool insertar(const uint32_t topicID);

    //int obtenerFrecuencia(const uint32_t topicID);
    uint16_t getOcupados() const;
    uint16_t getID() const;

    //para consultas
    SpaceSaving<uint16_t>* getSpaceSaving() const ;

    void mostrarResumen() const;

    //void compararDebug(const std::unordered_map<uint32_t, uint32_t>& realFreq) const; // SOLO TEST
};

#endif