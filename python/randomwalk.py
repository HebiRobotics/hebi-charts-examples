import time
import random
import hebi_charts

def main():
    # Select a desired Theme
    hebi_charts.Runtime.set_theme(hebi_charts.Theme.NORD_DARK)

    # Create a 1x1 grid window with a single chart element
    window = hebi_charts.GridWindow(title="Python Random Walk", size=(800, 600))
    chart = window.add_line_chart(title="Random Walk", xlabel="sample", ylabel="rnd(x)")

    # Add a trace
    line = chart.add_line("walk", line_style=hebi_charts.LineStyle.SOLID, max_point_count=10_000)

    # Continuously update a random walk
    x, y = 0, 0
    window.show()
    while window.is_showing():
        # reduce the rate (optional)
        time.sleep(0.001)

        # append one point (non-blocking)
        x += 1
        y += random.uniform(-1, 1)
        line.add_point(x, y)

# macOS needs to own the main thread, so we need to
# do a handover. no-op on other operating systems.
if __name__ == "__main__":
    hebi_charts.run_application(main)
