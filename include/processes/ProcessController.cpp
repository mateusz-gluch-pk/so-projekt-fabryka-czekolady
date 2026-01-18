//
// Created by mateusz on 3.12.2025.
//

#include "ProcessController.h"

#include <complex>
#include <filesystem>
#include <fstream>
#include <math.h>
#include <sys/wait.h>

#include "ipcs/key.h"
#include "ipcs/MessageQueue.h"

#define PROCESS_DIR "processes"

std::unique_ptr<ProcessController> ProcessController::_cls = nullptr;

ProcessController::ProcessController(std::unique_ptr<IRunnable> proc, const Logger &log, const bool create, const bool debug):
    _key(make_key(PROCESS_DIR, proc->name(), &log)),
    _debug(debug),
    _log(log),
    _proc(std::move(proc)),
    _stats(_key, sizeof(ProcessStats), &log, create),
    _pid(0) {
    _cls = nullptr;
    if (!create && !debug) {
        _msq = new MessageQueue<Message>(_log.key(), create);
        _log.setQueue(_msq);
    }
}

ProcessController::~ProcessController() {
    if (_pid > 0) {
        _log.debug(_msg("Parent terminated - sending SIGKILL").c_str());
        kill(_pid, SIGTERM);
        int status;
        waitpid(_pid, &status, 0);
        _log.info(_msg("Child process %d terminated with %d").c_str(), status, _pid);
        _pid = -1;
    }

    if (_msq != nullptr) {
        delete _msq;
        _msq = nullptr;
    }
}

void ProcessController::run() {
    // get args from runnable
    auto argv = _proc->argv();
    std::string cmd;
    std::vector<char*> c_argv;
    for (auto &arg : argv) {
        c_argv.push_back(arg.data());
        if (!cmd.empty()) cmd += ' ';
        cmd += arg;
    }
    _log.info(_msg("Exec: /proc/self/exe" + cmd).c_str());
    c_argv.push_back(nullptr);

    _pid = fork();
    if (_pid == 0) {
        execv("/proc/self/exe", c_argv.data());

        // exec only returns on error
        _log.fatal(_msg("Exec failed").c_str());
        std::exit(1);
    }

    if (_pid > 0) {
        _log.info(_msg("Created").c_str());
    } else {
        _log.fatal(_msg("Fork failed").c_str());
    }
}

void ProcessController::stop() {
    _log.debug(_msg("Sending SIGTERM").c_str());
    kill(_pid, SIGTERM);

    // this terminates the process - so we waitpid for him to die
    waitpid(_pid, nullptr, 0);
    _log.debug(_msg("Terminated").c_str());
    _pid = -1;
}

void ProcessController::pause() const {
    _log.debug(_msg("Sending SIGUSR1(stop)").c_str());
    kill(_pid, SIGUSR1);
}

void ProcessController::resume() const {
    _log.debug(_msg("Sending SIGCONT").c_str());
    kill(_pid, SIGCONT);
}

void ProcessController::reload() const {
    _log.debug(_msg("Sending SIGHUP").c_str());
    kill(_pid, SIGHUP);
}

void ProcessController::setup_handlers() {
    // _cls->_log.debug(_cls->_msg("setting up signals").c_str());
    signal(SIGTERM, _handle_stop);
    signal(SIGUSR1, _handle_pause);
    signal(SIGCONT, _handle_resume);
    signal(SIGHUP, _handle_reload);
}

// void ProcessController::_handle_run(int) {_cls->_proc->run(*_stats);}
void ProcessController::_handle_stop(int) {
    _cls->_log.debug(_cls->_msg("Received SIGTERM").c_str());
    _cls->_proc->stop();
}
void ProcessController::_handle_pause(int) {
    _cls->_log.debug(_cls->_msg("Received SIGUSR1(stop)").c_str());
    _cls->_proc->pause();
}
void ProcessController::_handle_resume(int) {
    _cls->_log.debug(_cls->_msg("Received SIGCONT").c_str());
    _cls->_proc->resume();
}
void ProcessController::_handle_reload(int) {
    _cls->_log.debug(_cls->_msg("Received SIGHUP").c_str());
    _cls->_proc->reload();
}
