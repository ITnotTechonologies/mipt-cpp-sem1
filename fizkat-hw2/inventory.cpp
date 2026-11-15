#include "inventory.hpp"

#include <cmath>

constexpr int cMaxScore = 10;

std::ostream& operator<<(std::ostream& out, const ItemCard& card) {
  return out << card.id << ' ' << card.name;
}

bool operator<(const InventoryItem& left, const InventoryItem& right) {
  return left.Id() < right.Id();
}

bool operator==(const InventoryItem& left, const InventoryItem& right) {
  return left.Id() == right.Id();
}

std::ostream& operator<<(std::ostream& out, const InventoryItem& item) {
  item.Describe(out);
  return out;
}

Item::Item(ItemCard card)
    : card_(std::make_shared<const ItemCard>(std::move(card))) {
  if (Id().empty() || Name().empty()) {
    throw CommandError("empty id or name");
  }
}

PowerUse::PowerUse(double energy) : energy_(energy) {
  if (!std::isfinite(energy) || energy < 0) {
    throw CommandError("invalid energy");
  }
}

Transport::Transport(ItemCard card, std::shared_ptr<const ModelSpec> spec,
                     int score)
    : Item(std::move(card)), spec_(std::move(spec)), score_(score) {
  if (score < 1 || score > cMaxScore) {
    throw CommandError("invalid score");
  }
}
