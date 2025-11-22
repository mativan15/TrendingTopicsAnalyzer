#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <signal.h>
#include <chrono>
#include <atomic>
#include "estructuras/json.h"
#include "gestorBloques/GestorBloques.h"

/*#include <queue>
#include <unordered_map>  // SOLO TEST PARA COMPARAR
*/ 
using namespace std;


atomic<bool> running(true);
int server_fd_global = -1;

void signalCerrar(int sig) {
    cout << "\n[Backend] Señal recibida Apagando todo..." << endl;
    running = false;
    if (server_fd_global != -1) {
        close(server_fd_global);
    }
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[4096] = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket failed");
        return 1;
    }
    server_fd_global = server_fd;

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(8081);

    if (::bind(server_fd, reinterpret_cast<struct sockaddr*>(&address), sizeof(address)) < 0) {
        perror("bind failed");
        return 1;
    }

    if (::listen(server_fd, 3) < 0) {
        perror("listen failed");
        return 1;
    }

    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, signalCerrar);
    signal(SIGTERM, signalCerrar);

    cout << "Backend C++ escuchando en puerto 8081...ok" << endl;
    GestorBloques gestor(5000); // capacidad de bloques space-saving IMPORTANTE

    //unordered_map<string, int> DiccionarioPrueba; // SOLO TEST PARA COMPARAR
    //priority_queue<pair<int, string>> TopKPrueba;
    while (running) {
        new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            perror("accept failed");
            continue;
        }
        
        if (!running) {
            close(new_socket);
            break;
        }
        
        string data = "";
        int openBraces = 0;
        bool started = false;

        while (true) {
            memset(buffer, 0, sizeof(buffer));
            ssize_t n = read(new_socket, buffer, sizeof(buffer));
            if (n <= 0) break;

            for (int i = 0; i < n; i++) {
                char c = buffer[i];
                data.push_back(c);

                if (c == '{') {
                    openBraces++;
                    started = true;
                } else if (c == '}') {
                    openBraces--;
                }
            }

            if (started && openBraces == 0) break;
        }
        cout << "Recibido\n"; //: " << data << endl;
        // Detectar si es inserción o consulta
        //bool esConsulta = (data.find("\"consulta\"") != string::npos);

        

        if (!data.empty()) {
//consultaa
            if (data.find("\"consultar\"") != string::npos) {
                string response = R"({"status":"ok","message":"Consulta recibida","result":"pendiente aun"})";
                struct ConsultaParams params;
                if (parseConsultaJSON(data, params)) {
                    TrendingTopic** resultados = nullptr;
                    auto start_cons = chrono::high_resolution_clock::now();
                    gestor.consultarTrendingTopics(params.k, params.inicio, params.fin, resultados);
                    auto end_cons = chrono::high_resolution_clock::now();
                    auto dur_cons = chrono::duration_cast<chrono::milliseconds>(end_cons - start_cons).count();
                    cout << "---Tiempo q tomo la consulta: " << dur_cons<<endl;
                    /*cout << "\n------------------------\nresultado de estructura de prueba:" << endl; // SOLO TEST
                    for(int i = 0; i < params.k; i++) {
                        cout << "- Topic: " << TopKPrueba.top().second << ", Frequency: " << TopKPrueba.top().first << endl;
                        TopKPrueba.pop();
                    }*/

                    if (resultados != nullptr) {
                        response = R"({"status":"ok","message":"Consulta procesada","results":[)";
                        cout << "\n------------------------\nresultado de mi Space Saving`:" << endl;
                        for (uint16_t i = 0; i < params.k; i++) {
                            if (resultados[i] == nullptr) break;
                            cout << "  - Topic: " << resultados[i]->topic << ", Frequency: " << resultados[i]->frequency << endl;
                            string topicEscapado = escapeJSON(resultados[i]->topic);

                            response += "{\"topic\":\"" + topicEscapado + "\",\"frequency\":" + to_string(resultados[i]->frequency) + "}";

                            if (i < params.k - 1 && resultados[i + 1] != nullptr) {
                                response += ",";
                            }
                        }

                        response += "],\"tiempo_consulta\":" + to_string(dur_cons) + "}";

                        for (uint16_t i = 0; i < params.k; i++) {
                            if (resultados[i] == nullptr) break;
                            free(resultados[i]->topic);
                            delete resultados[i];
                        }
                        delete[] resultados;
                    } else {
                        response = R"({"status":"error","message":"No se encontraron resultados para la consulta."})";
                    }
                } else {
                    response = R"({"status":"error","message":"Error al parsear parametros de consulta."})";
                }
                
                string packet = response + "\n<END>\n";
                send(new_socket, packet.c_str(), packet.size(), 0);

            } else if (data.find("\"insertar\"") != string::npos){
            //INSERCION
                char** tokensArray = nullptr;
                int tokenCount = 0;
                tokensArray = parseTokens(data, tokenCount);
                cout << "Tokens extraídos (" << tokenCount << "):" << endl;

                auto start_insert = chrono::high_resolution_clock::now();
                gestor.insertar(tokensArray, tokenCount);
                auto end_insert = chrono::high_resolution_clock::now();
                auto dur_insert = chrono::duration_cast<chrono::milliseconds>(end_insert - start_insert).count();
                cout << "---Tiempo q tomo la insercion: " << dur_insert<<endl;
                /*
                for (int i = 0; i < tokenCount; i++) {
                    DiccionarioPrueba[string(tokensArray[i])]++; // SOLO TEST PARA COMPARAR
                }
                for(auto& entry : DiccionarioPrueba) {
                    TopKPrueba.push({entry.second, entry.first});
                }*/

                if (tokensArray != nullptr && tokenCount > 0) {
                    for (int i = 0; i < tokenCount; i++) {
                        delete[] tokensArray[i];
                    }
                    delete[] tokensArray;
                } else {
                    cerr << "tokensArray es nullptr o tokenCount es 0" << endl;
                }
                tokensArray = nullptr;
                
                string response = string(
                    "{\"status\":\"ok\","
                    "\"message\":\"Datos insertados correctamente en el bloque " + to_string(gestor.getBloqueActual()) + "\","
                    "\"tiempo_actual\":" + to_string(gestor.getCantidadBloques()) + ","
                    "\"tiempo_insercion\":" + to_string(dur_insert) + ","
                    "\"token_count\":" + to_string(tokenCount) +
                    "}"
                );
                tokenCount = 0;
                send(new_socket, response.c_str(), response.size(), 0);
            } else {
                string response = R"({"status":"error","message":"Tipo de solicitud desconocido."})";
                send(new_socket, response.c_str(), response.size(), 0);
            }
        } else {
            //cout << "Connection close" << endl;
            close(new_socket);
            continue;
        }

        close(new_socket);
        memset(buffer, 0, sizeof(buffer));
    }
    close(server_fd);
    cerr << "\n[Backend] Finalizado correctamente :) \n";
    return 0;
}