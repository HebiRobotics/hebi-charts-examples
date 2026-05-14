import hebi_charts

def main():
    # Select a desired Theme
    hebi_charts.Runtime.set_theme(hebi_charts.Theme.NORD_DARK)

    # Create a 1x1 grid window with a single scope chart
    window = hebi_charts.GridWindow(title="PID Controller Simulation", size=(1200, 800))
    chart = window.add_scope(title="System Response", ylabel="Position")
    target_line = chart.add_line("Target Setpoint")
    state_line = chart.add_line("Actual State")

    # Setup interactive tuning controls
    controls = window.get_control_panel()
    controls.title = "PID Simulation"

    controls.add_section("Target")
    setpoint = controls.add_slider(label="Setpoint", limits=(-5, 5), value=1)
    auto_flip = controls.add_toggle(label="Auto Flip", selected=True)

    controls.add_section("PID Gains")
    kp = controls.add_slider(label="Kp (Proportional)", limits=(0, 100), value=5)
    ki = controls.add_slider(label="Ki (Integral)", limits=(0, 50), value=0)
    kd = controls.add_slider(label="Kd (Derivative)", limits=(0, 10), value=0)
    i_clamp = controls.add_toggle(label="i Clamp", selected=True)

    controls.add_section("System")
    mass = controls.add_slider(label="Mass", limits=(-1, 1), value=0.1)
    damping = controls.add_slider(label="Damping", limits=(0, 5), value=1.5)
    stiffness = controls.add_slider(label="Stiffness", limits=(0, 20), value=0)

    # State variables
    state = 0
    velocity = 0
    integral = 0
    last_error = 0

    # Setup timers
    flip_timer = hebi_charts.LoopTimer()
    loop_timer = hebi_charts.LoopTimer(frequency=200)

    # Main execution loop
    window.show()
    while window.is_showing():
        loop_timer.wait_for_next_tick()
        dt = loop_timer.tic_toc()

        # Auto-flip target
        if auto_flip.selected and flip_timer.elapsed_time > 2:
            flip_timer.restart()
            setpoint.value = -setpoint.value

        # Update PID terms
        target = setpoint.value
        error = target - state
        integral += error * dt
        derivative = (error - last_error) / dt if dt > 0 else 0
        last_error = error

        # Anti-windup
        if i_clamp.selected:
            integral = max(-1, min(1, integral))

        # Compute the PID controller output
        pid_effort = (kp.value * error) + (ki.value * integral) + (kd.value * derivative)

        # Standard Textbook Mass-Spring-Damper Plant Model
        spring_force = stiffness.value * state
        acceleration = (pid_effort - (damping.value * velocity) - spring_force) / mass.value
        velocity += acceleration * dt
        state += velocity * dt

        # Update chart lines
        t = loop_timer.elapsed_time
        target_line.add_point(t, target)
        state_line.add_point(t, state)

if __name__ == "__main__":
    hebi_charts.run_application(main)