#include <chrono>
#include <thread>
#include "hebi_charts.hpp"

int application_main(int argc, char **argv) {
    // Select a desired Theme
    hebi::charts::runtime::setTheme(hebi::charts::Theme::NordDark);

    // 2x1 Grid: Top = Scope (Time series), Bottom = LatencyChart (Aggregate distribution)
    hebi::charts::GridWindow window(2,1);
    window.setTitle("C++ Latency Profiler");
    window.setSize(800, 600);

    auto scope = window.addScope();
    scope.setTitle("Measured Sleep Duration vs Target");
    scope.setYLabel("Latency");
    scope.setYUnit("s");
    scope.setYAutoUnitScaling(true);

    auto histogram = window.addLatencyChart(1);
    histogram.setTitle("Aggregate HdrHistogram");

    // Add traces
    auto sleepScope = scope.addLine("C++");
    sleepScope.setLineStyle(hebi::charts::LineStyle::Points);
    sleepScope.setShowInLegend(false);
    auto sleepHistogram = histogram.addTrace("C++");

    auto timerScope = scope.addLine("LoopTimer");
    timerScope.setLineStyle(hebi::charts::LineStyle::Points);
    timerScope.setShowInLegend(false);
    auto timerHistogram = histogram.addTrace("LoopTimer");

    // Interactive Target Cursor (defaults to 1ms)
    auto target = histogram.addYCursor();
    target.setLabel("Target");
    target.setEditable(true);
    target.setValue(1e-3);

    // Main loop
    hebi::charts::LoopTimer loopTimer; // <- manages the Windows interrupt freq (see timeBeginPeriod)
    window.show();
    while (window.isShowing()) {
        // Gurantee floor of 1ns to avoid negative intervals
        auto seconds = std::max(target.getValue(), 1e-9);
        auto nanos = static_cast<long long>(seconds * 1e9);

        // Measure C++'s native sleep
        loopTimer.tic();
        std::this_thread::sleep_for(std::chrono::nanoseconds(nanos));
        double dtSleep = loopTimer.ticToc();

        // Measure LoopTimer's tiered spin
        hebi::charts::LoopTimer::spinNanos(nanos);
        double dtSpin = loopTimer.toc();

        // Update traces
        double time = loopTimer.getElapsedTime();
        sleepScope.addPoint(time, dtSleep);
        timerScope.addPoint(time, dtSpin);

        // Record statistics
        sleepHistogram.record(dtSleep);
        timerHistogram.record(dtSpin);

        // Sync visibility so a legend click hides both traces
        sleepScope.setVisible(sleepHistogram.isVisible());
        timerScope.setVisible(timerHistogram.isVisible());
    }

    return 0;
}

// used for macOS Cocoa handoff
int main(int argc, char **argv) {
    return hebi::charts::runApplication(application_main, argc, argv);
}
