import math
import hebi_charts

def main():
    # Select a desired Theme
    hebi_charts.Runtime.set_theme(hebi_charts.Theme.NORD_DARK)

    # Create a 1x1 grid window with a single scope (time series chart) and trace
    window = hebi_charts.GridWindow(title="Python Time Series", size=(1200, 600))
    chart = window.add_scope(title="Scope", ylabel="sin")
    sinewave = chart.add_line("sine", line_style=hebi_charts.LineStyle.SOLID)

    # Add some basic controls to change the parameters
    controls = window.get_control_panel()
    controls.width = 300
    amplitude = controls.add_slider(label="Amplitude", limits=(0, 20), value=1)
    frequency = controls.add_slider(label="Frequency", limits=(0.1, 10), value=1)

    # Initialization
    phase = 0
    window.show()

    # Use the built-in timer utility for more accurate loop rates
    loopTimer = hebi_charts.LoopTimer(frequency=100) # initializes tic()

    # Generate sine wave
    while window.is_showing():
        loopTimer.wait_for_next_tick()

        # accumulate phase to avoid discontinuities
        dt = loopTimer.tic_toc()
        phase += 2 * math.pi * frequency.value * dt
        y =  amplitude.value * math.sin(phase)

        # append one point (non-blocking)
        sinewave.add_point(loopTimer.elapsed_time, y)


# macOS needs to own the main thread, so we need to
# do a handover. no-op on other operating systems.
if __name__ == "__main__":
    hebi_charts.run_application(main)
