//
// Created by mateusz on 22.11.2025.
//

#ifndef PROJEKT_WAREHOUSE_H
#define PROJEKT_WAREHOUSE_H

#include <array>
#include <filesystem>
#include <vector>
#include <sys/types.h>

#include "objects/SharedVector.h"
#include "ipcs/Semaphore.h"
#include "ipcs/SharedMemory.h"
#include "objects/Item.h"

namespace fs = std::filesystem;

class Warehouse {
	public:
		Warehouse(const std::string& name, int capacity, Logger *log, int variety = 4);

		~Warehouse();
		void add(Item &item) const;

		// pointer is necessary; retrieved item can (and will in many cases) be null!
		void get(const std::string &itemName, Item *output) const;
		// WarehouseStats &stats();

		void reattach(Logger *log) {
			size_t total_size = sizeof(SharedVector<Item>) + sizeof(Item) * _variety;
			_sem = Semaphore::attach(_key, log);
			_shm = SharedMemory<SharedVector<Item>>::attach(_key, total_size, log);
			_log = log;
		};

	private:
		Warehouse(
			std::string  name,
			int capacity,
			int variety,
			size_t total_size,
			Logger *log
		);

		int _capacity;
		int _variety;
		std::string _name;
		fs::path _filename;

		// IPCS
		key_t _key;
		Semaphore _sem;
		SharedMemory<SharedVector<Item>> _shm;
		SharedVector<Item> *_content;

		// logger
		Logger *_log;

		// file wrappers
		void _write_file();
		void _read_file() const;
};

#endif //PROJEKT_WAREHOUSE_H