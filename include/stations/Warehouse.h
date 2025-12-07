//
// Created by mateusz on 22.11.2025.
//

#ifndef PROJEKT_WAREHOUSE_H
#define PROJEKT_WAREHOUSE_H

#include <array>
#include <filesystem>
#include <vector>
#include <sys/types.h>

#include "SharedVector.h"
#include "ipcs/Semaphore.h"
#include "ipcs/SharedMemory.h"
#include "objects/Item.h"

namespace fs = std::filesystem;

class Warehouse {
	public:
		Warehouse(const std::string& name, int capacity, int variety = 4);

		~Warehouse();
		void add(const Item &item) const;

		// pointer is necessary; retrieved item can (and will in many cases) be null!
		void get(const std::string &itemName, Item *output) const;
		// WarehouseStats &stats();

	private:
		Warehouse(
			std::string  name,
			int capacity,
			int variety,
			std::string filename,
			key_t key,
			size_t total_size
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

		// file wrappers
		void _write_file();
		void _read_file() const;
};

#endif //PROJEKT_WAREHOUSE_H