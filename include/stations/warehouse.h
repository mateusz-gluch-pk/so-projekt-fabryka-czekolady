//
// Created by mateusz on 22.11.2025.
//

#ifndef PROJEKT_WAREHOUSE_H
#define PROJEKT_WAREHOUSE_H

#include <array>
#include <vector>
#include <sys/types.h>
#include "objects/item.h"

class Warehouse {
	public:
		Warehouse(const std::string &name, int capacity);
		~Warehouse();
		int insert(Item *item);
		int take(std::string itemName);
		WarehouseStats &stats();

	private:
		int _capacity;
		std::string _name;
		std::string _filename;

		// IPCS
		key_t _key;

		std::vector<Item> _content;
		void *_shmaddr;
		int _shmid;
		int _semid;

		int _sync();

};

#endif //PROJEKT_WAREHOUSE_H