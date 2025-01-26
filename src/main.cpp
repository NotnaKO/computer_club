#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>

#include "club.h"

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <input_file>\n";
    return 1;
  }

  try {
    std::ifstream inputFile(argv[1]);

    std::string line;
    if (!inputFile.is_open()) {
      throw std::invalid_argument("Error opening file");
    }

    std::getline(inputFile, line);
    uint32_t numTables;
    try {
      auto x = std::stoll(line);
      if (x < 1 || x > std::numeric_limits<uint32_t>::max()) {
        throw std::invalid_argument(line);
      }
      numTables = static_cast<uint32_t>(x);
    } catch (std::exception&) {
      throw std::invalid_argument(line);
    }
    std::getline(inputFile, line);
    std::istringstream ss(line);
    std::string startStr, endStr;
    ss >> startStr >> endStr;
    if (!ss) {
      throw std::invalid_argument(line);
    }

    auto startTime = parseTime(startStr);
    auto endTime = parseTime(endStr);
    if (startTime > endTime) {
      throw std::invalid_argument(line);
    }

    std::getline(inputFile, line);
    uint64_t costPerHour;
    try {
      auto x = std::stoll(line);
      if (x < 0) {
        throw std::invalid_argument(line);
      }
      costPerHour = x;
    } catch (std::exception&) {
      throw std::invalid_argument(line);
    }
    ComputerClub club(numTables, startTime, endTime, costPerHour);

    ComputerClub::Event prevEvent, event;
    prevEvent.time = std::numeric_limits<time_t>::min();
    while (inputFile >> event) {
      if (event < prevEvent) {
        throw std::invalid_argument("Events are not sorted by time");
      }
      club.processEvent(event);
      prevEvent = event;
    }

    if (!inputFile.eof()) {
      throw std::invalid_argument("Error reading file");
    }

    club.endOfWork();
    return 0;
  } catch (std::exception& e) {
    std::cout << "Error: " << e.what() << '\n';
    return 2;
  }
}
