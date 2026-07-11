#pragma once

/// @brief Global frame counter shared across the application.
extern unsigned int globalCounter;

namespace Utils {

    /**
     * @struct TickCounter
     * @brief Game tick utility.
     * 
     * Has the ability to detect when on a game tick, and provide a tick count mod an interval.
     */
    struct TickCounter {
        /**
         * @brief Construct a new TickCounter.
         * 
         * If only wanting to use the onTick() method, use this methos to initialize.
         */
        TickCounter();

        /**
         * @brief Construct a new TickCounter with a given interval.
         *
         * If wanting to use the tickCount() method, use this method to initialize.
         * 
         * @param interval Number of ticks.
         */
        TickCounter(const int& interval);

        /**
         * @brief Check if we are currently on a game tick.
         *
         * A game tick will occur once every 8 frames.
         * Uses globalCounter to determine whether on a game tick.
         * 
         * @return true if currently on a game tick, false otherwise.
         */
        static bool onTick();

        /**
         * @brief Get the number of ticks elapsed within the interval.
         *
         * Divides the global counter by 8 (tick grouping) and applies modulo with the interval.
         *
         * @return int Current tick count within the interval.
         */
        int tickCount() const;

        /// @brief Number of game ticks to perform modulo for the tickCount().
        int m_interval;

        /// @brief Phase offset applied before modulo, allowing entities to start on different frames.
        int m_offset = 0;
    };

    /**
     * @brief Reset the global counter back to zero.
     */
    void resetGlobalCounter();

    /**
     * @brief Increment the global counter by one.
     */
    void incrementGlobalCounter();
}
