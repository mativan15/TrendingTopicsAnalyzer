#ifndef DICCIONARIO_H
#define DICCIONARIO_H
#include <cstdlib>
#include <iostream>
#include "../estructuras/hash.h"
using namespace std;

class Diccionario {
private:
    HashMap<uint32_t, uint32_t>* mapa;
    uint32_t capacidad;
    uint32_t contadorIDs;

    char** palabras;
    uint32_t capacidadPalabras;
    
    void asegurarCapacidadPalabras() {
        if (contadorIDs + 1 < capacidadPalabras) return;
        uint32_t nuevaCap = capacidadPalabras * 2;
        char** nuevoArr = new char*[nuevaCap];
        for (uint32_t i = 0; i < capacidadPalabras; i++) nuevoArr[i] = palabras[i];
        for (uint32_t i = capacidadPalabras; i < nuevaCap; i++) nuevoArr[i] = nullptr;
        delete[] palabras;
        palabras = nuevoArr;
        capacidadPalabras = nuevaCap;
    }

    char* copiarCadena(const char* src) {
        size_t n = 0;
        while (src[n] != '\0') n++;
        char* dst = new char[n+1];
        for (size_t i = 0; i <= n; i++) dst[i] = src[i];
        return dst;
    }

    uint32_t hashString(const char* str) const {
        uint32_t hash = 5381;
        int c;
        while ((c = *str++)) {
            hash = ((hash << 5) + hash) + c;
        }
        return hash;
    }

public:
    Diccionario(int capacidad_ = sizeof(uint32_t) * 1000) {
        capacidad = capacidad_;
        contadorIDs = 0;
        mapa = new HashMap<uint32_t, uint32_t>(capacidad);
        capacidadPalabras = 1024;
        palabras = new char*[capacidadPalabras];
        for (uint32_t i = 0; i < capacidadPalabras; i++) palabras[i] = nullptr;
    }

    ~Diccionario() {
        for (uint32_t i = 1; i <= contadorIDs; i++) {
            if (palabras[i] != nullptr) delete[] palabras[i];
        }
        delete[] palabras;
        delete mapa;
    }

    uint32_t insertar(const char* palabra) {
        uint32_t key = hashString(palabra);
        uint32_t& valor = (*mapa)[key]; 
        if (valor == 0) {        // 0 nunca será un ID válido
            contadorIDs++;
            valor = contadorIDs;
            //cerr << "[Diccionario] era 0. Nueva palabra insertada: " << palabra << " con ID: " << valor << endl;
            asegurarCapacidadPalabras();
            palabras[contadorIDs] = copiarCadena(palabra);
        }
        return valor;
    }

    int obtenerID(const char* palabra) const {
        uint32_t key = hashString(palabra);
        int idx = mapa->find(key);
        if (idx == -1) return -1;
        return (*mapa)[key];
    }

    bool existe(const char* palabra) const {
        uint32_t key = hashString(palabra);
        return mapa->find(key) != -1;
    }
    uint32_t getTotalPalabras() const {
        return contadorIDs;
    }

    const char* obtenerPalabra(uint32_t topicID) const {
        if (topicID == 0 || topicID > contadorIDs) return nullptr;
        return palabras[topicID];
    }

    void limpiar() {
        mapa->clear();
        contadorIDs = 0;
        for (uint32_t i = 0; i < capacidadPalabras; i++) {
            if (palabras[i] != nullptr) {
                delete[] palabras[i];
                palabras[i] = nullptr;
            }
        }
    }

    void mostrarEstado() const {
        cout << "[Diccionario] Capacidad lógica inicial declarada: " << capacidad << "\n";
        cout << "[Diccionario] Total de temas registrados: " << contadorIDs << "\n";
        cout << "[Diccionario] HashMap capacidad real: " << mapa->getCapacity() << "\n";
        cout << "[Diccionario] HashMap entradas usadas: " << mapa->getSize() << "\n";
    }
};

#endif // DICCIONARIO_H