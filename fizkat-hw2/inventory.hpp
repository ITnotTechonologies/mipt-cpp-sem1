#pragma once

#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>

class FleetError : public std::runtime_error {
 public:
  using std::runtime_error::runtime_error;
};

class CommandError : public FleetError {
 public:
  using FleetError::FleetError;
};

class InspectionError : public FleetError {
 public:
  InspectionError() : FleetError("inspection rejected") {}
};

struct ItemCard {
  std::string id;
  std::string name;
};

std::ostream& operator<<(std::ostream& out, const ItemCard& card);

class InventoryItem {
 public:
  virtual ~InventoryItem() = default;
  virtual const std::string& Id() const = 0;
  virtual const std::string& Name() const = 0;
  virtual void Describe(std::ostream& out) const = 0;
  virtual std::weak_ptr<const ItemCard> Card() const = 0;
  virtual bool ForNovice() const { return false; }
};

bool operator<(const InventoryItem& left, const InventoryItem& right);
bool operator==(const InventoryItem& left, const InventoryItem& right);
std::ostream& operator<<(std::ostream& out, const InventoryItem& item);

class Item : public InventoryItem {
 public:
  explicit Item(ItemCard card);
  const std::string& Id() const override { return card_->id; }
  const std::string& Name() const override { return card_->name; }
  void Describe(std::ostream& out) const override { out << *card_; }
  std::weak_ptr<const ItemCard> Card() const override { return card_; }

 private:
  std::shared_ptr<const ItemCard> card_;
};

class EnergyConsumer {
 public:
  virtual ~EnergyConsumer() = default;
  virtual double DailyEnergy() const = 0;
};

class PowerUse : public EnergyConsumer {
 public:
  explicit PowerUse(double energy);
  double DailyEnergy() const override { return energy_; }

 private:
  double energy_;
};

struct ModelSpec {
  std::string type;
  int novice_score;
};

class Transport : public Item {
 public:
  Transport(ItemCard card, std::shared_ptr<const ModelSpec> spec, int score);
  bool ForNovice() const override { return score_ >= spec_->novice_score; }

 private:
  std::shared_ptr<const ModelSpec> spec_;
  int score_;
};

class Bike final : public Transport {
 public:
  using Transport::Transport;
};

class ElectricTransport : public Transport, public PowerUse {
 public:
  ElectricTransport(ItemCard card, std::shared_ptr<const ModelSpec> spec,
                    int score, double energy)
      : Transport(std::move(card), std::move(spec), score), PowerUse(energy) {}
};

class Scooter final : public ElectricTransport {
 public:
  using ElectricTransport::ElectricTransport;
};

class ElectricBike final : public ElectricTransport {
 public:
  using ElectricTransport::ElectricTransport;
};

class Helmet final : public Item {
 public:
  using Item::Item;
};

class Dock final : public Item, public PowerUse {
 public:
  Dock(ItemCard card, double energy)
      : Item(std::move(card)), PowerUse(energy) {}
};

class BikeLock final : public Item {
 public:
  using Item::Item;
};
