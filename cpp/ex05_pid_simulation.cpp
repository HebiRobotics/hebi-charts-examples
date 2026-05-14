#include <cmath>
#include <algorithm>
#include "hebi_charts.hpp"

int application_main(int argc, char **argv) {
    // Select a desired Theme
    hebi::charts::runtime::setTheme(hebi::charts::Theme::NordDark);

    // Create a 1x1 grid window with a single scope chart
    hebi::charts::GridWindow window(1, 1);
    window.setTitle("PID Controller Simulation");
    window.setSize(1200, 800);

    auto chart = window.addScope();
    chart.setTitle("System Response");
    chart.setYLabel("Position");

    auto targetLine = chart.addLine("Target Setpoint");
    auto stateLine = chart.addLine("Actual State");

    // Setup interactive tuning controls
    auto controls = window.getControlPanel();
    controls.setTitle("PID Simulation");

    controls.addSection("Target");
    auto setpoint = controls.addSlider();
    setpoint.setLabel("Setpoint");
    setpoint.setLimits(-5.0, 5.0);
    setpoint.setValue(1.0);

    auto autoFlip = controls.addToggle();
    autoFlip.setLabel("Auto Flip");
    autoFlip.setSelected(true);

    //  ==== PID ====
    controls.addSection("PID Gains");
    auto kp = controls.addSlider();
    kp.setLabel("Kp (Proportional)");
    kp.setLimits(0.0, 100.0);
    kp.setValue(5.0);

    auto ki = controls.addSlider();
    ki.setLabel("Ki (Integral)");
    ki.setLimits(0.0, 50.0);
    ki.setValue(0.0);

    auto kd = controls.addSlider();
    kd.setLabel("Kd (Derivative)");
    kd.setLimits(0.0, 10.0);
    kd.setValue(0.0);

    auto iClamp = controls.addToggle();
    iClamp.setLabel("i Clamp");
    iClamp.setSelected(true);

    // ==== System ====
    controls.addSection("System");
    auto mass = controls.addSlider();
    mass.setLabel("Mass");
    mass.setLimits(0.01, 1.0);
    mass.setValue(0.1);

    auto damping = controls.addSlider();
    damping.setLabel("Damping");
    damping.setLimits(0.0, 5.0);
    damping.setValue(1.5);

    auto stiffness = controls.addSlider();
    stiffness.setLabel("Stiffness");
    stiffness.setLimits(0.0, 20.0);
    stiffness.setValue(0.0);

    // State variables
    double state = 0.0;
    double velocity = 0.0;
    double integral = 0.0;
    double lastError = 0.0;

    // Setup timers
    hebi::charts::LoopTimer flipTimer;
    hebi::charts::LoopTimer loopTimer;
    loopTimer.setFrequency(200);

    // Main execution loop
    window.show();
    while (window.isShowing()) {
        loopTimer.waitForNextTick();
        double dt = loopTimer.ticToc();

        // Auto-flip target
        if (autoFlip.isSelected() && flipTimer.getElapsedTime() > 2.0) {
            flipTimer.restart();
            setpoint.setValue(-setpoint.getValue());
        }
        double target = setpoint.getValue();

        // Update PID terms
        double error = target - state;
        integral += error * dt;
        double derivative = (dt > 0) ? (error - lastError) / dt : 0.0;
        lastError = error;

        // Anti-windup
        if (iClamp.isSelected()) {
            integral = std::max(-1.0, std::min(1.0, integral));
        }

        // Compute the PID controller output
        double pidEffort = (kp.getValue() * error) + (ki.getValue() * integral) + (kd.getValue() * derivative);

        // Standard Textbook Mass-Spring-Damper Plant Model
        double springForce = stiffness.getValue() * state;
        double acceleration = (pidEffort - (damping.getValue() * velocity) - springForce) / mass.getValue();
        velocity += acceleration * dt;
        state += velocity * dt;

        // Update chart buffers
        double t = loopTimer.getElapsedTime();
        targetLine.addPoint(t, target);
        stateLine.addPoint(t, state);
    }

    return 0;
}

int main(int argc, char **argv) {
    return hebi::charts::runApplication(application_main, argc, argv);
}