#include "tableState.h"

TableState::TableState(uint32_t count, uint64_t cost)
    : costPerHour(cost), tables_(count) {}

bool TableState::Full() const noexcept { return busyCount_ == tables_.size(); }

bool TableState::occupyPlace(uint32_t num, time_t currentTime) {
  if (tables_[num].startTime.has_value()) {
    return false;
  }
  tables_[num].startTime = currentTime;
  ++busyCount_;
  return true;
}

void TableState::freePlace(uint32_t num, time_t currentTime) {
  auto delta = currentTime - tables_[num].startTime.value();
  tables_[num].startTime.reset();
  --busyCount_;
  tables_[num].delta += delta;
  tables_[num].money += ((delta - 1) / 3600 + 1) * costPerHour;
}

void TableState::ShowStatistics(ConsoleOutputAdapter& adapter) const {
  for (size_t i = 0; i < tables_.size(); ++i) {
    adapter.showStatistics(i + 1, tables_[i].delta, tables_[i].money);
  }
}
