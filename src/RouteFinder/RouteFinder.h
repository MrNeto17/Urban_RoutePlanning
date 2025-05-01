
/**
 * @file RouteFinder.h
 * @brief Implements route finding algorithms for a road network
 */
#ifndef ROUTEFINDER_H
#define ROUTEFINDER_H

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <tuple>
#include "../Graph.h"

using namespace std;
class RouteFinder {
public:

    /**
 * @brief Finds the shortest path between two nodes using Dijkstra's algorithm
 * @param roadNetwork Reference to the Graph representing the road network
 * @param start ID of the starting node
 * @param end ID of the destination node
 * @param[out] path Vector that will contain the resulting path (if found)
 * @param[out] total_time Total travel time of the found path
 * @param driving Flag indicating whether to consider driving constraints (default: true)
 * @return true if a path was found, false otherwise
 * @note Uses different weights for driving (edge weight) vs walking (walking weight)
 * @warning Nodes with negative weights are considered non-drivable when driving=true
 */
    static bool dijkstra(Graph<int>& roadNetwork, int start, int end,
                        std::vector<int>& path, int& total_time, bool driving);


    /**
 * @brief Finds a hybrid driving-walking route with parking constraints
 * @param roadNetwork Reference to the Graph representing the road network
 * @param parkingData Map of parking availability (node ID -> isParking)
 * @param start ID of the starting node
 * @param end ID of the destination node
 * @param maxWalkTime Maximum allowed walking time in minutes
 * @param avoidNodes List of node IDs to avoid
 * @param avoidSegments List of edges (pairs of node IDs) to avoid
 * @param[out] drivingRoute Resulting driving portion of the route
 * @param[out] walkingRoute Resulting walking portion of the route
 * @param[out] parkingNode ID of the parking node where transition occurs
 * @param[out] totalTime Total travel time (driving + walking)
 * @param[out] message Information message about the result
 * @param[out] alternatives Optional pointer to store alternative routes
 * @return true if a valid route was found within constraints, false otherwise
 * @details Considers parking nodes as transition points between driving and  walking
 * */
    static bool findRouteWithIncludeNode(Graph<int>& roadNetwork, int start,
                                       int end, int include,
                                       std::vector<int>& path, int& total_time);


    /**
 * @brief Finds alternative hybrid routes when primary route doesn't meet constraints
 * @param roadNetwork Reference to the Graph representing the road network
 * @param parkingData Map of parking availability (node ID -> isParking)
 * @param start ID of the starting node
 * @param end ID of the destination node
 * @param originalMaxWalkTime Original maximum walking time constraint
 * @param avoidNodes List of node IDs to avoid
 * @param avoidSegments List of edges (pairs of node IDs) to avoid
 * @return Vector of alternative routes, each containing:
 *         - Parking node ID
 *         - Driving route segment
 *         - Walking route segment
 *         - Total time
 * @note Returns up to 2 best alternatives sorted by total time
 */
    static bool findHybridRoute(Graph<int>& roadNetwork,
                              const std::unordered_map<int, bool>& parkingData,
                              int start, int end, int maxWalkTime,
                              const std::vector<int>& avoidNodes,
                              const std::vector<std::pair<int, int>>& avoidSegments,
                              std::vector<int>& drivingRoute,
                              std::vector<int>& walkingRoute,
                              int& parkingNode, int& totalTime,
                              std::string& message,
                              std::vector<std::tuple<int, std::vector<int>, std::vector<int>, int>>* alternatives);


    /**
 * @brief Finds alternative driving-only routes between two nodes
 * @param roadNetwork Reference to the Graph representing the road network
 * @param start ID of the starting node
 * @param end ID of the destination node
 * @param avoidNodes List of node IDs to avoid
 * @param avoidSegments List of edges (pairs of node IDs) to avoid
 * @param maxAlternatives Maximum number of alternative routes to find
 * @return Vector of alternative paths (each path is a vector of node IDs)
 * @warning Only works when no avoidance constraints are specified
 * @details Uses edge removal strategy to find divergent paths from the primary route
 */
    static std::vector<std::tuple<int, std::vector<int>, std::vector<int>, int>>
    findAlternativeRoutes(
        Graph<int>& roadNetwork,
        const std::unordered_map<int, bool>& parkingData,
        int start, int end, int originalMaxWalkTime,
        const std::vector<int>& avoidNodes,
        const std::vector<std::pair<int, int>>& avoidSegments);

};

#endif // ROUTEFINDER_H