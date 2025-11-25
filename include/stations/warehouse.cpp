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

Warehouse::Warehouse(const std::string &name, const int capacity, const int variety) {
	_name = name;
	_filename = "warehouses/" + name + ".json";
	_capacity = capacity;
	_variety = variety;

	const fs::path _keyfile = "warehouses/" + name + ".key";

	// allocate memory for M kinds of Items
	size_t total_size = sizeof(SharedVector<Item>) + sizeof(Item) * _variety;

	// create key - common for sem and shared mem
	_key = ftok(_keyfile.c_str(), 1);

	// create ipcs
	_sem = Semaphore(_key);
	_shm = SharedMemory<SharedVector<Item>>(_key, total_size);

	_content = _shm.get();
	_content->init(_variety);

	// initialize warehouse content
	if (fs::exists(_filename)) {
		_content = _read_file();
	}
} ;

// if everything works as intended, warehouse is destroyed only once...
Warehouse::~Warehouse () {
	// send signal to workers and deliverers - warehouse stops existing

	// save warehouse state to file
	if (_content != nullptr_) {
		_write_file(*_content);
	} else {
		_write_file(SharedVector<Item>{});
	}

	// semaphore is removed automatically upon destruction
	// as well as shared memory
}

void Warehouse::add(const Item &item) {
	// lock warehouse
	_sem.lock();

	// check capacity - if no space, just release semaphore

	// add item to list (stack if already present)
	bool stacked = false;
	for (auto &warehouse_item: *_content) {
		if (warehouse_item.name() == item.name()) {
			warehouse_item.stack(item);
			stacked = true;
			break;
		}
	}
	if (!stacked) _content->push_back(item);

	// unlock warehouse
	_sem.unlock();
}

Item *Warehouse::get(const std::string &itemName) {
	// lock warehouse
	_sem.lock();

	Item *ret = nullptr;
	auto it = _content->begin();
	while (it != _content->end()) {
		if (it->name() == itemName) {
			ret = it->unstack();
		}

		if (it->count() == 0) {
			_content->erase(it);
		} else ++it;
	}

	// unlock warehouse
	_sem.unlock();
	return ret;
}

void Warehouse::_write_file(SharedVector<Item> &content) {
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

SharedVector<Item> &Warehouse::_read_file() {
	// open file for reading
	std::ifstream in{_filename};
	if (!in) {
		throw std::runtime_error("Cannot open file for reading");

	}

	// deserialize content
	json content_json;
	in >> content_json;
	SharedVector<Item> items = content_json.get<SharedVector<Item>>();

	return items;
}