#include <cmath>
#include "hebi_charts.hpp"

int application_main(int argc, char **argv) {
    // Select a desired Theme
    hebi::charts::runtime::setTheme(hebi::charts::Theme::NordDark);

    // Create a 1x1 grid window with a single scope (time series chart) and trace
    hebi::charts::GridWindow window;
    window.setTitle("Python Time Series");
    window.setSize(800, 600);

    auto chart = window.addScope();
    chart.setTitle("Scope");
    chart.setYLabel("sin");

    auto sinewave = chart.addLine("sine");
    sinewave.setLineStyle(hebi::charts::LineStyle::Solid);

    // Add some basic controls to change the parameters
    auto controls = window.getControlPanel();
    controls.setWidth(300);

    auto amplitude = controls.addSlider(); // label, limits(min, max), value
    amplitude.setLabel("Amplitude");
    amplitude.setLimits(0.0, 20.0);
    amplitude.setValue(1.0);

    auto frequency = controls.addSlider();
    frequency.setLabel("Frequency");
    frequency.setLimits(0.1, 10.0);
    frequency.setValue(1.0);

    // Initialization
    double phase = 0;
    window.show();

    // Use the built-in timer utility for more accurate loop rates
    hebi::charts::LoopTimer loopTimer; // initializes tic()
    loopTimer.setFrequency(100);

    // Generate sine wave
    while (window.isShowing()) {
        loopTimer.waitForNextTick();

        // accumulate phase to avoid discontinuities
        double dt = loopTimer.ticToc();
        phase += 2 * 3.14159 * frequency.getValue() * dt;
        double y = amplitude.getValue() * std::sin(phase);

        // append one point (non-blocking)
        sinewave.addPoint(loopTimer.getElapsedTime(), y);
    }

    return 0;
}

// macOS needs to own the main thread, so we need to
// do a handover. no-op on other operating systems.
int main(int argc, char **argv) {
    return hebi::charts::runApplication(application_main, argc, argv);
}
