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
#include <utility>

#include "item.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

Warehouse::Warehouse(
	std::string name,
	const int capacity,
	const int variety,
	std::string filename,
	const key_t key,
	const size_t total_size):
		_capacity(capacity),
		_variety(variety),
		_name(std::move(name)),
		_filename(std::move(filename)),
		_key(key),
		_sem(key),
		_shm(key, total_size)
{
	_content = _shm.get();
	_content->init(_variety);

	if (fs::exists(_filename)) {
		_read_file();
	}
}

Warehouse::Warehouse(const std::string &name, const int capacity, const int variety): Warehouse(
	name,
	capacity,
	variety,
	"warehouses/" + name + ".json",
	ftok(("warehouses/" + name + ".key").c_str(), 1),
	sizeof(SharedVector<Item>) + sizeof(Item) * variety
) {}

// if everything works as intended, warehouse is destroyed only once...
Warehouse::~Warehouse () {
	// save warehouse state to file - if there is any warehouse to begin with
	if (_content != nullptr) {
		_write_file();
	}

	// semaphore is removed automatically upon destruction
	// as well as shared memory
}

void Warehouse::add(const Item &item) const {
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

Item *Warehouse::get(const std::string &itemName) const {
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

void Warehouse::_write_file() {
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
	json content_json = *_content;
	out << content_json.dump(2) << std::endl;
	out.close();

	// atomic rewrite tmp to target
	fs::rename(tmp_filename, _filename);
}

void Warehouse::_read_file() const {
	// open file for reading
	std::ifstream in{_filename};
	if (!in) {
		throw std::runtime_error("Cannot open file for reading");
	}

	// deserialize content
	json content_json;
	in >> content_json;

	// i can call it explicitly!
	from_json(content_json, *_content);
}
