//
// Created by mateusz on 22.11.2025.
//

#include "Warehouse.h"

#include <sys/shm.h>
#include <sys/sem.h>

#include <cstring>
#include <cstdio>

#define WAREHOUSE_SHM_SIZE 4096
#define WAREHOUSE_IPC_MODE 0644
#define WAREHOUSE_DIR "warehouses"

#include <fstream>

#include "objects/SharedVector.h"
#include "objects/Item.h"

#include <nlohmann/json.hpp>
#include <utility>

template void to_json(nlohmann::json &j, SharedVector<Item> const &vec);
template void from_json(const nlohmann::json &j, SharedVector<Item> &vec);

using json = nlohmann::json;
namespace fs = std::filesystem;

static key_t make_key(const std::string& name, Logger *log) {
	const std::string dir(WAREHOUSE_DIR);
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

int Warehouse::variety() const {
	return _variety;
}

int Warehouse::usage() const {
	if (_content == nullptr) {
		return 0;
	}

	int usage = 0;
	for (auto &it : *_content) {
		usage += it.count() * it.size();
	}
	return usage;
}

Warehouse::Warehouse(
	std::string name,
	const int capacity,
	const int variety,
	const size_t total_size,
	key_t key,
	Logger *log,
	bool create):
		_capacity(capacity),
		_variety(variety),
		_name(std::move(name)),
		_sem(key, log, create),
		_shm(key, total_size, log, create),
		_log(log),
		_owner(create)
	{
	std::string dir(WAREHOUSE_DIR);
	_filename = dir + "/" + name + ".json";
	_content = _shm.get();
	_content->init(_variety);

	if (fs::exists(_filename)) {
		_read_file();
		_log->info("[%s] Warehouse state read from file", _name.c_str());
	} else {
		_log->info("[%s] Initializing warehouse", _name.c_str());
	}

	if (_owner) {
		_log->info("[%s] Created warehouse with variety %d", _name.c_str(), variety);
	} else {
		_log->info("[%s] Attached warehouse with variety %d", _name.c_str(), variety);
	}
}

Warehouse::Warehouse(const std::string &name, const int capacity, Logger *log, const int variety, bool create): Warehouse(
	name,
	capacity,
	variety,
	sizeof(SharedVector<Item>) + sizeof(Item) * variety,
	make_key(name, log),
	log,
	create
) {}

Warehouse Warehouse::attach(const std::string &name, int capacity, Logger *log, int variety) {
	return Warehouse(name, capacity, log, variety, false);
}

Warehouse Warehouse::create(const std::string &name, int capacity, Logger *log, int variety) {
	return Warehouse(name, capacity, log, variety);
}

// if everything works as intended, warehouse is destroyed only once...
Warehouse::~Warehouse () {
	// save warehouse state to file - if there is any warehouse to begin with
	if (_content != nullptr && _owner) {
		_write_file();
		_log->info("[%s] Saved warehouse content to file", _name.c_str());
	}

	// semaphore is removed automatically upon destruction
	// as well as shared memory
}

void Warehouse::add(Item &item) const {
	// lock warehouse
	_sem.lock();

	// check capacity - if no space, just release semaphore
	if (usage() + item.size() > _capacity) {
		_log->warn("[%s] Warehouse is full. Cannot add item %s", _name.c_str(), item.name().c_str());
		_sem.unlock();
		return;
	}


	// add item to list (stack if already present)
	bool stacked = false;
	for (auto &warehouse_item: *_content) {
		if (warehouse_item == item) {
			warehouse_item.stack(item);
			stacked = true;
			_log->info("[%s] Added item %s to warehouse", _name.c_str(), item.name().c_str());
			_log->debug("[%s] %s stack size: %d", _name.c_str(), warehouse_item.name().c_str(), warehouse_item.count());
			break;
		}
	}
	if (!stacked) {
		_content->push_back(item);
		_log->info("[%s] Added unique item %s to warehouse", _name.c_str(), item.name().c_str());
		_log->debug("[%s] Warehouse variety: %d/%d", _name.c_str(), _content->size(), variety());
	}

	_log->debug("[%s] Warehouse capacity: %d/%d", _name.c_str(), usage(), _capacity);
	// unlock warehouse
	_sem.unlock();
}

void Warehouse::get(const std::string &itemName, Item *output) const {
	// lock warehouse
	_sem.lock();

	auto it = _content->begin();
	while (it != _content->end()) {
		if (it->name() == itemName) {
			*output = *it->unstack();
			_log->info("[%s] Found item %s in warehouse", _name.c_str(), itemName.c_str());
			_log->debug("[%s] %s stack size: %d", _name.c_str(), itemName.c_str(), it->count());
		}

		if (it->count() == 0) {
			_log->info("[%s] Last item %s fetched from warehouse. Deleting", _name.c_str(), it->name().c_str());
			_content->erase(it);
			_log->debug("[%s] Warehouse variety: %d/%d", _name.c_str(), _content->size(), variety());
		} else ++it;
	}

	_log->debug("[%s] Warehouse capacity: %d/%d", _name.c_str(), usage(), _capacity);
	// unlock warehouse
	_sem.unlock();
}

std::vector<Item> Warehouse::items() const {
	if (_content == nullptr) {
		return {};
	}

	std::vector<Item> items;
	for (auto &item: *_content) {
		items.push_back(item);
	}
	return items;
}

const std::string & Warehouse::name() const {return _name;}

int Warehouse::capacity() const {return _capacity;}

void Warehouse::_write_file() {
	// create dir if not exists
	fs::create_directories(_filename.parent_path());

	// open tmp file for write
	fs::path tmp_filename = _filename;
	_filename += ".tmp";

	std::ofstream out{tmp_filename};
	if (!out) {
		_log->fatal("[%s] Cannot open file %s for writing", _name.c_str(), tmp_filename.c_str());
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
		_log->fatal("[%s] Cannot open file %s for reading", _name.c_str(), _filename.c_str());
	}

	// deserialize content
	json j;
	in >> j;

	// i can call it explicitly!
	// _content->_size = j.size();
	for (size_t i = 0; i < j.size(); ++i) {
		(*_content)[i] = j[i].get<Item>();
	}
}
