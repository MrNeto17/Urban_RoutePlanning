#include "InputParser.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include "../RouteFinder/RouteFinder.h"
#include "../OutputWriter/OutputWriter.h"
#include "../Graph.h"

using namespace std;

vector<int> InputParser::parseNodes(const string& str) {
    vector<int> nodes;
    if (str.empty() || str == "none") return nodes;

    stringstream ss(str);
    string token;
    while (getline(ss, token, ',')) {
        try {
            token.erase(remove_if(token.begin(), token.end(), ::isspace), token.end());
            if (!token.empty()) nodes.push_back(stoi(token));
        } catch (...) {
            throw runtime_error("ERRO: Valor invalido em AvoidNodes: '" + token + "'");
        }
    }
    return nodes;
}

vector<pair<int, int>> InputParser::parseSegments(const string& str) {
    vector<pair<int, int>> segments;
    if (str.empty() || str == "none") return segments;

    stringstream ss(str);
    string segment;
    while (getline(ss, segment, ')')) {
        size_t start = segment.find('(');
        if (start != string::npos) {
            string content = segment.substr(start + 1);
            size_t comma = content.find(',');
            if (comma != string::npos) {
                try {
                    int from = stoi(content.substr(0, comma));
                    int to = stoi(content.substr(comma + 1));
                    segments.emplace_back(from, to);
                } catch (...) {
                    throw runtime_error("ERRO: Segmento invalido: " + segment + ")");
                }
            }
        }
    }
    return segments;
}

void InputParser::processFileInput(Graph<int>& roadNetwork,
                                 const unordered_map<int, bool>& parkingData) {
    ifstream input("../input.txt");
    if (!input.is_open()) {
        cout << "ERRO: Arquivo input.txt nao encontrado" << endl;
        return;
    }

    string mode, line;
    int start = -1, end = -1, maxWalkTime = 0, includeNode = -1;
    vector<int> avoidNodes;
    vector<pair<int, int>> avoidSegments;

    while (getline(input, line)) {
        line.erase(remove(line.begin(), line.end(), ' '), line.end());
        if (line.empty()) continue;

        size_t colon = line.find(':');
        if (colon == string::npos) continue;

        string key = line.substr(0, colon);
        string value = line.substr(colon + 1);

        if (key == "Mode") mode = value;
        else if (key == "Source") {
            try { start = stoi(value); }
            catch (...) { throw runtime_error("Source invalido"); }
        }
        else if (key == "Destination") {
            try { end = stoi(value); }
            catch (...) { throw runtime_error("Destination invalido"); }
        }
        else if (key == "MaxWalkTime") {
            try { maxWalkTime = stoi(value); }
            catch (...) { cerr << "AVISO: MaxWalkTime é 0" << endl; }
        }
        else if (key == "IncludeNode") {
            if (value.empty() || value == "none") includeNode = -1;
            else {
                try { includeNode = stoi(value); }
                catch (...) { cerr << "AVISO: IncludeNode invalido" << endl; }
            }
        }
        else if (key == "AvoidNodes") {
            try { avoidNodes = parseNodes(value); }
            catch (const exception& e) { cerr << "AVISO: " << e.what() << endl; }
        }
        else if (key == "AvoidSegments") {
            try { avoidSegments = parseSegments(value); }
            catch (const exception& e) { cerr << "AVISO: " << e.what() << endl; }
        }
    }

    if (start == -1 || end == -1) {
        cout << "Erro: Origem/destino nao definidos" << endl;
        return;
    }
    if (!roadNetwork.findVertex(start)) {
        cout << "Erro: No de origem nao existe" << endl;
        return;
    }
    if (!roadNetwork.findVertex(end)) {
        cout << "Erro: No de destino nao existe" << endl;
        return;
    }

    cout << "\n=== VALORES ===" << endl;
    cout << "Modo: " << mode << endl;
    cout << "Origem: " << start << " -> Destino: " << end << endl;

    if (mode == "driving-walking") {
        vector<int> drivingRoute, walkingRoute;
        int parkingNode, totalTime;
        string message;
        vector<tuple<int, vector<int>, vector<int>, int>> alternatives;

        bool found = RouteFinder::findHybridRoute(roadNetwork, parkingData, start, end, maxWalkTime,
                                               avoidNodes, avoidSegments, drivingRoute, walkingRoute,
                                               parkingNode, totalTime, message, &alternatives);

        if (found) {
            cout << "\n=== ROTA ENCONTRADA ===" << endl;
            cout << "Trajeto de carro: ";
            for (int node : drivingRoute) cout << node << " ";
            cout << "\nEstacionamento: " << parkingNode;
            cout << "\nTrajeto a pe: ";
            for (int node : walkingRoute) cout << node << " ";
            cout << "\nTempo total: " << totalTime << " minutos" << endl;
        } else {
            cout << "\n=== RESULTADO ===" << endl;
            cout << message << endl;
        }

        OutputWriter::writeHybridOutput(roadNetwork, "../output.txt", start, end,
                                     drivingRoute, parkingNode, walkingRoute, totalTime,
                                     message, alternatives);
    } else {
        Graph<int> restrictedGraph = roadNetwork;
        for (int node : avoidNodes) restrictedGraph.removeVertex(node);
        for (auto& seg : avoidSegments) {
            restrictedGraph.removeEdge(seg.first, seg.second);
            restrictedGraph.removeEdge(seg.second, seg.first);
        }

        vector<int> bestPath, altPath;
        int bestTime = 0, altTime = 0;
        bool hasRestrictions = !avoidNodes.empty() || !avoidSegments.empty() || (includeNode != -1);

        bool found;
        if (includeNode != -1) {
            found = RouteFinder::findRouteWithIncludeNode(restrictedGraph, start, end, includeNode, bestPath, bestTime);
        } else {
            found = RouteFinder::dijkstra(restrictedGraph, start, end, bestPath, bestTime, true);

            if (!hasRestrictions && found) {
                Graph<int> altGraph = roadNetwork;
                for (size_t i = 0; i < bestPath.size() - 1; i++) {
                    altGraph.removeEdge(bestPath[i], bestPath[i+1]);
                    altGraph.removeEdge(bestPath[i+1], bestPath[i]);
                }

                if (!RouteFinder::dijkstra(altGraph, start, end, altPath, altTime, true)) {
                    Graph<int> altGraph2 = roadNetwork;
                    for (size_t i = 1; i < bestPath.size() - 1; i++) {
                        altGraph2.removeVertex(bestPath[i]);
                    }
                    RouteFinder::dijkstra(altGraph2, start, end, altPath, altTime, true);
                }
            }
        }

        cout << "\n=== RESULTADOS ===" << endl;
        if (found) {
            cout << "Melhor rota (" << bestTime << " min): ";
            for (size_t i = 0; i < bestPath.size(); i++) {
                if (i != 0) cout << " -> ";
                cout << bestPath[i];
            }
            cout << endl;

            if (!altPath.empty() && altTime < INF) {
                cout << "\nRota alternativa (" << altTime << " min): ";
                for (size_t i = 0; i < altPath.size(); i++) {
                    if (i != 0) cout << " -> ";
                    cout << altPath[i];
                }
                cout << endl;
            }
        } else {
            cout << "Nenhuma rota encontrada" << endl;
        }

        OutputWriter::writeOutput(roadNetwork, "../output.txt", start, end,
                               bestPath, bestTime, altPath, altTime, hasRestrictions);
    }
    cout << "\nResultados gravados em output.txt" << endl;
}

void InputParser::processManualInput(Graph<int>& roadNetwork,
                                   const unordered_map<int, bool>& parkingData) {
    showManualInputMenu();

    string input;
    int start, end, maxWalkTime = 0, includeNode = -1;
    vector<int> avoidNodes;
    vector<pair<int, int>> avoidSegments;
    string mode;

    cout << "Modo (driving/driving-walking): ";
    getline(cin, input);
    mode = input.empty() ? "driving" : input;

    cout << "No de origem: ";
    getline(cin, input);
    if (input.empty()) {
        cout << "No de origem e obrigatorio!\n";
        return;
    }
    start = stoi(input);

    cout << "No de destino: ";
    getline(cin, input);
    if (input.empty()) {
        cout << "No de destino e obrigatorio!\n";
        return;
    }
    end = stoi(input);

    if (mode == "driving-walking") {
        cout << "Tempo maximo a pe (minutos): ";
        getline(cin, input);
        maxWalkTime = input.empty() ? 0 : stoi(input);
    } else {
        cout << "No a incluir (ou deixe em branco): ";
        getline(cin, input);
        includeNode = input.empty() ? -1 : stoi(input);
    }

    cout << "Nos a evitar (separados por virgula, ou deixe em branco): ";
    getline(cin, input);
    if (!input.empty()) {
        try {
            avoidNodes = parseNodes(input);
        } catch (const exception& e) {
            cout << "Erro: " << e.what() << endl;
            return;
        }
    }

    cout << "Segmentos a evitar (formato (a,b)(c,d), ou deixe em branco): ";
    getline(cin, input);
    if (!input.empty()) {
        try {
            avoidSegments = parseSegments(input);
        } catch (const exception& e) {
            cout << "Erro: " << e.what() << endl;
            return;
        }
    }

    if (!roadNetwork.findVertex(start)) {
        cout << "Erro: No de origem nao existe!\n";
        return;
    }
    if (!roadNetwork.findVertex(end)) {
        cout << "Erro: No de destino nao existe!\n";
        return;
    }

    if (mode == "driving-walking") {
        vector<int> drivingRoute, walkingRoute;
        int parkingNode, totalTime;
        string message;
        vector<tuple<int, vector<int>, vector<int>, int>> alternatives;

        bool found = RouteFinder::findHybridRoute(roadNetwork, parkingData, start, end, maxWalkTime,
                                               avoidNodes, avoidSegments, drivingRoute, walkingRoute,
                                               parkingNode, totalTime, message, &alternatives);

        if (found) {
            cout << "\n=== RESULTADO ===" << endl;
            cout << "Rota encontrada!" << endl;
            cout << "Trajeto de carro: ";
            for (int node : drivingRoute) cout << node << " ";
            cout << "\nEstacionamento: " << parkingNode;
            cout << "\nTrajeto a pe: ";
            for (int node : walkingRoute) cout << node << " ";
            cout << "\nTempo total: " << totalTime << endl;
        } else {
            cout << "\n=== RESULTADO ===" << endl;
            cout << message << endl;
        }

        OutputWriter::writeHybridOutput(roadNetwork, "../output.txt", start, end,
                                     drivingRoute, parkingNode, walkingRoute, totalTime,
                                     message, alternatives);
    } else {
        Graph<int> restrictedGraph = roadNetwork;
        for (int node : avoidNodes) restrictedGraph.removeVertex(node);
        for (auto& seg : avoidSegments) {
            restrictedGraph.removeEdge(seg.first, seg.second);
            restrictedGraph.removeEdge(seg.second, seg.first);
        }

        vector<int> bestPath, altPath;
        int bestTime = 0, altTime = 0;
        bool hasRestrictions = !avoidNodes.empty() || !avoidSegments.empty() || (includeNode != -1);

        bool found;
        if (includeNode != -1) {
            found = RouteFinder::findRouteWithIncludeNode(restrictedGraph, start, end, includeNode, bestPath, bestTime);
        } else {
            found = RouteFinder::dijkstra(restrictedGraph, start, end, bestPath, bestTime, true);

            if (!hasRestrictions && found) {
                Graph<int> altGraph = roadNetwork;
                for (size_t i = 0; i < bestPath.size() - 1; i++) {
                    altGraph.removeEdge(bestPath[i], bestPath[i+1]);
                    altGraph.removeEdge(bestPath[i+1], bestPath[i]);
                }

                if (!RouteFinder::dijkstra(altGraph, start, end, altPath, altTime, true)) {
                    Graph<int> altGraph2 = roadNetwork;
                    for (size_t i = 1; i < bestPath.size() - 1; i++) {
                        altGraph2.removeVertex(bestPath[i]);
                    }
                    RouteFinder::dijkstra(altGraph2, start, end, altPath, altTime, true);
                }
            }
        }

        cout << "\n=== RESULTADOS ===" << endl;
        if (found) {
            cout << "Melhor rota (" << bestTime << " min): ";
            for (size_t i = 0; i < bestPath.size(); i++) {
                if (i != 0) cout << " -> ";
                cout << bestPath[i];
            }
            cout << endl;

            if (!altPath.empty() && altTime < INF) {
                cout << "\nRota alternativa (" << altTime << " min): ";
                for (size_t i = 0; i < altPath.size(); i++) {
                    if (i != 0) cout << " -> ";
                    cout << altPath[i];
                }
                cout << endl;
            }
        } else {
            cout << "Nenhuma rota encontrada" << endl;
        }

        OutputWriter::writeOutput(roadNetwork, "../output.txt", start, end,
                               bestPath, bestTime, altPath, altTime, hasRestrictions);
    }
    cout << "\nResultados gravados em output.txt" << endl;
}
void InputParser::showManualInputMenu() {
    cout << "\n=== ENTRADA MANUAL ===" << endl;
    cout << "Insira os parametros (deixe em branco para usar o padrão):\n";
}