//
// Created by mateusz on 3.12.2025.
//

#include "ProcessController.h"

#include <complex>
#include <filesystem>
#include <fstream>
#include <math.h>
#include <sys/wait.h>

#include "ipcs/MessageQueue.h"

#define RELEASE false
#define PROCESS_DIR "processes"

namespace fs = std::filesystem;

std::unique_ptr<ProcessController> ProcessController::_cls = nullptr;

static key_t make_key(const std::string& name, const Logger *log) {
    const std::string dir(PROCESS_DIR);
    fs::create_directories(dir);

    const std::string key_filename = dir + "/" + name + ".key";
    if (!fs::exists(key_filename)) {
        log->info("Creating key file %s", key_filename.c_str());
        std::ofstream _stream(key_filename);
    }

    const key_t key = ftok(key_filename.c_str(), 1);
    log->info("Fetched key from file %s", key_filename.c_str());
    return key;
}

ProcessController::ProcessController(std::unique_ptr<IRunnable> proc, const Logger &log, const bool debug):
    _debug(debug),
    _key(make_key(proc->name(), &log)),
    _msq(log.key(), false),
    _log(log),
    _proc(std::move(proc)),
    _stats(_key, sizeof(ProcessStats), &log) {
    _cls = nullptr;
}

ProcessController::~ProcessController() {
    if (_pid != -1) {
        _log.info("Controller terminated. Sending SIGKILL to %d", _pid);
        kill(_pid, SIGKILL);
    }
}

void ProcessController::run() {
    const pid_t pid = fork();
    if (pid == 0) {
        // this makes process controller behave like a singleton in child
        _cls = std::make_unique<ProcessController>(std::move(_proc), _log);

        // if debug - we are using Mock Queue, no need to reattach
        if (!_debug) {
            _msq = MessageQueue<Message>::attach(_log.key());
            _log.setQueue(&_msq);
        }

        _stats = SharedMemory<ProcessStats>::attach(_key, sizeof(ProcessStats), &_log);
        _setup_handlers();
        _cls->_proc->run(*_stats, _log);
        std::exit(0);
    }

    if (pid > 0) {
        _log.info("Created process %d", _pid);
        _pid = pid;
    } else {
        _log.fatal("Fork failed");
        // throw std::runtime_error("fork failed");
    }
}

void ProcessController::stop() {
    _log.debug("Sending SIGTERM to %d", _pid);
    kill(_pid, SIGTERM);

    // this terminates the process - so we waitpid for him to die
    waitpid(_pid, nullptr, 0);
    _log.debug("Process %d terminated", _pid);
    _pid = -1;
}

void ProcessController::pause() const {
    _log.debug("Sending SIGSTOP to %d", _pid);
    kill(_pid, SIGSTOP);
}

void ProcessController::resume() const {
    _log.debug("Sending SIGCONT to %d", _pid);
    kill(_pid, SIGCONT);
}

void ProcessController::reload() const {
    _log.debug("Sending SIGHUP to %d", _pid);
    kill(_pid, SIGHUP);
}

void ProcessController::_setup_handlers() {
    signal(SIGTERM, _handle_stop);
    signal(SIGSTOP, _handle_pause);
    signal(SIGCONT, _handle_resume);
    signal(SIGHUP, _handle_reload);
}

// void ProcessController::_handle_run(int) {_cls->_proc->run(*_stats);}
void ProcessController::_handle_stop(int) {_cls->_proc->stop();}
void ProcessController::_handle_pause(int) {_cls->_proc->pause();}
void ProcessController::_handle_resume(int) {_cls->_proc->resume();}
void ProcessController::_handle_reload(int) {_cls->_proc->reload();}
