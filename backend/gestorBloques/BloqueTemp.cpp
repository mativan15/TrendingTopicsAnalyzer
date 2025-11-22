#include "BloqueTemp.h"
//#include <unordered_map> // SOLO TEST
using namespace std;

BloqueTemp::BloqueTemp(uint16_t id, uint16_t capacidad_max) : idBloque(id), algoritmo(capacidad_max) {
    //algoritmo = new SpaceSaving(capacidad);
}
//BloqueTemp::~BloqueTemp() {
    //delete algoritmo;
//}
bool BloqueTemp::insertar(const uint32_t topicID) {
    bool resultado = algoritmo.insertar(topicID);
    //capacidad++;
    return resultado;
    //return true;
}

uint16_t BloqueTemp::getOcupados() const {   return algoritmo.getUsados();}

uint16_t BloqueTemp::getID() const {  return idBloque;}

void BloqueTemp::mostrarResumen() const {
    cout << "[BloqueTemp ID: " << idBloque << "] Resumen Space-Saving:" << endl;
    //algoritmo.mostrarResumen();
}

SpaceSaving<uint16_t>* BloqueTemp::getSpaceSaving() const {
    SpaceSaving<uint16_t>* puntero = const_cast<SpaceSaving<uint16_t>*>(&algoritmo);
    return puntero;
}

// SOLO TEST
/*
void BloqueTemp::compararDebug(const std::unordered_map<uint32_t, uint32_t>& realFreq) const {
    cout << "== Comparación SpaceSaving vs Frecuencias Reales ==" << endl;
    for (uint16_t i = 0; i < algoritmo.usados; i++) {
        uint32_t topic = algoritmo.contadores[i].topicID;
        uint32_t count = algoritmo.contadores[i].count;
        uint32_t error = algoritmo.contadores[i].error;

        auto it = realFreq.find(topic);
        uint32_t real = (it != realFreq.end()) ? it->second : 0;
        bool ok = (count - error <= real && real <= count);

        cout << "[ID: " << topic << "] real=" << real << "  count=" << count << "  error=" << error << "  rango=[" << (count - error) << ", " << count << "]" << "  => " << (ok ? "OK" : "FAIL") << endl;
    }
}*/