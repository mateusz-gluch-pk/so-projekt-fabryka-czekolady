//
// Created by mateusz on 23.12.2025.
//

#ifndef FACTORY_PROCESSSTATS_H
#define FACTORY_PROCESSSTATS_H
#include <string>


/**
 * @brief Represents the current state of a process.
 */
enum ProcessState {
    CREATED = 0,   /**< Process has been created but not started */
    RUNNING,       /**< Process is currently running */
    PAUSED,        /**< Process is paused */
    RELOADING,     /**< Process is reloading configuration or resources */
    STOPPED,       /**< Process is terminated */
};

/**
 * @brief Convert a ProcessState to a human-readable string.
 * @param state ProcessState value.
 * @return Corresponding string representation.
 */
inline std::string state_to_string(const ProcessState state) {
    switch (state) {
        case CREATED:   return "CREATED";
        case RUNNING:   return "RUNNING";
        case PAUSED:    return "PAUSED";
        case RELOADING: return "RELOADING";
        case STOPPED:   return "STOPPED";
        default:        return "UNKNOWN";
    }
}

/**
 * @brief Holds runtime statistics for a process.
 *
 * Tracks state, loop count, reload count, and process ID.
 */
class ProcessStats {
public:
    ProcessState state = CREATED; /**< Current state of the process */
    int loops = 0;                /**< Number of execution loops completed */
    int reloads = 0;              /**< Number of reloads performed */
    int pid = 0;                  /**< Process ID */
};

#endif //FACTORY_PROCESSSTATS_H