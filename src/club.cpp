#include "club.h"

time_t parseTime(const std::string& timeStr) {
  std::tm tm{};
  std::istringstream ss(timeStr);
  ss >> std::get_time(&tm, "%H:%M");
  if (ss.fail()) {
    throw std::invalid_argument("Failed to parse time");
  }
  return std::mktime(&tm);
}

std::istream& operator>>(std::istream& input, ComputerClub::Event& event) {
  std::getline(input, event.inputLine);
  if (input.fail()) {
    return input;
  }
  std::stringstream is(event.inputLine);

  std::string timeStr;
  is >> timeStr;
  if (is.fail()) {
    return input;
  }

  event.time = parseTime(timeStr);
  uint8_t raw_id;
  is >> raw_id;
  if (is.fail()) {
    return input;
  }

  switch (raw_id) {
    case '1':
      event.id = ComputerClub::InputEventId::kNewClient;
      is >> event.name;
      break;
    case '2':
      event.id = ComputerClub::InputEventId::kClientChooseTable;
      is >> event.name >> event.tableNum;
      --event.tableNum;
      break;
    case '3':
      event.id = ComputerClub::InputEventId::kClientWaiting;
      is >> event.name;
      break;
    case '4':
      event.id = ComputerClub::InputEventId::kClientLeave;
      is >> event.name;
      break;
    default:
      throw std::invalid_argument(event.inputLine);
  }
  if (is.fail()) {
    throw std::invalid_argument(event.inputLine);
  }

  return input;
}

bool ComputerClub::Event::operator<(const Event& other) const {
  return time < other.time;
}

ComputerClub::ComputerClub(uint32_t numTables, time_t startTime, time_t endTime,
                           uint32_t costPerHour, ConsoleOutputAdapter adapter)
    : startTime_(startTime),
      endTime_(endTime),
      tablesNums_(numTables),
      tableState_(numTables, costPerHour),
      adapter_(adapter) {
  adapter_.printTime(startTime);
}

void ComputerClub::endOfWork() {
  for (const auto& [name, table] : clientsTables_) {
    if (table.has_value()) {
      tableState_.freePlace(table.value(), endTime_);
    }
    adapter_.processEvent(
        OutputEvent<OutputEventId::kClientLeave>(endTime_, name));
  }
  adapter_.printTime(endTime_);
  tableState_.ShowStatistics(adapter_);
}

void ComputerClub::checkWaiting(uint32_t possibility, time_t currentTime) {
  if (!waiting_.empty()) {
    auto iter = waiting_.front();
    waiting_.pop();
    iter->second = possibility;
    tableState_.occupyPlace(possibility, currentTime);

    adapter_.processEvent(OutputEvent<OutputEventId::kClientChooseTable>(
        currentTime, iter->first, possibility));
  }
}

void ComputerClub::processEvent(const Event& event) {
  adapter_.repeatMessage(event.inputLine);
  switch (event.id) {
    case InputEventId::kNewClient:
      if (clientsTables_.contains(event.name)) {
        adapter_.processEvent(
            OutputEvent<OutputEventId::kError>(event.time, "YouShallNotPass"));
        return;
      }
      if (event.time < startTime_ || event.time > endTime_) {
        adapter_.processEvent(
            OutputEvent<OutputEventId::kError>(event.time, "NotOpenYet"));
        return;
      }
      clientsTables_.emplace(event.name, std::nullopt);
      break;

    case InputEventId::kClientChooseTable: {
      if (event.tableNum >= tablesNums_) {
        throw std::invalid_argument(event.inputLine);
      }

      if (bool success = tableState_.occupyPlace(event.tableNum, event.time);
          !success) {
        adapter_.processEvent(
            OutputEvent<OutputEventId::kError>(event.time, "PlaceIsBusy"));
        return;
      }
      const auto search = clientsTables_.find(event.name);
      if (search == clientsTables_.end()) {
        adapter_.processEvent(
            OutputEvent<OutputEventId::kError>(event.time, "ClientUnknown"));
        return;
      }

      if (search->second.has_value()) {
        tableState_.freePlace(search->second.value(), event.time);
      }

      search->second = event.tableNum;
      tableState_.occupyPlace(event.tableNum, event.time);
      break;
    }

    case InputEventId::kClientWaiting: {
      if (!tableState_.Full()) {
        adapter_.processEvent(
            OutputEvent<OutputEventId::kError>(event.time, "ICanWaitNoLonger!"));
        return;
      }
      if (waiting_.size() == tablesNums_) {
        adapter_.processEvent(
            OutputEvent<OutputEventId::kClientLeave>(event.time, event.name));
        return;
      }
      auto search = clientsTables_.find(event.name);
      if (search == clientsTables_.end()) {
        adapter_.processEvent(
            OutputEvent<OutputEventId::kError>(event.time, "ClientUnknown"));
        return;
      }
      waiting_.push(search);
      break;
    }
    case InputEventId::kClientLeave: {
      auto search = clientsTables_.find(event.name);
      if (search == clientsTables_.end()) {
        adapter_.processEvent(
            OutputEvent<OutputEventId::kError>(event.time, "ClientUnknown"));
        return;
      }

      if (search->second.has_value()) {
        tableState_.freePlace(search->second.value(), event.time);
        checkWaiting(search->second.value(), event.time);
      }
      clientsTables_.erase(search);
      break;
    }
    default:
      throw std::invalid_argument(event.inputLine);
  }
}
