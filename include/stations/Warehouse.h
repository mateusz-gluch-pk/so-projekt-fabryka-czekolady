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

#define WAREHOUSE_MAX_VARIETY 1024

namespace fs = std::filesystem;

class Warehouse {
	public:
		static Warehouse attach(const std::string& name, int capacity, Logger *log);
		static Warehouse create(const std::string& name, int capacity, Logger *log);
		Warehouse(const std::string& name, int capacity, Logger *log, bool create = true);
		~Warehouse();

		void add(Item &item) const;

		// pointer is necessary; retrieved item can (and will in many cases) be null!
		void get(const std::string &itemName, Item *output) const;

		[[nodiscard]] std::vector<Item> items() const;
		[[nodiscard]] std::string name() const;
		[[nodiscard]] int capacity() const;

		static int variety() ;
		[[nodiscard]] int usage() const;

	private:
		Warehouse(std::string name, int capacity, size_t total_size, key_t key, Logger *log, bool create);

		[[nodiscard]] std::string _msg(const std::string &msg) const {
			return "stations/Warehouse/" + _name + ":\t" + msg;
		}

		int _capacity;
		std::string _name;
		fs::path _filename;

		// IPCS
		bool _owner;
		Semaphore _sem;
		SharedMemory<SharedVector<Item, WAREHOUSE_MAX_VARIETY>> _shm;
		SharedVector<Item, WAREHOUSE_MAX_VARIETY> *_content;

		// logger
		Logger *_log;

		// file wrappers
		void _write_file();
		void _read_file() const;
};

#endif //PROJEKT_WAREHOUSE_H