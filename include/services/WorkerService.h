//
// Created by mateusz on 27.12.2025.
//

#ifndef FACTORY_WORKERSERVICE_H
#define FACTORY_WORKERSERVICE_H
#include "actors/Worker.h"
#include "logger/Logger.h"
#include "processes/ProcessController.h"

class WorkerService {
    public:
    explicit WorkerService(Logger &_log): _log(_log) {};
    ~WorkerService() {
        for (auto &pair: _workers) {
            pair.second->stop();
            delete pair.second;
        }
        _workers.clear();
    }

    ProcessController *create(const std::string &name, const Recipe &recipe, Warehouse &in, Warehouse &out) {
        if (_workers.contains(name)) {
            _log.error(_msg("Deliverer exists: " + name).c_str());
            return nullptr;
        }

        try {
            auto w = std::make_unique<Worker>(name, recipe, in, out, _log);
            const auto pw = new ProcessController(std::move(w), _log);
            pw->run();
            _workers[name] = pw;
            _log.info(_msg("Created deliverer: " + name).c_str());
            return pw;
        } catch (...) {
            _log.error(_msg("Failed to create deliverer: " + name).c_str());
            return nullptr;
        }
    }

    void destroy(const std::string &name) {
        auto it = _workers.find(name);
        if (it == _workers.end()) {
            _log.error(_msg("Deliverer not found: " + name).c_str());
            return;
        }

        it->second->stop();
        delete it->second;
        _workers.erase(it);
        _log.info(_msg("Deleted deliverer: " + name).c_str());
    }

    ProcessController *get(const std::string &name) {
        auto it = _workers.find(name);
        if (it == _workers.end()) {
            _log.error(_msg("Deliverer not found: " + name).c_str());
            return nullptr;
        }

        _log.info(_msg("Fetched deliverer: " + name).c_str());
        return it->second;
    }

    const ProcessStats *get_stats(const std::string &name) {
        auto it = _workers.find(name);
        if (it == _workers.end()) {
            _log.error(_msg("Deliverer not found: " + name).c_str());
            return nullptr;
        }

        _log.info(_msg("Fetched deliverer: " + name).c_str());
        return it->second->stats();
    }

    std::vector<ProcessController *> get_all() {
        std::vector<ProcessController *> result;
        result.reserve(_workers.size());

        for (auto &pair : _workers) {
            result.push_back(pair.second);
        }

        _log.info(_msg("Fetched all deliverers").c_str());
        return result;
    }

    std::vector<const ProcessStats *> get_all_stats() {
        std::vector<const ProcessStats *> result;
        result.reserve(_workers.size());

        for (auto &pair : _workers) {
            result.push_back(pair.second->stats());
        }

        _log.info(_msg("Fetched all deliverers stats").c_str());
        return result;
    }

    void pause(const std::string &name) {
        auto it = _workers.find(name);
        if (it == _workers.end()) {
            _log.error(_msg("Deliverer not found: " + name).c_str());
        }

        const ProcessState state = it->second->stats()->state;
        if (state != RUNNING) {
            _log.warn(_msg("Deliverer is not running: " + name).c_str());
        }

        _log.info(_msg("Pausing deliverer: " + name).c_str());
        it->second->pause();
    }

    void resume(const std::string &name) {
        auto it = _workers.find(name);
        if (it == _workers.end()) {
            _log.error(_msg("Deliverer not found: " + name).c_str());
        }

        const ProcessState state = it->second->stats()->state;
        if (state != PAUSED) {
            _log.warn(_msg("Deliverer is not paused: " + name).c_str());
        }

        _log.info(_msg("Resuming deliverer: " + name).c_str());
        it->second->resume();
    }

    void reload(const std::string &name) {
        auto it = _workers.find(name);
        if (it == _workers.end()) {
            _log.error(_msg("Deliverer not found: " + name).c_str());
        }

        const ProcessState state = it->second->stats()->state;
        if (state != RUNNING) {
            _log.warn(_msg("Deliverer is not running: " + name).c_str());
        }

        _log.info(_msg("Reloading deliverer: " + name).c_str());
        it->second->reload();
    }

    void reload_all() {
        for (auto &pair : _workers) {
            reload(pair.first);
        }
    }

private:
    static std::string _msg(const std::string &msg) {
        return "services/DeliverersService:\t" + msg;
    }

    Logger &_log;
    std::map<std::string, ProcessController *> _workers;

};


#endif //FACTORY_WORKERSERVICE_H