#include "fleet.hpp"

#include <cctype>

void Fleet::Add(std::unique_ptr<InventoryItem> item) {
  if (dynamic_cast<const Transport*>(item.get()) != nullptr) {
    std::string name = item->Name();
    for (char& ch : name) {
      ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    if (name.find("broken") != std::string::npos) {
      throw InspectionError();
    }
  }
  registry_.Add(std::move(item));
}

void Fleet::ShowFocus(std::ostream& out) const {
  if (auto card = focus_.lock()) {
    out << *card << '\n';
    return;
  }
  out << "expired\n";
}

void Fleet::List(std::ostream& out, bool novice_only) const {
  for (const auto& item : registry_.Items()) {
    if (!novice_only || item->ForNovice()) {
      out << *item << '\n';
    }
  }
}

double Fleet::Energy() const {
  double sum = 0;
  for (const auto& item : registry_.Items()) {
    if (const auto* consumer =
            dynamic_cast<const EnergyConsumer*>(item.get())) {
      sum += consumer->DailyEnergy();
    }
  }
  return sum;
}

std::size_t Fleet::CountNovice() const {
  return std::count_if(registry_.Items().begin(), registry_.Items().end(),
                       [](const auto& item) { return item->ForNovice(); });
}
