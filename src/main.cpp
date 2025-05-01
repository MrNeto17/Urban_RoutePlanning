/**
 * @file main.cpp
 * @brief Main program for the route finding system
 *
 * This program provides a menu-driven interface to find optimal routes
 * either from an input file or through manual parameter entry.
 */

#include <iostream>
#include <fstream>
#include <unordered_map>
#include "Graph.h"
#include "FileParser/FileParser.h"
#include "InputParser/InputParser.h"
#include "OutputWriter/OutputWriter.h"
#include "RouteFinder/RouteFinder.h"

using namespace std;

/**
 * @brief Displays the main menu options
 *
 * Shows the available options for the user to interact with the system.
 */
void showMainMenu() {
    cout << "\n=== MENU PRINCIPAL ===" << endl;
    cout << "1. Executar com arquivo input.txt (gera output.txt)" << endl;
    cout << "2. Inserir parametros manualmente" << endl;
    cout << "3. Sair" << endl;
    cout << "Escolha: ";
}

/**
 * @brief Processes the user's menu choice
 *
 * @param choice The user's menu selection
 * @param roadNetwork Reference to the graph representing the road network
 * @param parkingData Reference to the parking availability data
 * @return bool True to continue the menu loop, false to exit
 */
bool processChoice(const string& choice, Graph<int>& roadNetwork, unordered_map<int, bool>& parkingData) {
    if (choice == "1") {
        InputParser::processFileInput(roadNetwork, parkingData);
        return false; // Don't continue the loop
    }
    else if (choice == "2") {
        InputParser::processManualInput(roadNetwork, parkingData);
        return false; // Don't continue the loop
    }
    else if (choice == "3") {
        return false; // Exit
    }
    else {
        cout << "Opcao invalida!" << endl;
        return true; // Continue the loop
    }
}

/**
 * @brief Main function of the program
 *
 * Initializes the system, loads data, and manages the main menu loop.
 *
 * @return int Program exit status (0 for success, 1 for error)
 */
int main() {
    try {
        cout << "=== BATCH MODE ===" << endl;

        // Inicialização do grafo
        Graph<int> roadNetwork;
        unordered_map<int, bool> parkingData;

        // Carregar dados
        auto codeToId = FileParser::loadLocationMappings("../data/locations.csv", parkingData);
        FileParser::loadGraph(roadNetwork, "../data/Distances.csv", codeToId);

        // Menu principal
        while (true) {
            showMainMenu();

            string choice;
            getline(cin, choice);

            if (!processChoice(choice, roadNetwork, parkingData)) {
                break;
            }
        }

        cout << "\n=== SISTEMA ENCERRADO ===" << endl;
        return 0;
    }
    catch (const exception& e) {
        cerr << "\nERRO FATAL: " << e.what() << endl;
        return 1;
    }
}