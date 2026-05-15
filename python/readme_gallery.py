import time
import numpy as np
from hebi_charts import *

size = (800, 600)

def line_chart():
    window = GridWindow(size=size)
    chart = window.add_line_chart(title="Line Chart", xunit="rad")
    x = np.linspace(0, 2*np.pi, 100)
    chart.add_line("tanh", line_style=LineStyle.SOLID).set_data(x, np.tanh(x))
    chart.add_line("sin", line_style=LineStyle.DASHED).set_data(x, np.sin(x))
    chart.add_line("cos",
                   line_style=LineStyle.POINTS,
                   marker_type=MarkerType.CIRCLE2,
                   marker_size=3
                   ).set_data(x, np.cos(x))
    window.show()
    window.wait_until_closed()


def latency_chart():
    window = GridWindow(size=size)
    chart = window.add_latency_chart(title="Latency Chart")
    window.show()
    loopTrace = chart.add_trace("Loop")
    loopTimer = hebi_charts.LoopTimer()
    for i in range(100_000):
        loopTrace.record(loopTimer.tic_toc())
    window.wait_until_closed()


def frame_3d():
    window = GridWindow(size=size, title="3D Frame")
    scene = window.add_scene3d()
    scene.add_frame(length_in_meters=0.2)
    window.show()
    window.wait_until_closed()

def mesh_3d():
    window = GridWindow(size=size, title="3D Mesh")
    scene = window.add_scene3d()
    mesh = scene.add_mesh("https://files.hebi.us/meshes/tready/A-2362-01.obj")
    window.show()
    while window.is_showing():
        mesh.set_transform4x4([
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        ])

def line_3d():
    window = GridWindow(size=size, title="3D Lissajous Figure")
    scene = window.add_scene3d()
    line3d = scene.add_line(color=Color.RED)
    loopTimer = hebi_charts.LoopTimer()
    window.show()
    while window.is_showing():
        dt = loopTimer.elapsed_time
        t = np.linspace(0, 2 * np.pi, 500)
        x = np.sin(3 * t + np.pi / 2 + dt)
        y = np.sin(2 * t + dt)
        z = np.sin(5 * t + dt)
        line3d.set_data(x, y, z)

def points_3d():
    window = GridWindow(size=size, title="3D Points")
    scene = window.add_scene3d()
    points = scene.add_points(color=Color.RED, marker_size=0.2, marker_shape=MarkerShape.CUBE)
    loopTimer = hebi_charts.LoopTimer()
    window.show()
    while window.is_showing():
        dt = loopTimer.elapsed_time
        t = np.linspace(0, 2 * np.pi, 5)
        x = np.sin(3 * t + np.pi / 2 + dt)
        y = np.sin(2 * t + dt)
        z = np.sin(5 * t + dt)
        points.set_data(x, y, z)

def robot_3d():
    hrdf = "https://raw.githubusercontent.com/HebiRobotics/hebi-hrdf/main/A-2240-06G.hrdf"
    positions = [0, 0.5, 0.5, 0, 0, 0]

    window = GridWindow(size=size)
    scene = window.add_scene3d(grid_min_z=0)

    robot = scene.add_robot(hrdf)
    robot.set_positions(positions)

    ghosted = scene.add_robot(hrdf, display_style=DisplayStyle.GHOSTED)
    ghosted.set_positions(positions)
    ghosted.set_translation(0, 0.3, 0)

    translucent = scene.add_robot(hrdf, display_style=DisplayStyle.TRANSLUCENT)
    translucent.set_positions(positions)
    translucent.set_translation(0, -0.3, 0)

    window.show()
    window.wait_until_closed()


def panda_control():
    # 3D robot (Franka Panda)
    window = GridWindow(size=(1200, 800))
    scene = window.add_scene3d(grid_bounds=(-1.5, 1.5, -1.5, 1.5, 0, 1.5))
    robot = scene.add_robot("https://files.hebi.us/external/robots/franka_panda/panda.hrdf")

    controls = window.get_control_panel()

    controls.add_section("Styling")
    theme = controls.add_dropdown(options=Theme.__members__, label="Theme", selected_index=3)
    displayStyle = controls.add_dropdown(label="Style", options=DisplayStyle.__members__, selected_index=0)

    controls.add_section("Joints")
    home_button = controls.add_start_button(text="Home")
    home_positions = [0, 0.7, -0.5, -0.8, 0.3, 1.6, 1.3]
    joint_names = [f"J{i}" for i in range(7)]
    joints = [controls.add_slider(label=n, limits=(-np.pi, np.pi)) for n in joint_names]
    gripper = controls.add_slider(label="Gripper", limits=(0, 0.05), value=0.02)

    window.show()
    while window.is_showing():

        if theme.was_changed():
            Runtime.set_theme(Theme(theme.selected_index))

        if displayStyle.was_changed():
            robot.display_style = DisplayStyle(displayStyle.selected_index)

        if home_button.was_pressed():
            print(home_positions)
            for slider, val in zip(joints, home_positions):
                slider.value = val
            gripper.value = 0.02

        robot.set_positions(np.concatenate([
            [s.value for s in joints],
            [gripper.value, gripper.value]
        ]))


def control_panel():
    window = GridWindow(size=(1200, 800))
    controls = window.get_control_panel(title="Controls")

    controls.add_section("Buttons")
    controls.add_start_button()
    controls.add_button(label="Button", text="Action!")
    stop = controls.add_stop_button()

    controls.add_section("Inputs")
    controls.add_dropdown(label="Dropdown", options=["Option1", "Option2"], selected_index=0)
    controls.add_slider(label="Slider", limits=(0, 1), value=0.5)
    controls.add_toggle(label="Toggle", selected=True)

    controls.add_section("Labels")
    controls.add_label(label="Text", text="Text")
    controls.add_label(label="Value", value=1234)

    window.show()
    while window.is_showing():
        if stop.was_pressed():
            window.hide()


def latency_chart_jitter():
    window = GridWindow(size=(1200, 800))
    chart = window.add_latency_chart(title="Python Plot Latency")
    line = chart.add_trace("tic_toc")

    window.show()
    while window.is_showing():
        line.tic()
        for _ in range(10_000_000):
            line.tic_toc()


def record_video():
    window = GridWindow(size=size, title="3D Lissajous Figure")
    scene = window.add_scene3d()
    line3d = scene.add_line(color=Color.RED)
    loopTimer = hebi_charts.LoopTimer()
    window.show()

    stream = window.create_image_stream()
    stream.start_recording("lissajous", overwrite=True)

    while window.is_showing():
        dt = loopTimer.elapsed_time
        t = np.linspace(0, 2 * np.pi, 500)
        x = np.sin(3 * t + np.pi / 2 + dt)
        y = np.sin(2 * t + dt)
        z = np.sin(5 * t + dt)
        line3d.set_data(x, y, z)

    record = stream.stop_recording()
    print(record.get_ffmpeg_command(output_format=VideoOutputFormat.H264))


def HdrHistogram_hgrm():
    print("Starting timing measurements (.hgrm)...")
    hist = hebi_charts.HdrHistogramTrace.create_local()
    for _ in range(10000):
        time.sleep(0.001) # measure Python sleep
        hist.tic_toc()  # built-in measurements

    output_units_us = 1e6
    print(hist.to_hgrm_string(output_units_us))

def HdrHistogram_hlog():
    print("Starting timing measurements (.hlog)...")
    recorder = hebi_charts.HdrHistogramRecorder(frequency=1)
    hist = recorder.add_trace("loop")
    recorder.start_recording("loop.hlog")

    timer = hebi_charts.LoopTimer(frequency=1000)
    while timer.elapsed_time < 5:
        timer.wait_for_next_tick()
        hist.tic_toc()

    recorder.stop_recording()


def HdrHistogram_overhead():
    # HdrHistogram - measure overhead of itself
    print("Measuring 100M calls (.hgrm)...")
    hist = hebi_charts.HdrHistogramTrace.create_local()
    for _ in range(100_000_000):
        hist.tic_toc()
    hist.save_as_hgrm("overhead_python.hgrm", 1e9)

    # HdrHistogram - Recorder
    print("Measuring 100M calls (.hlog)...")
    recorder = HdrHistogramRecorder(frequency=1)
    hist = recorder.add_trace("python")
    recorder.start_recording("loop_python.hlog")
    for _ in range(100_000_000):
        hist.tic_toc()
    recorder.stop_recording()

def main():
    Runtime.set_theme(Theme.NORD_DARK)
    line_chart()
    latency_chart()
    frame_3d()
    mesh_3d()
    line_3d()
    points_3d()
    robot_3d()
    panda_control()
    control_panel()
    latency_chart_jitter()
    record_video()
    HdrHistogram_hgrm()
    HdrHistogram_hlog()
    HdrHistogram_overhead()

# macOS needs to own the main thread, so we need to
# do a handover. no-op on other operating systems.
if __name__ == "__main__":
    hebi_charts.run_application(main)
