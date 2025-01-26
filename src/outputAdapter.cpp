#include "outputAdapter.h"

void ConsoleOutputAdapter::printTime(time_t time) {
  std::tm* tm = std::localtime(&time);
  std::cout << std::put_time(tm, "%H:%M") << '\n';
}

void ConsoleOutputAdapter::repeatMessage(std::string_view message) {
  std::cout << message << '\n';
}

void ConsoleOutputAdapter::showStatistics(uint32_t number, time_t delta_time,
                                          uint64_t money) {
  delta_time /= 60;
  std::tm tm = {};
  tm.tm_min = delta_time % 60;
  tm.tm_hour = delta_time / 60;
  std::cout << number << ' ' << money << ' ' << std::put_time(&tm, "%H:%M")
            << '\n';
}
