#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include "inventory.hpp"

// Позиции всегда упорядочены по инвентарному номеру.
template <typename T>
class Registry {
 public:
  void Add(std::unique_ptr<T> item) {
    auto pos = std::lower_bound(
        items_.begin(), items_.end(), item,
        [](const auto& left, const auto& right) { return *left < *right; });
    if (pos != items_.end() && **pos == *item) {
      throw CommandError("duplicate id");
    }
    items_.insert(pos, std::move(item));
  }

  const T& Get(const std::string& id) const { return **Find(id); }
  void Remove(const std::string& id) { items_.erase(Find(id)); }

  const auto& Items() const { return items_; }

 private:
  std::vector<std::unique_ptr<T>> items_;

  auto LowerBound(const std::string& id) const {
    return std::lower_bound(
        items_.begin(), items_.end(), id,
        [](const auto& item, const auto& key) { return item->Id() < key; });
  }

  auto Find(const std::string& id) const {
    auto pos = LowerBound(id);
    if (pos == items_.end() || (*pos)->Id() != id) {
      throw CommandError("unknown id");
    }
    return pos;
  }
};

class Fleet {
 public:
  void Add(std::unique_ptr<InventoryItem> item);
  void Focus(const std::string& id) { focus_ = registry_.Get(id).Card(); }
  void ShowFocus(std::ostream& out) const;
  void Remove(const std::string& id) { registry_.Remove(id); }
  void List(std::ostream& out, bool novice_only) const;
  double Energy() const;
  auto Size() const { return registry_.Items().size(); }
  std::size_t CountNovice() const;

 private:
  Registry<InventoryItem> registry_;
  std::weak_ptr<const ItemCard> focus_;
};
