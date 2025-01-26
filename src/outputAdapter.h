#pragma once
#include <cstdint>
#include <iomanip>
#include <iostream>

enum class OutputEventId : uint8_t {
  kClientLeave = 11,
  kClientChooseTable,
  kError,
};

struct OutputEventBase {
 protected:
  explicit OutputEventBase(time_t time) : time(time) {}

 public:
  std::time_t time;
};

template <OutputEventId Id>
struct OutputEvent;

template <>
struct OutputEvent<OutputEventId::kClientLeave> : OutputEventBase {
  OutputEvent(time_t time, std::string_view name)
      : OutputEventBase(time), name(name) {}

  std::string_view name;
};

template <>
struct OutputEvent<OutputEventId::kClientChooseTable> : OutputEventBase {
  OutputEvent(time_t time, std::string_view name, uint32_t tableNum)
      : OutputEventBase(time), name(name), tableNum(tableNum) {}

  std::string_view name;
  uint32_t tableNum;
};

template <>
struct OutputEvent<OutputEventId::kError> : OutputEventBase {
  OutputEvent(time_t time, std::string_view message)
      : OutputEventBase(time), message(message) {}

  std::string_view message;
};

class ConsoleOutputAdapter {
 public:
  template <OutputEventId Id>
  static void processEvent(const OutputEvent<Id>& event) {
    std::tm* tm = std::localtime(&event.time);
    std::cout << std::put_time(tm, "%H:%M") << ' ' << static_cast<uint16_t>(Id)
              << ' ';

    if constexpr (Id == OutputEventId::kClientLeave) {
      std::cout << event.name;
    } else if constexpr (Id == OutputEventId::kClientChooseTable) {
      std::cout << event.name << ' ' << event.tableNum + 1;
    } else if constexpr (Id == OutputEventId::kError) {
      std::cout << event.message;
    }
    std::cout << '\n';
  }

  static void printTime(time_t time);
  static void repeatMessage(std::string_view message);
  static void showStatistics(uint32_t number, time_t time, uint64_t money);
};
