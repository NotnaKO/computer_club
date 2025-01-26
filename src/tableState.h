#pragma once
#include <cstdint>
#include <optional>
#include <vector>

#include "outputAdapter.h"

class TableState {
  struct Table {
    time_t delta = 0;
    uint64_t money = 0;
    std::optional<time_t> startTime;
  };

 public:
  explicit TableState(uint32_t count, uint64_t costPerHour);
  bool Full() const noexcept;
  bool occupyPlace(uint32_t num, time_t currentTime);
  void freePlace(uint32_t num, time_t currentTime);

  void ShowStatistics(ConsoleOutputAdapter& adapter) const;

 private:
  uint64_t costPerHour;
  std::vector<Table> tables_;
  uint32_t busyCount_ = 0;
};