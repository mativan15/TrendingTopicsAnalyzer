#include "GestorBloques.h"
#include "../estructuras/hash.h"
#include "../estructuras/diccionario.h"
#include "../estructuras/minHeapTK.h"
//#include <unordered_map>  // SOLO TEST
using namespace std;

GestorBloques::GestorBloques(int capacidadBloq) : bloquesArray(nullptr),  cantidad(0),  capacidadBloques(capacidadBloq),bloqueActual(-1),capacidadArray(0), diccionario(nullptr){
    try {
        diccionario = new Diccionario();
    } catch (const bad_alloc& e) {
        cerr << "[GestorBloques] Error allocating diccionario: " << e.what() << endl;
        diccionario = nullptr;
    }
}

GestorBloques::~GestorBloques() {
    cout << "[GestorBloques] Liberando memoria, destructor invocado" << endl;
    if (bloquesArray != nullptr) {
        for (uint32_t i = 0; i < cantidad; i++) {
            if (bloquesArray[i] != nullptr) {
                delete bloquesArray[i];
                bloquesArray[i] = nullptr;
            }
        }
        delete[] bloquesArray;
        bloquesArray = nullptr;
    }
    if (diccionario != nullptr) {
        delete diccionario;
        diccionario = nullptr;
    }
}

void GestorBloques::crearNuevoBloque() {
    if (capacidadArray == 0) {
        capacidadArray = 4;
        try {
            bloquesArray = new BloqueTemp*[capacidadArray];
        } catch (const bad_alloc& e) {
            cerr << "[GestorBloques] Error allocating bloquesArray: " << e.what() << endl;
            bloquesArray = nullptr;
            return;
        }
        for (uint32_t i = 0; i < capacidadArray; i++) {
            bloquesArray[i] = nullptr;
        }
    } else if (cantidad >= capacidadArray) {
        uint32_t nuevaCapacidad = capacidadArray * 2;
        BloqueTemp** nuevoArray = nullptr;
        try {
            nuevoArray = new BloqueTemp*[nuevaCapacidad];
        } catch (const bad_alloc& e) {
            cerr << "[GestorBloques] Error reallocating bloquesArray: " << e.what() << endl;
            return;
        }
        for (uint32_t i = 0; i < cantidad; i++) {
            nuevoArray[i] = bloquesArray[i];
        }
        for (uint32_t i = cantidad; i < nuevaCapacidad; i++) {
            nuevoArray[i] = nullptr;
        }
        delete[] bloquesArray;
        bloquesArray = nuevoArray;
        capacidadArray = nuevaCapacidad;
    }
    try {
        bloquesArray[cantidad] = new BloqueTemp(cantidad, capacidadBloques);
    } catch (const bad_alloc& e) {
        cerr << "[GestorBloques] Error allocating new BloqueTemp: " << e.what() << endl;
        bloquesArray[cantidad] = nullptr;
        return;
    }
    bloqueActual = cantidad;
    cantidad++;
}

bool GestorBloques::insertar(char **topics, int numTopics) {
    //static std::unordered_map<uint32_t, uint32_t> debugFreq;  // SOLO TEST
    //if (bloqueActual == -1 || bloquesArray[bloqueActual]->getOcupados() >= capacidadBloques) {
    if (topics == nullptr || numTopics <= 0) {
        cerr << "[GestorBloques] Inserción inválida: topics es nullptr o numTopics <= 0" << endl;
        return false;
    }
    crearNuevoBloque();
        if (bloqueActual < 0 || bloquesArray == nullptr || bloquesArray[bloqueActual] == nullptr) {
        cerr << "[GestorBloques] No se pudo crear un nuevo bloque para la inserción" << endl;
        return false;
    }

    int exitos = 0;
    int fallidos = 0;

    for (int i = 0; i < numTopics; i++) {
        if (topics[i] == nullptr) {
            cerr << "[GestorBloques] Topic nulo en posición " << i << ", se omite." << endl;
            fallidos++;
            continue;
        }
        uint32_t topicID = diccionario->insertar(topics[i]);
        //debugFreq[topicID]++; // SOLO TEST
        if (topicID == 0) {
            cerr << "[GestorBloques] Error al insertar topic en diccionario (topic == 0): " << topics[i] << endl;
            fallidos++;
            continue;
        }
        bool inserted = bloquesArray[bloqueActual]->insertar(topicID);
        if (inserted) {
            exitos++;
        } else {
            fallidos++;
        }
    }
    
    cout << "[GestorBloques] Inserción completada: " << exitos << " éxitos, " << fallidos << " fallidos." << endl;
    mostrarDiagnostico();
    //bloquesArray[bloqueActual]->compararDebug(debugFreq); // SOLO TEST
    return exitos > 0;
}

void GestorBloques::mostrarDiagnostico() {
    cout << "[GestorBloques] Diagnóstico:" << endl;
    cout << "  Cantidad de bloques: " << cantidad << endl;
    cout << "  Capacidad por bloque: " << capacidadBloques << endl;
    if (diccionario != nullptr) {
        cout << "  Diccionario:"<< endl;
        diccionario->mostrarEstado();
    } else {
        cout << "  Diccionario no inicializado." << endl;
    }
    cout << "  Bloque actual index : " << bloqueActual << endl;
    if (bloquesArray != nullptr) {
        for (int i = 0; i < cantidad; i++) {
            if (bloquesArray[i] != nullptr) {
                //cout << "    - Bloque ID: " << bloquesArray[i]->getID() << ", Ocupados: " << bloquesArray[i]->getOcupados() << endl;
                bloquesArray[i]->mostrarResumen();
            } else {
                cout << "    - Bloque " << i << " es nullptr" << endl;
            }
        }
    } else {
        cout << "  bloquesArray no inicializado." << endl;
    }
    cout << "-------" << endl;
}

bool GestorBloques::consultarTrendingTopics(uint32_t k, uint16_t inicio, uint16_t fin, TrendingTopic** &resultados) {
    if (cantidad == 0 || bloquesArray == nullptr) {
        cout << "[GestorBloques] No hay bloques para consultar." << endl;
        return false;
    }

    //fusionar hashes
    // ahora la fusión guarda count yerror empaquetados en 64
    HashMap<uint32_t, uint64_t> fusion = fusionarHash(inicio, fin);

    uint32_t cap = fusion.getCapacity();
    uint32_t nElem = fusion.getSize();
    if (nElem == 0) {
        resultados = nullptr;
        return false;
    }

    uint32_t topK = (k < nElem) ? k : nElem;
    resultados = new TrendingTopic*[topK];

    
    if (topK < 1024) {//MinHeap optimizado

        //arreglo temporal de candidatos a topK
        struct Candidato {
            uint32_t topicID;
            uint32_t count;
            uint32_t err;
        };

        Candidato* tmp = new Candidato[nElem];
        uint32_t w = 0;

        for (uint32_t i = 0; i < cap; i++) {
            const HashEntry<uint32_t,uint64_t>& entry = fusion.entryAt(i);
            if (!entry.used) continue;

            uint64_t packed = entry.value;
            uint32_t count = (uint32_t)(packed >> 32);
            uint32_t err   = (uint32_t)(packed & 0xffffffffu);

            tmp[w].topicID = entry.key;
            tmp[w].count   = count;
            tmp[w].err     = err;
            w++;
        }

        //crear MinHeap
        MinHeapTopK mh(topK);

        for (uint32_t i = 0; i < w; i++) {
            uint32_t score = tmp[i].count; // prioridad por count
            mh.push(tmp[i].topicID, score);
        }

        //extraer topK
        for (uint32_t j = 0; j < topK; j++) {
            auto node = mh[j];
            const char* palabra = diccionario->obtenerPalabra(node.topic);

            resultados[j] = new TrendingTopic();
            resultados[j]->topic = palabra ? strdup(palabra) : strdup("??");
            resultados[j]->frequency = node.freq;
        }

        delete[] tmp;
    }
    else {//Quickselect
         //construye arreglo temporal con count y error y usaremos score = count - error
        struct TempTT { char* topic; uint32_t count; uint32_t err; };

        TempTT* arr = new TempTT[nElem];
        uint32_t idx = 0;

        for (uint32_t i = 0; i < cap; i++) {
            const HashEntry<uint32_t,uint64_t> &entry = fusion.entryAt(i);
            if (!entry.used) continue;

            uint64_t packed = entry.value;
            uint32_t count = (uint32_t)(packed >> 32);
            uint32_t err = (uint32_t)(packed & 0xffffffffu);

            const char* palabra = diccionario->obtenerPalabra(entry.key);
            arr[idx].topic = palabra ? strdup(palabra) : strdup("<?>");
            arr[idx].count = count;
            arr[idx].err = err;
            idx++;
        }

        //idx debería ser igual a nElem
        if (idx != nElem) {
            nElem = idx;
            if (topK > nElem) topK = nElem;
        }

        //alg quickselect usando score = count - err
        int left = 0;
        int right = (int)nElem - 1;
        uint32_t kIndex = (topK == 0) ? 0 : (topK - 1);

        while (left < right) {
            uint32_t pivotScore = (arr[(left + right) / 2].count > arr[(left + right) / 2].err)? (arr[(left + right) / 2].count - arr[(left + right) / 2].err) : 0u;
            int i = left;
            int j = right;

            while (i <= j) {
                uint32_t si = (arr[i].count > arr[i].err) ? (arr[i].count - arr[i].err) : 0u;
                uint32_t sj = (arr[j].count > arr[j].err) ? (arr[j].count - arr[j].err) : 0u;

                while (si > pivotScore) { i++; if (i>right) break; si = (arr[i].count>arr[i].err)?(arr[i].count-arr[i].err):0u; }
                while (sj < pivotScore) { j--; if (j<left) break; sj = (arr[j].count>arr[j].err)?(arr[j].count-arr[j].err):0u; }

                if (i <= j) {
                    TempTT tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp;
                    i++; j--;
                }
            }

            if (kIndex <= (uint32_t)j) {
                right = j;
            } else if (kIndex >= (uint32_t)i) {
                left = i;
            } else {
                break;
            }
        }

        //los primeros topK elementos
        for (uint32_t i = 0; i < topK; i++) {
            resultados[i] = new TrendingTopic();
            resultados[i]->topic = strdup(arr[i].topic);
            resultados[i]->frequency = arr[i].count;
        }

        // liberar arr
        for (uint32_t i = 0; i < nElem; i++) {
            if (arr[i].topic) free(arr[i].topic);
        }
        delete[] arr;
    }

    return true;
}

HashMap<uint32_t, uint64_t> GestorBloques::fusionarHash(uint16_t inicio, uint16_t fin) {
    // estimacion para reducir rehashes
    const uint32_t estimadoUnicosBloque = 15000u;
    uint32_t numBloques = (inicio >= fin) ? (uint32_t)(inicio - fin + 1) : 1u;
    uint32_t expected = estimadoUnicosBloque * numBloques;
    uint32_t capInit = expected + expected/3 + 16;
    if (capInit < 1024) capInit = 1024;

    HashMap<uint32_t, uint64_t> acumulado(capInit);
    if (cantidad == 0) return acumulado;
    int idxInicio = bloqueActual - (inicio - 1);
    int idxFin    = bloqueActual - (fin - 1);

    if (idxInicio < 0) idxInicio = 0;
    if (idxFin < 0) idxFin = 0;
    if (idxInicio >= cantidad) idxInicio = cantidad - 1;
    if (idxFin >= cantidad) idxFin = cantidad - 1;
    if (idxFin > idxInicio) swap(idxFin, idxInicio);

    for (int i = idxInicio; i >= idxFin; i--) {
        if (bloquesArray[i] == nullptr) continue;
        SpaceSaving<uint16_t>* ss = bloquesArray[i]->getSpaceSaving();
        if (!ss) continue;

        uint16_t usados = ss->getUsados();
        for (uint16_t j = 0; j < usados; j++) {
            const auto& c = ss->getContador(j);
            uint32_t id = c.topicID;
            uint32_t freq = c.count;
            uint32_t err = c.error;

            uint64_t existing;
            if (acumulado.getValue(id, existing)) {
                uint32_t prevCount = (uint32_t)(existing >> 32);
                uint32_t prevErr = (uint32_t)(existing & 0xffffffffu);
                uint64_t newPacked = (((uint64_t)prevCount + (uint64_t)freq) << 32) |  ((uint64_t)prevErr + (uint64_t)err);
                acumulado.insert(id, newPacked);
            } else {
                uint64_t packed = (((uint64_t)freq) << 32) | ((uint64_t)err);
                acumulado.insert(id, packed);
            }
        }
    }

    return acumulado;
}