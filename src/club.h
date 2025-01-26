#pragma once
#include <cstdint>
#include <iomanip>
#include <map>
#include <optional>
#include <queue>
#include <string>

#include "outputAdapter.h"
#include "tableState.h"

time_t parseTime(const std::string& timeStr);

class ComputerClub {
 public:
  enum class InputEventId : uint8_t {
    kNewClient = 1,
    kClientChooseTable,
    kClientWaiting,
    kClientLeave,
  };

  class Event {
   public:
    friend std::istream& operator>>(std::istream& input, Event& event);
    bool operator<(const Event& other) const;

    std::string inputLine;
    time_t time{};

    // Possible arguments
    std::string name;
    uint32_t tableNum = 0;

    InputEventId id;
  };

  ComputerClub(uint32_t numTables, time_t startTime, time_t endTime,
               uint32_t costPerHour, ConsoleOutputAdapter adapter = {});

  void processEvent(const Event& event);

  void endOfWork();

 private:
  void checkWaiting(uint32_t possibility, time_t currentTime);

  std::time_t startTime_;
  std::time_t endTime_;
  uint32_t tablesNums_;

  // Internal state
  std::map<std::string, std::optional<uint32_t>> clientsTables_;
  TableState tableState_;
  std::queue<decltype(clientsTables_)::iterator> waiting_;

  [[no_unique_address]] ConsoleOutputAdapter adapter_;
};
