//
// Created by mateusz on 22.11.2025.
//

#ifndef PROJEKT_WAREHOUSE_H
#define PROJEKT_WAREHOUSE_H

#include <array>
#include <filesystem>
#include <vector>
#include <sys/types.h>

#include "WarehouseStats.h"
#include "objects/SharedVector.h"
#include "ipcs/Semaphore.h"
#include "ipcs/SharedMemory.h"
#include "objects/Item.h"

namespace fs = std::filesystem;

class Warehouse {
	public:
		static Warehouse attach(const std::string& name, int capacity, Logger *log, int variety = 4);
		static Warehouse create(const std::string& name, int capacity, Logger *log, int variety = 4);
		Warehouse(const std::string& name, int capacity, Logger *log, int variety = 4, bool create = true);
		~Warehouse();

		void add(Item &item);

		// pointer is necessary; retrieved item can (and will in many cases) be null!
		void get(const std::string &itemName, Item *output);

		[[nodiscard]] std::vector<Item> items() const;
		[[nodiscard]] const std::string &name() const;
		[[nodiscard]] int capacity() const;
		[[nodiscard]] int variety() const;
		[[nodiscard]] int usage() const;

	private:
		Warehouse(std::string name, int capacity, int variety, size_t total_size, key_t key, Logger *log, bool create);

		int _capacity;
		int _variety;
		std::string _name;
		fs::path _filename;

		// IPCS
		bool _owner;
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