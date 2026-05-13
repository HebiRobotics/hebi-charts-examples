#include <cmath>
#include <vector>
#include <string>
#include <random>
#include "hebi_charts.hpp"

#define PI 3.14159

int application_main(int argc, char **argv) {
    // Select a desired Theme
    hebi::charts::runtime::setTheme(hebi::charts::Theme::NordDark);

    // 3x1 Grid: Top 2/3 = 3D robot view, Bottom 1/3 = Scope
    hebi::charts::GridWindow window(3);
    window.setTitle("Python Robot");
    window.setSize(1000, 600);

    // 3D Robot
    auto scene = window.addScene3d(0,0,2,1);
    scene.setMinZ(0);
    auto robot = scene.addRobot("https://raw.githubusercontent.com/HebiRobotics/hebi-hrdf/main/A-2240-06G.hrdf");

    // 2D Scope
    auto chart = window.addScope(2,0,1,1);
    chart.setTitle("Position");
    chart.setYUnit("rad");
    std::vector<hebi::charts::Line> jointTraces;
    for (int i = 0; i < robot.getDof(); ++i) {
        jointTraces.push_back(chart.addLine("Joint " + std::to_string(i + 1)));
        jointTraces.back().setLineStyle(hebi::charts::LineStyle::Solid);
    }

    // Basic controls
    auto controls = window.getControlPanel();
    controls.addSection("Styling");

    std::vector<std::string> themes = {
        // Must match Theme enum
        "PrimerLight",
        "PrimerDark",
        "NordLight",
        "NordDark"
    };
    auto theme = controls.addDropdown();
    theme.setLabel("Theme");
    theme.setOptions(themes);
    theme.setSelectedIndex(3);

    std::vector<std::string> style_options = {"Original", "Translucent", "Ghosted"};
    auto displayStyle = controls.addDropdown();
    displayStyle.setLabel("Style");
    displayStyle.setOptions(style_options);
    displayStyle.setSelectedIndex(0);

    // Joint sliders
    controls.addSection("Positions [rad]");
    std::vector<hebi::charts::Slider> jointSliders;
    for (int i = 0; i < robot.getDof(); ++i) {
        jointSliders.push_back(controls.addSlider());
        jointSliders.back().setLabel("Joint " + std::to_string(i + 1));
        jointSliders.back().setLimits(-PI, PI);
        jointSliders.back().setValue(0.0);
    }
    auto btnZero = controls.addStartButton();
    btnZero.setText("Zero");
    auto btnRand = controls.addStopButton();
    btnRand.setText("Random");

    // Main loop
    window.show();
    hebi::charts::LoopTimer loopTimer;
    loopTimer.setFrequency(100);

    // Seed standard random number generator
    std::random_device rd;
    std::default_random_engine rng(rd());
    std::uniform_real_distribution<double> random(-PI, PI);

    while (window.isShowing()) {
        loopTimer.waitForNextTick();

        // Poll for user input
        if (theme.wasChanged()) {
            hebi::charts::runtime::setTheme(static_cast<hebi::charts::Theme>(theme.getSelectedIndex()));
        }

        if (displayStyle.wasChanged()) {
            robot.setDisplayStyle(static_cast<hebi::charts::DisplayStyle>(displayStyle.getSelectedIndex()));
        }

        if (btnZero.wasPressed()) {
            for (auto& j : jointSliders) {
                j.setValue(0.0);
            }
        }

        if (btnRand.wasPressed()) {
            for (auto& j : jointSliders) {
                // Pushing new values back to the UI sliders
                j.setValue(random(rng));
            }
        }

        // Create a numpy array for the positions (zero-copy FFI)
        std::vector<double> positions(robot.getDof());
        for (size_t i = 0; i < jointSliders.size(); ++i) {
            positions[i] = jointSliders[i].getValue();
        }

        // Update the 3D positions
        robot.setPositions(positions);

        // Update the 2D scope traces
        double t = loopTimer.getElapsedTime();
        for (size_t i = 0; i < positions.size(); ++i) {
            jointTraces[i].addPoint(t, positions[i]);
        }
    }

    return 0;
}

// macOS needs to own the main thread, so we need to
// do a handover. no-op on other operating systems.
int main(int argc, char **argv) {
    return hebi::charts::runApplication(application_main, argc, argv);
}
