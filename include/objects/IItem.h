//
// Created by mateusz on 20.01.2026.
//

#ifndef FACTORY_IITEM_H
#define FACTORY_IITEM_H

#include <string>

#include "Item.h"
#include "Item.h"
#include "Item.h"
#include "Item.h"
#include "ftxui/component/event.hpp"

class IItem {
public:
    virtual ~IItem() = default;  // REQUIRED for polymorphic deletion
    [[nodiscard]] virtual std::string name() const = 0;
    [[nodiscard]] virtual int size() const = 0;
    virtual std::unique_ptr<IItem> clone() const = 0;
};

#endif //FACTORY_IITEM_H