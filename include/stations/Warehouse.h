//
// Created by mateusz on 22.11.2025.
//

#ifndef PROJEKT_WAREHOUSE_H
#define PROJEKT_WAREHOUSE_H

#include <array>
#include <filesystem>
#include <vector>
#include <sys/types.h>

#include "IWarehouse.h"
#include "WarehouseStats.h"
#include "objects/SharedVector.h"
#include "ipcs/Semaphore.h"
#include "ipcs/SharedMemory.h"
#include "objects/Item.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include <utility>

#include "ipcs/key.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

#define WAREHOUSE_DIR "warehouses"

namespace fs = std::filesystem;

/**
 * @brief Represents a warehouse with IPC-safe storage and concurrency control.
 *
 * Provides an interface to add and retrieve items while supporting multiple
 * processes via System V shared memory and semaphores. Supports RAII-style
 * creation and automatic cleanup.
 *
 * Ownership model:
 * - If created via `create`, the instance owns the underlying IPC resources
 *   (shared memory, semaphore) and deletes them in the destructor.
 * - If attached via `attach`, the instance does not own the resources and
 *   leaves them intact on destruction.
 */
template<int Size, int Capacity>
class Warehouse: public IWarehouse {
public:
    /**
     * @brief Attach to an existing warehouse.
     * @param name Warehouse name.
     * @param log Logger for debug/info/error messages.
     * @return Warehouse instance attached to existing IPC resources.
     */
    static Warehouse attach(const std::string& name, Logger *log);

    /**
     * @brief Create a new warehouse.
     * @param name Warehouse name.
     * @param log Logger for debug/info/error messages.
     * @return Warehouse instance owning the IPC resources.
     */
    static Warehouse create(const std::string& name, Logger *log);

	/**
	 * @brief Internal constructor used by `attach`/`create`.
	 * @param name Warehouse name.
	 * @param log Logger instance.
	 * @param create True to create resources; false to attach.
	 */
    Warehouse(const std::string& name, Logger *log, bool create = true);

    /** @brief Destructor cleans up IPC resources if owned. */
    ~Warehouse() override;

    /**
     * @brief Add an item to the warehouse.
     * @param item Item to add.
     * @note Thread/process-safe via internal semaphore.
     */
    void add(IItem &item) const override;

    /**
     * @brief Retrieve an item from the warehouse by name.
     * @param itemName Name of the item to fetch.
     * @return output Pointer to store the retrieved item; may be null.
     */
    std::unique_ptr<IItem> get(const std::string &itemName) const override;


    /** @brief Get the warehouse name. */
    [[nodiscard]] std::string name() const override {return _name;}

    /** @brief Get the maximum capacity of the warehouse. */
    [[nodiscard]] int capacity() const override {return Capacity;}

	/** @brief Get the item size of the warehouse. */
	[[nodiscard]] int size() const override {return Size;}

    /** @brief Get current number of items stored (usage). */
    [[nodiscard]] int usage() const override {return _content == nullptr? 0 : _content->size;}

	int empty() const override {return _empty.value();}
	int full() const override {return _full.value();}

private:
    /**
     * @brief Format a log message with warehouse context.
     * @param msg Message text.
     * @return Formatted string.
     */
    [[nodiscard]] std::string _msg(const std::string &msg) const {
        return "stations/Warehouse/" + _name + ":\t" + msg;
    }

    std::string _name;  /**< Warehouse name */
    fs::path _filename; /**< Path for file-based persistence */

    // IPC resources
    bool _owner;  /**< True if this instance created the IPC resources */
    Semaphore _sem;  /**< Semaphore for thread/process-safe access */
	Semaphore _full;  /**< Semaphore for thread/process-safe access */
	Semaphore _empty;  /**< Semaphore for thread/process-safe access */
    SharedMemory<SharedVector<Item<Size>, Capacity>> _shm; /**< Shared memory for items */
    SharedVector<Item<Size>, Capacity> *_content; /**< Pointer to shared memory content */

    // Logger
    Logger *_log;  /**< Logger instance for debug/info/error messages */

    // File persistence helpers
    void _write_file();
    void _read_file() const;
};

template<int Size, int Capacity>
Warehouse<Size, Capacity>::Warehouse(
	const std::string &name,
	Logger *log,
	bool create):
		_name(std::move(name)),
		_sem(make_key(WAREHOUSE_DIR, name, log), log, create),
		_full(make_key(WAREHOUSE_DIR, name+"_full", log), log, create),
		_empty(make_key(WAREHOUSE_DIR, name+"_empty", log), log, create),
		_shm(make_key(WAREHOUSE_DIR, name, log), sizeof(SharedVector<Item<Size>, Capacity>), log, create),
		_log(log),
		_owner(create)
	{
	std::string dir(WAREHOUSE_DIR);
	_filename = dir + "/" + _name + ".json";
	_content = _shm.get();

	if (fs::exists(_filename) && _owner) {
		_read_file();
		_log->info(_msg("Loaded state").c_str());
		if (_content->size == Capacity-1) {
			_full.lock();
		}
		if (_content->size == 0) {
			_empty.lock();
		}
		_log->info(_msg("Created - item size: %d cap: %d").c_str(), Size, Capacity);
	} else if (_owner) {
		_log->info(_msg("Initializing").c_str());
		_empty.lock();
		_log->info(_msg("Created - item size: %d cap: %d").c_str(), Size, Capacity);
	} else {
		_log->info(_msg("Attached - item size: %d cap: %d").c_str(), Size, Capacity);
	}
}

template<int Size, int Capacity>
Warehouse<Size, Capacity> Warehouse<Size, Capacity>::attach(const std::string &name, Logger *log) {
	return Warehouse(name, log, false);
}

template<int Size, int Capacity>
Warehouse<Size, Capacity> Warehouse<Size, Capacity>::create(const std::string &name, Logger *log) {
	return Warehouse(name, log);
}

// if everything works as intended, warehouse is destroyed only once...
template<int Size, int Capacity>
Warehouse<Size, Capacity>::~Warehouse () {
	// save warehouse state to file - if there is any warehouse to begin with
	if (_content != nullptr && _owner) {
		_write_file();
		_log->info(_msg("Saved to file %s").c_str(), _filename.c_str());
	}

	if (_owner) _sem.lock();
	// semaphore is removed automatically upon destruction
	// as well as shared memory
}

template<int Size, int Capacity>
void Warehouse<Size, Capacity>::add(IItem &item) const {
	// lock warehouse
	_full.lock();
	_sem.lock();

	// check capacity - if no space, just release semaphore
	if (_content->size >= Capacity) {
		_log->warn(_msg("Max capacity - cannot add item %s").c_str(), item.name().c_str());
		_sem.unlock();
		return;
	}

	// add item to list
	auto input = Item<Size>(item.name());
	_content->push_back(input);
	_log->info(_msg("Added item %s").c_str(), item.name().c_str());
	_log->debug(_msg("Capacity: %d/%d").c_str(), usage(), Capacity);

	// i *should* not do that
	if (_content->size == 1 && _empty.value() == 0) {
		_empty.unlock();
	}
	if (_content->size < Capacity) {
		_full.unlock();
	}
	// unlock warehouse
	_sem.unlock();
}

template<int Size, int Capacity>
std::unique_ptr<IItem> Warehouse<Size, Capacity>::get(const std::string &itemName) const {
	// lock warehouse
	_empty.lock();
	_sem.lock();

	if (_content->size == 0) {
		_log->info(_msg("No item %s").c_str(), itemName.c_str());
		_sem.unlock();
		return nullptr;
	}


	auto output = _content->pop_back();
	_log->info(_msg("Fetched item %s").c_str(), itemName.c_str());
	_log->debug(_msg("Capacity: %d/%d").c_str(), usage(), Capacity);

	// i *should* not do that
	if (_content->size == Capacity - 1 && _full.value() == 0) {
		_full.unlock();
	}
	if (_content->size > 0) {
		_empty.unlock();
	}

	// unlock warehouse
	_sem.unlock();

	// copy Item over to heap
	return new_item(output.name(), output.size());
}

template<int Size, int Capacity>
void Warehouse<Size, Capacity>::_write_file() {
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

template<int Size, int Capacity>
void Warehouse<Size, Capacity>::_read_file() const {
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


#endif //PROJEKT_WAREHOUSE_H