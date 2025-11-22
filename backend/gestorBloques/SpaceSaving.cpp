#include "SpaceSaving.h"
#include <limits>

using namespace std;

template<typename K>
SpaceSaving<K>::SpaceSaving(uint16_t capacidad_) : capacidad(capacidad_), usados(0) {
    if (capacidad == 0) capacidad = 2;
    contadores = new Contador<K>[capacidad];
    for (uint16_t i = 0; i < capacidad; i++) {
        contadores[i].topicID = 0;
        contadores[i].count = K();
        contadores[i].error = K();
    }

    heap = new MinHeap<K>(capacidad);
    uint32_t mapCap = static_cast<uint32_t>(capacidad) * 2u;
    if (mapCap == 0) mapCap = 2;
    map = new HashMap<uint32_t, uint16_t, false>(mapCap);
}

template<typename K>
SpaceSaving<K>::~SpaceSaving() {
    delete[] contadores;
    delete heap;
    delete map;
}

template<typename K>
bool SpaceSaving<K>::insertar(const uint32_t topic) {
    //0 es dummy
    if (topic == 0) return false;
    uint16_t idx;
    //si ya existe en el hash
    if (map->getValue(topic, idx)) {
        contadores[idx].count++;
        heap->updateKey(idx, contadores[idx].count);
        return true;
    }

    //si no existe pero hay espacio
    if (usados < capacidad) {
        uint16_t newIdx = usados;
        contadores[newIdx].topicID = topic;
        contadores[newIdx].count = static_cast<K>(1);
        contadores[newIdx].error = static_cast<K>(0);
        //inserta
        map->insert(topic, newIdx);
        K priority = static_cast<K>(static_cast<uint64_t>(contadores[newIdx].count) + static_cast<uint64_t>(contadores[newIdx].error));
        heap->insert(newIdx, priority);
        usados++;
        return true;
    }

    //si no existe y no hay espacio
    uint16_t idxMin = static_cast<uint16_t>(heap->getMinValue());
    uint32_t oldTopic = contadores[idxMin].topicID;

    uint64_t oldTotal64 = static_cast<uint64_t>(contadores[idxMin].count) + static_cast<uint64_t>(contadores[idxMin].error);
    if (oldTotal64 > static_cast<uint64_t>(numeric_limits<K>::max()) - 1ULL) { //porsi hay overf
        oldTotal64 = static_cast<uint64_t>(numeric_limits<K>::max()) - 1ULL;
    }

    //elimniar
    if (oldTopic != 0) {
        map->remove(oldTopic);
    }
//actualizar
    contadores[idxMin].topicID = topic;
    contadores[idxMin].error = static_cast<K>(oldTotal64);
    contadores[idxMin].count = static_cast<K>(oldTotal64 + 1ULL);


    map->insert(topic, idxMin);
    K newPriority = static_cast<K>(static_cast<uint64_t>(contadores[idxMin].count) + static_cast<uint64_t>(contadores[idxMin].error));
    heap->updateKey(idxMin, newPriority);
    return true;
}

template<typename K>
uint16_t SpaceSaving<K>::buscarMinimo() const {
    if (usados == 0) return UINT16_MAX;
    return static_cast<uint16_t>(heap->getMinValue());
}

template<typename K>
void SpaceSaving<K>::mostrarResumen() const {
    cout << "---- SpaceSaving ----\n";
    cout << "Capacidad: " << capacidad << ", Usados: " << usados << "\n";
    for (uint16_t i = 0; i < usados; i++) {
        cout << "[ID: " << contadores[i].topicID  << ", Count: " << +contadores[i].count << ", Error: " << +contadores[i].error << "]\n";
    }
    cout << "---------------------\n";
}

//da error si borro
template class SpaceSaving<uint16_t>;
//template class SpaceSaving<uint16_t, true>;