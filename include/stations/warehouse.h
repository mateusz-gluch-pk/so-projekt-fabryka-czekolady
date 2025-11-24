//
// Created by mateusz on 22.11.2025.
//

#ifndef PROJEKT_WAREHOUSE_H
#define PROJEKT_WAREHOUSE_H

#include <array>
#include <filesystem>
#include <vector>
#include <sys/types.h>
#include "objects/item.h"

namespace fs = std::filesystem;

class Warehouse {
	public:
		Warehouse(const std::string &name, int capacity);
		~Warehouse();
		void add(const Item &item);

		// pointer is necessary; retrieved item can (and will in many cases) be null!
		Item *get(const std::string &itemName);
		// WarehouseStats &stats();

	private:
		int _capacity;
		std::string _name;
		fs::path _filename;

		// IPCS
		key_t _key;

		std::vector<Item> _content;
		void *_shmaddr;
		int _shmid;
		int _semid;

		// shared memory wrappers
		void _write_shm(std::vector<Item> &content);
		std::vector<Item> &_read_shm();

		// file wrappers
		void _write_file(std::vector<Item> &content);
		std::vector<Item> &_read_file();
};

#endif //PROJEKT_WAREHOUSE_H