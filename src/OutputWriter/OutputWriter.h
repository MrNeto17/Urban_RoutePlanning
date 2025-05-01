/**
* @file OutputWriter.h
 * @brief Interface para geração de arquivos de saída de rotas
 */

#ifndef OUTPUT_WRITER_H
#define OUTPUT_WRITER_H

#include "../Graph.h"
#include <vector>
#include <string>
#include <tuple>

/**
 * @class OutputWriter
 * @brief Gerador de arquivos de saída para rotas de transporte
 *
 * Oferece métodos estáticos para criar arquivos contendo:
 * - Rotas convencionais (melhor/alternativa)
 * - Rotas híbridas (veículo + pedestrianismo)
 * - Mensagens de status/erro
 */
class OutputWriter {
public:
    /**
 * @brief Writes the output of the best found route
 *
 * @param roadNetwork Graph of the road network
 * @param filename Output file name
 * @param start Source node
 * @param end Destination node
 * @param bestPath Best route found
 * @param bestTime Duration of the best route
 * @param altPath Alternative route (empty if nonexistent)
 * @param altTime Duration of the alternative route
 * @param hasRestrictions Indicates if restrictions were applied
 *
 * @throws std::runtime_error If file creation fails
 */
    static void writeOutput(Graph<int>&, const std::string&, int, int,
                          const std::vector<int>&, int,
                          const std::vector<int>&, int,
                          bool);

    /**
 * @brief Writes the output for a hybrid route (driving + walking)
 *
 * @param roadNetwork Graph of the road network
 * @param filename Output file name
 * @param start Source node
 * @param end Destination node
 * @param drivingRoute Driving route
 * @param parkingNode Parking node
 * @param walkingRoute Walking route
 * @param totalTime Total travel time
 * @param message Additional message (if applicable)
 * @param alternatives Vector of alternatives (parking, driving, walking, total time)
 *
 * @throws std::runtime_error If file creation fails
 */
    static void writeHybridOutput(Graph<int>&, const std::string&, int, int,
                                const std::vector<int>&, int,
                                const std::vector<int>&, int,
                                const std::string& = "",
                                const std::vector<std::tuple<int, std::vector<int>, std::vector<int>, int>>& = {});
};

#endif // OUTPUT_WRITER_H