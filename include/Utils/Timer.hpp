#pragma once

#include <chrono>

namespace QomMetro::Utils {


class Timer {

public:

    Timer() = default;

    void start();
    void stop();

    // Returns elapsed time between the last start()/stop() pair, in milliseconds.
    // Returns 0 if start() was never called.
    double elapsed_milli_seconds() const;

private:

    std::chrono::steady_clock::time_point start_time_{};
    std::chrono::steady_clock::time_point end_time_{};
    bool running_ = false;
};

} // namespace QomMetro::Utils