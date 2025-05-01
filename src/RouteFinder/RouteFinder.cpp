#include "RouteFinder.h"
#include <algorithm>
#include <limits>
#include <climits>
#include <vector>
#include <tuple>
#include <unordered_map>
#include "../Graph.h"

using namespace std;

bool RouteFinder::dijkstra(Graph<int>& roadNetwork, int start, int end, vector<int>& path, int& total_time, bool driving = true) {
    Vertex<int>* startVertex = roadNetwork.findVertex(start);
    Vertex<int>* endVertex = roadNetwork.findVertex(end);
    if (!startVertex || !endVertex) return false;

    for (auto v : roadNetwork.getVertexSet()) {
        v->setDist(INF);
        v->setPath(nullptr);
        v->setVisited(false);
    }

    MutablePriorityQueue<Vertex<int>> pq;
    unordered_set<Vertex<int>*> inQueue;

    startVertex->setDist(0);
    pq.insert(startVertex);
    inQueue.insert(startVertex);

    while (!pq.empty()) {
        Vertex<int>* current = pq.extractMin();
        inQueue.erase(current);
        current->setVisited(true);

        if (current == endVertex) break;

        for (Edge<int>* edge : current->getAdj()) {
            Vertex<int>* neighbor = edge->getDest();

            // Se for driving, ignora arestas não dirigíveis
            if (driving && edge->getWeight() < 0) continue;

            int weight = driving ? edge->getWeight() : edge->getWalkingWeight();
            int newDist;
            if (current->getDist() > INF - weight) {
                newDist = INF; // Evita overflow
            } else {
                newDist = current->getDist() + weight;
            }

            if (!neighbor->isVisited() && newDist < neighbor->getDist()) {
                neighbor->setDist(newDist);
                neighbor->setPath(edge);

                if (inQueue.find(neighbor) == inQueue.end()) {
                    pq.insert(neighbor);
                    inQueue.insert(neighbor);
                } else {
                    pq.decreaseKey(neighbor);
                }
            }
        }
    }

    if (endVertex->getDist() >= INF) return false;

    total_time = endVertex->getDist();
    Vertex<int>* current = endVertex;
    while (current != nullptr) {
        path.push_back(current->getInfo());
        current = current->getPath() ? current->getPath()->getOrig() : nullptr;
    }
    reverse(path.begin(), path.end());
    return true;
}

bool RouteFinder::findRouteWithIncludeNode(Graph<int>& roadNetwork, int start, int end, int include,
                            vector<int>& path, int& total_time) {
    vector<int> directPath;
    int directTime;
    if (dijkstra(roadNetwork, start, end, directPath, directTime)) {
        if (find(directPath.begin(), directPath.end(), include) != directPath.end()) {
            path = directPath;
            total_time = directTime;
            return true;
        }
    }

    vector<int> firstLeg, secondLeg;
    int time1, time2;

    if (!dijkstra(roadNetwork, start, include, firstLeg, time1) ||
        !dijkstra(roadNetwork, include, end, secondLeg, time2)) {
        return false;
        }

    path = firstLeg;
    path.insert(path.end(), secondLeg.begin() + 1, secondLeg.end());
    total_time = time1 + time2;
    return true;
}


bool RouteFinder::findHybridRoute(Graph<int>& roadNetwork, const unordered_map<int, bool>& parkingData,
                    int start, int end, int maxWalkTime,
                    const vector<int>& avoidNodes, const vector<pair<int, int>>& avoidSegments,
                    vector<int>& drivingRoute, vector<int>& walkingRoute,
                    int& parkingNode, int& totalTime, string& message,
                    vector<tuple<int, vector<int>, vector<int>, int>>* alternatives = nullptr) {

    // First check if the nodes exist
    if (!roadNetwork.findVertex(start)|| !roadNetwork.findVertex(end)) {
        message = "O no" + to_string(start) + " nao existe";
        return false;
    }

    // Create restricted graph with avoided nodes/segments
    Graph<int> restrictedGraph = roadNetwork;
    for (int node : avoidNodes) restrictedGraph.removeVertex(node);
    for (auto& seg : avoidSegments) {
        restrictedGraph.removeEdge(seg.first, seg.second);
        restrictedGraph.removeEdge(seg.second, seg.first);
    }

    vector<tuple<int, int, int>> validCandidates;  // Within walking time
    vector<tuple<int, int, int>> allCandidates;    // All possible candidates

    for (auto vertex : restrictedGraph.getVertexSet()) {
        int p = vertex->getInfo();

        // Skip if not a parking node or is start/end
        if (p == start || p == end || !parkingData.at(p)) continue;

        // Calculate driving path
        vector<int> drivePath;
        int driveTime;
        if (!dijkstra(restrictedGraph, start, p, drivePath, driveTime, true)) continue;

        // Calculate walking path
        vector<int> walkPath;
        int walkTime;
        if (!dijkstra(restrictedGraph, p, end, walkPath, walkTime, false)) continue;

        allCandidates.emplace_back(p, driveTime, walkTime);
        if (walkTime <= maxWalkTime) {
            validCandidates.emplace_back(p, driveTime, walkTime);
        }
    }

    // If we have valid candidates within walking time, use them
    if (!validCandidates.empty()) {
        auto best = *min_element(validCandidates.begin(), validCandidates.end(),
            [](const auto& a, const auto& b) {
                return (get<1>(a) + get<2>(a)) < (get<1>(b) + get<2>(b));
            });

        parkingNode = get<0>(best);
        dijkstra(restrictedGraph, start, parkingNode, drivingRoute, get<1>(best), true);
        dijkstra(restrictedGraph, parkingNode, end, walkingRoute, get<2>(best), false);
        totalTime = get<1>(best) + get<2>(best);
        return true;
    }

    // If we get here, no valid route was found within walking time constraints
    if (allCandidates.empty()) {
        // Check if there are any parking nodes at all
        bool hasParking = any_of(parkingData.begin(), parkingData.end(),
            [](const auto& p) { return p.second; });

        if (!hasParking) {
            message = "No parking nodes available in the network";
            return false;
        }

        // Check if driving route exists to any parking node
        bool hasDrivingRoute = false;
        for (auto& p : parkingData) {
            if (p.second) {
                vector<int> testPath;
                int testTime;
                if (dijkstra(restrictedGraph, start, p.first, testPath, testTime, true)) {
                    hasDrivingRoute = true;
                    break;
                }
            }
        }

        if (!hasDrivingRoute) {
            message = "Nao existe rota sem estes segmentos/nos.";
            return false;
        }

        // Check if walking route exists from any parking node
        bool hasWalkingRoute = false;
        for (auto& p : parkingData) {
            if (p.second) {
                vector<int> testPath;
                int testTime;
                if (dijkstra(restrictedGraph, p.first, end, testPath, testTime, false)) {
                    hasWalkingRoute = true;
                    break;
                }
            }
        }

        if (!hasWalkingRoute) {
            message = "Nao existe rota sem estes segmentos/nos.";
            return false;
        }

        message = "Nao existe rota sem estes segmentos/nos.";
    } else {
        // We have candidates but walking time exceeds maximum
        int minWalkTime = INT_MAX;
        for (const auto& c : allCandidates) {
            if (get<2>(c) < minWalkTime) minWalkTime = get<2>(c);
        }
        message = "Minimo tempo necessario (" + to_string(minWalkTime) +
                 ") maximo tempo permitido (" + to_string(maxWalkTime) + ")";
    }

    // If alternatives pointer was provided, find them
    if (alternatives != nullptr) {
        *alternatives = findAlternativeRoutes(roadNetwork, parkingData, start, end, maxWalkTime,
                                           avoidNodes, avoidSegments);
    }

    return false;
}

vector<tuple<int, vector<int>, vector<int>, int>> RouteFinder::findAlternativeRoutes(
    Graph<int>& roadNetwork, const unordered_map<int, bool>& parkingData,
    int start, int end, int originalMaxWalkTime,
    const vector<int>& avoidNodes, const vector<pair<int, int>>& avoidSegments) {

    vector<tuple<int, vector<int>, vector<int>, int>> alternatives;
    vector<tuple<int, int, int>> allCandidates; // {parkingNode, driveTime, walkTime}

    // Criar grafo restrito
    Graph<int> restrictedGraph = roadNetwork;
    for (int node : avoidNodes) restrictedGraph.removeVertex(node);
    for (auto& seg : avoidSegments) {
        restrictedGraph.removeEdge(seg.first, seg.second);
        restrictedGraph.removeEdge(seg.second, seg.first);
    }

    // Coletar TODOS os candidatos possíveis, sem considerar maxWalkTime
    for (auto vertex : restrictedGraph.getVertexSet()) {
        int p = vertex->getInfo();

        // Verificar se é um nó de estacionamento válido
        if (p == start || p == end || !parkingData.at(p))
            continue;

        vector<int> drivePath;
        int driveTime;
        if (!dijkstra(restrictedGraph, start, p, drivePath, driveTime, true)) continue;

        vector<int> walkPath;
        int walkTime;
        if (!dijkstra(restrictedGraph, p, end, walkPath, walkTime, false)) continue;

        allCandidates.emplace_back(p, driveTime, walkTime);
    }

    sort(allCandidates.begin(), allCandidates.end(),
        [](const auto& a, const auto& b) {
            return (get<1>(a) + get<2>(a)) < (get<1>(b) + get<2>(b));
        });

    for (int i = 0; i < min(2, (int)allCandidates.size()); i++) {
        auto& candidate = allCandidates[i];
        int p = get<0>(candidate);
        vector<int> drivePath, walkPath;
        int driveTime, walkTime;

        dijkstra(restrictedGraph, start, p, drivePath, driveTime, true);
        dijkstra(restrictedGraph, p, end, walkPath, walkTime, false);

        alternatives.emplace_back(p, drivePath, walkPath, driveTime + walkTime);
    }

    return alternatives;
}

