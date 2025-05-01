/**
 * @file OutputWriter.cpp
 * @brief Implementation of route writing methods
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <sstream>
#include <limits>
#include <climits>
#include <tuple>
#include <unordered_map>
#include "../Graph.h"
#include "../MutablePriorityQueue.h"
#include "OutputWriter.h"

using namespace std;


void OutputWriter::writeOutput(Graph<int>& roadNetwork, const string& filename, int start, int end,
                const vector<int>& bestPath, int bestTime,
                const vector<int>& altPath, int altTime,
                bool hasRestrictions) {
    ofstream out(filename);
    if (!out.is_open()) {
        throw runtime_error("ERROR: Unable to create " + filename);
    }

    out << "Source:" << start << "\n";
    out << "Destination:" << end << "\n";

    if (hasRestrictions) {
        if (!bestPath.empty() && bestTime < INF) {
            out << "RestrictedDrivingRoute:";
            for (size_t i = 0; i < bestPath.size(); i++) {
                if (i != 0) out << ",";
                out << bestPath[i];
            }
            out << "(" << bestTime << ")\n";
        } else {
            out << "RestrictedDrivingRoute:none\n";
        }
    } else {
        if (!bestPath.empty() && bestTime < INF) {
            out << "BestDrivingRoute:";
            for (size_t i = 0; i < bestPath.size(); i++) {
                if (i != 0) out << ",";
                out << bestPath[i];
            }
            out << "(" << bestTime << ")\n";
        } else {
            out << "BestDrivingRoute:none\n";
        }

        if (!altPath.empty() && altTime < INF) {
            out << "AlternativeDrivingRoute:";
            for (size_t i = 0; i < altPath.size(); i++) {
                if (i != 0) out << ",";
                out << altPath[i];
            }
            out << "(" << altTime << ")\n";
        } else {
            out << "AlternativeDrivingRoute:none\n";
        }
    }
}


void OutputWriter::writeHybridOutput(Graph<int>& roadNetwork, const string& filename, int start, int end,
                      const vector<int>& drivingRoute, int parkingNode,
                      const vector<int>& walkingRoute, int totalTime,
                      const string& message,
                      const vector<tuple<int, vector<int>, vector<int>, int>>& alternatives) {
    ofstream out(filename);
    if (!out.is_open()) {
        throw runtime_error("ERROR: Unable to create " + filename);
    }

    out << "Source:" << start << "\n";
    out << "Destination:" << end << "\n";

    if (drivingRoute.empty() || walkingRoute.empty()) {
        out << "DrivingRoute:none\n";
        out << "ParkingNode:none\n";
        out << "WalkingRoute:none\n";
        out << "TotalTime:\n";

        if (!message.empty()) {
            out << "Message:" << message << "\n";
        }

        // Escreve alternativas se existirem
        for (size_t i = 0; i < alternatives.size(); i++) {
            const auto& alt = alternatives[i];
            int p = get<0>(alt);
            const vector<int>& drivePath = get<1>(alt);
            const vector<int>& walkPath = get<2>(alt);
            int total = get<3>(alt);

            // Calcula tempo de condução separadamente
            int driveTime = 0;
            for (size_t j = 0; j < drivePath.size() - 1; j++) {
                auto v = roadNetwork.findVertex(drivePath[j]);
                for (auto e : v->getAdj()) {
                    if (e->getDest()->getInfo() == drivePath[j+1]) {
                        driveTime += e->getWeight();
                        break;
                    }
                }
            }
            int walkTime = total - driveTime;

            out << "DrivingRoute" << (i+1) << ":";
            for (size_t j = 0; j < drivePath.size(); j++) {
                if (j != 0) out << ",";
                out << drivePath[j];
            }
            out << "(" << driveTime << ")\n";

            out << "ParkingNode" << (i+1) << ":" << p << "\n";
            out << "WalkingRoute" << (i+1) << ":";
            for (size_t j = 0; j < walkPath.size(); j++) {
                if (j != 0) out << ",";
                out << walkPath[j];
            }
            out << "(" << walkTime << ")\n";

            out << "TotalTime" << (i+1) << ":" << total << "\n";
        }
    } else {
        // Calcula tempos separadamente para rota principal
        int driveTime = 0;
        for (size_t i = 0; i < drivingRoute.size() - 1; i++) {
            auto v = roadNetwork.findVertex(drivingRoute[i]);
            for (auto e : v->getAdj()) {
                if (e->getDest()->getInfo() == drivingRoute[i+1]) {
                    driveTime += e->getWeight();
                    break;
                }
            }
        }
        int walkTime = totalTime - driveTime;

        out << "DrivingRoute:";
        for (size_t i = 0; i < drivingRoute.size(); i++) {
            if (i != 0) out << ",";
            out << drivingRoute[i];
        }
        out << "(" << driveTime << ")\n";

        out << "ParkingNode:" << parkingNode << "\n";

        out << "WalkingRoute:";
        for (size_t i = 0; i < walkingRoute.size(); i++) {
            if (i != 0) out << ",";
            out << walkingRoute[i];
        }
        out << "(" << walkTime << ")\n";

        out << "TotalTime:" << totalTime << "\n";
    }
}
