//
// Created by mateusz on 27.12.2025.
//

#ifndef FACTORY_DELIVERERSERVICE_H
#define FACTORY_DELIVERERSERVICE_H
#include <map>
#include <string>

#include "actors/Deliverer.h"
#include "logger/Logger.h"
#include "processes/ProcessController.h"

/**
 * @brief Data class representing statistics for a Deliverer.
 */
class DelivererStats {
public:
    /**
     * @brief Default constructor.
     */
    DelivererStats() = default;

    /**
     * @brief Constructs a DelivererStats instance.
     * @param name Name of the deliverer.
     * @param dst_name Name of the destination warehouse.
     * @param tpl Item template used by the deliverer.
     * @param stats Pointer to shared process statistics (read-only).
     */
    DelivererStats(std::string name, std::string dst_name, const ItemTemplate &tpl, const ProcessStats *stats)
        : name(std::move(name)),
          dst_name(std::move(dst_name)),
          tpl(tpl),
          stats(stats)
    {}

    std::string name;           ///< Deliverer's name
    std::string dst_name;       ///< Destination warehouse name
    ItemTemplate tpl;           ///< Template of the item being delivered
    const ProcessStats *stats = nullptr; ///< Read-only pointer to process statistics

    /**
     * @brief Returns column headers for tabular output.
     * @return Vector of header strings.
     */
    static std::vector<std::string> headers();

    /**
     * @brief Returns this deliverer's data as a table row.
     * @return Vector of string representations of the data fields.
     */
    std::vector<std::string> row() const;
};


/**
 * @brief Service class for managing Deliverer instances and their statistics.
 */
class DelivererService {
public:
    /**
     * @brief Constructs a DelivererService.
     * @param _log Reference to a logger.
     * @param debug Whether to enable debug mode (default false).
     */
    explicit DelivererService(Logger &_log, const bool debug = false)
        : _log(_log), _debug(debug) {};

    /**
     * @brief Destructor; stops all deliverers and cleans up resources.
     */
    ~DelivererService();

    /**
     * @brief Creates a new deliverer instance.
     * @param name Name of the deliverer.
     * @param tpl Item template for delivery.
     * @param dst Destination warehouse reference.
     * @return Pointer to the new DelivererStats object.
     * @throws std::runtime_error if creation fails or name exists.
     */
    DelivererStats *create(const std::string &name, const ItemTemplate &tpl, Warehouse &dst);

    /**
     * @brief Destroys an existing deliverer.
     * @param name Name of the deliverer to destroy.
     */
    void destroy(const std::string &name);

    /**
     * @brief Retrieves statistics for a deliverer.
     * @param name Name of the deliverer.
     * @return Pointer to the DelivererStats, or nullptr if not found.
     */
    DelivererStats *get(const std::string &name);

    /**
     * @brief Retrieves statistics for all deliverers.
     * @return Vector of pointers to all DelivererStats.
     */
    std::vector<DelivererStats *> get_all();

    /**
     * @brief Pauses a specific deliverer.
     * @param name Name of the deliverer.
     */
    void pause(const std::string &name);

    /**
     * @brief Resumes a specific paused deliverer.
     * @param name Name of the deliverer.
     */
    void resume(const std::string &name);

    /**
     * @brief Reloads a specific deliverer's configuration.
     * @param name Name of the deliverer.
     */
    void reload(const std::string &name);

    /**
     * @brief Reloads all deliverers' configurations.
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
    std::map<std::string, ProcessController *> _deliverers; ///< Managed deliverer controllers
    std::map<std::string, DelivererStats> _stats;      ///< Deliverer statistics
    bool _debug;                                       ///< Debug mode flag
};



#endif //FACTORY_DELIVERERSERVICE_H