//
// Created by mateusz on 3.12.2025.
//

#include "ProcessController.h"

ProcessController::~ProcessController() {
    if (_pid != -1) {
        kill(_pid, SIGKILL);
    }
}

void ProcessController::run() {
    const pid_t pid = fork();
    if (pid == 0) {
        _setup_handlers();
        _proc->run();
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
