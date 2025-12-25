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

#include "ipcs/key.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

int Warehouse::variety() {
	return WAREHOUSE_MAX_VARIETY;
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
	const size_t total_size,
	key_t key,
	Logger *log,
	bool create):
		_capacity(capacity),
		_name(std::move(name)),
		_sem(key, log, create),
		_shm(key, total_size, log, create),
		_log(log),
		_owner(create)
	{
	std::string dir(WAREHOUSE_DIR);
	_filename = dir + "/" + _name + ".json";
	_content = _shm.get();

	constexpr int variety = WAREHOUSE_MAX_VARIETY;
	if (fs::exists(_filename) && _owner) {
		_read_file();
		_log->info(_msg("Loaded state").c_str());
		_log->info(_msg("Created - variety: %d").c_str(), variety);
	} else if (_owner) {
		_log->info(_msg("Initializing").c_str());
		_log->info(_msg("Created - variety: %d").c_str(), variety);
	} else {
		_log->info(_msg("Attached - variety: %d").c_str(), variety);
	}
}

Warehouse::Warehouse(const std::string &name, const int capacity, Logger *log, bool create): Warehouse(
	name,
	capacity,
	sizeof(SharedVector<Item, WAREHOUSE_MAX_VARIETY>) + sizeof(Item) * WAREHOUSE_MAX_VARIETY,
	make_key(WAREHOUSE_DIR, name, *log),
	log,
	create
) {}

Warehouse Warehouse::attach(const std::string &name, int capacity, Logger *log) {
	return Warehouse(name, capacity, log, false);
}

Warehouse Warehouse::create(const std::string &name, int capacity, Logger *log) {
	return Warehouse(name, capacity, log);
}

// if everything works as intended, warehouse is destroyed only once...
Warehouse::~Warehouse () {
	// save warehouse state to file - if there is any warehouse to begin with
	if (_content != nullptr && _owner) {
		_write_file();
		_log->info(_msg("Saved to file %s").c_str(), _filename.c_str());
	}

	// semaphore is removed automatically upon destruction
	// as well as shared memory
}

void Warehouse::add(Item &item) const {
	// lock warehouse
	_sem.lock();

	// check capacity - if no space, just release semaphore
	if (usage() + item.size() > _capacity) {
		_log->warn(_msg("Max capacity - cannot add item %s").c_str(), item.name().c_str());
		_sem.unlock();
		return;
	}


	// add item to list (stack if already present)
	bool stacked = false;
	for (auto &warehouse_item: *_content) {
		if (warehouse_item == item) {
			warehouse_item.stack(item);
			stacked = true;
			_log->info(_msg("Added item %s").c_str(), item.name().c_str());
			_log->debug(_msg("%s stack size: %d").c_str(), item.name().c_str(), warehouse_item.count());
			break;
		}
	}

	if (!stacked && _content->size != variety()) {
		_content->push_back(item);
		item = Item(item.name(), item.size(), 0);
		_log->info(_msg("Added new item %s").c_str(), item.name().c_str());
		_log->debug(_msg("Variety: %d/%d").c_str(), _content->size, variety());
	} else if (!stacked && _content->size == variety()) {
		_log->error(_msg("Max variety - cannot add item %s").c_str(), item.name().c_str());
	}

	_log->debug(_msg("Capacity: %d/%d").c_str(), usage(), _capacity);
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
			_log->info(_msg("Found item %s").c_str(), itemName.c_str());
			_log->debug(_msg("%s stack size: %d").c_str(), itemName.c_str(), it->count());
		}

		if (it->count() == 0) {
			_log->info(_msg("Last item %s fetched. Deleting.").c_str(), it->name().c_str());
			_content->erase(it);
			_log->debug(_msg("Variety: %d/%d").c_str(), _content->size, variety());
		} else ++it;
	}

	_log->debug(_msg("Capacity: %d/%d").c_str(), usage(), _capacity);
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

std::string Warehouse::name() const {return _name;}

int Warehouse::capacity() const {return _capacity;}

void Warehouse::_write_file() {
	// create dir if not exists
	fs::create_directories(_filename.parent_path());

	// open tmp file for write
	fs::path tmp_filename = _filename.generic_string() + ".tmp";

	std::ofstream out{tmp_filename};
	if (!out) {
		_log->fatal("[%s] Cannot open file %s for writing", _name.c_str(), tmp_filename.c_str());
	}

	// serialize content
	json content_json = *_content;
	out << content_json.dump() << std::endl;
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
	from_json(j, *_content);
}
