#ifndef JSON_H
#define JSON_H

#include <string>
#include <cstring>
#include <iostream>
using namespace std;

struct TrendingTopic {
    char* topic;
    uint32_t frequency;
};

inline char** parseTokens(const string& json, int& count) {
    count = 0;
    size_t keyPos = json.find("\"tokens\"");
    if (keyPos == string::npos) return nullptr;
    //cerr << "tokens encontrado en pos: " << keyPos << endl;
    size_t start = json.find('[', keyPos);
    if (start == string::npos) return nullptr;
    //cerr << "inicio del array '[' encontrado en pos: " << start << endl;
    size_t end = json.find(']', start);
    if (end == string::npos) return nullptr;
    //cerr << "fin del array ']' encontrado en pos: " << end << endl;
    string arr = json.substr(start + 1, end - start - 1);

    //contar tokens
    size_t pos = 0;
    while (true) {
        size_t q1 = arr.find('"', pos);
        if (q1 == string::npos) break;
        size_t q2 = arr.find('"', q1 + 1);
        if (q2 == string::npos) break;
        count++;
        pos = q2 + 1;
    }
    cerr << "Tokens encontrados #: " << count << endl;
    if (count == 0) return nullptr;

    //parsear tokens
    char** tokens = new char*[count];
    pos = 0;
    int index = 0;
    while (index < count) {
        size_t q1 = arr.find('"', pos);
        size_t q2 = arr.find('"', q1 + 1);
        if (q1 == string::npos || q2 == string::npos) break;
        string token = arr.substr(q1 + 1, q2 - q1 - 1);

        char* cstr = new char[token.size() + 1];
        memcpy(cstr, token.c_str(), token.size() + 1);

        tokens[index] = cstr;
        pos = q2 + 1;
        index++;
    }
    return tokens;
}

struct ConsultaParams {
    uint16_t k;
    uint16_t inicio;
    uint16_t fin;
};

inline bool parseConsultaJSON(const string& json, ConsultaParams& params) {
    size_t posK = json.find("\"k\"");
    if (posK == string::npos) return false;
    size_t colonK = json.find(':', posK);
    if (colonK == string::npos) return false;
    unsigned long tmp = stoul(json.substr(colonK + 1));
    params.k = static_cast<uint16_t>(tmp);

    size_t posInicio = json.find("\"inicio\"");
    if (posInicio == string::npos) return false;
    size_t colonInicio = json.find(':', posInicio);
    if (colonInicio == string::npos) return false;
    tmp = stoul(json.substr(colonInicio + 1));
    params.inicio = static_cast<uint16_t>(tmp);

    size_t posFin = json.find("\"fin\"");
    if (posFin == string::npos) return false;
    size_t colonFin = json.find(':', posFin);
    if (colonFin == string::npos) return false;
    tmp = stoul(json.substr(colonFin + 1));
    params.fin = static_cast<uint16_t>(tmp);

    return true;
}


inline string escapeJSON(const string& input) { //sino me da error en front
    string out;
    out.reserve(input.size());
    for (char c : input) {
        switch (c) {
            case '\"': out += "\\\""; break;
            case '\\':  out += "\\\\" ;  break;
            case '\b': out += "\\b"; break;
            case '\f': out += "\\f"; break;
            case '\n':  out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t";  break;
            default:
                if ((unsigned char)c < 0x20) {
                    char buf[7];
                    snprintf(buf, sizeof(buf), "\\u%04x", (unsigned char)c);
                    out += buf;
                } else {
                    out += c;
                }
        }
    }
    return out;
}

#endif