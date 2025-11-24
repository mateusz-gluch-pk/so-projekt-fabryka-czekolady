//
// Created by mateusz on 22.11.2025.
//

#include "warehouse.h"

#include <sys/shm.h>
#include <sys/sem.h>

#include <cstring>
#include <cstdio>

#define WAREHOUSE_SHM_SIZE 4096
#define WAREHOUSE_IPC_MODE 0644

Warehouse::Warehouse(const std::string &name, const int capacity) {
	_name = name;
	_filename = name + ".json";
	_capacity = capacity;

	const std::string _keyfile = name + ".key";

	// create key - common for sem and shared mem
	_key = ftok(_keyfile.c_str(), 1);

	// create ipcs
	_shmid = shmget(_key, WAREHOUSE_SHM_SIZE, IPC_CREAT | WAREHOUSE_IPC_MODE);
	_semid = semget(_key, IPC_CREAT | WAREHOUSE_IPC_MODE, 1);

	// initialize warehouse content
	_content = std::vector<Item>();
	if (true) {
		_content = _read_file();
	}
	_shmaddr = shmat(_shmid, nullptr, SHM_RDONLY);

};



Warehouse::~Warehouse () {
	_sync();

	// send signal to workers and deliverers - warehouse stops existing

	// unattach shared memory (internal)
	shmdt(_shmaddr);

	// remove shared memory
	shmctl(_shmid, IPC_RMID, NULL);

	// remove semaphore
	semctl(_semid, 0, IPC_RMID);
}