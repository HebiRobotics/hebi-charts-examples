import math
import hebi_charts

def main():
    # Select a desired Theme
    hebi_charts.Runtime.set_theme(hebi_charts.Theme.NORD_DARK)

    # Create a 1x1 grid window with a single scope (time series chart) and trace
    window = hebi_charts.GridWindow(title="Python Time Series", size=(800, 600))
    chart = window.add_scope(title="Scope", ylabel="sin")
    sinewave = chart.add_line("sine")

    # Use the built-in timer utility for more accurate loop rates
    loopTimer = hebi_charts.LoopTimer(frequency=100)

    # Populate sine wave
    window.show()
    while window.is_showing():
        loopTimer.wait_for_next_tick()

        # compute sine wave
        f = 1
        t = loopTimer.elapsed_time
        y =  math.sin(2 * math.pi * f * t)

        # append one point (non-blocking)
        sinewave.add_point(t, y)


# macOS needs to own the main thread, so we need to
# do a handover. no-op on other operating systems.
if __name__ == "__main__":
    hebi_charts.run_application(main)
