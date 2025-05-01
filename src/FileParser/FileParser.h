/**
* @file FileParser.cpp
 * @brief Implementation of file parsing utilities for road network data
 * @author Your Name
 * @date 2023-10-10
 */

#ifndef FILEPARSER_H
#define FILEPARSER_H

#include <string>
#include <unordered_map>
#include "../Graph.h"

class FileParser {
public:
    /**
 * @brief Loads location mappings from a CSV file
 *
 * Parses a CSV file containing location codes and their corresponding IDs,
 * while also identifying parking locations.
 *
 * @param filename Path to the CSV file containing location mappings
 * @param parkingData[out] Map to be populated with parking information (ID -> isParking)
 * @return unordered_map<string, int> Mapping of location codes to IDs
 * @throw runtime_error If file cannot be opened or has invalid format
 *
 * @note Expected CSV format: (columns) description,id,code,is_parking
 * @warning Header row is automatically skipped
 */
    static std::unordered_map<std::string, int> loadLocationMappings(
        const std::string& filename,
        std::unordered_map<int, bool>& parkingData);

    /**
 * @brief Loads road network data into a graph structure
 *
 * Parses a CSV file containing road segments and their properties,
 * building a graph representation of the road network.
 *
 * @param roadNetwork[out] Graph to be populated with road network data
 * @param filename Path to the CSV file containing road segments
 * @param codeToId Mapping of location codes to IDs (from loadLocationMappings)
 *
 * @throw runtime_error If file cannot be opened
 *
 * @note Expected CSV format: (columns) from_code,to_code,driving_time,walking_time
 * @note "X" in driving_time column indicates non-drivable segment
 * @warning Header row is automatically skipped
 * @warning Invalid lines are skipped with warning messages
 */
    static void loadGraph(
        Graph<int>& roadNetwork,
        const std::string& filename,
        const std::unordered_map<std::string, int>& codeToId);
};

#endif // FILEPARSER_H