#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <unordered_map>
#include "FileParser.h"
#include "../Graph.h"

using namespace std;

unordered_map<string, int> FileParser::loadLocationMappings(
    const string& filename, unordered_map<int, bool>& parkingData) {

    unordered_map<string, int> codeToId;
    ifstream file(filename);
    if (!file.is_open()) throw runtime_error("ERRO: Não foi possível abrir " + filename);
    string line;
    getline(file, line); // Skip header

    while (getline(file, line)) {
        stringstream ss(line);
        vector<string> tokens;
        string token;

        while (getline(ss, token, ',')) tokens.push_back(token);

        if (tokens.size() >= 4) {
            try {
                int id = stoi(tokens[1]);
                string code = tokens[2];
                bool isParking = (tokens[3] == "1");
                codeToId[code] = id;
                parkingData[id] = isParking;
            } catch (...) {
                throw runtime_error("ERRO: Formato inválido no arquivo de localizações: " + line);
            }
        }
    }
    return codeToId;
}


void FileParser::loadGraph(Graph<int>& roadNetwork, const string& filename,
                         const unordered_map<string, int>& codeToId) {

    ifstream file(filename);
    if (!file.is_open()) throw runtime_error("ERRO: Não foi possível abrir " + filename);

    string line;
    getline(file, line); // Skip header

    while (getline(file, line)) {
        line.erase(remove(line.begin(), line.end(), ' '), line.end());
        if (line.empty()) continue;

        size_t pos1 = line.find(',');
        size_t pos2 = line.find(',', pos1 + 1);
        size_t pos3 = line.find(',', pos2 + 1);

        if (pos1 == string::npos || pos2 == string::npos || pos3 == string::npos) {
            cerr << "AVISO: Formato inválido - linha ignorada: " << line << endl;
            continue;
        }

        string fromCode = line.substr(0, pos1);
        string toCode = line.substr(pos1 + 1, pos2 - pos1 - 1);
        string drivingStr = line.substr(pos2 + 1, pos3 - pos2 - 1);
        string walkingStr = line.substr(pos3 + 1);

        if (!codeToId.count(fromCode) || !codeToId.count(toCode)) {
            cerr << "AVISO: Código não mapeado - linha ignorada: " << fromCode << " ou " << toCode << endl;
            continue;
        }

        int fromId = codeToId.at(fromCode);
        int toId = codeToId.at(toCode);
        int driving, walking;

        try {
            driving = (drivingStr == "X") ? -1 : max(1, stoi(drivingStr));
            walking = max(1, stoi(walkingStr));
        } catch (...) {
            cerr << "AVISO: Valores inválidos - linha ignorada: " << line << endl;
            continue;
        }

        roadNetwork.addVertex(fromId);
        roadNetwork.addVertex(toId);

        if (roadNetwork.addBidirectionalEdge(fromId, toId, driving)) {
            Vertex<int>* fromVertex = roadNetwork.findVertex(fromId);
            Vertex<int>* toVertex = roadNetwork.findVertex(toId);

            if (fromVertex && toVertex) {
                for (auto edge : fromVertex->getAdj()) {
                    if (edge->getDest()->getInfo() == toId) {
                        edge->setDrivable(driving > 0);
                        edge->setWalkingWeight(walking);
                        if (edge->getReverse()) {
                            edge->getReverse()->setDrivable(driving > 0);
                            edge->getReverse()->setWalkingWeight(walking);
                        }
                        break;
                    }
                }
            }
        }
    }
}