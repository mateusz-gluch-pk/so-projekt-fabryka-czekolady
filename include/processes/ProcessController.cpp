//
// Created by mateusz on 3.12.2025.
//

#include "ProcessController.h"

#include <complex>
#include <filesystem>
#include <fstream>
#include <math.h>

#include "ipcs/MessageQueue.h"

#define PROCESS_DIR "processes"
namespace fs = std::filesystem;


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

ProcessController::ProcessController(std::unique_ptr<IRunnable> proc, const Logger &log):
    _key(make_key(proc->name(), &log)),
    _msq(log.key(), false),
    _log(log),
    _proc(std::move(proc)),
    _stats(_key, sizeof(ProcessStats), &log) {
}

ProcessController::~ProcessController() {
    if (_pid != -1) {
        kill(_pid, SIGKILL);
    }
}

void ProcessController::run() {
    const pid_t pid = fork();
    if (pid == 0) {
        auto msq = MessageQueue<Message>::attach(_log.key());
        _log.setQueue(&msq);
        _stats = SharedMemory<ProcessStats>::attach(_key, sizeof(ProcessStats), &_log);
        _setup_handlers();
        _proc->run(*_stats);
        std::exit(0);
    }

    if (pid > 0) {
        _pid = pid;
    } else {
        throw std::runtime_error("fork failed");
    }
}

void ProcessController::stop() const {kill(_pid, SIGTERM);}
void ProcessController::pause() const {kill(_pid, SIGSTOP);}
void ProcessController::resume() const {kill(_pid, SIGCONT);}
void ProcessController::reload() const {kill(_pid, SIGHUP);}

void ProcessController::_setup_handlers() {
    signal(SIGTERM, _handle_stop);
    signal(SIGSTOP, _handle_pause);
    signal(SIGCONT, _handle_resume);
    signal(SIGHUP, _handle_reload);
}

void ProcessController::_handle_run(int) {_cls->_proc->run();}
void ProcessController::_handle_stop(int) {_cls->_proc->stop();}
void ProcessController::_handle_pause(int) {_cls->_proc->pause();}
void ProcessController::_handle_resume(int) {_cls->_proc->resume();}
void ProcessController::_handle_reload(int) {_cls->_proc->reload();}
