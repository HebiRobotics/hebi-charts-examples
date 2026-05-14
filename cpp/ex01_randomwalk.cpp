#include <random>
#include <chrono>
#include <thread>
#include "hebi_charts.hpp"

int application_main(int argc, char **argv) {
    // Select a desired Theme
    hebi::charts::runtime::setTheme(hebi::charts::Theme::NordDark);

    // Create a 1x1 grid window with a single chart element
    hebi::charts::GridWindow window;
    window.setTitle("Python Random Walk");
    window.setSize(1200, 600);

    auto chart = window.addLineChart();
    chart.setTitle("Random Walk");
    chart.setXLabel("sample");
    chart.setYLabel("rnd(x)");

    // Add a trace
    auto line = chart.addLine("walk");
    line.setLineStyle(hebi::charts::LineStyle::Solid);
    line.setMaxPointCount(10000);

    // Continuously update a random walk
    std::default_random_engine rng;
    std::uniform_real_distribution<double> random(-1.0, 1.0);

    double x = 0, y = 0;
    window.show();
    while (window.isShowing()) {
        // reduce the rate (optional)
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // append one point (non-blocking)
        x += 1;
        y += random(rng);
        line.addPoint(x, y);
    }

    return 0;
}

// macOS needs to own the main thread, so we need to
// do a handover. no-op on other operating systems.
int main(int argc, char **argv) {
    return hebi::charts::runApplication(application_main, argc, argv);
}
