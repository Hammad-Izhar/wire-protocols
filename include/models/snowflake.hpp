#include <mutex>

class SnowflakeIDGenerator {
   public:
    SnowflakeIDGenerator(int64_t machineId, int64_t processId);

    static SnowflakeIDGenerator& get_instance();

    int64_t nextId();

   private:
    int64_t machineId;
    int64_t processId;
    int64_t lastTimestamp = -1;
    int64_t sequence = 0;
    std::mutex mutex;

    static constexpr int64_t epoch = 1738022400000;  // Custom epoch (Jan 28th, 2025)
    static constexpr int64_t machineIdBits = 10;
    static constexpr int64_t processIdBits = 5;
    static constexpr int64_t sequenceBits = 8;

    static constexpr int64_t maxMachineId = (1L << machineIdBits) - 1;
    static constexpr int64_t maxProcessId = (1L << processIdBits) - 1;
    static constexpr int64_t maxSequence = (1L << sequenceBits) - 1;

    int64_t currentTimestamp();
};