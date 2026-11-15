#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "fleet.hpp"

constexpr int cNoviceScore = 6;
constexpr int cPrecision = 1;

std::string ReadWord(std::istream& in) {
  std::string word;
  if (!(in >> word)) {
    throw CommandError("missing argument");
  }
  return word;
}

void CheckEnd(std::istream& in) {
  std::string extra;
  if (in >> extra) {
    throw CommandError("extra argument");
  }
}

template <typename T>
T ReadNumber(std::istream& in) {
  std::istringstream token(ReadWord(in));
  T value{};
  if (!(token >> value) || !token.eof()) {
    throw CommandError("invalid number");
  }
  return value;
}

std::unique_ptr<Transport> ReadTransport(const std::string& type, ItemCard card,
                                         std::istream& in) {
  static const auto bike =
      std::make_shared<const ModelSpec>(ModelSpec{"bike", cNoviceScore});
  static const auto scooter =
      std::make_shared<const ModelSpec>(ModelSpec{"scooter", cNoviceScore});
  static const auto ebike =
      std::make_shared<const ModelSpec>(ModelSpec{"ebike", cNoviceScore});
  if (type == bike->type) {
    return std::make_unique<Bike>(std::move(card), bike, ReadNumber<int>(in));
  }
  if (type != scooter->type && type != ebike->type) {
    throw CommandError("unknown item type");
  }
  double energy = ReadNumber<double>(in);
  int score = ReadNumber<int>(in);
  if (type == scooter->type) {
    return std::make_unique<Scooter>(std::move(card), scooter, score, energy);
  }
  return std::make_unique<ElectricBike>(std::move(card), ebike, score, energy);
}

std::unique_ptr<InventoryItem> ReadItem(std::istream& in) {
  std::string type = ReadWord(in);
  ItemCard card{ReadWord(in), ReadWord(in)};
  if (type == "helmet") {
    return std::make_unique<Helmet>(std::move(card));
  }
  if (type == "lock") {
    return std::make_unique<BikeLock>(std::move(card));
  }
  if (type == "dock") {
    return std::make_unique<Dock>(std::move(card), ReadNumber<double>(in));
  }
  return ReadTransport(type, std::move(card), in);
}

void PrintReport(const std::string& command, const Fleet& fleet,
                 std::ostream& out) {
  if (command == "list" || command == "novice") {
    fleet.List(out, command == "novice");
  } else if (command == "energy") {
    out << fleet.Energy() << '\n';
  } else if (command == "report") {
    out << fleet.Size() << ' ' << fleet.Energy() << '\n';
  } else if (command == "count_novice") {
    out << fleet.CountNovice() << '\n';
  } else if (command == "show_focus") {
    fleet.ShowFocus(out);
  } else {
    throw CommandError("unknown command");
  }
}

bool RunCommand(std::istream& in, Fleet& fleet, std::ostream& out) {
  std::string command;
  if (!(in >> command)) {
    return true;
  }
  if (command == "add") {
    auto item = ReadItem(in);
    CheckEnd(in);
    fleet.Add(std::move(item));
  } else if (command == "focus" || command == "remove") {
    std::string id = ReadWord(in);
    CheckEnd(in);
    if (command == "focus") {
      fleet.Focus(id);
    } else {
      fleet.Remove(id);
    }
  } else {
    CheckEnd(in);
    if (command == "quit") {
      return false;
    }
    PrintReport(command, fleet, out);
  }
  return true;
}

int main() {
  Fleet fleet;
  std::string line;
  std::cout << std::fixed << std::setprecision(cPrecision);
  while (std::getline(std::cin, line)) {
    std::istringstream in(line);
    try {
      if (!RunCommand(in, fleet, std::cout)) {
        break;
      }
    } catch (const FleetError& error) {
      std::cout << "error: " << error.what() << '\n';
    }
  }
}
