#include <mutex>

/**
 * @brief Generates unique identifiers using a Snowflake-like algorithm.
 *
 * The SnowflakeIDGenerator class produces unique 64-bit IDs based on the current timestamp,
 * machine identifier, process identifier, and an internal sequence number. The generated IDs are
 * unique across machines and processes, and the class ensures thread-safe ID generation.
 */
class SnowflakeIDGenerator {
   public:
    /**
     * @brief Constructs a new SnowflakeIDGenerator instance.
     *
     * @param machineId A unique identifier for the machine (should be within [0, maxMachineId]).
     * @param processId A unique identifier for the process on the machine (should be within [0, maxProcessId]).
     */
    SnowflakeIDGenerator(int64_t machineId, int64_t processId);

    /**
     * @brief Retrieves the singleton instance of the SnowflakeIDGenerator.
     *
     * This method returns a reference to a thread-local instance of SnowflakeIDGenerator, ensuring that
     * each thread uses its own instance.
     *
     * @return Reference to the singleton SnowflakeIDGenerator instance.
     */
    static SnowflakeIDGenerator& get_instance();

    /**
     * @brief Generates the next unique identifier.
     *
     * Produces a new 64-bit ID by combining the current timestamp, machine ID, process ID, and an
     * internal sequence number. This method is thread-safe and guarantees that no two IDs are identical.
     *
     * @return A unique 64-bit integer ID.
     */
    int64_t nextId();

   private:
    /// Unique identifier for the machine.
    int64_t machineId;
    /// Unique identifier for the process.
    int64_t processId;
    /// The timestamp of the last generated ID (in milliseconds). Initialized to -1.
    int64_t lastTimestamp = -1;
    /// Sequence number for IDs generated within the same millisecond.
    int64_t sequence = 0;
    /// Mutex to ensure thread-safe access during ID generation.
    std::mutex mutex;

    /// Custom epoch (in milliseconds) from which timestamps are measured (Jan 28th, 2025).
    static constexpr int64_t epoch = 1738022400000;
    /// Number of bits allocated for the machine identifier.
    static constexpr int64_t machineIdBits = 10;
    /// Number of bits allocated for the process identifier.
    static constexpr int64_t processIdBits = 5;
    /// Number of bits allocated for the sequence number.
    static constexpr int64_t sequenceBits = 8;

    /// Maximum allowed value for machineId.
    static constexpr int64_t maxMachineId = (1L << machineIdBits) - 1;
    /// Maximum allowed value for processId.
    static constexpr int64_t maxProcessId = (1L << processIdBits) - 1;
    /// Maximum allowed value for sequence.
    static constexpr int64_t maxSequence = (1L << sequenceBits) - 1;

    /**
     * @brief Retrieves the current timestamp.
     *
     * Returns the current time in milliseconds. This timestamp is used as part of the ID generation
     * process.
     *
     * @return Current timestamp in milliseconds.
     */
    int64_t currentTimestamp();
};
