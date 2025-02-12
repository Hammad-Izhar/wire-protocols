#include <stdexcept>

#include "models/snowflake.hpp"

SnowflakeIDGenerator::SnowflakeIDGenerator(int64_t machineId, int64_t processId) {
    if (machineId > maxMachineId || machineId < 0) {
        throw std::runtime_error("Machine ID out of range");
    }
    if (processId > maxProcessId || processId < 0) {
        throw std::runtime_error("Process ID out of range");
    }
    this->machineId = machineId;
    this->processId = processId;
}

SnowflakeIDGenerator& SnowflakeIDGenerator::get_instance() {
    static SnowflakeIDGenerator instance(1, 1);
    return instance;
}

int64_t SnowflakeIDGenerator::currentTimestamp() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}

int64_t SnowflakeIDGenerator::nextId() {
    std::lock_guard<std::mutex> lock(mutex);
    int64_t timestamp = currentTimestamp();

    if (timestamp < lastTimestamp) {
        throw std::runtime_error("Clock moved backwards");
    }

    if (timestamp == lastTimestamp) {
        sequence = (sequence + 1) & maxSequence;
        if (sequence == 0) {
            while (timestamp <= lastTimestamp) {
                timestamp = currentTimestamp();  // Wait for next millisecond
            }
        }
    } else {
        sequence = 0;
    }

    lastTimestamp = timestamp;

    return ((timestamp - epoch) << (machineIdBits + processIdBits + sequenceBits)) |
           (machineId << (processIdBits + sequenceBits)) | (processId << sequenceBits) | sequence;
}