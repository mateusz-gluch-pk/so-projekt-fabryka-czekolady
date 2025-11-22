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
	_key = ftok(_keyfile.c_str(), 1);
	_shmid = shmget(_key, WAREHOUSE_SHM_SIZE, IPC_CREAT | WAREHOUSE_IPC_MODE);
	_semid = semget(_key, IPC_CREAT | WAREHOUSE_IPC_MODE, 1);

	_content = new std::vector<Item>;
	_shmaddr = shmat(_shmid, nullptr, SHM_RDONLY);
};


Warehouse::~Warehouse () {
	_sync();
	shmdt(_shmaddr);
	shmctl(_shmid, IPC_RMID, NULL);
}