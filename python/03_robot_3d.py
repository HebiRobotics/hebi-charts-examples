import math
import random
import numpy as np
import hebi_charts

def main():
    # Select a desired Theme
    hebi_charts.Runtime.set_theme(hebi_charts.Theme.NORD_DARK)

    # 3x1 Grid: Top 2/3 = 3D robot view, Bottom 1/3 = Scope
    window = hebi_charts.GridWindow(title="Python Robot", size=(1000, 600), rows=3)

    # 3D Robot
    scene = window.add_scene3d(row_span=2, grid_min_z=0)
    robot = scene.add_robot("https://raw.githubusercontent.com/HebiRobotics/hebi-hrdf/main/A-2240-06G.hrdf")

    # 2D Scope
    chart = window.add_scope(row=2, title="Position", yunit="rad")
    joint_traces = [
        chart.add_line(f"Joint {i + 1}", line_style=hebi_charts.LineStyle.SOLID)
        for i in range(robot.dof)
    ]

    # Basic controls
    controls = window.get_control_panel()
    controls.add_section("Styling")
    theme = controls.add_dropdown(options=hebi_charts.Theme.__members__, label="Theme", selected_index=3)
    displayStyle = controls.add_dropdown(label="Style", options=hebi_charts.DisplayStyle.__members__, selected_index=0)

    # Joint sliders
    controls.add_section("Positions [rad]")
    joint_sliders = [
        controls.add_slider(label=f"Joint {i + 1}", limits=(-math.pi, math.pi), value=0)
        for i in range(robot.dof)
    ]
    btn_zero = controls.add_start_button(text="Zero")
    btn_rand = controls.add_stop_button(text="Random")

    # Main loop
    window.show()
    loopTimer = hebi_charts.LoopTimer(frequency=100)
    while window.is_showing():
        loopTimer.wait_for_next_tick()

        # Poll for user input
        if theme.was_changed():
            hebi_charts.Runtime.set_theme(hebi_charts.Theme(theme.selected_index))

        if displayStyle.was_changed():
            robot.display_style = hebi_charts.DisplayStyle(displayStyle.selected_index)

        if btn_zero.was_pressed():
            for j in joint_sliders:
                j.value = 0.0

        if btn_rand.was_pressed():
            for j in joint_sliders:
                # Pushing new values back to the UI sliders
                j.value = random.uniform(-math.pi, math.pi)

        # Create a numpy array for the positions (zero-copy FFI)
        positions = np.array([s.value for s in joint_sliders], dtype=np.float64)

        # Update the 3D positions
        robot.set_positions(positions)

        # Update the 2D scope traces
        t = loopTimer.elapsed_time
        for i, position in enumerate(positions):
            joint_traces[i].add_point(t, position)


# macOS needs to own the main thread, so we need to
# do a handover. no-op on other operating systems.
if __name__ == "__main__":
    hebi_charts.run_application(main)
