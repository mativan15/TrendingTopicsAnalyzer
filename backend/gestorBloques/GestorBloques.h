#ifndef GESTOR_BLOQUES_H
#define GESTOR_BLOQUES_H

#include "BloqueTemp.h"
#include "../estructuras/hash.h"
#include "../estructuras/diccionario.h"
#include "../estructuras/json.h"

class GestorBloques {
private:
    BloqueTemp** bloquesArray; //punteros a BloqueTemp
    uint16_t cantidad; //cantidad de bloques almacenados
    uint32_t capacidadBloques; //capacidad de cada bloque sapceSaving(m_b)
    uint32_t bloqueActual;
    uint32_t capacidadArray;  //capacidad actual del array
    Diccionario* diccionario;
    void crearNuevoBloque();
    HashMap<uint32_t, uint64_t> fusionarHash(uint16_t inicio, uint16_t fin);

public:
    GestorBloques(int capacidadBloq);
    ~GestorBloques();

    bool insertar(char **topics, int numTopics);
    bool consultarTrendingTopics(uint32_t k, uint16_t inicio, uint16_t fin, TrendingTopic** &resultados);

    //para debug
    void mostrarDiagnostico();

    uint32_t getBloqueActual() const { return bloqueActual; }
    uint16_t getCantidadBloques() const { return cantidad; }
};

#endif