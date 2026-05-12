import time
import hebi_charts

def main():
    # Select a desired Theme
    hebi_charts.Runtime.set_theme(hebi_charts.Theme.NORD_DARK)

    # 2x1 Grid: Top = Scope (Time series), Bottom = LatencyChart (Aggregate distribution)
    window = hebi_charts.GridWindow(title="Python Latency Profiler", size=(1000, 800), rows=2)
    scope = window.add_scope(row=0, title="Measured Sleep Duration vs Target", ylabel="Latency", yunit="s", yauto=True)
    histogram = window.add_latency_chart(row=1, title="Aggregate HdrHistogram")

    # Add Traces
    sleep_scope = scope.add_line("Python", line_style=hebi_charts.LineStyle.POINTS, show_in_legend=False)
    sleep_histogram = histogram.add_trace("Python")
    timer_scope = scope.add_line("LoopTimer", line_style=hebi_charts.LineStyle.POINTS, show_in_legend=False)
    timer_histogram = histogram.add_trace("LoopTimer")

    # Interactive Target Cursor (defaults to 1ms)
    target = histogram.add_y_cursor(label="Target", editable=True, value=1e-3)

    # Main loop
    loopTimer = hebi_charts.LoopTimer() # <-- manages Windows interrupt freq (see timeBeginPeriod)
    window.show()
    while window.is_showing():
        # Guarantee floor of 1ns to avoid negative intervals
        seconds = max(target.value, 1e-9)
        nanos = int(seconds * 1e9)

        # Measure Python's native sleep
        loopTimer.tic()
        time.sleep(seconds)
        sleep_dt = loopTimer.tic_toc()

        # Measure LoopTimer's tiered spin
        hebi_charts.LoopTimer.spin_nanos(nanos)
        timer_dt = loopTimer.toc()

        # Update traces
        t = loopTimer.elapsed_time
        sleep_scope.add_point(t, sleep_dt)
        sleep_histogram.record(sleep_dt)
        timer_scope.add_point(t, timer_dt)
        timer_histogram.record(timer_dt)

        # Sync visibility so a legend click hides both traces
        sleep_scope.visible = sleep_histogram.visible
        timer_scope.visible = timer_histogram.visible

# macOS needs to own the main thread, so we need to
# do a handover. no-op on other operating systems.
if __name__ == "__main__":
    hebi_charts.run_application(main)
