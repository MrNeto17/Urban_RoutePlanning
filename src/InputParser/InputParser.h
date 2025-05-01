
/**
 * @file InputParser.h
 * @brief Handles input parsing for route finding system
 */
#ifndef INPUT_PARSER_H
#define INPUT_PARSER_H

#include <string>
#include <vector>
#include <utility>
#include <unordered_map>
#include "../Graph.h"

using namespace std;

/**
 * @class InputParser
 * @brief Provides methods for parsing route finding inputs from files and manual entry
 */

class InputParser {
public:

    /**
    * @brief Parses a string of comma-separated node IDs
    * @param str Input string containing node IDs separated by commas
    * @return Vector of parsed node IDs
    * @throw runtime_error If invalid node IDs are found
    * @note Handles "none" and empty strings as empty vectors
    */
    static vector<int> parseNodes(const string& str);

    /**
    * @brief Parses segments in format (a,b)(c,d)
    * @param str Input string containing segments
    * @return Vector of segment pairs
    * @throw runtime_error If invalid segment format is found
    * @note Handles "none" and empty strings as empty vectors
    */
    static vector<pair<int, int>> parseSegments(const string& str);

    /**
     * @brief Processes input from a configuration file
     * @param roadNetwork Reference to the road network graph
     * @param parkingData Map of parking node availability
     *
     * Reads from "../input.txt" with format:
     * Mode:driving-walking
     * Source:1
     * Destination:5
     * MaxWalkTime:10
     * AvoidNodes:2,3
     * AvoidSegments:(1,2)(3,4)
     */
    static void processFileInput(Graph<int>& roadNetwork,
                               const unordered_map<int, bool>& parkingData);

    /**
    * @brief Processes manual user input through console
    * @param roadNetwork Reference to the road network graph
    * @param parkingData Map of parking node availability
    *
    * Guides user through step-by-step input:
    * 1. Mode selection
    * 2. Source/destination nodes
    * 3. Parameters based on mode
    * 4. Avoidance constraints
    */
    static void processManualInput(Graph<int>& roadNetwork,
                                 const unordered_map<int, bool>& parkingData);

    /**
    * @brief Displays manual input menu
    * @brief Shows available input options and format hints
    */
    static void showManualInputMenu();
};

#endif // INPUT_PARSER_H