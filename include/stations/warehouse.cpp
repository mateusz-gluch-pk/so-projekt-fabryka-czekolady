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

#include <fstream>

#include <nlohmann/json.hpp>

#include "item.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

Warehouse::Warehouse(const std::string &name, const int capacity) {
	_name = name;
	_filename = "warehouses/" + name + ".json";
	_capacity = capacity;

	const fs::path _keyfile = "warehouses/" + name + ".key";

	// create key - common for sem and shared mem
	_key = ftok(_keyfile.c_str(), 1);

	// create ipcs
	_shmid = shmget(_key, WAREHOUSE_SHM_SIZE, IPC_CREAT | WAREHOUSE_IPC_MODE);
	_semid = semget(_key, IPC_CREAT | WAREHOUSE_IPC_MODE, 1);

	// initialize warehouse content
	if (fs::exists(_filename)) {
		_content = _read_file();
	} else {
		_content = std::vector<Item>();
	}

	_shmaddr = shmat(_shmid, nullptr, SHM_RDONLY);
};

// if everything works as intended, warehouse is destroyed only once...
Warehouse::~Warehouse () {
	// send signal to workers and deliverers - warehouse stops existing

	// read final shm state
	_content = _read_shm();
	// save warehouse state to file
	_write_file(_content);

	// unattach shared memory (internal)
	shmdt(_shmaddr);
	// remove shared memory
	shmctl(_shmid, IPC_RMID, nullptr);
	// remove semaphore
	semctl(_semid, 0, IPC_RMID);
}

void Warehouse::add(const Item &item) {
	// read content from shm
	std::vector<Item> content = _read_shm();

	// check capacity - if no space, just release semaphore

	// add item to list (stack if already present)
	bool stacked = false;
	for (auto &warehouse_item: content) {
		if (warehouse_item.name() == item.name()) {
			warehouse_item.stack(item);
			stacked = true;
			break;
		}
	}
	if (!stacked) content.push_back(item);

	// write content to shm
	_write_shm(content);
}

Item *Warehouse::get(const std::string &itemName) {
	// read content from shm
	std::vector<Item> content = _read_shm();

	Item *ret = nullptr;
	auto it = content.begin();
	while (it != content.end()) {
		if (it->name() == itemName) {
			ret = it->unstack();
		}

		if (it->count() == 0) {
			content.erase(it);
		} else ++it;
	}

	// write content to shm
	_write_shm(content);
	return ret;
}

void Warehouse::_write_file(std::vector<Item> &content) {
	// create dir if not exists
	fs::create_directories(_filename.parent_path());

	// open tmp file for write
	fs::path tmp_filename = _filename;
	_filename += ".tmp";

	std::ofstream out{tmp_filename};
	if (!out) {
		throw std::runtime_error("Cannot open file for writing");
	}

	// serialize content
	json content_json = content;
	out << content_json.dump(2) << std::endl;
	out.close();

	// atomic rewrite tmp to target
	fs::rename(tmp_filename, _filename);
}

std::vector<Item> &Warehouse::_read_file() {

	// open file for reading
	std::ifstream in{_filename};
	if (!in) {
		throw std::runtime_error("Cannot open file for reading");

	}

	// deserialize content
	json content_json;
	in >> content_json;
	std::vector<Item> items = content_json.get<std::vector<Item>>();
	return items;
}