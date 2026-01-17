//
// Created by mateusz on 27.12.2025.
//

#ifndef FACTORY_WORKERSERVICE_H
#define FACTORY_WORKERSERVICE_H
#include "actors/Worker.h"
#include "logger/Logger.h"
#include "processes/ProcessController.h"

/**
 * @brief Data class representing statistics for a Worker.
 */
class WorkerStats {
public:
    /**
     * @brief Default constructor.
     */
    WorkerStats() {};

    /**
     * @brief Constructs a WorkerStats instance.
     * @param name Name of the worker.
     * @param in_name Name of the input warehouse.
     * @param out_name Name of the output warehouse.
     * @param recipe Recipe processed by the worker.
     * @param stats Pointer to shared process statistics (read-only).
     */
    WorkerStats(std::string name, std::string in_name, std::string out_name, const Recipe &recipe, const ProcessStats *stats)
        : name(std::move(name)),
          in_name(std::move(in_name)),
          out_name(std::move(out_name)),
          recipe(recipe),
          stats(stats)
    {}

    std::string name;           ///< Name of the worker
    std::string in_name;        ///< Name of the input warehouse
    std::string out_name;       ///< Name of the output warehouse
    Recipe recipe;              ///< Recipe processed by the worker
    const ProcessStats *stats = nullptr; ///< Read-only pointer to process statistics

    /**
     * @brief Returns column headers for tabular output.
     * @return Vector of header strings.
     */
    static std::vector<std::string> headers();

    /**
     * @brief Returns this worker's data as a table row.
     * @return Vector of string representations of the data fields.
     */
    std::vector<std::string> row() const;
};

/**
 * @brief Service class for managing Worker instances and their statistics.
 */
class WorkerService {
public:
    /**
     * @brief Constructs a WorkerService.
     * @param _log Reference to a logger.
     * @param debug Whether to enable debug mode (default false).
     */
    explicit WorkerService(Logger &_log, const bool debug = false) : _log(_log), _debug(debug) {};

    /**
     * @brief Destructor; stops all workers and cleans up resources.
     */
    ~WorkerService();

    /**
     * @brief Creates a new worker instance.
     * @param name Name of the worker.
     * @param recipe Recipe for processing.
     * @param in Reference to input warehouse.
     * @param out Reference to output warehouse.
     * @return Pointer to the new WorkerStats object.
     * @throws std::runtime_error if creation fails or name exists.
     */
    WorkerStats *create(const std::string &name, const Recipe &recipe, Warehouse &in, Warehouse &out);

    /**
     * @brief Destroys an existing worker.
     * @param name Name of the worker to destroy.
     */
    void destroy(const std::string &name);

    /**
     * @brief Retrieves statistics for a worker.
     * @param name Name of the worker.
     * @return Pointer to the WorkerStats, or nullptr if not found.
     */
    WorkerStats *get(const std::string &name);

    /**
     * @brief Retrieves statistics for all workers.
     * @return Vector of pointers to all WorkerStats.
     */
    std::vector<WorkerStats *> get_all();

    /**
     * @brief Pauses a specific worker.
     * @param name Name of the worker.
     */
    void pause(const std::string &name);

    /**
     * @brief Resumes a specific paused worker.
     * @param name Name of the worker.
     */
    void resume(const std::string &name);

    /**
     * @brief Reloads a specific worker's configuration.
     * @param name Name of the worker.
     */
    void reload(const std::string &name);

    /**
     * @brief Reloads all workers' configurations.
     */
    void reload_all();

private:
    /**
     * @brief Formats a service log message.
     * @param msg Message content.
     * @return Formatted string with service prefix.
     */
    static std::string _msg(const std::string &msg);

    Logger &_log;                                      ///< Reference to logger
    std::map<std::string, ProcessController *> _workers; ///< Managed worker controllers
    std::map<std::string, WorkerStats> _stats;         ///< Worker statistics
    bool _debug;                                       ///< Debug mode flag
};
#endif //FACTORY_WORKERSERVICE_H