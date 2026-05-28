from __future__ import annotations
from ctypes import *
from enum import IntEnum
import os
import platform
from typing import Sequence, Tuple, Any

# Load the shared library relative to this file
def load_library():
    lib_name = 'hebi_charts'
    version = '0.9.3'
    build_number = '123'

    architecture = platform.architecture()[0]  # gets '64bit' or '32bit'
    os_name = platform.system()  # gets 'Windows', 'Linux', 'Darwin'

    # Determine the current platform
    if os_name == 'Windows' and architecture == '64bit':
        lib_platform = 'win_amd64'
        lib_file = lib_name + '.dll'
        cache_home = os.getenv('LOCALAPPDATA')
    elif os_name == 'Linux' and architecture == '64bit':
        lib_platform = 'linux_amd64'
        lib_file = 'lib' + lib_name + '.so'
        cache_home = os.getenv('XDG_CACHE_HOME')
        if not cache_home:
            cache_home = os.path.join(os.getenv('HOME'), '.cache')
    elif os_name == 'Darwin' and architecture == '64bit':  # Darwin is the underlying system for macOS.
        if platform.machine() in ['arm64', 'aarch64']:  # Apple Silicon
            lib_platform = 'osx_arm64'
        else:  # Intel Mac
            lib_platform = 'osx_amd64'
        lib_file = 'lib' + lib_name + '.dylib'
        cache_home = os.path.join(os.getenv('HOME'), 'Library', 'Caches')
    else:
        raise Exception('Unsupported system: ' + os_name + ' ' + platform.machine())

    # Prioritize locally bundled lib binaries
    this_file_dir = os.path.dirname(os.path.realpath(__file__))
    lib_path = os.path.join(this_file_dir, 'lib', lib_platform, lib_file)

    # Fall back to shared downloaded version in local cache
    if not os.path.exists(lib_path):
        download_dir = os.path.join(cache_home, lib_name, f'{version}-build{build_number}')
        lib_path = os.path.join(download_dir, lib_name, 'lib', lib_platform, lib_file)

        # If necessary, start a new download it into a shared dir. All files are
        # kept so we can share a single download for all languages.
        if not os.path.exists(lib_path):
            import urllib.request
            import zipfile

            # Build download url
            dl_name = f"{lib_name}-{version}-{lib_platform}.zip"
            url = f"https://files.hebi.us/download/{lib_name}/snapshot/{build_number}/{dl_name}"

            # Download zip file
            print(f"Downloading {url}")
            os.makedirs(download_dir, exist_ok=True)
            zip_path = os.path.join(download_dir, dl_name)

            # Some platforms (e.g. macOS via brew) do not include SSL certificates and may
            # fail, so we provide a curl fallback that should be available on most systems.
            try:
                urllib.request.urlretrieve(
                    url,
                    zip_path,
                    lambda count, block_size, total_size: print(
                        f"\rDownloading... {int(count * block_size * 100 / total_size)}% ", end="")
                )
                print('')
            except Exception as e:
                print(f"\nurllib download failed ({e}), retrying with curl...")
                import subprocess
                subprocess.run(["curl", "-L", "-f", "-o", zip_path, url], check=True)

            # Extract the entire archive
            print(f'Extracting to {download_dir}')
            with zipfile.ZipFile(zip_path, 'r') as zip_ref:
                zip_ref.extractall(download_dir)

            print(f'Removing temporary files')
            os.remove(zip_path)

    # Sanity check due to loadlibrary errors being harder to parse
    if not os.path.exists(lib_path):
        raise Exception(f'Required binary was not found: {lib_path}')

    # Load the library
    return cdll.LoadLibrary(lib_path)

libhebi_charts = load_library()

# Optional lazy-loaded numpy dependency for bulk methods
_numpy = None
def _require_numpy():
    global _numpy
    if _numpy is None:
        try:
            import numpy as np
            _numpy = np
        except ImportError:
            raise ImportError("This method requires 'numpy'. Please run: pip install numpy")
    return _numpy

# API definitions
class struct_Camera_(Structure):
    pass
CameraPtr = POINTER(struct_Camera_)

class struct_Control_(Structure):
    pass
ControlPtr = POINTER(struct_Control_)

class struct_Button_(Structure):
    pass
ButtonPtr = POINTER(struct_Button_)

class struct_Dropdown_(Structure):
    pass
DropdownPtr = POINTER(struct_Dropdown_)

class struct_Label_(Structure):
    pass
LabelPtr = POINTER(struct_Label_)

class struct_Slider_(Structure):
    pass
SliderPtr = POINTER(struct_Slider_)

class struct_Toggle_(Structure):
    pass
TogglePtr = POINTER(struct_Toggle_)

class struct_ControlPanel_(Structure):
    pass
ControlPanelPtr = POINTER(struct_ControlPanel_)

class struct_Cursor_(Structure):
    pass
CursorPtr = POINTER(struct_Cursor_)

class struct_FxmlView_(Structure):
    pass
FxmlViewPtr = POINTER(struct_FxmlView_)

class struct_GridWindow_(Structure):
    pass
GridWindowPtr = POINTER(struct_GridWindow_)

class struct_HdrHistogramRecorder_(Structure):
    pass
HdrHistogramRecorderPtr = POINTER(struct_HdrHistogramRecorder_)

class struct_HdrHistogramTrace_(Structure):
    pass
HdrHistogramTracePtr = POINTER(struct_HdrHistogramTrace_)

class struct_ImageStream_(Structure):
    pass
ImageStreamPtr = POINTER(struct_ImageStream_)

class struct_LoopTimer_(Structure):
    pass
LoopTimerPtr = POINTER(struct_LoopTimer_)

class struct_Object3d_(Structure):
    pass
Object3dPtr = POINTER(struct_Object3d_)

class struct_Frame_(Structure):
    pass
FramePtr = POINTER(struct_Frame_)

class struct_Mesh_(Structure):
    pass
MeshPtr = POINTER(struct_Mesh_)

class struct_Robot_(Structure):
    pass
RobotPtr = POINTER(struct_Robot_)

class struct_Series3d_(Structure):
    pass
Series3dPtr = POINTER(struct_Series3d_)

class struct_Line3d_(Structure):
    pass
Line3dPtr = POINTER(struct_Line3d_)

class struct_Points3d_(Structure):
    pass
Points3dPtr = POINTER(struct_Points3d_)

class struct_RecordingResult_(Structure):
    pass
RecordingResultPtr = POINTER(struct_RecordingResult_)

class struct_Scene3d_(Structure):
    pass
Scene3dPtr = POINTER(struct_Scene3d_)

class struct_StreamView_(Structure):
    pass
StreamViewPtr = POINTER(struct_StreamView_)

class struct_XYChart_(Structure):
    pass
XYChartPtr = POINTER(struct_XYChart_)

class struct_LatencyChart_(Structure):
    pass
LatencyChartPtr = POINTER(struct_LatencyChart_)

class struct_LineChart_(Structure):
    pass
LineChartPtr = POINTER(struct_LineChart_)

class struct_XYSeries_(Structure):
    pass
XYSeriesPtr = POINTER(struct_XYSeries_)

class struct_LatencyTrace_(Structure):
    pass
LatencyTracePtr = POINTER(struct_LatencyTrace_)

class struct_Line_(Structure):
    pass
LinePtr = POINTER(struct_Line_)

class struct_Runtime_(Structure):
    pass
RuntimePtr = POINTER(struct_Runtime_)

UserCallbackFunction = CFUNCTYPE(None, c_void_p)

# ==== Camera ====
libhebi_charts.hebi_charts_Camera_setView.argtypes = [CameraPtr, c_int32]
libhebi_charts.hebi_charts_Camera_setView.restype = None
libhebi_charts.hebi_charts_Camera_reset.argtypes = [CameraPtr]
libhebi_charts.hebi_charts_Camera_reset.restype = None
libhebi_charts.hebi_charts_Camera_applyRotation.argtypes = [CameraPtr, c_double, c_double, c_double, c_double]
libhebi_charts.hebi_charts_Camera_applyRotation.restype = c_int
libhebi_charts.hebi_charts_Camera_setDistance.argtypes = [CameraPtr, c_double]
libhebi_charts.hebi_charts_Camera_setDistance.restype = c_int
libhebi_charts.hebi_charts_Camera_setPan.argtypes = [CameraPtr, c_double, c_double, c_double]
libhebi_charts.hebi_charts_Camera_setPan.restype = c_int
libhebi_charts.hebi_charts_Camera_setControlsVisible.argtypes = [CameraPtr, c_int]
libhebi_charts.hebi_charts_Camera_setControlsVisible.restype = None
libhebi_charts.hebi_charts_Camera_release.argtypes = [CameraPtr]
libhebi_charts.hebi_charts_Camera_release.restype = None

# ==== Control ====
libhebi_charts.hebi_charts_Control_isEnabled.argtypes = [ControlPtr]
libhebi_charts.hebi_charts_Control_isEnabled.restype = c_int
libhebi_charts.hebi_charts_Control_setEnabled.argtypes = [ControlPtr, c_int]
libhebi_charts.hebi_charts_Control_setEnabled.restype = None
libhebi_charts.hebi_charts_Control_getLabel.argtypes = [ControlPtr]
libhebi_charts.hebi_charts_Control_getLabel.restype = c_char_p
libhebi_charts.hebi_charts_Control_setLabel.argtypes = [ControlPtr, c_char_p]
libhebi_charts.hebi_charts_Control_setLabel.restype = None
libhebi_charts.hebi_charts_Control_getTooltip.argtypes = [ControlPtr]
libhebi_charts.hebi_charts_Control_getTooltip.restype = c_char_p
libhebi_charts.hebi_charts_Control_setTooltip.argtypes = [ControlPtr, c_char_p]
libhebi_charts.hebi_charts_Control_setTooltip.restype = None
libhebi_charts.hebi_charts_Control_isVisible.argtypes = [ControlPtr]
libhebi_charts.hebi_charts_Control_isVisible.restype = c_int
libhebi_charts.hebi_charts_Control_setVisible.argtypes = [ControlPtr, c_int]
libhebi_charts.hebi_charts_Control_setVisible.restype = None
libhebi_charts.hebi_charts_Control_release.argtypes = [ControlPtr]
libhebi_charts.hebi_charts_Control_release.restype = None

# ==== Button ====
libhebi_charts.hebi_charts_Button_getText.argtypes = [ButtonPtr]
libhebi_charts.hebi_charts_Button_getText.restype = c_char_p
libhebi_charts.hebi_charts_Button_setText.argtypes = [ButtonPtr, c_char_p]
libhebi_charts.hebi_charts_Button_setText.restype = None
libhebi_charts.hebi_charts_Button_isPressed.argtypes = [ButtonPtr]
libhebi_charts.hebi_charts_Button_isPressed.restype = c_int
libhebi_charts.hebi_charts_Button_wasPressed.argtypes = [ButtonPtr]
libhebi_charts.hebi_charts_Button_wasPressed.restype = c_int
libhebi_charts.hebi_charts_Button_to_Control.argtypes = [ButtonPtr]
libhebi_charts.hebi_charts_Button_to_Control.restype = ControlPtr

# ==== Dropdown ====
libhebi_charts.hebi_charts_Dropdown_setOptions.argtypes = [DropdownPtr, POINTER(c_char_p), c_size_t]
libhebi_charts.hebi_charts_Dropdown_setOptions.restype = None
libhebi_charts.hebi_charts_Dropdown_getSelectedIndex.argtypes = [DropdownPtr]
libhebi_charts.hebi_charts_Dropdown_getSelectedIndex.restype = c_int
libhebi_charts.hebi_charts_Dropdown_setSelectedIndex.argtypes = [DropdownPtr, c_int]
libhebi_charts.hebi_charts_Dropdown_setSelectedIndex.restype = None
libhebi_charts.hebi_charts_Dropdown_addOption.argtypes = [DropdownPtr, c_char_p]
libhebi_charts.hebi_charts_Dropdown_addOption.restype = None
libhebi_charts.hebi_charts_Dropdown_wasChanged.argtypes = [DropdownPtr]
libhebi_charts.hebi_charts_Dropdown_wasChanged.restype = c_int
libhebi_charts.hebi_charts_Dropdown_to_Control.argtypes = [DropdownPtr]
libhebi_charts.hebi_charts_Dropdown_to_Control.restype = ControlPtr

# ==== Label ====
libhebi_charts.hebi_charts_Label_getText.argtypes = [LabelPtr]
libhebi_charts.hebi_charts_Label_getText.restype = c_char_p
libhebi_charts.hebi_charts_Label_setText.argtypes = [LabelPtr, c_char_p]
libhebi_charts.hebi_charts_Label_setText.restype = None
libhebi_charts.hebi_charts_Label_getValue.argtypes = [LabelPtr]
libhebi_charts.hebi_charts_Label_getValue.restype = c_double
libhebi_charts.hebi_charts_Label_setValue.argtypes = [LabelPtr, c_double]
libhebi_charts.hebi_charts_Label_setValue.restype = None
libhebi_charts.hebi_charts_Label_to_Control.argtypes = [LabelPtr]
libhebi_charts.hebi_charts_Label_to_Control.restype = ControlPtr

# ==== Slider ====
libhebi_charts.hebi_charts_Slider_setLimits.argtypes = [SliderPtr, c_double, c_double]
libhebi_charts.hebi_charts_Slider_setLimits.restype = None
libhebi_charts.hebi_charts_Slider_getMax.argtypes = [SliderPtr]
libhebi_charts.hebi_charts_Slider_getMax.restype = c_double
libhebi_charts.hebi_charts_Slider_setMax.argtypes = [SliderPtr, c_double]
libhebi_charts.hebi_charts_Slider_setMax.restype = None
libhebi_charts.hebi_charts_Slider_getMin.argtypes = [SliderPtr]
libhebi_charts.hebi_charts_Slider_getMin.restype = c_double
libhebi_charts.hebi_charts_Slider_setMin.argtypes = [SliderPtr, c_double]
libhebi_charts.hebi_charts_Slider_setMin.restype = None
libhebi_charts.hebi_charts_Slider_getValue.argtypes = [SliderPtr]
libhebi_charts.hebi_charts_Slider_getValue.restype = c_double
libhebi_charts.hebi_charts_Slider_setValue.argtypes = [SliderPtr, c_double]
libhebi_charts.hebi_charts_Slider_setValue.restype = None
libhebi_charts.hebi_charts_Slider_wasChanged.argtypes = [SliderPtr]
libhebi_charts.hebi_charts_Slider_wasChanged.restype = c_int
libhebi_charts.hebi_charts_Slider_to_Control.argtypes = [SliderPtr]
libhebi_charts.hebi_charts_Slider_to_Control.restype = ControlPtr

# ==== Toggle ====
libhebi_charts.hebi_charts_Toggle_isSelected.argtypes = [TogglePtr]
libhebi_charts.hebi_charts_Toggle_isSelected.restype = c_int
libhebi_charts.hebi_charts_Toggle_setSelected.argtypes = [TogglePtr, c_int]
libhebi_charts.hebi_charts_Toggle_setSelected.restype = None
libhebi_charts.hebi_charts_Toggle_wasChanged.argtypes = [TogglePtr]
libhebi_charts.hebi_charts_Toggle_wasChanged.restype = c_int
libhebi_charts.hebi_charts_Toggle_to_Control.argtypes = [TogglePtr]
libhebi_charts.hebi_charts_Toggle_to_Control.restype = ControlPtr

# ==== ControlPanel ====
libhebi_charts.hebi_charts_ControlPanel_getTitle.argtypes = [ControlPanelPtr]
libhebi_charts.hebi_charts_ControlPanel_getTitle.restype = c_char_p
libhebi_charts.hebi_charts_ControlPanel_setTitle.argtypes = [ControlPanelPtr, c_char_p]
libhebi_charts.hebi_charts_ControlPanel_setTitle.restype = None
libhebi_charts.hebi_charts_ControlPanel_getWidth.argtypes = [ControlPanelPtr]
libhebi_charts.hebi_charts_ControlPanel_getWidth.restype = c_double
libhebi_charts.hebi_charts_ControlPanel_setWidth.argtypes = [ControlPanelPtr, c_double]
libhebi_charts.hebi_charts_ControlPanel_setWidth.restype = None
libhebi_charts.hebi_charts_ControlPanel_addSection.argtypes = [ControlPanelPtr, c_char_p]
libhebi_charts.hebi_charts_ControlPanel_addSection.restype = c_int
libhebi_charts.hebi_charts_ControlPanel_addLabel.argtypes = [ControlPanelPtr]
libhebi_charts.hebi_charts_ControlPanel_addLabel.restype = LabelPtr
libhebi_charts.hebi_charts_ControlPanel_addButton.argtypes = [ControlPanelPtr]
libhebi_charts.hebi_charts_ControlPanel_addButton.restype = ButtonPtr
libhebi_charts.hebi_charts_ControlPanel_addStartButton.argtypes = [ControlPanelPtr]
libhebi_charts.hebi_charts_ControlPanel_addStartButton.restype = ButtonPtr
libhebi_charts.hebi_charts_ControlPanel_addStopButton.argtypes = [ControlPanelPtr]
libhebi_charts.hebi_charts_ControlPanel_addStopButton.restype = ButtonPtr
libhebi_charts.hebi_charts_ControlPanel_addSlider.argtypes = [ControlPanelPtr]
libhebi_charts.hebi_charts_ControlPanel_addSlider.restype = SliderPtr
libhebi_charts.hebi_charts_ControlPanel_addToggle.argtypes = [ControlPanelPtr]
libhebi_charts.hebi_charts_ControlPanel_addToggle.restype = TogglePtr
libhebi_charts.hebi_charts_ControlPanel_addDropdown.argtypes = [ControlPanelPtr]
libhebi_charts.hebi_charts_ControlPanel_addDropdown.restype = DropdownPtr
libhebi_charts.hebi_charts_ControlPanel_release.argtypes = [ControlPanelPtr]
libhebi_charts.hebi_charts_ControlPanel_release.restype = None

# ==== Cursor ====
libhebi_charts.hebi_charts_Cursor_isEditable.argtypes = [CursorPtr]
libhebi_charts.hebi_charts_Cursor_isEditable.restype = c_int
libhebi_charts.hebi_charts_Cursor_setEditable.argtypes = [CursorPtr, c_int]
libhebi_charts.hebi_charts_Cursor_setEditable.restype = None
libhebi_charts.hebi_charts_Cursor_getLabel.argtypes = [CursorPtr]
libhebi_charts.hebi_charts_Cursor_getLabel.restype = c_char_p
libhebi_charts.hebi_charts_Cursor_setLabel.argtypes = [CursorPtr, c_char_p]
libhebi_charts.hebi_charts_Cursor_setLabel.restype = None
libhebi_charts.hebi_charts_Cursor_getValue.argtypes = [CursorPtr]
libhebi_charts.hebi_charts_Cursor_getValue.restype = c_double
libhebi_charts.hebi_charts_Cursor_setValue.argtypes = [CursorPtr, c_double]
libhebi_charts.hebi_charts_Cursor_setValue.restype = None
libhebi_charts.hebi_charts_Cursor_isVisible.argtypes = [CursorPtr]
libhebi_charts.hebi_charts_Cursor_isVisible.restype = c_int
libhebi_charts.hebi_charts_Cursor_setVisible.argtypes = [CursorPtr, c_int]
libhebi_charts.hebi_charts_Cursor_setVisible.restype = None
libhebi_charts.hebi_charts_Cursor_release.argtypes = [CursorPtr]
libhebi_charts.hebi_charts_Cursor_release.restype = None

# ==== FxmlView ====
libhebi_charts.hebi_charts_FxmlView_isAutoReload.argtypes = [FxmlViewPtr]
libhebi_charts.hebi_charts_FxmlView_isAutoReload.restype = c_int
libhebi_charts.hebi_charts_FxmlView_setAutoReload.argtypes = [FxmlViewPtr, c_int]
libhebi_charts.hebi_charts_FxmlView_setAutoReload.restype = None
libhebi_charts.hebi_charts_FxmlView_getSource.argtypes = [FxmlViewPtr]
libhebi_charts.hebi_charts_FxmlView_getSource.restype = c_char_p
libhebi_charts.hebi_charts_FxmlView_setSource.argtypes = [FxmlViewPtr, c_char_p]
libhebi_charts.hebi_charts_FxmlView_setSource.restype = c_int
libhebi_charts.hebi_charts_FxmlView_addLineChart.argtypes = [FxmlViewPtr, c_char_p]
libhebi_charts.hebi_charts_FxmlView_addLineChart.restype = LineChartPtr
libhebi_charts.hebi_charts_FxmlView_addScope.argtypes = [FxmlViewPtr, c_char_p]
libhebi_charts.hebi_charts_FxmlView_addScope.restype = LineChartPtr
libhebi_charts.hebi_charts_FxmlView_addLatencyChart.argtypes = [FxmlViewPtr, c_char_p]
libhebi_charts.hebi_charts_FxmlView_addLatencyChart.restype = LatencyChartPtr
libhebi_charts.hebi_charts_FxmlView_addScene3d.argtypes = [FxmlViewPtr, c_char_p]
libhebi_charts.hebi_charts_FxmlView_addScene3d.restype = Scene3dPtr
libhebi_charts.hebi_charts_FxmlView_addStreamView.argtypes = [FxmlViewPtr, c_char_p, c_char_p]
libhebi_charts.hebi_charts_FxmlView_addStreamView.restype = StreamViewPtr
libhebi_charts.hebi_charts_FxmlView_addFxmlView.argtypes = [FxmlViewPtr, c_char_p]
libhebi_charts.hebi_charts_FxmlView_addFxmlView.restype = FxmlViewPtr
libhebi_charts.hebi_charts_FxmlView_release.argtypes = [FxmlViewPtr]
libhebi_charts.hebi_charts_FxmlView_release.restype = None

# ==== GridWindow ====
libhebi_charts.hebi_charts_GridWindow_create.argtypes = [c_int, c_int]
libhebi_charts.hebi_charts_GridWindow_create.restype = GridWindowPtr
libhebi_charts.hebi_charts_GridWindow_isFullScreen.argtypes = [GridWindowPtr]
libhebi_charts.hebi_charts_GridWindow_isFullScreen.restype = c_int
libhebi_charts.hebi_charts_GridWindow_setFullScreen.argtypes = [GridWindowPtr, c_int]
libhebi_charts.hebi_charts_GridWindow_setFullScreen.restype = None
libhebi_charts.hebi_charts_GridWindow_getHeight.argtypes = [GridWindowPtr]
libhebi_charts.hebi_charts_GridWindow_getHeight.restype = c_int
libhebi_charts.hebi_charts_GridWindow_setHeight.argtypes = [GridWindowPtr, c_int]
libhebi_charts.hebi_charts_GridWindow_setHeight.restype = None
libhebi_charts.hebi_charts_GridWindow_getKeepOpen.argtypes = [GridWindowPtr]
libhebi_charts.hebi_charts_GridWindow_getKeepOpen.restype = c_int
libhebi_charts.hebi_charts_GridWindow_setKeepOpen.argtypes = [GridWindowPtr, c_int]
libhebi_charts.hebi_charts_GridWindow_setKeepOpen.restype = None
libhebi_charts.hebi_charts_GridWindow_setLocation.argtypes = [GridWindowPtr, c_int, c_int]
libhebi_charts.hebi_charts_GridWindow_setLocation.restype = None
libhebi_charts.hebi_charts_GridWindow_setSize.argtypes = [GridWindowPtr, c_int, c_int]
libhebi_charts.hebi_charts_GridWindow_setSize.restype = None
libhebi_charts.hebi_charts_GridWindow_getTitle.argtypes = [GridWindowPtr]
libhebi_charts.hebi_charts_GridWindow_getTitle.restype = c_char_p
libhebi_charts.hebi_charts_GridWindow_setTitle.argtypes = [GridWindowPtr, c_char_p]
libhebi_charts.hebi_charts_GridWindow_setTitle.restype = None
libhebi_charts.hebi_charts_GridWindow_getWidth.argtypes = [GridWindowPtr]
libhebi_charts.hebi_charts_GridWindow_getWidth.restype = c_int
libhebi_charts.hebi_charts_GridWindow_setWidth.argtypes = [GridWindowPtr, c_int]
libhebi_charts.hebi_charts_GridWindow_setWidth.restype = None
libhebi_charts.hebi_charts_GridWindow_getX.argtypes = [GridWindowPtr]
libhebi_charts.hebi_charts_GridWindow_getX.restype = c_int
libhebi_charts.hebi_charts_GridWindow_setX.argtypes = [GridWindowPtr, c_int]
libhebi_charts.hebi_charts_GridWindow_setX.restype = None
libhebi_charts.hebi_charts_GridWindow_getY.argtypes = [GridWindowPtr]
libhebi_charts.hebi_charts_GridWindow_getY.restype = c_int
libhebi_charts.hebi_charts_GridWindow_setY.argtypes = [GridWindowPtr, c_int]
libhebi_charts.hebi_charts_GridWindow_setY.restype = None
libhebi_charts.hebi_charts_GridWindow_addLineChart.argtypes = [GridWindowPtr, c_int, c_int, c_int, c_int]
libhebi_charts.hebi_charts_GridWindow_addLineChart.restype = LineChartPtr
libhebi_charts.hebi_charts_GridWindow_addScope.argtypes = [GridWindowPtr, c_int, c_int, c_int, c_int]
libhebi_charts.hebi_charts_GridWindow_addScope.restype = LineChartPtr
libhebi_charts.hebi_charts_GridWindow_addLatencyChart.argtypes = [GridWindowPtr, c_int, c_int, c_int, c_int]
libhebi_charts.hebi_charts_GridWindow_addLatencyChart.restype = LatencyChartPtr
libhebi_charts.hebi_charts_GridWindow_addScene3d.argtypes = [GridWindowPtr, c_int, c_int, c_int, c_int]
libhebi_charts.hebi_charts_GridWindow_addScene3d.restype = Scene3dPtr
libhebi_charts.hebi_charts_GridWindow_addStreamView.argtypes = [GridWindowPtr, c_char_p, c_int, c_int, c_int, c_int]
libhebi_charts.hebi_charts_GridWindow_addStreamView.restype = StreamViewPtr
libhebi_charts.hebi_charts_GridWindow_addFxmlView.argtypes = [GridWindowPtr, c_int, c_int, c_int, c_int]
libhebi_charts.hebi_charts_GridWindow_addFxmlView.restype = FxmlViewPtr
libhebi_charts.hebi_charts_GridWindow_addStylesheet.argtypes = [GridWindowPtr, c_char_p, c_int]
libhebi_charts.hebi_charts_GridWindow_addStylesheet.restype = c_int
libhebi_charts.hebi_charts_GridWindow_show.argtypes = [GridWindowPtr]
libhebi_charts.hebi_charts_GridWindow_show.restype = None
libhebi_charts.hebi_charts_GridWindow_showOffScreen.argtypes = [GridWindowPtr]
libhebi_charts.hebi_charts_GridWindow_showOffScreen.restype = c_int
libhebi_charts.hebi_charts_GridWindow_hide.argtypes = [GridWindowPtr]
libhebi_charts.hebi_charts_GridWindow_hide.restype = None
libhebi_charts.hebi_charts_GridWindow_isShowing.argtypes = [GridWindowPtr]
libhebi_charts.hebi_charts_GridWindow_isShowing.restype = c_int
libhebi_charts.hebi_charts_GridWindow_waitUntilClosed.argtypes = [GridWindowPtr]
libhebi_charts.hebi_charts_GridWindow_waitUntilClosed.restype = c_int
libhebi_charts.hebi_charts_GridWindow_getControlPanel.argtypes = [GridWindowPtr]
libhebi_charts.hebi_charts_GridWindow_getControlPanel.restype = ControlPanelPtr
libhebi_charts.hebi_charts_GridWindow_createImageStream.argtypes = [GridWindowPtr]
libhebi_charts.hebi_charts_GridWindow_createImageStream.restype = ImageStreamPtr
libhebi_charts.hebi_charts_GridWindow_dispatchMouseEvent.argtypes = [GridWindowPtr, c_int, c_int, c_int, c_int, c_double, c_double]
libhebi_charts.hebi_charts_GridWindow_dispatchMouseEvent.restype = None
libhebi_charts.hebi_charts_GridWindow_dispatchScrollEvent.argtypes = [GridWindowPtr, c_double, c_double, c_double, c_double, c_int]
libhebi_charts.hebi_charts_GridWindow_dispatchScrollEvent.restype = None
libhebi_charts.hebi_charts_GridWindow_release.argtypes = [GridWindowPtr]
libhebi_charts.hebi_charts_GridWindow_release.restype = None

# ==== HdrHistogramRecorder ====
libhebi_charts.hebi_charts_HdrHistogramRecorder_create.argtypes = []
libhebi_charts.hebi_charts_HdrHistogramRecorder_create.restype = HdrHistogramRecorderPtr
libhebi_charts.hebi_charts_HdrHistogramRecorder_getFrequency.argtypes = [HdrHistogramRecorderPtr]
libhebi_charts.hebi_charts_HdrHistogramRecorder_getFrequency.restype = c_double
libhebi_charts.hebi_charts_HdrHistogramRecorder_setFrequency.argtypes = [HdrHistogramRecorderPtr, c_double]
libhebi_charts.hebi_charts_HdrHistogramRecorder_setFrequency.restype = c_int
libhebi_charts.hebi_charts_HdrHistogramRecorder_getMax.argtypes = [HdrHistogramRecorderPtr]
libhebi_charts.hebi_charts_HdrHistogramRecorder_getMax.restype = c_double
libhebi_charts.hebi_charts_HdrHistogramRecorder_setMax.argtypes = [HdrHistogramRecorderPtr, c_double]
libhebi_charts.hebi_charts_HdrHistogramRecorder_setMax.restype = c_int
libhebi_charts.hebi_charts_HdrHistogramRecorder_getMin.argtypes = [HdrHistogramRecorderPtr]
libhebi_charts.hebi_charts_HdrHistogramRecorder_getMin.restype = c_double
libhebi_charts.hebi_charts_HdrHistogramRecorder_setMin.argtypes = [HdrHistogramRecorderPtr, c_double]
libhebi_charts.hebi_charts_HdrHistogramRecorder_setMin.restype = c_int
libhebi_charts.hebi_charts_HdrHistogramRecorder_getSignificantDigits.argtypes = [HdrHistogramRecorderPtr]
libhebi_charts.hebi_charts_HdrHistogramRecorder_getSignificantDigits.restype = c_int
libhebi_charts.hebi_charts_HdrHistogramRecorder_setSignificantDigits.argtypes = [HdrHistogramRecorderPtr, c_int]
libhebi_charts.hebi_charts_HdrHistogramRecorder_setSignificantDigits.restype = c_int
libhebi_charts.hebi_charts_HdrHistogramRecorder_addTrace.argtypes = [HdrHistogramRecorderPtr, c_char_p]
libhebi_charts.hebi_charts_HdrHistogramRecorder_addTrace.restype = HdrHistogramTracePtr
libhebi_charts.hebi_charts_HdrHistogramRecorder_startRecording.argtypes = [HdrHistogramRecorderPtr, c_char_p]
libhebi_charts.hebi_charts_HdrHistogramRecorder_startRecording.restype = c_char_p
libhebi_charts.hebi_charts_HdrHistogramRecorder_stopRecording.argtypes = [HdrHistogramRecorderPtr]
libhebi_charts.hebi_charts_HdrHistogramRecorder_stopRecording.restype = None
libhebi_charts.hebi_charts_HdrHistogramRecorder_release.argtypes = [HdrHistogramRecorderPtr]
libhebi_charts.hebi_charts_HdrHistogramRecorder_release.restype = None

# ==== HdrHistogramTrace ====
libhebi_charts.hebi_charts_HdrHistogramTrace_getMax.argtypes = [HdrHistogramTracePtr]
libhebi_charts.hebi_charts_HdrHistogramTrace_getMax.restype = c_double
libhebi_charts.hebi_charts_HdrHistogramTrace_getMean.argtypes = [HdrHistogramTracePtr]
libhebi_charts.hebi_charts_HdrHistogramTrace_getMean.restype = c_double
libhebi_charts.hebi_charts_HdrHistogramTrace_getMin.argtypes = [HdrHistogramTracePtr]
libhebi_charts.hebi_charts_HdrHistogramTrace_getMin.restype = c_double
libhebi_charts.hebi_charts_HdrHistogramTrace_getStdDeviation.argtypes = [HdrHistogramTracePtr]
libhebi_charts.hebi_charts_HdrHistogramTrace_getStdDeviation.restype = c_double
libhebi_charts.hebi_charts_HdrHistogramTrace_getTag.argtypes = [HdrHistogramTracePtr]
libhebi_charts.hebi_charts_HdrHistogramTrace_getTag.restype = c_char_p
libhebi_charts.hebi_charts_HdrHistogramTrace_getTotalCount.argtypes = [HdrHistogramTracePtr]
libhebi_charts.hebi_charts_HdrHistogramTrace_getTotalCount.restype = c_size_t
libhebi_charts.hebi_charts_HdrHistogramTrace_createLocal.argtypes = [c_int, c_double, c_double]
libhebi_charts.hebi_charts_HdrHistogramTrace_createLocal.restype = HdrHistogramTracePtr
libhebi_charts.hebi_charts_HdrHistogramTrace_getValueAtPercentile.argtypes = [HdrHistogramTracePtr, c_double]
libhebi_charts.hebi_charts_HdrHistogramTrace_getValueAtPercentile.restype = c_double
libhebi_charts.hebi_charts_HdrHistogramTrace_tic.argtypes = [HdrHistogramTracePtr]
libhebi_charts.hebi_charts_HdrHistogramTrace_tic.restype = None
libhebi_charts.hebi_charts_HdrHistogramTrace_toc.argtypes = [HdrHistogramTracePtr]
libhebi_charts.hebi_charts_HdrHistogramTrace_toc.restype = c_double
libhebi_charts.hebi_charts_HdrHistogramTrace_ticToc.argtypes = [HdrHistogramTracePtr]
libhebi_charts.hebi_charts_HdrHistogramTrace_ticToc.restype = c_double
libhebi_charts.hebi_charts_HdrHistogramTrace_recordValue.argtypes = [HdrHistogramTracePtr, c_double]
libhebi_charts.hebi_charts_HdrHistogramTrace_recordValue.restype = None
libhebi_charts.hebi_charts_HdrHistogramTrace_recordValueWithCount.argtypes = [HdrHistogramTracePtr, c_double, c_size_t]
libhebi_charts.hebi_charts_HdrHistogramTrace_recordValueWithCount.restype = None
libhebi_charts.hebi_charts_HdrHistogramTrace_recordValueWithExpectedInterval.argtypes = [HdrHistogramTracePtr, c_double, c_double]
libhebi_charts.hebi_charts_HdrHistogramTrace_recordValueWithExpectedInterval.restype = None
libhebi_charts.hebi_charts_HdrHistogramTrace_reset.argtypes = [HdrHistogramTracePtr]
libhebi_charts.hebi_charts_HdrHistogramTrace_reset.restype = None
libhebi_charts.hebi_charts_HdrHistogramTrace_toHgrmString.argtypes = [HdrHistogramTracePtr, c_double]
libhebi_charts.hebi_charts_HdrHistogramTrace_toHgrmString.restype = c_char_p
libhebi_charts.hebi_charts_HdrHistogramTrace_saveAsHgrm.argtypes = [HdrHistogramTracePtr, c_char_p, c_double]
libhebi_charts.hebi_charts_HdrHistogramTrace_saveAsHgrm.restype = c_char_p
libhebi_charts.hebi_charts_HdrHistogramTrace_release.argtypes = [HdrHistogramTracePtr]
libhebi_charts.hebi_charts_HdrHistogramTrace_release.restype = None

# ==== ImageStream ====
libhebi_charts.hebi_charts_ImageStream_getBuffer.argtypes = [ImageStreamPtr]
libhebi_charts.hebi_charts_ImageStream_getBuffer.restype = c_void_p
libhebi_charts.hebi_charts_ImageStream_getBufferSize.argtypes = [ImageStreamPtr]
libhebi_charts.hebi_charts_ImageStream_getBufferSize.restype = c_size_t
libhebi_charts.hebi_charts_ImageStream_getChannels.argtypes = [ImageStreamPtr]
libhebi_charts.hebi_charts_ImageStream_getChannels.restype = c_int
libhebi_charts.hebi_charts_ImageStream_getHeight.argtypes = [ImageStreamPtr]
libhebi_charts.hebi_charts_ImageStream_getHeight.restype = c_int
libhebi_charts.hebi_charts_ImageStream_getPixelFormat.argtypes = [ImageStreamPtr]
libhebi_charts.hebi_charts_ImageStream_getPixelFormat.restype = c_int32
libhebi_charts.hebi_charts_ImageStream_setPixelFormat.argtypes = [ImageStreamPtr, c_int32]
libhebi_charts.hebi_charts_ImageStream_setPixelFormat.restype = None
libhebi_charts.hebi_charts_ImageStream_getRateLimit.argtypes = [ImageStreamPtr]
libhebi_charts.hebi_charts_ImageStream_getRateLimit.restype = c_double
libhebi_charts.hebi_charts_ImageStream_setRateLimit.argtypes = [ImageStreamPtr, c_double]
libhebi_charts.hebi_charts_ImageStream_setRateLimit.restype = c_int
libhebi_charts.hebi_charts_ImageStream_getRecorderThreads.argtypes = [ImageStreamPtr]
libhebi_charts.hebi_charts_ImageStream_getRecorderThreads.restype = c_size_t
libhebi_charts.hebi_charts_ImageStream_setRecorderThreads.argtypes = [ImageStreamPtr, c_size_t]
libhebi_charts.hebi_charts_ImageStream_setRecorderThreads.restype = c_int
libhebi_charts.hebi_charts_ImageStream_isRecording.argtypes = [ImageStreamPtr]
libhebi_charts.hebi_charts_ImageStream_isRecording.restype = c_int
libhebi_charts.hebi_charts_ImageStream_getRenderScale.argtypes = [ImageStreamPtr]
libhebi_charts.hebi_charts_ImageStream_getRenderScale.restype = c_double
libhebi_charts.hebi_charts_ImageStream_getRenderScaleX.argtypes = [ImageStreamPtr]
libhebi_charts.hebi_charts_ImageStream_getRenderScaleX.restype = c_double
libhebi_charts.hebi_charts_ImageStream_getRenderScaleY.argtypes = [ImageStreamPtr]
libhebi_charts.hebi_charts_ImageStream_getRenderScaleY.restype = c_double
libhebi_charts.hebi_charts_ImageStream_getSequence.argtypes = [ImageStreamPtr]
libhebi_charts.hebi_charts_ImageStream_getSequence.restype = c_size_t
libhebi_charts.hebi_charts_ImageStream_getStride.argtypes = [ImageStreamPtr]
libhebi_charts.hebi_charts_ImageStream_getStride.restype = c_int
libhebi_charts.hebi_charts_ImageStream_getTimestamp.argtypes = [ImageStreamPtr]
libhebi_charts.hebi_charts_ImageStream_getTimestamp.restype = c_double
libhebi_charts.hebi_charts_ImageStream_getWidth.argtypes = [ImageStreamPtr]
libhebi_charts.hebi_charts_ImageStream_getWidth.restype = c_int
libhebi_charts.hebi_charts_ImageStream_setResolution.argtypes = [ImageStreamPtr, c_int, c_int]
libhebi_charts.hebi_charts_ImageStream_setResolution.restype = c_int
libhebi_charts.hebi_charts_ImageStream_waitForNext.argtypes = [ImageStreamPtr, c_size_t]
libhebi_charts.hebi_charts_ImageStream_waitForNext.restype = c_int
libhebi_charts.hebi_charts_ImageStream_tryGetNext.argtypes = [ImageStreamPtr]
libhebi_charts.hebi_charts_ImageStream_tryGetNext.restype = c_int
libhebi_charts.hebi_charts_ImageStream_startRecording.argtypes = [ImageStreamPtr, c_char_p, c_int]
libhebi_charts.hebi_charts_ImageStream_startRecording.restype = c_int
libhebi_charts.hebi_charts_ImageStream_stopRecording.argtypes = [ImageStreamPtr]
libhebi_charts.hebi_charts_ImageStream_stopRecording.restype = RecordingResultPtr
libhebi_charts.hebi_charts_ImageStream_saveToFile.argtypes = [ImageStreamPtr, c_char_p]
libhebi_charts.hebi_charts_ImageStream_saveToFile.restype = c_int
libhebi_charts.hebi_charts_ImageStream_release.argtypes = [ImageStreamPtr]
libhebi_charts.hebi_charts_ImageStream_release.restype = None

# ==== LoopTimer ====
libhebi_charts.hebi_charts_LoopTimer_create.argtypes = []
libhebi_charts.hebi_charts_LoopTimer_create.restype = LoopTimerPtr
libhebi_charts.hebi_charts_LoopTimer_getElapsedTime.argtypes = [LoopTimerPtr]
libhebi_charts.hebi_charts_LoopTimer_getElapsedTime.restype = c_double
libhebi_charts.hebi_charts_LoopTimer_getFrequency.argtypes = [LoopTimerPtr]
libhebi_charts.hebi_charts_LoopTimer_getFrequency.restype = c_double
libhebi_charts.hebi_charts_LoopTimer_setFrequency.argtypes = [LoopTimerPtr, c_double]
libhebi_charts.hebi_charts_LoopTimer_setFrequency.restype = None
libhebi_charts.hebi_charts_LoopTimer_getPeriod.argtypes = [LoopTimerPtr]
libhebi_charts.hebi_charts_LoopTimer_getPeriod.restype = c_double
libhebi_charts.hebi_charts_LoopTimer_setPeriod.argtypes = [LoopTimerPtr, c_double]
libhebi_charts.hebi_charts_LoopTimer_setPeriod.restype = None
libhebi_charts.hebi_charts_LoopTimer_restart.argtypes = [LoopTimerPtr]
libhebi_charts.hebi_charts_LoopTimer_restart.restype = None
libhebi_charts.hebi_charts_LoopTimer_tic.argtypes = [LoopTimerPtr]
libhebi_charts.hebi_charts_LoopTimer_tic.restype = None
libhebi_charts.hebi_charts_LoopTimer_toc.argtypes = [LoopTimerPtr]
libhebi_charts.hebi_charts_LoopTimer_toc.restype = c_double
libhebi_charts.hebi_charts_LoopTimer_ticToc.argtypes = [LoopTimerPtr]
libhebi_charts.hebi_charts_LoopTimer_ticToc.restype = c_double
libhebi_charts.hebi_charts_LoopTimer_waitForNextTick.argtypes = [LoopTimerPtr]
libhebi_charts.hebi_charts_LoopTimer_waitForNextTick.restype = None
libhebi_charts.hebi_charts_LoopTimer_getSecondsToNextTick.argtypes = [LoopTimerPtr]
libhebi_charts.hebi_charts_LoopTimer_getSecondsToNextTick.restype = c_double
libhebi_charts.hebi_charts_LoopTimer_getNanosToNextTick.argtypes = [LoopTimerPtr]
libhebi_charts.hebi_charts_LoopTimer_getNanosToNextTick.restype = c_size_t
libhebi_charts.hebi_charts_LoopTimer_timeSeconds.argtypes = []
libhebi_charts.hebi_charts_LoopTimer_timeSeconds.restype = c_double
libhebi_charts.hebi_charts_LoopTimer_timeNanos.argtypes = []
libhebi_charts.hebi_charts_LoopTimer_timeNanos.restype = c_size_t
libhebi_charts.hebi_charts_LoopTimer_sleepNanos.argtypes = [c_size_t]
libhebi_charts.hebi_charts_LoopTimer_sleepNanos.restype = None
libhebi_charts.hebi_charts_LoopTimer_parkNanos.argtypes = [c_size_t]
libhebi_charts.hebi_charts_LoopTimer_parkNanos.restype = None
libhebi_charts.hebi_charts_LoopTimer_waitNanos.argtypes = [c_size_t]
libhebi_charts.hebi_charts_LoopTimer_waitNanos.restype = None
libhebi_charts.hebi_charts_LoopTimer_spinNanos.argtypes = [c_size_t]
libhebi_charts.hebi_charts_LoopTimer_spinNanos.restype = None
libhebi_charts.hebi_charts_LoopTimer_release.argtypes = [LoopTimerPtr]
libhebi_charts.hebi_charts_LoopTimer_release.restype = None

# ==== Object3d ====
libhebi_charts.hebi_charts_Object3d_isVisible.argtypes = [Object3dPtr]
libhebi_charts.hebi_charts_Object3d_isVisible.restype = c_int
libhebi_charts.hebi_charts_Object3d_setVisible.argtypes = [Object3dPtr, c_int]
libhebi_charts.hebi_charts_Object3d_setVisible.restype = None
libhebi_charts.hebi_charts_Object3d_setOrientation.argtypes = [Object3dPtr, c_double, c_double, c_double, c_double]
libhebi_charts.hebi_charts_Object3d_setOrientation.restype = c_int
libhebi_charts.hebi_charts_Object3d_setOrientationRPY.argtypes = [Object3dPtr, c_double, c_double, c_double]
libhebi_charts.hebi_charts_Object3d_setOrientationRPY.restype = c_int
libhebi_charts.hebi_charts_Object3d_setTranslation.argtypes = [Object3dPtr, c_double, c_double, c_double]
libhebi_charts.hebi_charts_Object3d_setTranslation.restype = c_int
libhebi_charts.hebi_charts_Object3d_setPose.argtypes = [Object3dPtr, c_double, c_double, c_double, c_double, c_double, c_double, c_double]
libhebi_charts.hebi_charts_Object3d_setPose.restype = c_int
libhebi_charts.hebi_charts_Object3d_setTransform4x4.argtypes = [Object3dPtr, POINTER(c_double), c_int32]
libhebi_charts.hebi_charts_Object3d_setTransform4x4.restype = c_int
libhebi_charts.hebi_charts_Object3d_release.argtypes = [Object3dPtr]
libhebi_charts.hebi_charts_Object3d_release.restype = None

# ==== Frame ====
libhebi_charts.hebi_charts_Frame_to_Object3d.argtypes = [FramePtr]
libhebi_charts.hebi_charts_Frame_to_Object3d.restype = Object3dPtr

# ==== Mesh ====
libhebi_charts.hebi_charts_Mesh_isCentered.argtypes = [MeshPtr]
libhebi_charts.hebi_charts_Mesh_isCentered.restype = c_int
libhebi_charts.hebi_charts_Mesh_setCentered.argtypes = [MeshPtr, c_int]
libhebi_charts.hebi_charts_Mesh_setCentered.restype = None
libhebi_charts.hebi_charts_Mesh_getScale.argtypes = [MeshPtr]
libhebi_charts.hebi_charts_Mesh_getScale.restype = c_double
libhebi_charts.hebi_charts_Mesh_setScale.argtypes = [MeshPtr, c_double]
libhebi_charts.hebi_charts_Mesh_setScale.restype = None
libhebi_charts.hebi_charts_Mesh_getDisplayStyle.argtypes = [MeshPtr]
libhebi_charts.hebi_charts_Mesh_getDisplayStyle.restype = c_int32
libhebi_charts.hebi_charts_Mesh_setDisplayStyle.argtypes = [MeshPtr, c_int32]
libhebi_charts.hebi_charts_Mesh_setDisplayStyle.restype = None
libhebi_charts.hebi_charts_Mesh_to_Object3d.argtypes = [MeshPtr]
libhebi_charts.hebi_charts_Mesh_to_Object3d.restype = Object3dPtr

# ==== Robot ====
libhebi_charts.hebi_charts_Robot_getDisplayStyle.argtypes = [RobotPtr]
libhebi_charts.hebi_charts_Robot_getDisplayStyle.restype = c_int32
libhebi_charts.hebi_charts_Robot_setDisplayStyle.argtypes = [RobotPtr, c_int32]
libhebi_charts.hebi_charts_Robot_setDisplayStyle.restype = None
libhebi_charts.hebi_charts_Robot_getDof.argtypes = [RobotPtr]
libhebi_charts.hebi_charts_Robot_getDof.restype = c_size_t
libhebi_charts.hebi_charts_Robot_setPositions.argtypes = [RobotPtr, POINTER(c_double), c_size_t]
libhebi_charts.hebi_charts_Robot_setPositions.restype = c_int
libhebi_charts.hebi_charts_Robot_to_Object3d.argtypes = [RobotPtr]
libhebi_charts.hebi_charts_Robot_to_Object3d.restype = Object3dPtr

# ==== Series3d ====
libhebi_charts.hebi_charts_Series3d_getColor.argtypes = [Series3dPtr]
libhebi_charts.hebi_charts_Series3d_getColor.restype = c_int32
libhebi_charts.hebi_charts_Series3d_setColor.argtypes = [Series3dPtr, c_int32]
libhebi_charts.hebi_charts_Series3d_setColor.restype = None
libhebi_charts.hebi_charts_Series3d_to_Object3d.argtypes = [Series3dPtr]
libhebi_charts.hebi_charts_Series3d_to_Object3d.restype = Object3dPtr

# ==== Line3d ====
libhebi_charts.hebi_charts_Line3d_setMaxPointCount.argtypes = [Line3dPtr, c_size_t]
libhebi_charts.hebi_charts_Line3d_setMaxPointCount.restype = None
libhebi_charts.hebi_charts_Line3d_clear.argtypes = [Line3dPtr]
libhebi_charts.hebi_charts_Line3d_clear.restype = None
libhebi_charts.hebi_charts_Line3d_setData.argtypes = [Line3dPtr, POINTER(c_double), POINTER(c_double), POINTER(c_double), c_size_t]
libhebi_charts.hebi_charts_Line3d_setData.restype = None
libhebi_charts.hebi_charts_Line3d_addPoints.argtypes = [Line3dPtr, POINTER(c_double), POINTER(c_double), POINTER(c_double), c_size_t]
libhebi_charts.hebi_charts_Line3d_addPoints.restype = None
libhebi_charts.hebi_charts_Line3d_addPoint.argtypes = [Line3dPtr, c_double, c_double, c_double]
libhebi_charts.hebi_charts_Line3d_addPoint.restype = None
libhebi_charts.hebi_charts_Line3d_to_Series3d.argtypes = [Line3dPtr]
libhebi_charts.hebi_charts_Line3d_to_Series3d.restype = Series3dPtr

# ==== Points3d ====
libhebi_charts.hebi_charts_Points3d_getMarkerShape.argtypes = [Points3dPtr]
libhebi_charts.hebi_charts_Points3d_getMarkerShape.restype = c_int32
libhebi_charts.hebi_charts_Points3d_setMarkerShape.argtypes = [Points3dPtr, c_int32]
libhebi_charts.hebi_charts_Points3d_setMarkerShape.restype = None
libhebi_charts.hebi_charts_Points3d_getMarkerSize.argtypes = [Points3dPtr]
libhebi_charts.hebi_charts_Points3d_getMarkerSize.restype = c_double
libhebi_charts.hebi_charts_Points3d_setMarkerSize.argtypes = [Points3dPtr, c_double]
libhebi_charts.hebi_charts_Points3d_setMarkerSize.restype = None
libhebi_charts.hebi_charts_Points3d_isSelfIllumination.argtypes = [Points3dPtr]
libhebi_charts.hebi_charts_Points3d_isSelfIllumination.restype = c_int
libhebi_charts.hebi_charts_Points3d_setSelfIllumination.argtypes = [Points3dPtr, c_int]
libhebi_charts.hebi_charts_Points3d_setSelfIllumination.restype = None
libhebi_charts.hebi_charts_Points3d_isVertexSharing.argtypes = [Points3dPtr]
libhebi_charts.hebi_charts_Points3d_isVertexSharing.restype = c_int
libhebi_charts.hebi_charts_Points3d_setVertexSharing.argtypes = [Points3dPtr, c_int]
libhebi_charts.hebi_charts_Points3d_setVertexSharing.restype = None
libhebi_charts.hebi_charts_Points3d_setMaxPointCount.argtypes = [Points3dPtr, c_size_t]
libhebi_charts.hebi_charts_Points3d_setMaxPointCount.restype = None
libhebi_charts.hebi_charts_Points3d_clear.argtypes = [Points3dPtr]
libhebi_charts.hebi_charts_Points3d_clear.restype = None
libhebi_charts.hebi_charts_Points3d_setData.argtypes = [Points3dPtr, POINTER(c_double), POINTER(c_double), POINTER(c_double), c_size_t]
libhebi_charts.hebi_charts_Points3d_setData.restype = None
libhebi_charts.hebi_charts_Points3d_addPoints.argtypes = [Points3dPtr, POINTER(c_double), POINTER(c_double), POINTER(c_double), c_size_t]
libhebi_charts.hebi_charts_Points3d_addPoints.restype = None
libhebi_charts.hebi_charts_Points3d_addPoint.argtypes = [Points3dPtr, c_double, c_double, c_double]
libhebi_charts.hebi_charts_Points3d_addPoint.restype = None
libhebi_charts.hebi_charts_Points3d_to_Series3d.argtypes = [Points3dPtr]
libhebi_charts.hebi_charts_Points3d_to_Series3d.restype = Series3dPtr

# ==== RecordingResult ====
libhebi_charts.hebi_charts_RecordingResult_getDirectory.argtypes = [RecordingResultPtr]
libhebi_charts.hebi_charts_RecordingResult_getDirectory.restype = c_char_p
libhebi_charts.hebi_charts_RecordingResult_getDroppedCount.argtypes = [RecordingResultPtr]
libhebi_charts.hebi_charts_RecordingResult_getDroppedCount.restype = c_size_t
libhebi_charts.hebi_charts_RecordingResult_getDuration.argtypes = [RecordingResultPtr]
libhebi_charts.hebi_charts_RecordingResult_getDuration.restype = c_double
libhebi_charts.hebi_charts_RecordingResult_getAverageFps.argtypes = [RecordingResultPtr]
libhebi_charts.hebi_charts_RecordingResult_getAverageFps.restype = c_double
libhebi_charts.hebi_charts_RecordingResult_getManifest.argtypes = [RecordingResultPtr]
libhebi_charts.hebi_charts_RecordingResult_getManifest.restype = c_char_p
libhebi_charts.hebi_charts_RecordingResult_getRecordedCount.argtypes = [RecordingResultPtr]
libhebi_charts.hebi_charts_RecordingResult_getRecordedCount.restype = c_size_t
libhebi_charts.hebi_charts_RecordingResult_getSkippedCount.argtypes = [RecordingResultPtr]
libhebi_charts.hebi_charts_RecordingResult_getSkippedCount.restype = c_size_t
libhebi_charts.hebi_charts_RecordingResult_getTotalFrames.argtypes = [RecordingResultPtr]
libhebi_charts.hebi_charts_RecordingResult_getTotalFrames.restype = c_size_t
libhebi_charts.hebi_charts_RecordingResult_getFfmpegCommand.argtypes = [RecordingResultPtr, c_int32, c_int]
libhebi_charts.hebi_charts_RecordingResult_getFfmpegCommand.restype = c_char_p
libhebi_charts.hebi_charts_RecordingResult_runFfmpeg.argtypes = [RecordingResultPtr, c_int32, c_int]
libhebi_charts.hebi_charts_RecordingResult_runFfmpeg.restype = c_char_p
libhebi_charts.hebi_charts_RecordingResult_release.argtypes = [RecordingResultPtr]
libhebi_charts.hebi_charts_RecordingResult_release.restype = None

# ==== Scene3d ====
libhebi_charts.hebi_charts_Scene3d_setGridBounds.argtypes = [Scene3dPtr, c_double, c_double, c_double, c_double, c_double, c_double]
libhebi_charts.hebi_charts_Scene3d_setGridBounds.restype = None
libhebi_charts.hebi_charts_Scene3d_getMaxX.argtypes = [Scene3dPtr]
libhebi_charts.hebi_charts_Scene3d_getMaxX.restype = c_double
libhebi_charts.hebi_charts_Scene3d_setMaxX.argtypes = [Scene3dPtr, c_double]
libhebi_charts.hebi_charts_Scene3d_setMaxX.restype = None
libhebi_charts.hebi_charts_Scene3d_getMaxY.argtypes = [Scene3dPtr]
libhebi_charts.hebi_charts_Scene3d_getMaxY.restype = c_double
libhebi_charts.hebi_charts_Scene3d_setMaxY.argtypes = [Scene3dPtr, c_double]
libhebi_charts.hebi_charts_Scene3d_setMaxY.restype = None
libhebi_charts.hebi_charts_Scene3d_getMaxZ.argtypes = [Scene3dPtr]
libhebi_charts.hebi_charts_Scene3d_getMaxZ.restype = c_double
libhebi_charts.hebi_charts_Scene3d_setMaxZ.argtypes = [Scene3dPtr, c_double]
libhebi_charts.hebi_charts_Scene3d_setMaxZ.restype = None
libhebi_charts.hebi_charts_Scene3d_getMinX.argtypes = [Scene3dPtr]
libhebi_charts.hebi_charts_Scene3d_getMinX.restype = c_double
libhebi_charts.hebi_charts_Scene3d_setMinX.argtypes = [Scene3dPtr, c_double]
libhebi_charts.hebi_charts_Scene3d_setMinX.restype = None
libhebi_charts.hebi_charts_Scene3d_getMinY.argtypes = [Scene3dPtr]
libhebi_charts.hebi_charts_Scene3d_getMinY.restype = c_double
libhebi_charts.hebi_charts_Scene3d_setMinY.argtypes = [Scene3dPtr, c_double]
libhebi_charts.hebi_charts_Scene3d_setMinY.restype = None
libhebi_charts.hebi_charts_Scene3d_getMinZ.argtypes = [Scene3dPtr]
libhebi_charts.hebi_charts_Scene3d_getMinZ.restype = c_double
libhebi_charts.hebi_charts_Scene3d_setMinZ.argtypes = [Scene3dPtr, c_double]
libhebi_charts.hebi_charts_Scene3d_setMinZ.restype = None
libhebi_charts.hebi_charts_Scene3d_getGridStep.argtypes = [Scene3dPtr]
libhebi_charts.hebi_charts_Scene3d_getGridStep.restype = c_double
libhebi_charts.hebi_charts_Scene3d_setGridStep.argtypes = [Scene3dPtr, c_double]
libhebi_charts.hebi_charts_Scene3d_setGridStep.restype = None
libhebi_charts.hebi_charts_Scene3d_getCamera.argtypes = [Scene3dPtr]
libhebi_charts.hebi_charts_Scene3d_getCamera.restype = CameraPtr
libhebi_charts.hebi_charts_Scene3d_addRobot.argtypes = [Scene3dPtr, c_char_p]
libhebi_charts.hebi_charts_Scene3d_addRobot.restype = RobotPtr
libhebi_charts.hebi_charts_Scene3d_addMesh.argtypes = [Scene3dPtr, c_char_p]
libhebi_charts.hebi_charts_Scene3d_addMesh.restype = MeshPtr
libhebi_charts.hebi_charts_Scene3d_addFrame.argtypes = [Scene3dPtr, c_double]
libhebi_charts.hebi_charts_Scene3d_addFrame.restype = FramePtr
libhebi_charts.hebi_charts_Scene3d_addLine.argtypes = [Scene3dPtr]
libhebi_charts.hebi_charts_Scene3d_addLine.restype = Line3dPtr
libhebi_charts.hebi_charts_Scene3d_addPoints.argtypes = [Scene3dPtr]
libhebi_charts.hebi_charts_Scene3d_addPoints.restype = Points3dPtr
libhebi_charts.hebi_charts_Scene3d_release.argtypes = [Scene3dPtr]
libhebi_charts.hebi_charts_Scene3d_release.restype = None

# ==== StreamView ====
libhebi_charts.hebi_charts_StreamView_release.argtypes = [StreamViewPtr]
libhebi_charts.hebi_charts_StreamView_release.restype = None

# ==== XYChart ====
libhebi_charts.hebi_charts_XYChart_getTitle.argtypes = [XYChartPtr]
libhebi_charts.hebi_charts_XYChart_getTitle.restype = c_char_p
libhebi_charts.hebi_charts_XYChart_setTitle.argtypes = [XYChartPtr, c_char_p]
libhebi_charts.hebi_charts_XYChart_setTitle.restype = None
libhebi_charts.hebi_charts_XYChart_getXAssumeSorted.argtypes = [XYChartPtr]
libhebi_charts.hebi_charts_XYChart_getXAssumeSorted.restype = c_int
libhebi_charts.hebi_charts_XYChart_setXAssumeSorted.argtypes = [XYChartPtr, c_int]
libhebi_charts.hebi_charts_XYChart_setXAssumeSorted.restype = None
libhebi_charts.hebi_charts_XYChart_isXAutoUnitScaling.argtypes = [XYChartPtr]
libhebi_charts.hebi_charts_XYChart_isXAutoUnitScaling.restype = c_int
libhebi_charts.hebi_charts_XYChart_setXAutoUnitScaling.argtypes = [XYChartPtr, c_int]
libhebi_charts.hebi_charts_XYChart_setXAutoUnitScaling.restype = None
libhebi_charts.hebi_charts_XYChart_getXLabel.argtypes = [XYChartPtr]
libhebi_charts.hebi_charts_XYChart_getXLabel.restype = c_char_p
libhebi_charts.hebi_charts_XYChart_setXLabel.argtypes = [XYChartPtr, c_char_p]
libhebi_charts.hebi_charts_XYChart_setXLabel.restype = None
libhebi_charts.hebi_charts_XYChart_setXLim.argtypes = [XYChartPtr, c_double, c_double]
libhebi_charts.hebi_charts_XYChart_setXLim.restype = c_int
libhebi_charts.hebi_charts_XYChart_getXMax.argtypes = [XYChartPtr]
libhebi_charts.hebi_charts_XYChart_getXMax.restype = c_double
libhebi_charts.hebi_charts_XYChart_setXMax.argtypes = [XYChartPtr, c_double]
libhebi_charts.hebi_charts_XYChart_setXMax.restype = None
libhebi_charts.hebi_charts_XYChart_getXMin.argtypes = [XYChartPtr]
libhebi_charts.hebi_charts_XYChart_getXMin.restype = c_double
libhebi_charts.hebi_charts_XYChart_setXMin.argtypes = [XYChartPtr, c_double]
libhebi_charts.hebi_charts_XYChart_setXMin.restype = None
libhebi_charts.hebi_charts_XYChart_getXUnit.argtypes = [XYChartPtr]
libhebi_charts.hebi_charts_XYChart_getXUnit.restype = c_char_p
libhebi_charts.hebi_charts_XYChart_setXUnit.argtypes = [XYChartPtr, c_char_p]
libhebi_charts.hebi_charts_XYChart_setXUnit.restype = None
libhebi_charts.hebi_charts_XYChart_isYAutoUnitScaling.argtypes = [XYChartPtr]
libhebi_charts.hebi_charts_XYChart_isYAutoUnitScaling.restype = c_int
libhebi_charts.hebi_charts_XYChart_setYAutoUnitScaling.argtypes = [XYChartPtr, c_int]
libhebi_charts.hebi_charts_XYChart_setYAutoUnitScaling.restype = None
libhebi_charts.hebi_charts_XYChart_getYLabel.argtypes = [XYChartPtr]
libhebi_charts.hebi_charts_XYChart_getYLabel.restype = c_char_p
libhebi_charts.hebi_charts_XYChart_setYLabel.argtypes = [XYChartPtr, c_char_p]
libhebi_charts.hebi_charts_XYChart_setYLabel.restype = None
libhebi_charts.hebi_charts_XYChart_setYLim.argtypes = [XYChartPtr, c_double, c_double]
libhebi_charts.hebi_charts_XYChart_setYLim.restype = None
libhebi_charts.hebi_charts_XYChart_getYMax.argtypes = [XYChartPtr]
libhebi_charts.hebi_charts_XYChart_getYMax.restype = c_double
libhebi_charts.hebi_charts_XYChart_setYMax.argtypes = [XYChartPtr, c_double]
libhebi_charts.hebi_charts_XYChart_setYMax.restype = None
libhebi_charts.hebi_charts_XYChart_getYMin.argtypes = [XYChartPtr]
libhebi_charts.hebi_charts_XYChart_getYMin.restype = c_double
libhebi_charts.hebi_charts_XYChart_setYMin.argtypes = [XYChartPtr, c_double]
libhebi_charts.hebi_charts_XYChart_setYMin.restype = None
libhebi_charts.hebi_charts_XYChart_getYUnit.argtypes = [XYChartPtr]
libhebi_charts.hebi_charts_XYChart_getYUnit.restype = c_char_p
libhebi_charts.hebi_charts_XYChart_setYUnit.argtypes = [XYChartPtr, c_char_p]
libhebi_charts.hebi_charts_XYChart_setYUnit.restype = None
libhebi_charts.hebi_charts_XYChart_addXCursor.argtypes = [XYChartPtr]
libhebi_charts.hebi_charts_XYChart_addXCursor.restype = CursorPtr
libhebi_charts.hebi_charts_XYChart_addYCursor.argtypes = [XYChartPtr]
libhebi_charts.hebi_charts_XYChart_addYCursor.restype = CursorPtr
libhebi_charts.hebi_charts_XYChart_release.argtypes = [XYChartPtr]
libhebi_charts.hebi_charts_XYChart_release.restype = None

# ==== LatencyChart ====
libhebi_charts.hebi_charts_LatencyChart_addTrace.argtypes = [LatencyChartPtr, c_char_p]
libhebi_charts.hebi_charts_LatencyChart_addTrace.restype = LatencyTracePtr
libhebi_charts.hebi_charts_LatencyChart_to_XYChart.argtypes = [LatencyChartPtr]
libhebi_charts.hebi_charts_LatencyChart_to_XYChart.restype = XYChartPtr

# ==== LineChart ====
libhebi_charts.hebi_charts_LineChart_addLine.argtypes = [LineChartPtr, c_char_p]
libhebi_charts.hebi_charts_LineChart_addLine.restype = LinePtr
libhebi_charts.hebi_charts_LineChart_to_XYChart.argtypes = [LineChartPtr]
libhebi_charts.hebi_charts_LineChart_to_XYChart.restype = XYChartPtr

# ==== XYSeries ====
libhebi_charts.hebi_charts_XYSeries_getColor.argtypes = [XYSeriesPtr]
libhebi_charts.hebi_charts_XYSeries_getColor.restype = c_int32
libhebi_charts.hebi_charts_XYSeries_setColor.argtypes = [XYSeriesPtr, c_int32]
libhebi_charts.hebi_charts_XYSeries_setColor.restype = None
libhebi_charts.hebi_charts_XYSeries_getLabel.argtypes = [XYSeriesPtr]
libhebi_charts.hebi_charts_XYSeries_getLabel.restype = c_char_p
libhebi_charts.hebi_charts_XYSeries_setLabel.argtypes = [XYSeriesPtr, c_char_p]
libhebi_charts.hebi_charts_XYSeries_setLabel.restype = None
libhebi_charts.hebi_charts_XYSeries_getLineStyle.argtypes = [XYSeriesPtr]
libhebi_charts.hebi_charts_XYSeries_getLineStyle.restype = c_int32
libhebi_charts.hebi_charts_XYSeries_setLineStyle.argtypes = [XYSeriesPtr, c_int32]
libhebi_charts.hebi_charts_XYSeries_setLineStyle.restype = None
libhebi_charts.hebi_charts_XYSeries_getLineWidth.argtypes = [XYSeriesPtr]
libhebi_charts.hebi_charts_XYSeries_getLineWidth.restype = c_double
libhebi_charts.hebi_charts_XYSeries_setLineWidth.argtypes = [XYSeriesPtr, c_double]
libhebi_charts.hebi_charts_XYSeries_setLineWidth.restype = None
libhebi_charts.hebi_charts_XYSeries_getMarkerSize.argtypes = [XYSeriesPtr]
libhebi_charts.hebi_charts_XYSeries_getMarkerSize.restype = c_double
libhebi_charts.hebi_charts_XYSeries_setMarkerSize.argtypes = [XYSeriesPtr, c_double]
libhebi_charts.hebi_charts_XYSeries_setMarkerSize.restype = None
libhebi_charts.hebi_charts_XYSeries_getMarkerType.argtypes = [XYSeriesPtr]
libhebi_charts.hebi_charts_XYSeries_getMarkerType.restype = c_int32
libhebi_charts.hebi_charts_XYSeries_setMarkerType.argtypes = [XYSeriesPtr, c_int32]
libhebi_charts.hebi_charts_XYSeries_setMarkerType.restype = None
libhebi_charts.hebi_charts_XYSeries_isShowInLegend.argtypes = [XYSeriesPtr]
libhebi_charts.hebi_charts_XYSeries_isShowInLegend.restype = c_int
libhebi_charts.hebi_charts_XYSeries_setShowInLegend.argtypes = [XYSeriesPtr, c_int]
libhebi_charts.hebi_charts_XYSeries_setShowInLegend.restype = None
libhebi_charts.hebi_charts_XYSeries_isVisible.argtypes = [XYSeriesPtr]
libhebi_charts.hebi_charts_XYSeries_isVisible.restype = c_int
libhebi_charts.hebi_charts_XYSeries_setVisible.argtypes = [XYSeriesPtr, c_int]
libhebi_charts.hebi_charts_XYSeries_setVisible.restype = None
libhebi_charts.hebi_charts_XYSeries_release.argtypes = [XYSeriesPtr]
libhebi_charts.hebi_charts_XYSeries_release.restype = None

# ==== LatencyTrace ====
libhebi_charts.hebi_charts_LatencyTrace_tic.argtypes = [LatencyTracePtr]
libhebi_charts.hebi_charts_LatencyTrace_tic.restype = None
libhebi_charts.hebi_charts_LatencyTrace_toc.argtypes = [LatencyTracePtr]
libhebi_charts.hebi_charts_LatencyTrace_toc.restype = c_double
libhebi_charts.hebi_charts_LatencyTrace_ticToc.argtypes = [LatencyTracePtr]
libhebi_charts.hebi_charts_LatencyTrace_ticToc.restype = c_double
libhebi_charts.hebi_charts_LatencyTrace_record.argtypes = [LatencyTracePtr, c_double]
libhebi_charts.hebi_charts_LatencyTrace_record.restype = None
libhebi_charts.hebi_charts_LatencyTrace_recordWithCount.argtypes = [LatencyTracePtr, c_double, c_size_t]
libhebi_charts.hebi_charts_LatencyTrace_recordWithCount.restype = None
libhebi_charts.hebi_charts_LatencyTrace_recordCompensated.argtypes = [LatencyTracePtr, c_double, c_double]
libhebi_charts.hebi_charts_LatencyTrace_recordCompensated.restype = None
libhebi_charts.hebi_charts_LatencyTrace_reset.argtypes = [LatencyTracePtr]
libhebi_charts.hebi_charts_LatencyTrace_reset.restype = None
libhebi_charts.hebi_charts_LatencyTrace_to_XYSeries.argtypes = [LatencyTracePtr]
libhebi_charts.hebi_charts_LatencyTrace_to_XYSeries.restype = XYSeriesPtr

# ==== Line ====
libhebi_charts.hebi_charts_Line_getMaxPointCount.argtypes = [LinePtr]
libhebi_charts.hebi_charts_Line_getMaxPointCount.restype = c_size_t
libhebi_charts.hebi_charts_Line_setMaxPointCount.argtypes = [LinePtr, c_size_t]
libhebi_charts.hebi_charts_Line_setMaxPointCount.restype = None
libhebi_charts.hebi_charts_Line_clear.argtypes = [LinePtr]
libhebi_charts.hebi_charts_Line_clear.restype = None
libhebi_charts.hebi_charts_Line_setData.argtypes = [LinePtr, POINTER(c_double), POINTER(c_double), c_size_t]
libhebi_charts.hebi_charts_Line_setData.restype = None
libhebi_charts.hebi_charts_Line_addPoints.argtypes = [LinePtr, POINTER(c_double), POINTER(c_double), c_size_t]
libhebi_charts.hebi_charts_Line_addPoints.restype = None
libhebi_charts.hebi_charts_Line_addPoint.argtypes = [LinePtr, c_double, c_double]
libhebi_charts.hebi_charts_Line_addPoint.restype = None
libhebi_charts.hebi_charts_Line_to_XYSeries.argtypes = [LinePtr]
libhebi_charts.hebi_charts_Line_to_XYSeries.restype = XYSeriesPtr

# ==== Runtime ====
libhebi_charts.hebi_charts_Runtime_setOption.argtypes = [c_int32, c_char_p]
libhebi_charts.hebi_charts_Runtime_setOption.restype = c_int
libhebi_charts.hebi_charts_Runtime_setTheme.argtypes = [c_int32]
libhebi_charts.hebi_charts_Runtime_setTheme.restype = None
libhebi_charts.hebi_charts_Runtime_setAutoCloseWindows.argtypes = [c_int]
libhebi_charts.hebi_charts_Runtime_setAutoCloseWindows.restype = None
libhebi_charts.hebi_charts_Runtime_waitUntilWindowsClosed.argtypes = []
libhebi_charts.hebi_charts_Runtime_waitUntilWindowsClosed.restype = c_int
libhebi_charts.hebi_charts_Runtime_collect.argtypes = []
libhebi_charts.hebi_charts_Runtime_collect.restype = None
libhebi_charts.hebi_charts_Runtime_closeAll.argtypes = []
libhebi_charts.hebi_charts_Runtime_closeAll.restype = None
libhebi_charts.hebi_charts_Runtime_runOnUiThread.argtypes = [UserCallbackFunction, c_void_p]
libhebi_charts.hebi_charts_Runtime_runOnUiThread.restype = None
libhebi_charts.hebi_charts_Runtime_printLastErrorDetails.argtypes = []
libhebi_charts.hebi_charts_Runtime_printLastErrorDetails.restype = None
libhebi_charts.hebi_charts_Runtime_printThreadInfo.argtypes = [c_char_p]
libhebi_charts.hebi_charts_Runtime_printThreadInfo.restype = None
libhebi_charts.hebi_charts_Runtime_getLastErrorString.argtypes = []
libhebi_charts.hebi_charts_Runtime_getLastErrorString.restype = c_char_p

# ==== Enum Wrappers ====
class CameraView(IntEnum):
    ISOMETRIC = 0
    FRONT = 1
    BACK = 2
    LEFT = 3
    RIGHT = 4
    TOP = 5
    BOTTOM = 6

class Color(IntEnum):
    DEFAULT = 0
    BLACK = 1
    WHITE = 2
    GRAY = 3
    RED = 4
    GREEN = 5
    BLUE = 6
    YELLOW = 7
    MAGENTA = 8

class DisplayStyle(IntEnum):
    ORIGINAL = 0
    TRANSLUCENT = 1
    GHOSTED = 2

class LineStyle(IntEnum):
    DEFAULT = 0
    SOLID = 1
    DASHED = 2
    POINTS = 3

class MarkerShape(IntEnum):
    DEFAULT = 0
    TETRAHEDRON = 1
    CUBE = 2
    OCTAHEDRON = 3
    ICOSAHEDRON = 4
    SPHERE = 5

class MarkerType(IntEnum):
    DEFAULT = 0
    RECTANGLE = 1
    RECTANGLE1 = 2
    RECTANGLE2 = 3
    CIRCLE = 4
    CIRCLE1 = 5
    CIRCLE2 = 6
    PLUS = 7
    CROSS = 8
    DIAMOND = 9
    DIAMOND1 = 10
    DIAMOND2 = 11

class MatrixOrdering(IntEnum):
    ROW_MAJOR = 0
    COLUMN_MAJOR = 1

class PixelFormat(IntEnum):
    UNKNOWN = 0
    BGRA_PRE = 1
    BGRA = 2
    ARGB = 3
    RGBA = 4
    RGB = 5
    BGR = 6
    GRAY = 7

class RuntimeOption(IntEnum):
    DPI_SCALE = 0
    GRAPHICS_PIPELINE = 1
    TEXT_ANTIALIASING = 2
    V_SYNC = 3
    VERBOSE_GRAPHICS = 4
    VERBOSE_PULSE = 5
    VERBOSE_FONTS = 6
    VERBOSE_WINDOWING = 7

class Theme(IntEnum):
    PRIMER_LIGHT = 0
    PRIMER_DARK = 1
    NORD_LIGHT = 2
    NORD_DARK = 3
    CUPERTINO_LIGHT = 4
    CUPERTINO_DARK = 5
    DRACULA = 6

class VideoOutputFormat(IntEnum):
    H264 = 0
    H265 = 1
    PRO_RES = 2
    WEB_M = 3
    GIF = 4


# ==== Class Wrappers ====
class Camera:
    """Represents a view point looking at a 3d scene"""

    __slots__ = ('ptr',)

    def __init__(self, ptr: CameraPtr):
        if not isinstance(ptr, CameraPtr):
            raise TypeError(f'Expected CameraPtr, got {type(ptr).__name__}')
        self.ptr = ptr

    def __del__(self):
        if self.ptr is not None:
            libhebi_charts.hebi_charts_Camera_release(self.ptr)
        self.ptr = None

    def __repr__(self):
        return f"{self.__class__.__name__}()"

    def set_view(self, view: CameraView) -> None:
        """Sets the view point to a predefined standard view
        
        Args:
            view:
        """
        libhebi_charts.hebi_charts_Camera_setView(self.ptr, view)

    def reset(self) -> None:
        """Resets the camera orientation, zoom, and pan to defaults"""
        libhebi_charts.hebi_charts_Camera_reset(self.ptr)

    def apply_rotation(self, qx: float, qy: float, qz: float, qw: float) -> None:
        """Applies an incremental rotation to the current camera view using a unit quaternion (x, y, z, w).
        This follows the ROS/REP-103 convention where the scalar component 'w' is last.
        
        This rotation is multiplied by the current camera orientation.
        The input is not verified!
        
        Args:
            qx: quaternion x (vector part)
            qy: quaternion y (vector part)
            qz: quaternion z (vector part)
            qw: quaternion w (scalar part)
        
        Raises:
            Exception: on internal errors
        """
        status_ = libhebi_charts.hebi_charts_Camera_applyRotation(self.ptr, qx, qy, qz, qw)
        if status_ != 0:
            raise RuntimeError(f'Encountered error in Camera::apply_rotation ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')

    def set_distance(self, distance_in_meters: float) -> None:
        """Sets the distance (zoom) from the camera to the pivot center. Out of range values are clamped.
        
        Args:
            distance_in_meters: distance [m]
        
        Raises:
            Exception: on internal errors
        """
        status_ = libhebi_charts.hebi_charts_Camera_setDistance(self.ptr, distance_in_meters)
        if status_ != 0:
            raise RuntimeError(f'Encountered error in Camera::set_distance ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')

    def set_pan(self, x: float, y: float, z: float) -> None:
        """Sets the 3D pan offset (panning) of the camera.
        This slides the entire scene relative to the camera view.
        
        To center the camera on a specific object, use this to offset
        the world origin.
        
        Args:
            x: position x [m]
            y: position y [m]
            z: position z [m]
        
        Raises:
            Exception: on internal errors
        """
        status_ = libhebi_charts.hebi_charts_Camera_setPan(self.ptr, x, y, z)
        if status_ != 0:
            raise RuntimeError(f'Encountered error in Camera::set_pan ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')

    def set_controls_visible(self, value: bool) -> None:
        """Shows or hides the on-screen navigation UI controls (buttons/overlays).
        
        Args:
            value:
        """
        libhebi_charts.hebi_charts_Camera_setControlsVisible(self.ptr, value)


class Control:
    """Shared control functionality"""

    __slots__ = ('ptr',)

    def __init__(self, ptr: ControlPtr):
        if not isinstance(ptr, ControlPtr):
            raise TypeError(f'Expected ControlPtr, got {type(ptr).__name__}')
        self.ptr = ptr

    def __del__(self):
        if self.ptr is not None:
            libhebi_charts.hebi_charts_Control_release(self.ptr)
        self.ptr = None

    def __repr__(self):
        return f"{self.__class__.__name__}(enabled={self.enabled!r}, label={self.label!r}, tooltip={self.tooltip!r}, visible={self.visible!r})"

    @property
    def enabled(self) -> bool:
        """Gets the current disabled state of this node"""
        return libhebi_charts.hebi_charts_Control_isEnabled(self.ptr) != 0

    @enabled.setter
    def enabled(self, enabled: bool):
        """Sets the disabled state of this node
        
        Args:
            enabled:
        """
        libhebi_charts.hebi_charts_Control_setEnabled(self.ptr, enabled)

    @property
    def label(self) -> str:
        """Gets the name or descriptor of the control in the left column"""
        ptr_ = libhebi_charts.hebi_charts_Control_getLabel(self.ptr)
        return ptr_.decode("utf-8") if ptr_ is not None else ""

    @label.setter
    def label(self, name: str):
        """Sets the name or descriptor of the control in the left column
        
        Args:
            name:
        """
        if name is not None and not isinstance(name, c_char_p):
            name = c_char_p(name.encode('utf-8') if isinstance(name, str) else name)
        libhebi_charts.hebi_charts_Control_setLabel(self.ptr, name)

    @property
    def tooltip(self) -> str:
        """Gets the tooltip"""
        ptr_ = libhebi_charts.hebi_charts_Control_getTooltip(self.ptr)
        return ptr_.decode("utf-8") if ptr_ is not None else ""

    @tooltip.setter
    def tooltip(self, tooltip: str):
        """Sets the tooltip
        
        Args:
            tooltip:
        """
        if tooltip is not None and not isinstance(tooltip, c_char_p):
            tooltip = c_char_p(tooltip.encode('utf-8') if isinstance(tooltip, str) else tooltip)
        libhebi_charts.hebi_charts_Control_setTooltip(self.ptr, tooltip)

    @property
    def visible(self) -> bool:
        """Gets the current visibility state of this node"""
        return libhebi_charts.hebi_charts_Control_isVisible(self.ptr) != 0

    @visible.setter
    def visible(self, visible: bool):
        """Sets the visibility of this node
        
        Args:
            visible:
        """
        libhebi_charts.hebi_charts_Control_setVisible(self.ptr, visible)


class Button(Control):
    """A button control"""

    __slots__ = ('ref_Button',)

    def __init__(self, ref_Button: ButtonPtr):
        if not isinstance(ref_Button, ButtonPtr):
            raise TypeError(f'Expected ButtonPtr, got {type(ref_Button).__name__}')
        self.ref_Button = ref_Button
        super().__init__(libhebi_charts.hebi_charts_Button_to_Control(ref_Button))

    def __del__(self):
        super().__del__()
        self.ref_Button = None

    def __repr__(self):
        return f"{self.__class__.__name__}(text={self.text!r}, enabled={self.enabled!r}, label={self.label!r}, tooltip={self.tooltip!r}, visible={self.visible!r})"

    @property
    def text(self) -> str:
        """Gets the text of this button"""
        ptr_ = libhebi_charts.hebi_charts_Button_getText(self.ref_Button)
        return ptr_.decode("utf-8") if ptr_ is not None else ""

    @text.setter
    def text(self, text: str):
        """Sets the text of this button
        
        Args:
            text:
        """
        if text is not None and not isinstance(text, c_char_p):
            text = c_char_p(text.encode('utf-8') if isinstance(text, str) else text)
        libhebi_charts.hebi_charts_Button_setText(self.ref_Button, text)

    def is_pressed(self) -> bool:
        """Returns the current pressed state"""
        return libhebi_charts.hebi_charts_Button_isPressed(self.ref_Button) != 0

    def was_pressed(self) -> bool:
        """Returns whether the button was pressed at least once since the last call and
        resets the flag. Toggling visibility or enabled states also reset the flag.
        """
        return libhebi_charts.hebi_charts_Button_wasPressed(self.ref_Button) != 0


class Dropdown(Control):
    """A dropdown selection control for switching between discrete modes"""

    __slots__ = ('ref_Dropdown',)

    def __init__(self, ref_Dropdown: DropdownPtr):
        if not isinstance(ref_Dropdown, DropdownPtr):
            raise TypeError(f'Expected DropdownPtr, got {type(ref_Dropdown).__name__}')
        self.ref_Dropdown = ref_Dropdown
        super().__init__(libhebi_charts.hebi_charts_Dropdown_to_Control(ref_Dropdown))

    def __del__(self):
        super().__del__()
        self.ref_Dropdown = None

    def __repr__(self):
        return f"{self.__class__.__name__}(selected_index={self.selected_index!r}, enabled={self.enabled!r}, label={self.label!r}, tooltip={self.tooltip!r}, visible={self.visible!r})"

    @property
    def options(self):
        raise AttributeError("Property 'options' is write-only")

    @options.setter
    def options(self, options: Sequence[str]):
        """Sets the list of available options
        
        Args:
            options: 
            count:
        """
        count = 0 if options is None else len(options)
        if options is not None and not isinstance(options, POINTER(c_char_p)):
            encoded_list = [s.encode('utf-8') if isinstance(s, str) else s for s in options]
            options = (c_char_p * len(encoded_list))(*encoded_list)
        libhebi_charts.hebi_charts_Dropdown_setOptions(self.ref_Dropdown, options, count)

    @property
    def selected_index(self) -> int:
        """Returns the currently selected index"""
        return libhebi_charts.hebi_charts_Dropdown_getSelectedIndex(self.ref_Dropdown)

    @selected_index.setter
    def selected_index(self, index: int):
        """Sets the selected index
        
        Args:
            index:
        """
        libhebi_charts.hebi_charts_Dropdown_setSelectedIndex(self.ref_Dropdown, index)

    def add_option(self, option: str) -> None:
        """Adds an option to the list
        
        Args:
            option:
        """
        if option is not None and not isinstance(option, c_char_p):
            option = c_char_p(option.encode('utf-8') if isinstance(option, str) else option)
        libhebi_charts.hebi_charts_Dropdown_addOption(self.ref_Dropdown, option)

    def was_changed(self) -> bool:
        """Returns whether the control value has changed since the last call and
        resets the flag. Toggling visibility or enabled states also reset the flag.
        """
        return libhebi_charts.hebi_charts_Dropdown_wasChanged(self.ref_Dropdown) != 0


class Label(Control):
    """A control that displays some text or value"""

    __slots__ = ('ref_Label',)

    def __init__(self, ref_Label: LabelPtr):
        if not isinstance(ref_Label, LabelPtr):
            raise TypeError(f'Expected LabelPtr, got {type(ref_Label).__name__}')
        self.ref_Label = ref_Label
        super().__init__(libhebi_charts.hebi_charts_Label_to_Control(ref_Label))

    def __del__(self):
        super().__del__()
        self.ref_Label = None

    def __repr__(self):
        return f"{self.__class__.__name__}(text={self.text!r}, value={self.value!r}, enabled={self.enabled!r}, label={self.label!r}, tooltip={self.tooltip!r}, visible={self.visible!r})"

    @property
    def text(self) -> str:
        """Gets the text of this status label"""
        ptr_ = libhebi_charts.hebi_charts_Label_getText(self.ref_Label)
        return ptr_.decode("utf-8") if ptr_ is not None else ""

    @text.setter
    def text(self, text: str):
        """Sets the text of this status label
        
        Args:
            text:
        """
        if text is not None and not isinstance(text, c_char_p):
            text = c_char_p(text.encode('utf-8') if isinstance(text, str) else text)
        libhebi_charts.hebi_charts_Label_setText(self.ref_Label, text)

    @property
    def value(self) -> float:
        """Gets the current numeric value (may be NaN if text was set last)"""
        return libhebi_charts.hebi_charts_Label_getValue(self.ref_Label)

    @value.setter
    def value(self, value: float):
        """Sets the numeric value of this label using a standard engineering format
        
        Args:
            value:
        """
        libhebi_charts.hebi_charts_Label_setValue(self.ref_Label, value)


class Slider(Control):
    """A slider control for numeric input"""

    __slots__ = ('ref_Slider',)

    def __init__(self, ref_Slider: SliderPtr):
        if not isinstance(ref_Slider, SliderPtr):
            raise TypeError(f'Expected SliderPtr, got {type(ref_Slider).__name__}')
        self.ref_Slider = ref_Slider
        super().__init__(libhebi_charts.hebi_charts_Slider_to_Control(ref_Slider))

    def __del__(self):
        super().__del__()
        self.ref_Slider = None

    def __repr__(self):
        return f"{self.__class__.__name__}(max={self.max!r}, min={self.min!r}, value={self.value!r}, enabled={self.enabled!r}, label={self.label!r}, tooltip={self.tooltip!r}, visible={self.visible!r})"

    @property
    def limits(self):
        raise AttributeError("Property 'limits' is write-only")

    @limits.setter
    def limits(self, limits: Tuple[float, float]):
        """Sets the possible range of the slider [min, max]
        
        Args:
            min: 
            max:
        """
        min, max = limits
        libhebi_charts.hebi_charts_Slider_setLimits(self.ref_Slider, min, max)

    @property
    def max(self) -> float:
        """Gets the maximum value of the slider range"""
        return libhebi_charts.hebi_charts_Slider_getMax(self.ref_Slider)

    @max.setter
    def max(self, max: float):
        """Sets the maximum value of the slider range. Must be greater than min.
        
        Args:
            max:
        """
        libhebi_charts.hebi_charts_Slider_setMax(self.ref_Slider, max)

    @property
    def min(self) -> float:
        """Gets the minimum value of the slider range"""
        return libhebi_charts.hebi_charts_Slider_getMin(self.ref_Slider)

    @min.setter
    def min(self, min: float):
        """Sets the minimum value of the slider range. Must be less than max.
        
        Args:
            min:
        """
        libhebi_charts.hebi_charts_Slider_setMin(self.ref_Slider, min)

    @property
    def value(self) -> float:
        """Gets the current value of the slider"""
        return libhebi_charts.hebi_charts_Slider_getValue(self.ref_Slider)

    @value.setter
    def value(self, value: float):
        """Sets the value of this slider
        
        Args:
            value:
        """
        libhebi_charts.hebi_charts_Slider_setValue(self.ref_Slider, value)

    def was_changed(self) -> bool:
        """Returns whether the control value has changed since the last call and
        resets the flag. Toggling visibility or enabled states also reset the flag.
        """
        return libhebi_charts.hebi_charts_Slider_wasChanged(self.ref_Slider) != 0


class Toggle(Control):
    """A boolean toggle switch for on/off states"""

    __slots__ = ('ref_Toggle',)

    def __init__(self, ref_Toggle: TogglePtr):
        if not isinstance(ref_Toggle, TogglePtr):
            raise TypeError(f'Expected TogglePtr, got {type(ref_Toggle).__name__}')
        self.ref_Toggle = ref_Toggle
        super().__init__(libhebi_charts.hebi_charts_Toggle_to_Control(ref_Toggle))

    def __del__(self):
        super().__del__()
        self.ref_Toggle = None

    def __repr__(self):
        return f"{self.__class__.__name__}(selected={self.selected!r}, enabled={self.enabled!r}, label={self.label!r}, tooltip={self.tooltip!r}, visible={self.visible!r})"

    @property
    def selected(self) -> bool:
        """Returns the current toggle state"""
        return libhebi_charts.hebi_charts_Toggle_isSelected(self.ref_Toggle) != 0

    @selected.setter
    def selected(self, selected: bool):
        """Sets the toggle state
        
        Args:
            selected:
        """
        libhebi_charts.hebi_charts_Toggle_setSelected(self.ref_Toggle, selected)

    def was_changed(self) -> bool:
        """Returns whether the control value has changed since the last call and
        resets the flag. Toggling visibility or enabled states also reset the flag.
        """
        return libhebi_charts.hebi_charts_Toggle_wasChanged(self.ref_Toggle) != 0


class ControlPanel:
    """A panel containing a list of interactive controls like buttons, sliders, and labels"""

    __slots__ = ('ptr',)

    def __init__(self, ptr: ControlPanelPtr):
        if not isinstance(ptr, ControlPanelPtr):
            raise TypeError(f'Expected ControlPanelPtr, got {type(ptr).__name__}')
        self.ptr = ptr

    def __del__(self):
        if self.ptr is not None:
            libhebi_charts.hebi_charts_ControlPanel_release(self.ptr)
        self.ptr = None

    def __repr__(self):
        return f"{self.__class__.__name__}(title={self.title!r}, width={self.width!r})"

    @property
    def title(self) -> str:
        """Gets the title"""
        ptr_ = libhebi_charts.hebi_charts_ControlPanel_getTitle(self.ptr)
        return ptr_.decode("utf-8") if ptr_ is not None else ""

    @title.setter
    def title(self, title: str):
        """Sets the title
        
        Args:
            title:
        """
        if title is not None and not isinstance(title, c_char_p):
            title = c_char_p(title.encode('utf-8') if isinstance(title, str) else title)
        libhebi_charts.hebi_charts_ControlPanel_setTitle(self.ptr, title)

    @property
    def width(self) -> float:
        """Gets the current width"""
        return libhebi_charts.hebi_charts_ControlPanel_getWidth(self.ptr)

    @width.setter
    def width(self, width: float):
        """Sets the desired width
        
        Args:
            width:
        """
        libhebi_charts.hebi_charts_ControlPanel_setWidth(self.ptr, width)

    def add_section(self, title: str) -> None:
        """Starts a new section with the given header
        
        Args:
            title: 
        
        Raises:
            Exception: on internal errors
        """
        if title is not None and not isinstance(title, c_char_p):
            title = c_char_p(title.encode('utf-8') if isinstance(title, str) else title)
        status_ = libhebi_charts.hebi_charts_ControlPanel_addSection(self.ptr, title)
        if status_ != 0:
            raise RuntimeError(f'Encountered error in ControlPanel::add_section ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')

    def add_label(self, *, text: str = None, value: float = None, enabled: bool = None, label: str = None, tooltip: str = None, visible: bool = None) -> Label:
        """Adds a label for displaying text to the control panel
        
        Raises:
            Exception: on internal errors
        """
        ptr_ = libhebi_charts.hebi_charts_ControlPanel_addLabel(self.ptr)
        if not ptr_:
            raise RuntimeError(f'Failed to create Label in ControlPanel::add_label ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')
        obj_ = Label(ptr_)
        if text is not None: obj_.text = text
        if value is not None: obj_.value = value
        if enabled is not None: obj_.enabled = enabled
        if label is not None: obj_.label = label
        if tooltip is not None: obj_.tooltip = tooltip
        if visible is not None: obj_.visible = visible
        return obj_

    def add_button(self, *, text: str = None, enabled: bool = None, label: str = None, tooltip: str = None, visible: bool = None) -> Button:
        """Adds a button to the control panel
        
        Raises:
            Exception: on internal errors
        """
        ptr_ = libhebi_charts.hebi_charts_ControlPanel_addButton(self.ptr)
        if not ptr_:
            raise RuntimeError(f'Failed to create Button in ControlPanel::add_button ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')
        obj_ = Button(ptr_)
        if text is not None: obj_.text = text
        if enabled is not None: obj_.enabled = enabled
        if label is not None: obj_.label = label
        if tooltip is not None: obj_.tooltip = tooltip
        if visible is not None: obj_.visible = visible
        return obj_

    def add_start_button(self, *, text: str = None, enabled: bool = None, label: str = None, tooltip: str = None, visible: bool = None) -> Button:
        """Adds a 'start'-style button to the control panel
        
        Raises:
            Exception: on internal errors
        """
        ptr_ = libhebi_charts.hebi_charts_ControlPanel_addStartButton(self.ptr)
        if not ptr_:
            raise RuntimeError(f'Failed to create Button in ControlPanel::add_start_button ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')
        obj_ = Button(ptr_)
        if text is not None: obj_.text = text
        if enabled is not None: obj_.enabled = enabled
        if label is not None: obj_.label = label
        if tooltip is not None: obj_.tooltip = tooltip
        if visible is not None: obj_.visible = visible
        return obj_

    def add_stop_button(self, *, text: str = None, enabled: bool = None, label: str = None, tooltip: str = None, visible: bool = None) -> Button:
        """Adds a 'stop'-style (red) button to the control panel
        
        Raises:
            Exception: on internal errors
        """
        ptr_ = libhebi_charts.hebi_charts_ControlPanel_addStopButton(self.ptr)
        if not ptr_:
            raise RuntimeError(f'Failed to create Button in ControlPanel::add_stop_button ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')
        obj_ = Button(ptr_)
        if text is not None: obj_.text = text
        if enabled is not None: obj_.enabled = enabled
        if label is not None: obj_.label = label
        if tooltip is not None: obj_.tooltip = tooltip
        if visible is not None: obj_.visible = visible
        return obj_

    def add_slider(self, *, limits: Tuple[float, float] = None, max: float = None, min: float = None, value: float = None, enabled: bool = None, label: str = None, tooltip: str = None, visible: bool = None) -> Slider:
        """Adds a slider to the control panel
        
        Raises:
            Exception: on internal errors
        """
        ptr_ = libhebi_charts.hebi_charts_ControlPanel_addSlider(self.ptr)
        if not ptr_:
            raise RuntimeError(f'Failed to create Slider in ControlPanel::add_slider ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')
        obj_ = Slider(ptr_)
        if limits is not None: obj_.limits = limits
        if max is not None: obj_.max = max
        if min is not None: obj_.min = min
        if value is not None: obj_.value = value
        if enabled is not None: obj_.enabled = enabled
        if label is not None: obj_.label = label
        if tooltip is not None: obj_.tooltip = tooltip
        if visible is not None: obj_.visible = visible
        return obj_

    def add_toggle(self, *, selected: bool = None, enabled: bool = None, label: str = None, tooltip: str = None, visible: bool = None) -> Toggle:
        """Adds a toggle to the control panel
        
        Raises:
            Exception: on internal errors
        """
        ptr_ = libhebi_charts.hebi_charts_ControlPanel_addToggle(self.ptr)
        if not ptr_:
            raise RuntimeError(f'Failed to create Toggle in ControlPanel::add_toggle ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')
        obj_ = Toggle(ptr_)
        if selected is not None: obj_.selected = selected
        if enabled is not None: obj_.enabled = enabled
        if label is not None: obj_.label = label
        if tooltip is not None: obj_.tooltip = tooltip
        if visible is not None: obj_.visible = visible
        return obj_

    def add_dropdown(self, *, options: Sequence[str] = None, selected_index: int = None, enabled: bool = None, label: str = None, tooltip: str = None, visible: bool = None) -> Dropdown:
        """Adds a dropdown choice selector to the control panel
        
        Raises:
            Exception: on internal errors
        """
        ptr_ = libhebi_charts.hebi_charts_ControlPanel_addDropdown(self.ptr)
        if not ptr_:
            raise RuntimeError(f'Failed to create Dropdown in ControlPanel::add_dropdown ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')
        obj_ = Dropdown(ptr_)
        if options is not None: obj_.options = options
        if selected_index is not None: obj_.selected_index = selected_index
        if enabled is not None: obj_.enabled = enabled
        if label is not None: obj_.label = label
        if tooltip is not None: obj_.tooltip = tooltip
        if visible is not None: obj_.visible = visible
        return obj_


class Cursor:
    """A vertical or horizontal cursor to measure or mark an axis value. Can be draggable."""

    __slots__ = ('ptr',)

    def __init__(self, ptr: CursorPtr):
        if not isinstance(ptr, CursorPtr):
            raise TypeError(f'Expected CursorPtr, got {type(ptr).__name__}')
        self.ptr = ptr

    def __del__(self):
        if self.ptr is not None:
            libhebi_charts.hebi_charts_Cursor_release(self.ptr)
        self.ptr = None

    def __repr__(self):
        return f"{self.__class__.__name__}(editable={self.editable!r}, label={self.label!r}, value={self.value!r}, visible={self.visible!r})"

    @property
    def editable(self) -> bool:
        """Gets the editable state of the indicator"""
        return libhebi_charts.hebi_charts_Cursor_isEditable(self.ptr) != 0

    @editable.setter
    def editable(self, editable: bool):
        """Editable indicators can be dragged around by users
        
        Args:
            editable:
        """
        libhebi_charts.hebi_charts_Cursor_setEditable(self.ptr, editable)

    @property
    def label(self) -> str:
        """Gets the indicator label"""
        ptr_ = libhebi_charts.hebi_charts_Cursor_getLabel(self.ptr)
        return ptr_.decode("utf-8") if ptr_ is not None else ""

    @label.setter
    def label(self, label: str):
        """Sets the indicator label. Empty or null hides the label.
        
        Args:
            label:
        """
        if label is not None and not isinstance(label, c_char_p):
            label = c_char_p(label.encode('utf-8') if isinstance(label, str) else label)
        libhebi_charts.hebi_charts_Cursor_setLabel(self.ptr, label)

    @property
    def value(self) -> float:
        """Gets the current value"""
        return libhebi_charts.hebi_charts_Cursor_getValue(self.ptr)

    @value.setter
    def value(self, value: float):
        """Sets the indicated value. NaN hides the indicator.
        
        Args:
            value:
        """
        libhebi_charts.hebi_charts_Cursor_setValue(self.ptr, value)

    @property
    def visible(self) -> bool:
        """Gets the visibility in the chart"""
        return libhebi_charts.hebi_charts_Cursor_isVisible(self.ptr) != 0

    @visible.setter
    def visible(self, visible: bool):
        """Sets the visibility in the chart
        
        Args:
            visible:
        """
        libhebi_charts.hebi_charts_Cursor_setVisible(self.ptr, visible)


class FxmlView:
    """A completely customizable view that is defined by FXML."""

    __slots__ = ('ptr',)

    def __init__(self, ptr: FxmlViewPtr):
        if not isinstance(ptr, FxmlViewPtr):
            raise TypeError(f'Expected FxmlViewPtr, got {type(ptr).__name__}')
        self.ptr = ptr

    def __del__(self):
        if self.ptr is not None:
            libhebi_charts.hebi_charts_FxmlView_release(self.ptr)
        self.ptr = None

    def __repr__(self):
        return f"{self.__class__.__name__}(auto_reload={self.auto_reload!r}, source={self.source!r})"

    @property
    def auto_reload(self) -> bool:
        """Gets the auto-reload state"""
        return libhebi_charts.hebi_charts_FxmlView_isAutoReload(self.ptr) != 0

    @auto_reload.setter
    def auto_reload(self, enable_auto_reload: bool):
        """Sets the auto-reload state
        
        Args:
            enable_auto_reload:
        """
        libhebi_charts.hebi_charts_FxmlView_setAutoReload(self.ptr, enable_auto_reload)

    @property
    def source(self) -> str:
        """Gets the FXML source location"""
        ptr_ = libhebi_charts.hebi_charts_FxmlView_getSource(self.ptr)
        return ptr_.decode("utf-8") if ptr_ is not None else ""

    @source.setter
    def source(self, path_or_url: str):
        """Sets the file path or web-url to a .fxml file
        
        Args:
            path_or_url: path or url to an FXML file
        
        Raises:
            Exception: on internal errors
        """
        if path_or_url is not None and not isinstance(path_or_url, c_char_p):
            path_or_url = c_char_p(path_or_url.encode('utf-8') if isinstance(path_or_url, str) else path_or_url)
        status_ = libhebi_charts.hebi_charts_FxmlView_setSource(self.ptr, path_or_url)
        if status_ != 0:
            raise RuntimeError(f'Encountered error in FxmlView::source ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')

    def add_line_chart(self, fx_id: str, *, title: str = None, x_assume_sorted: bool = None, xauto: bool = None, xlabel: str = None, xlim: Tuple[float, float] = None, xmax: float = None, xmin: float = None, xunit: str = None, yauto: bool = None, ylabel: str = None, ylim: Tuple[float, float] = None, ymax: float = None, ymin: float = None, yunit: str = None) -> LineChart:
        """Creates a 2d line chart with the given size
        
        Args:
            fx_id: 
        
        Raises:
            Exception: on internal errors
        """
        if fx_id is not None and not isinstance(fx_id, c_char_p):
            fx_id = c_char_p(fx_id.encode('utf-8') if isinstance(fx_id, str) else fx_id)
        ptr_ = libhebi_charts.hebi_charts_FxmlView_addLineChart(self.ptr, fx_id)
        if not ptr_:
            raise RuntimeError(f'Failed to create LineChart in FxmlView::add_line_chart ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')
        obj_ = LineChart(ptr_)
        if title is not None: obj_.title = title
        if x_assume_sorted is not None: obj_.x_assume_sorted = x_assume_sorted
        if xauto is not None: obj_.xauto = xauto
        if xlabel is not None: obj_.xlabel = xlabel
        if xlim is not None: obj_.xlim = xlim
        if xmax is not None: obj_.xmax = xmax
        if xmin is not None: obj_.xmin = xmin
        if xunit is not None: obj_.xunit = xunit
        if yauto is not None: obj_.yauto = yauto
        if ylabel is not None: obj_.ylabel = ylabel
        if ylim is not None: obj_.ylim = ylim
        if ymax is not None: obj_.ymax = ymax
        if ymin is not None: obj_.ymin = ymin
        if yunit is not None: obj_.yunit = yunit
        return obj_

    def add_scope(self, fx_id: str, *, title: str = None, x_assume_sorted: bool = None, xauto: bool = None, xlabel: str = None, xlim: Tuple[float, float] = None, xmax: float = None, xmin: float = None, xunit: str = None, yauto: bool = None, ylabel: str = None, ylim: Tuple[float, float] = None, ymax: float = None, ymin: float = None, yunit: str = None) -> LineChart:
        """Creates a line chart with a pre-set time axis in [s]
        
        Args:
            fx_id: 
        
        Raises:
            Exception: on internal errors
        """
        if fx_id is not None and not isinstance(fx_id, c_char_p):
            fx_id = c_char_p(fx_id.encode('utf-8') if isinstance(fx_id, str) else fx_id)
        ptr_ = libhebi_charts.hebi_charts_FxmlView_addScope(self.ptr, fx_id)
        if not ptr_:
            raise RuntimeError(f'Failed to create LineChart in FxmlView::add_scope ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')
        obj_ = LineChart(ptr_)
        if title is not None: obj_.title = title
        if x_assume_sorted is not None: obj_.x_assume_sorted = x_assume_sorted
        if xauto is not None: obj_.xauto = xauto
        if xlabel is not None: obj_.xlabel = xlabel
        if xlim is not None: obj_.xlim = xlim
        if xmax is not None: obj_.xmax = xmax
        if xmin is not None: obj_.xmin = xmin
        if xunit is not None: obj_.xunit = xunit
        if yauto is not None: obj_.yauto = yauto
        if ylabel is not None: obj_.ylabel = ylabel
        if ylim is not None: obj_.ylim = ylim
        if ymax is not None: obj_.ymax = ymax
        if ymin is not None: obj_.ymin = ymin
        if yunit is not None: obj_.yunit = yunit
        return obj_

    def add_latency_chart(self, fx_id: str, *, title: str = None, x_assume_sorted: bool = None, xauto: bool = None, xlabel: str = None, xlim: Tuple[float, float] = None, xmax: float = None, xmin: float = None, xunit: str = None, yauto: bool = None, ylabel: str = None, ylim: Tuple[float, float] = None, ymax: float = None, ymin: float = None, yunit: str = None) -> LatencyChart:
        """Creates a latency chart for displaying latency measurements [s]
        
        Args:
            fx_id: 
        
        Raises:
            Exception: on internal errors
        """
        if fx_id is not None and not isinstance(fx_id, c_char_p):
            fx_id = c_char_p(fx_id.encode('utf-8') if isinstance(fx_id, str) else fx_id)
        ptr_ = libhebi_charts.hebi_charts_FxmlView_addLatencyChart(self.ptr, fx_id)
        if not ptr_:
            raise RuntimeError(f'Failed to create LatencyChart in FxmlView::add_latency_chart ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')
        obj_ = LatencyChart(ptr_)
        if title is not None: obj_.title = title
        if x_assume_sorted is not None: obj_.x_assume_sorted = x_assume_sorted
        if xauto is not None: obj_.xauto = xauto
        if xlabel is not None: obj_.xlabel = xlabel
        if xlim is not None: obj_.xlim = xlim
        if xmax is not None: obj_.xmax = xmax
        if xmin is not None: obj_.xmin = xmin
        if xunit is not None: obj_.xunit = xunit
        if yauto is not None: obj_.yauto = yauto
        if ylabel is not None: obj_.ylabel = ylabel
        if ylim is not None: obj_.ylim = ylim
        if ymax is not None: obj_.ymax = ymax
        if ymin is not None: obj_.ymin = ymin
        if yunit is not None: obj_.yunit = yunit
        return obj_

    def add_scene3d(self, fx_id: str, *, grid_bounds: Tuple[float, float, float, float, float, float] = None, grid_max_x: float = None, grid_max_y: float = None, grid_max_z: float = None, grid_min_x: float = None, grid_min_y: float = None, grid_min_z: float = None, grid_step: float = None) -> Scene3d:
        """Creates a 3d chart with the given size
        
        Args:
            fx_id: 
        
        Raises:
            Exception: on internal errors
        """
        if fx_id is not None and not isinstance(fx_id, c_char_p):
            fx_id = c_char_p(fx_id.encode('utf-8') if isinstance(fx_id, str) else fx_id)
        ptr_ = libhebi_charts.hebi_charts_FxmlView_addScene3d(self.ptr, fx_id)
        if not ptr_:
            raise RuntimeError(f'Failed to create Scene3d in FxmlView::add_scene3d ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')
        obj_ = Scene3d(ptr_)
        if grid_bounds is not None: obj_.grid_bounds = grid_bounds
        if grid_max_x is not None: obj_.grid_max_x = grid_max_x
        if grid_max_y is not None: obj_.grid_max_y = grid_max_y
        if grid_max_z is not None: obj_.grid_max_z = grid_max_z
        if grid_min_x is not None: obj_.grid_min_x = grid_min_x
        if grid_min_y is not None: obj_.grid_min_y = grid_min_y
        if grid_min_z is not None: obj_.grid_min_z = grid_min_z
        if grid_step is not None: obj_.grid_step = grid_step
        return obj_

    def add_stream_view(self, file: str, fx_id: str) -> StreamView:
        """Shows a shared-memory stream generated by hebi-video tools
        
        Args:
            file: 
            fx_id: 
        
        Raises:
            Exception: on internal errors
        """
        if file is not None and not isinstance(file, c_char_p):
            file = c_char_p(file.encode('utf-8') if isinstance(file, str) else file)
        if fx_id is not None and not isinstance(fx_id, c_char_p):
            fx_id = c_char_p(fx_id.encode('utf-8') if isinstance(fx_id, str) else fx_id)
        ptr_ = libhebi_charts.hebi_charts_FxmlView_addStreamView(self.ptr, file, fx_id)
        if not ptr_:
            raise RuntimeError(f'Failed to create StreamView in FxmlView::add_stream_view ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')
        return StreamView(ptr_)

    def add_fxml_view(self, fx_id: str, *, auto_reload: bool = None, source: str = None) -> FxmlView:
        """Shows a panel for interactive controls
        
        Args:
            fx_id: 
        
        Raises:
            Exception: on internal errors
        """
        if fx_id is not None and not isinstance(fx_id, c_char_p):
            fx_id = c_char_p(fx_id.encode('utf-8') if isinstance(fx_id, str) else fx_id)
        ptr_ = libhebi_charts.hebi_charts_FxmlView_addFxmlView(self.ptr, fx_id)
        if not ptr_:
            raise RuntimeError(f'Failed to create FxmlView in FxmlView::add_fxml_view ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')
        obj_ = FxmlView(ptr_)
        if auto_reload is not None: obj_.auto_reload = auto_reload
        if source is not None: obj_.source = source
        return obj_


class GridWindow:
    """Represents a window containing an equally sized row/col grid"""

    __slots__ = ('ptr',)

    def __init__(self, rows: int = 1, cols: int = 1, *, full_screen: bool = None, height: int = None, keep_open: bool = None, location: Tuple[int, int] = None, size: Tuple[int, int] = None, title: str = None, width: int = None, x: int = None, y: int = None) -> None:
        """Creates a grid of equally sized rows and columns
        
        Args:
            rows: 
            cols: 
        
        Raises:
            Exception: on internal errors
        """
        ptr_ = libhebi_charts.hebi_charts_GridWindow_create(rows, cols)
        if not ptr_:
            raise RuntimeError(f'Failed to create GridWindow in GridWindow::__init__ ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')
        self.ptr = ptr_
        if full_screen is not None: self.full_screen = full_screen
        if height is not None: self.height = height
        if keep_open is not None: self.keep_open = keep_open
        if location is not None: self.location = location
        if size is not None: self.size = size
        if title is not None: self.title = title
        if width is not None: self.width = width
        if x is not None: self.x = x
        if y is not None: self.y = y

    def __del__(self):
        if self.ptr is not None:
            libhebi_charts.hebi_charts_GridWindow_release(self.ptr)
        self.ptr = None

    def __repr__(self):
        return f"{self.__class__.__name__}(full_screen={self.full_screen!r}, height={self.height!r}, keep_open={self.keep_open!r}, title={self.title!r}, width={self.width!r}, x={self.x!r}, y={self.y!r})"

    @property
    def full_screen(self) -> bool:
        """Gets the current full screen mode"""
        return libhebi_charts.hebi_charts_GridWindow_isFullScreen(self.ptr) != 0

    @full_screen.setter
    def full_screen(self, full_screen: bool):
        """Enters or exits fullscreen mode. Does not apply to off screen windows
        
        Args:
            full_screen:
        """
        libhebi_charts.hebi_charts_GridWindow_setFullScreen(self.ptr, full_screen)

    @property
    def height(self) -> int:
        """Gets the content height in display points"""
        return libhebi_charts.hebi_charts_GridWindow_getHeight(self.ptr)

    @height.setter
    def height(self, height: int):
        """Sets the content height in display points
        
        Args:
            height:
        """
        libhebi_charts.hebi_charts_GridWindow_setHeight(self.ptr, height)

    @property
    def keep_open(self) -> bool:
        """Gets the current keepOpen state"""
        return libhebi_charts.hebi_charts_GridWindow_getKeepOpen(self.ptr) != 0

    @keep_open.setter
    def keep_open(self, keep_open: bool):
        """Keeps the window open after the destructor gets called
        
        Args:
            keep_open:
        """
        libhebi_charts.hebi_charts_GridWindow_setKeepOpen(self.ptr, keep_open)

    @property
    def location(self):
        raise AttributeError("Property 'location' is write-only")

    @location.setter
    def location(self, location: Tuple[int, int]):
        """Sets the window's screen location in pixels (x, y)
        
        Args:
            x_offset: 
            y_offset:
        """
        x_offset, y_offset = location
        libhebi_charts.hebi_charts_GridWindow_setLocation(self.ptr, x_offset, y_offset)

    @property
    def size(self):
        raise AttributeError("Property 'size' is write-only")

    @size.setter
    def size(self, size: Tuple[int, int]):
        """Sets the content size in display points (width, height), excluding the title bar.
        
        Args:
            width: 
            height:
        """
        width, height = size
        libhebi_charts.hebi_charts_GridWindow_setSize(self.ptr, width, height)

    @property
    def title(self) -> str:
        """Gets the title of the window header bar"""
        ptr_ = libhebi_charts.hebi_charts_GridWindow_getTitle(self.ptr)
        return ptr_.decode("utf-8") if ptr_ is not None else ""

    @title.setter
    def title(self, title: str):
        """Sets the title of the window header bar
        
        Args:
            title:
        """
        if title is not None and not isinstance(title, c_char_p):
            title = c_char_p(title.encode('utf-8') if isinstance(title, str) else title)
        libhebi_charts.hebi_charts_GridWindow_setTitle(self.ptr, title)

    @property
    def width(self) -> int:
        """Gets the content width in display points"""
        return libhebi_charts.hebi_charts_GridWindow_getWidth(self.ptr)

    @width.setter
    def width(self, width: int):
        """Sets the content width in display points
        
        Args:
            width:
        """
        libhebi_charts.hebi_charts_GridWindow_setWidth(self.ptr, width)

    @property
    def x(self) -> int:
        """Gets the window's horizontal screen location in pixels"""
        return libhebi_charts.hebi_charts_GridWindow_getX(self.ptr)

    @x.setter
    def x(self, x: int):
        """Sets the window's horizontal screen location in pixels
        
        Args:
            x:
        """
        libhebi_charts.hebi_charts_GridWindow_setX(self.ptr, x)

    @property
    def y(self) -> int:
        """Gets the window's vertical screen location in pixels"""
        return libhebi_charts.hebi_charts_GridWindow_getY(self.ptr)

    @y.setter
    def y(self, y: int):
        """Sets the window's vertical screen location in pixels
        
        Args:
            y:
        """
        libhebi_charts.hebi_charts_GridWindow_setY(self.ptr, y)

    def add_line_chart(self, row: int = 0, col: int = 0, row_span: int = 1, col_span: int = 1, *, title: str = None, x_assume_sorted: bool = None, xauto: bool = None, xlabel: str = None, xlim: Tuple[float, float] = None, xmax: float = None, xmin: float = None, xunit: str = None, yauto: bool = None, ylabel: str = None, ylim: Tuple[float, float] = None, ymax: float = None, ymin: float = None, yunit: str = None) -> LineChart:
        """Creates a 2d line chart with the given size
        
        Args:
            row: 
            col: 
            row_span: 
            col_span: 
        
        Raises:
            Exception: on internal errors
        """
        ptr_ = libhebi_charts.hebi_charts_GridWindow_addLineChart(self.ptr, row, col, row_span, col_span)
        if not ptr_:
            raise RuntimeError(f'Failed to create LineChart in GridWindow::add_line_chart ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')
        obj_ = LineChart(ptr_)
        if title is not None: obj_.title = title
        if x_assume_sorted is not None: obj_.x_assume_sorted = x_assume_sorted
        if xauto is not None: obj_.xauto = xauto
        if xlabel is not None: obj_.xlabel = xlabel
        if xlim is not None: obj_.xlim = xlim
        if xmax is not None: obj_.xmax = xmax
        if xmin is not None: obj_.xmin = xmin
        if xunit is not None: obj_.xunit = xunit
        if yauto is not None: obj_.yauto = yauto
        if ylabel is not None: obj_.ylabel = ylabel
        if ylim is not None: obj_.ylim = ylim
        if ymax is not None: obj_.ymax = ymax
        if ymin is not None: obj_.ymin = ymin
        if yunit is not None: obj_.yunit = yunit
        return obj_

    def add_scope(self, row: int = 0, col: int = 0, row_span: int = 1, col_span: int = 1, *, title: str = None, x_assume_sorted: bool = None, xauto: bool = None, xlabel: str = None, xlim: Tuple[float, float] = None, xmax: float = None, xmin: float = None, xunit: str = None, yauto: bool = None, ylabel: str = None, ylim: Tuple[float, float] = None, ymax: float = None, ymin: float = None, yunit: str = None) -> LineChart:
        """Creates a line chart with a pre-set time axis in [s]
        
        Args:
            row: 
            col: 
            row_span: 
            col_span: 
        
        Raises:
            Exception: on internal errors
        """
        ptr_ = libhebi_charts.hebi_charts_GridWindow_addScope(self.ptr, row, col, row_span, col_span)
        if not ptr_:
            raise RuntimeError(f'Failed to create LineChart in GridWindow::add_scope ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')
        obj_ = LineChart(ptr_)
        if title is not None: obj_.title = title
        if x_assume_sorted is not None: obj_.x_assume_sorted = x_assume_sorted
        if xauto is not None: obj_.xauto = xauto
        if xlabel is not None: obj_.xlabel = xlabel
        if xlim is not None: obj_.xlim = xlim
        if xmax is not None: obj_.xmax = xmax
        if xmin is not None: obj_.xmin = xmin
        if xunit is not None: obj_.xunit = xunit
        if yauto is not None: obj_.yauto = yauto
        if ylabel is not None: obj_.ylabel = ylabel
        if ylim is not None: obj_.ylim = ylim
        if ymax is not None: obj_.ymax = ymax
        if ymin is not None: obj_.ymin = ymin
        if yunit is not None: obj_.yunit = yunit
        return obj_

    def add_latency_chart(self, row: int = 0, col: int = 0, row_span: int = 1, col_span: int = 1, *, title: str = None, x_assume_sorted: bool = None, xauto: bool = None, xlabel: str = None, xlim: Tuple[float, float] = None, xmax: float = None, xmin: float = None, xunit: str = None, yauto: bool = None, ylabel: str = None, ylim: Tuple[float, float] = None, ymax: float = None, ymin: float = None, yunit: str = None) -> LatencyChart:
        """Creates a latency chart for recording latency measurements in [s]
        
        Args:
            row: 
            col: 
            row_span: 
            col_span: 
        
        Raises:
            Exception: on internal errors
        """
        ptr_ = libhebi_charts.hebi_charts_GridWindow_addLatencyChart(self.ptr, row, col, row_span, col_span)
        if not ptr_:
            raise RuntimeError(f'Failed to create LatencyChart in GridWindow::add_latency_chart ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')
        obj_ = LatencyChart(ptr_)
        if title is not None: obj_.title = title
        if x_assume_sorted is not None: obj_.x_assume_sorted = x_assume_sorted
        if xauto is not None: obj_.xauto = xauto
        if xlabel is not None: obj_.xlabel = xlabel
        if xlim is not None: obj_.xlim = xlim
        if xmax is not None: obj_.xmax = xmax
        if xmin is not None: obj_.xmin = xmin
        if xunit is not None: obj_.xunit = xunit
        if yauto is not None: obj_.yauto = yauto
        if ylabel is not None: obj_.ylabel = ylabel
        if ylim is not None: obj_.ylim = ylim
        if ymax is not None: obj_.ymax = ymax
        if ymin is not None: obj_.ymin = ymin
        if yunit is not None: obj_.yunit = yunit
        return obj_

    def add_scene3d(self, row: int = 0, col: int = 0, row_span: int = 1, col_span: int = 1, *, grid_bounds: Tuple[float, float, float, float, float, float] = None, grid_max_x: float = None, grid_max_y: float = None, grid_max_z: float = None, grid_min_x: float = None, grid_min_y: float = None, grid_min_z: float = None, grid_step: float = None) -> Scene3d:
        """Creates a 3d chart with the given size
        
        Args:
            row: 
            col: 
            row_span: 
            col_span: 
        
        Raises:
            Exception: on internal errors
        """
        ptr_ = libhebi_charts.hebi_charts_GridWindow_addScene3d(self.ptr, row, col, row_span, col_span)
        if not ptr_:
            raise RuntimeError(f'Failed to create Scene3d in GridWindow::add_scene3d ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')
        obj_ = Scene3d(ptr_)
        if grid_bounds is not None: obj_.grid_bounds = grid_bounds
        if grid_max_x is not None: obj_.grid_max_x = grid_max_x
        if grid_max_y is not None: obj_.grid_max_y = grid_max_y
        if grid_max_z is not None: obj_.grid_max_z = grid_max_z
        if grid_min_x is not None: obj_.grid_min_x = grid_min_x
        if grid_min_y is not None: obj_.grid_min_y = grid_min_y
        if grid_min_z is not None: obj_.grid_min_z = grid_min_z
        if grid_step is not None: obj_.grid_step = grid_step
        return obj_

    def add_stream_view(self, file: str, row: int = 0, col: int = 0, row_span: int = 1, col_span: int = 1) -> StreamView:
        """Shows a shared-memory stream generated by hebi-video tools
        
        Args:
            file: 
            row: 
            col: 
            row_span: 
            col_span: 
        
        Raises:
            Exception: on internal errors
        """
        if file is not None and not isinstance(file, c_char_p):
            file = c_char_p(file.encode('utf-8') if isinstance(file, str) else file)
        ptr_ = libhebi_charts.hebi_charts_GridWindow_addStreamView(self.ptr, file, row, col, row_span, col_span)
        if not ptr_:
            raise RuntimeError(f'Failed to create StreamView in GridWindow::add_stream_view ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')
        return StreamView(ptr_)

    def add_fxml_view(self, row: int = 0, col: int = 0, row_span: int = 1, col_span: int = 1, *, auto_reload: bool = None, source: str = None) -> FxmlView:
        """Shows a panel for interactive controls
        
        Args:
            row: 
            col: 
            row_span: 
            col_span: 
        
        Raises:
            Exception: on internal errors
        """
        ptr_ = libhebi_charts.hebi_charts_GridWindow_addFxmlView(self.ptr, row, col, row_span, col_span)
        if not ptr_:
            raise RuntimeError(f'Failed to create FxmlView in GridWindow::add_fxml_view ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')
        obj_ = FxmlView(ptr_)
        if auto_reload is not None: obj_.auto_reload = auto_reload
        if source is not None: obj_.source = source
        return obj_

    def add_stylesheet(self, path_or_url: str, auto_reload: bool = False) -> None:
        """Adds a CSS file to the root scene
        
        Args:
            path_or_url: path to a CSS file
            auto_reload: true adds a hot-reload capability on file change
        
        Raises:
            Exception: on internal errors
        """
        if path_or_url is not None and not isinstance(path_or_url, c_char_p):
            path_or_url = c_char_p(path_or_url.encode('utf-8') if isinstance(path_or_url, str) else path_or_url)
        status_ = libhebi_charts.hebi_charts_GridWindow_addStylesheet(self.ptr, path_or_url, auto_reload)
        if status_ != 0:
            raise RuntimeError(f'Encountered error in GridWindow::add_stylesheet ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')

    def show(self) -> None:
        """Shows the window. May be called multiple times"""
        libhebi_charts.hebi_charts_GridWindow_show(self.ptr)

    def show_off_screen(self) -> None:
        """[EXPERIMENTAL] Shows the window in a hidden utility window. Use this if you encounter issues with headless rendering.
        
        Raises:
            Exception: on internal errors
        """
        status_ = libhebi_charts.hebi_charts_GridWindow_showOffScreen(self.ptr)
        if status_ != 0:
            raise RuntimeError(f'Encountered error in GridWindow::show_off_screen ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')

    def hide(self) -> None:
        """Hides the window. May be called multiple times. Hidden windows are not destroyed"""
        libhebi_charts.hebi_charts_GridWindow_hide(self.ptr)

    def is_showing(self) -> bool:
        """Returns whether the window is currently showing"""
        return libhebi_charts.hebi_charts_GridWindow_isShowing(self.ptr) != 0

    def wait_until_closed(self) -> None:
        """Waits until this window gets closed by the user
        
        Raises:
            Exception: when called from the FX thread
        """
        status_ = libhebi_charts.hebi_charts_GridWindow_waitUntilClosed(self.ptr)
        if status_ != 0:
            raise RuntimeError(f'Encountered error in GridWindow::wait_until_closed ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')

    def get_control_panel(self, *, title: str = None, width: float = None) -> ControlPanel:
        """Returns a fixed-size panel on the side of the window that can be used for interactive controls
        
        Raises:
            Exception: on internal errors
        """
        ptr_ = libhebi_charts.hebi_charts_GridWindow_getControlPanel(self.ptr)
        if not ptr_:
            raise RuntimeError(f'Failed to create ControlPanel in GridWindow::get_control_panel ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')
        obj_ = ControlPanel(ptr_)
        if title is not None: obj_.title = title
        if width is not None: obj_.width = width
        return obj_

    def create_image_stream(self, *, pixel_format: PixelFormat = None, rate_limit: float = None, recorder_threads: int = None) -> ImageStream:
        """[EXPERIMENTAL API]
        Creates an image stream that continuously stores the content as images.
        
        Raises:
            Exception: on internal errors
        """
        ptr_ = libhebi_charts.hebi_charts_GridWindow_createImageStream(self.ptr)
        if not ptr_:
            raise RuntimeError(f'Failed to create ImageStream in GridWindow::create_image_stream ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')
        obj_ = ImageStream(ptr_)
        if pixel_format is not None: obj_.pixel_format = pixel_format
        if rate_limit is not None: obj_.rate_limit = rate_limit
        if recorder_threads is not None: obj_.recorder_threads = recorder_threads
        return obj_

    def dispatch_mouse_event(self, action: int, button: int, down_mask: int, modifiers: int, x: float, y: float) -> None:
        """[EXPERIMENTAL API]
        Injects a mouse event into this window
        
        Args:
            action: 
            button: 
            down_mask: 
            modifiers: 
            x: 
            y:
        """
        libhebi_charts.hebi_charts_GridWindow_dispatchMouseEvent(self.ptr, action, button, down_mask, modifiers, x, y)

    def dispatch_scroll_event(self, x: float, y: float, delta_x: float, delta_y: float, modifiers: int) -> None:
        """[EXPERIMENTAL API]
        Injects a mouse event into this window
        
        Args:
            x: 
            y: 
            delta_x: 
            delta_y: 
            modifiers:
        """
        libhebi_charts.hebi_charts_GridWindow_dispatchScrollEvent(self.ptr, x, y, delta_x, delta_y, modifiers)


class HdrHistogramRecorder:
    """Manages background logging of high-dynamic-range histograms to disk."""

    __slots__ = ('ptr',)

    def __init__(self, *, frequency: float = None, max: float = None, min: float = None, significant_digits: int = None) -> None:
        ptr_ = libhebi_charts.hebi_charts_HdrHistogramRecorder_create()
        if not ptr_:
            raise RuntimeError(f'Failed to create HdrHistogramRecorder in HdrHistogramRecorder::__init__')
        self.ptr = ptr_
        if frequency is not None: self.frequency = frequency
        if max is not None: self.max = max
        if min is not None: self.min = min
        if significant_digits is not None: self.significant_digits = significant_digits

    def __del__(self):
        if self.ptr is not None:
            libhebi_charts.hebi_charts_HdrHistogramRecorder_release(self.ptr)
        self.ptr = None

    def __repr__(self):
        return f"{self.__class__.__name__}(frequency={self.frequency!r}, max={self.max!r}, min={self.min!r}, significant_digits={self.significant_digits!r})"

    @property
    def frequency(self) -> float:
        return libhebi_charts.hebi_charts_HdrHistogramRecorder_getFrequency(self.ptr)

    @frequency.setter
    def frequency(self, frequency: float):
        """Sets the logging frequency (Hz) for background file updates.
        
        Args:
            frequency: hz
        
        Raises:
            Exception: on internal errors
        """
        status_ = libhebi_charts.hebi_charts_HdrHistogramRecorder_setFrequency(self.ptr, frequency)
        if status_ != 0:
            raise RuntimeError(f'Encountered error in HdrHistogramRecorder::frequency')

    @property
    def max(self) -> float:
        return libhebi_charts.hebi_charts_HdrHistogramRecorder_getMax(self.ptr)

    @max.setter
    def max(self, value: float):
        """Sets the highest trackable value [s]. Values above this will throw an error.
        
        Args:
            value: seconds
        
        Raises:
            Exception: on internal errors
        """
        status_ = libhebi_charts.hebi_charts_HdrHistogramRecorder_setMax(self.ptr, value)
        if status_ != 0:
            raise RuntimeError(f'Encountered error in HdrHistogramRecorder::max')

    @property
    def min(self) -> float:
        return libhebi_charts.hebi_charts_HdrHistogramRecorder_getMin(self.ptr)

    @min.setter
    def min(self, value: float):
        """Sets the lowest trackable value [s]. Values below this will be rounded up.
        
        Args:
            value: seconds
        
        Raises:
            Exception: on internal errors
        """
        status_ = libhebi_charts.hebi_charts_HdrHistogramRecorder_setMin(self.ptr, value)
        if status_ != 0:
            raise RuntimeError(f'Encountered error in HdrHistogramRecorder::min')

    @property
    def significant_digits(self) -> int:
        return libhebi_charts.hebi_charts_HdrHistogramRecorder_getSignificantDigits(self.ptr)

    @significant_digits.setter
    def significant_digits(self, significant_digits: int):
        """Sets the number of significant decimal digits to maintain (1-5).
        
        Args:
            significant_digits: 
        
        Raises:
            Exception: on internal errors
        """
        status_ = libhebi_charts.hebi_charts_HdrHistogramRecorder_setSignificantDigits(self.ptr, significant_digits)
        if status_ != 0:
            raise RuntimeError(f'Encountered error in HdrHistogramRecorder::significant_digits')

    def add_trace(self, tag: str) -> HdrHistogramTrace:
        """Creates a new single-writer trace that gets recorded in intervals.
        
        Args:
            tag:
        """
        if tag is not None and not isinstance(tag, c_char_p):
            tag = c_char_p(tag.encode('utf-8') if isinstance(tag, str) else tag)
        ptr_ = libhebi_charts.hebi_charts_HdrHistogramRecorder_addTrace(self.ptr, tag)
        if not ptr_:
            raise RuntimeError(f'Failed to create HdrHistogramTrace in HdrHistogramRecorder::add_trace')
        return HdrHistogramTrace(ptr_)

    def start_recording(self, log_file: str) -> str:
        """Starts the background recording thread. Returns the absolute normalized path.
        
        Args:
            log_file: Path to the .hlog file
        
        Raises:
            Exception: on internal errors
        """
        if log_file is not None and not isinstance(log_file, c_char_p):
            log_file = c_char_p(log_file.encode('utf-8') if isinstance(log_file, str) else log_file)
        ptr_ = libhebi_charts.hebi_charts_HdrHistogramRecorder_startRecording(self.ptr, log_file)
        return ptr_.decode("utf-8") if ptr_ is not None else ""

    def stop_recording(self) -> None:
        """Stops the recording thread."""
        libhebi_charts.hebi_charts_HdrHistogramRecorder_stopRecording(self.ptr)


class HdrHistogramTrace:
    """A wait-free single-writer HdrHistogram record"""

    __slots__ = ('ptr',)

    def __init__(self, ptr: HdrHistogramTracePtr):
        if not isinstance(ptr, HdrHistogramTracePtr):
            raise TypeError(f'Expected HdrHistogramTracePtr, got {type(ptr).__name__}')
        self.ptr = ptr

    def __del__(self):
        if self.ptr is not None:
            libhebi_charts.hebi_charts_HdrHistogramTrace_release(self.ptr)
        self.ptr = None

    def __repr__(self):
        return f"{self.__class__.__name__}(max={self.max!r}, mean={self.mean!r}, min={self.min!r}, std_deviation={self.std_deviation!r}, tag={self.tag!r}, total_count={self.total_count!r})"

    @property
    def max(self) -> float:
        """Maximum recorded value in [s]."""
        return libhebi_charts.hebi_charts_HdrHistogramTrace_getMax(self.ptr)

    @property
    def mean(self) -> float:
        """Arithmetic mean of recorded values in [s]."""
        return libhebi_charts.hebi_charts_HdrHistogramTrace_getMean(self.ptr)

    @property
    def min(self) -> float:
        """Minimum recorded value in [s]."""
        return libhebi_charts.hebi_charts_HdrHistogramTrace_getMin(self.ptr)

    @property
    def std_deviation(self) -> float:
        """Standard deviation of recorded values in [s]."""
        return libhebi_charts.hebi_charts_HdrHistogramTrace_getStdDeviation(self.ptr)

    @property
    def tag(self) -> str:
        """The internal tag"""
        ptr_ = libhebi_charts.hebi_charts_HdrHistogramTrace_getTag(self.ptr)
        return ptr_.decode("utf-8") if ptr_ is not None else ""

    @property
    def total_count(self) -> int:
        """The total number of samples recorded."""
        return libhebi_charts.hebi_charts_HdrHistogramTrace_getTotalCount(self.ptr)

    @classmethod
    def create_local(cls, number_of_significant_digits: int = 2, min_seconds: float = 1e-9, max_seconds: float = 3600) -> HdrHistogramTrace:
        """Creates an unattached and untagged histogram. The local version removes
        all synchronization overhead, but is not thread-safe.
        
        Args:
            number_of_significant_digits: 
            min_seconds: 
            max_seconds:
        """
        ptr_ = libhebi_charts.hebi_charts_HdrHistogramTrace_createLocal(number_of_significant_digits, min_seconds, max_seconds)
        if not ptr_:
            raise RuntimeError(f'Failed to create HdrHistogramTrace in HdrHistogramTrace::create_local')
        return HdrHistogramTrace(ptr_)

    def get_value_at_percentile(self, percentile: float) -> float:
        """Returns the value at a specific percentile (0-100) in [s].
        
        Args:
            percentile:
        """
        return libhebi_charts.hebi_charts_HdrHistogramTrace_getValueAtPercentile(self.ptr, percentile)

    def tic(self) -> None:
        """Sets a timestamp for subsequent toc calls"""
        libhebi_charts.hebi_charts_HdrHistogramTrace_tic(self.ptr)

    def toc(self) -> float:
        """Records the elapsed time since the last tic call. Returns the recorded value in [s]"""
        return libhebi_charts.hebi_charts_HdrHistogramTrace_toc(self.ptr)

    def tic_toc(self) -> float:
        """Calls tic and toc in one call. Returns the recorded value in [s]"""
        return libhebi_charts.hebi_charts_HdrHistogramTrace_ticToc(self.ptr)

    def record_value(self, value: float) -> None:
        """Records a single latency value in seconds. Values outside the
        min/max range are clamped. Returns the recorded value in [s]
        
        Args:
            value: latency in [s]
        """
        libhebi_charts.hebi_charts_HdrHistogramTrace_recordValue(self.ptr, value)

    def record_value_with_count(self, value: float, count: int) -> None:
        """Records a latency value in seconds with a specific occurrence count.
        
        Args:
            value: latency in [s]
            count: occurrence count
        """
        libhebi_charts.hebi_charts_HdrHistogramTrace_recordValueWithCount(self.ptr, value, count)

    def record_value_with_expected_interval(self, value: float, expected_interval_between_samples: float) -> None:
        """Records a value in seconds with Coordinated Omission compensation.
        If the value is larger than the expected interval, additional samples
        are auto-generated to fill the gap.
        
        Args:
            value: latency in [s]
            expected_interval_between_samples: expected interval in [s]
        """
        libhebi_charts.hebi_charts_HdrHistogramTrace_recordValueWithExpectedInterval(self.ptr, value, expected_interval_between_samples)

    def reset(self) -> None:
        """Reset the contents and stats of this histogram"""
        libhebi_charts.hebi_charts_HdrHistogramTrace_reset(self.ptr)

    def to_hgrm_string(self, output_units_per_second: float = 1e6) -> str:
        """A string of the percentile distribution for plotting .hgrm files
        
        Args:
            output_units_per_second: output scale (ms=1e3, us=1e6, ns=1e9
        """
        ptr_ = libhebi_charts.hebi_charts_HdrHistogramTrace_toHgrmString(self.ptr, output_units_per_second)
        return ptr_.decode("utf-8") if ptr_ is not None else ""

    def save_as_hgrm(self, file_name: str, output_units_per_second: float = 1e6) -> str:
        """Saves the percentile distribution as an .hgrm file in the desired output units. This
        can be loaded into standard hgrm plotting tools. Returns the absolute path to the output.
        
        Args:
            file_name: 
            output_units_per_second: output scale (ms=1e3, us=1e6, ns=1e9
        
        Raises:
            Exception: on internal errors
        """
        if file_name is not None and not isinstance(file_name, c_char_p):
            file_name = c_char_p(file_name.encode('utf-8') if isinstance(file_name, str) else file_name)
        ptr_ = libhebi_charts.hebi_charts_HdrHistogramTrace_saveAsHgrm(self.ptr, file_name, output_units_per_second)
        return ptr_.decode("utf-8") if ptr_ is not None else ""


class ImageStream:
    """[EXPERIMENTAL API]
    Represents a stream of images with an accessible pixel buffer. This class
    is not thread-safe and should only be used from one thread. Buffers and metadata
    for an image are only valid in between successful next() calls.
    
    The stream reuses multiple buffers internally and provides efficient access to the raw memory.
    """

    __slots__ = ('ptr',)

    def __init__(self, ptr: ImageStreamPtr):
        if not isinstance(ptr, ImageStreamPtr):
            raise TypeError(f'Expected ImageStreamPtr, got {type(ptr).__name__}')
        self.ptr = ptr

    def __del__(self):
        if self.ptr is not None:
            libhebi_charts.hebi_charts_ImageStream_release(self.ptr)
        self.ptr = None

    def __repr__(self):
        return f"{self.__class__.__name__}(buffer={self.buffer!r}, buffer_size={self.buffer_size!r}, channels={self.channels!r}, height={self.height!r}, pixel_format={self.pixel_format!r}, rate_limit={self.rate_limit!r}, recorder_threads={self.recorder_threads!r}, recording={self.recording!r}, render_scale={self.render_scale!r}, render_scale_x={self.render_scale_x!r}, render_scale_y={self.render_scale_y!r}, sequence={self.sequence!r}, stride={self.stride!r}, timestamp={self.timestamp!r}, width={self.width!r})"

    @property
    def buffer(self) -> Any | bytes | None:
        """@return the raw internal pixel buffer. Only valid between getNext calls."""
        return libhebi_charts.hebi_charts_ImageStream_getBuffer(self.ptr)

    @property
    def buffer_size(self) -> int:
        """@return the raw buffer size in bytes"""
        return libhebi_charts.hebi_charts_ImageStream_getBufferSize(self.ptr)

    @property
    def channels(self) -> int:
        """@return the number of channels of the current frame (e.g. 4 for BGRA)"""
        return libhebi_charts.hebi_charts_ImageStream_getChannels(self.ptr)

    @property
    def height(self) -> int:
        """@return the height of the current frame in pixels"""
        return libhebi_charts.hebi_charts_ImageStream_getHeight(self.ptr)

    @property
    def pixel_format(self) -> PixelFormat:
        """Represents the pixel format of the current buffer. The native and most
        performant format is BGRA_PRE, which stores pixels in adjacent bytes
        with premultiplied alpha components.
        
        Other convenience formats may be added in the future, but as of this point
        the others are all experimental.
        
        [Memory Layout (BGRA_PRE)]
        Bytes are stored in order of increasing index: Blue, Green, Red, Alpha
        
        [Compatibility]
        With an Alpha of 255 BGRA_PRE is identical to BGRA and is binary-compatible with the following:
        
            OpenCV:    CV_8UC4
        
            wxWidgets: BitmapBufferFormat_ARGB32
                       BitmapBufferFormat_RGB32
        
            Qt:        Format_ARGB32 (on little endian)
                       Format_BGRA8888
        
        Alpha less than 255 would show the image as darker or distorted, in which case
        the channels would need to be un-multiplied first.
        
        [Usage]
        Pixels in this format can be decoded using the following sample code:
        
            int i = rowstart + x * 4;
            int blue  = buffer[i + 0] & 0xff;
            int green = buffer[i + 1] & 0xff;
            int red   = buffer[i + 2] & 0xff;
            int alpha = buffer[i + 3] & 0xff;
        
        @return the pixel format of the current frame
        """
        return PixelFormat(libhebi_charts.hebi_charts_ImageStream_getPixelFormat(self.ptr))

    @pixel_format.setter
    def pixel_format(self, pixel_format: PixelFormat):
        """Sets the desired pixel format for future frames. The stream starts
        with the default of BGRA_PRE. Setting Unknown also reverts back to
        the default.
        
        All other formats are considered experimental.
        
        Args:
            pixel_format:
        """
        libhebi_charts.hebi_charts_ImageStream_setPixelFormat(self.ptr, pixel_format)

    @property
    def rate_limit(self) -> float:
        """An artificial rate limit above which frames get ignored"""
        return libhebi_charts.hebi_charts_ImageStream_getRateLimit(self.ptr)

    @rate_limit.setter
    def rate_limit(self, max_frames_per_second: float):
        """Sets the artificial rate limit above which frames get ignored.
        
        Args:
            max_frames_per_second: Rate limit in fps, e.g., 30.0
        
        Raises:
            Exception: on internal errors
        """
        status_ = libhebi_charts.hebi_charts_ImageStream_setRateLimit(self.ptr, max_frames_per_second)
        if status_ != 0:
            raise RuntimeError(f'Encountered error in ImageStream::rate_limit ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')

    @property
    def recorder_threads(self) -> int:
        """Gets the number of threads used for encoding individual frames
        
        Raises:
            Exception: on internal errors
        """
        return libhebi_charts.hebi_charts_ImageStream_getRecorderThreads(self.ptr)

    @recorder_threads.setter
    def recorder_threads(self, num_threads: int):
        """Sets the number of threads used for encoding individual frames
        
        Args:
            num_threads: 
        
        Raises:
            Exception: on internal errors
        """
        status_ = libhebi_charts.hebi_charts_ImageStream_setRecorderThreads(self.ptr, num_threads)
        if status_ != 0:
            raise RuntimeError(f'Encountered error in ImageStream::recorder_threads ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')

    @property
    def recording(self) -> bool:
        """[EXPERIMENTAL API] Checks whether the stream is currently recording.
        
        Raises:
            Exception: on internal errors
        """
        return libhebi_charts.hebi_charts_ImageStream_isRecording(self.ptr) != 0

    @property
    def render_scale(self) -> float:
        """@return the logical to actual pixel scale that was applied to this snapshot"""
        return libhebi_charts.hebi_charts_ImageStream_getRenderScale(self.ptr)

    @property
    def render_scale_x(self) -> float:
        """@return the logical to actual pixel scale that was applied to this snapshot in x"""
        return libhebi_charts.hebi_charts_ImageStream_getRenderScaleX(self.ptr)

    @property
    def render_scale_y(self) -> float:
        """@return the logical to actual pixel scale that was applied to this snapshot in y"""
        return libhebi_charts.hebi_charts_ImageStream_getRenderScaleY(self.ptr)

    @property
    def sequence(self) -> int:
        """@return the sequence number of the current frame"""
        return libhebi_charts.hebi_charts_ImageStream_getSequence(self.ptr)

    @property
    def stride(self) -> int:
        """@return the number of bytes from the start of one row to the next.
        This includes any padding for memory alignment.
        """
        return libhebi_charts.hebi_charts_ImageStream_getStride(self.ptr)

    @property
    def timestamp(self) -> float:
        """@return the timestamp of the current frame in seconds"""
        return libhebi_charts.hebi_charts_ImageStream_getTimestamp(self.ptr)

    @property
    def width(self) -> int:
        """@return the width of the current frame in pixels"""
        return libhebi_charts.hebi_charts_ImageStream_getWidth(self.ptr)

    def set_resolution(self, width: int, height: int) -> None:
        """Sets the resolution for future snapshots. Defaults to the initial resolution. Set 0 to auto-size.
        
        Args:
            width: 
            height: 
        
        Raises:
            Exception: on internal errors
        """
        status_ = libhebi_charts.hebi_charts_ImageStream_setResolution(self.ptr, width, height)
        if status_ != 0:
            raise RuntimeError(f'Encountered error in ImageStream::set_resolution ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')

    def wait_for_next(self, max_timeout_millis: int) -> bool:
        """Waits until there is a new image, and flips internal buffers
        as needed. Similar in behavior, but more efficient than.
        
            while (!tryGetNext() && !timeout) yield();
        
        @return true if a new image is available
        
        Args:
            max_timeout_millis: zero waits forever
        
        Raises:
            Exception: on internal errors
        """
        return libhebi_charts.hebi_charts_ImageStream_waitForNext(self.ptr, max_timeout_millis) != 0

    def try_get_next(self) -> bool:
        """Checks whether there is a new image, and flips
        internal buffers as needed. Any metadata is only
        valid until the next call.
        
        @return true if a new image is available
        """
        return libhebi_charts.hebi_charts_ImageStream_tryGetNext(self.ptr) != 0

    def start_recording(self, base_name: str, overwrite: bool = False) -> None:
        """[EXPERIMENTAL API]
        Losslessly records individual frames to disk in a way that ffmpeg can convert.
        
        The base name represents the file name without the extension. Images get stored
        in dir/<base>/*.png and the result will be in dir/<base>.<extension>.
        
        Args:
            base_name: The target base name
            overwrite: Whether the recorder is allowed to overwrite existing files
        
        Raises:
            Exception: IO errors when creating the directory
        """
        if base_name is not None and not isinstance(base_name, c_char_p):
            base_name = c_char_p(base_name.encode('utf-8') if isinstance(base_name, str) else base_name)
        status_ = libhebi_charts.hebi_charts_ImageStream_startRecording(self.ptr, base_name, overwrite)
        if status_ != 0:
            raise RuntimeError(f'Encountered error in ImageStream::start_recording ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')

    def stop_recording(self) -> RecordingResult:
        """[EXPERIMENTAL API] Blocking call that stops recording and returns the result.
        
        Raises:
            Exception: on internal errors
        """
        ptr_ = libhebi_charts.hebi_charts_ImageStream_stopRecording(self.ptr)
        if not ptr_:
            raise RuntimeError(f'Failed to create RecordingResult in ImageStream::stop_recording ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')
        return RecordingResult(ptr_)

    def save_to_file(self, file_name: str) -> None:
        """Saves the image to a file
        
        Args:
            file_name: 
        
        Raises:
            Exception: on internal errors
        """
        if file_name is not None and not isinstance(file_name, c_char_p):
            file_name = c_char_p(file_name.encode('utf-8') if isinstance(file_name, str) else file_name)
        status_ = libhebi_charts.hebi_charts_ImageStream_saveToFile(self.ptr, file_name)
        if status_ != 0:
            raise RuntimeError(f'Encountered error in ImageStream::save_to_file ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')


class LoopTimer:
    """Provides various time related functionality for timing, benchmarking,
    and scheduling loops. On Windows, using any method will request an
    interrupt timer of 1ms - beginTimePeriod(1).
    """

    __slots__ = ('ptr',)

    def __init__(self, *, frequency: float = None, period: float = None) -> None:
        ptr_ = libhebi_charts.hebi_charts_LoopTimer_create()
        if not ptr_:
            raise RuntimeError(f'Failed to create LoopTimer in LoopTimer::__init__')
        self.ptr = ptr_
        if frequency is not None: self.frequency = frequency
        if period is not None: self.period = period

    def __del__(self):
        if self.ptr is not None:
            libhebi_charts.hebi_charts_LoopTimer_release(self.ptr)
        self.ptr = None

    def __repr__(self):
        return f"{self.__class__.__name__}(elapsed_time={self.elapsed_time!r}, frequency={self.frequency!r}, period={self.period!r})"

    @property
    def elapsed_time(self) -> float:
        """Returns the elapsed time in seconds since the starting point."""
        return libhebi_charts.hebi_charts_LoopTimer_getElapsedTime(self.ptr)

    @property
    def frequency(self) -> float:
        return libhebi_charts.hebi_charts_LoopTimer_getFrequency(self.ptr)

    @frequency.setter
    def frequency(self, frequency: float):
        """Sets the desired frequency for waitForNextTick
        
        Args:
            frequency: hz
        """
        libhebi_charts.hebi_charts_LoopTimer_setFrequency(self.ptr, frequency)

    @property
    def period(self) -> float:
        return libhebi_charts.hebi_charts_LoopTimer_getPeriod(self.ptr)

    @period.setter
    def period(self, seconds: float):
        """Sets the desired period of waitForNextTick
        
        Args:
            seconds: seconds
        """
        libhebi_charts.hebi_charts_LoopTimer_setPeriod(self.ptr, seconds)

    def restart(self) -> None:
        """Resets the internal start time offset to now. This gets
        used to determine the elapsed time and the starting point
        for periodic ticks.
        """
        libhebi_charts.hebi_charts_LoopTimer_restart(self.ptr)

    def tic(self) -> None:
        """Sets a timestamp for subsequent toc calls"""
        libhebi_charts.hebi_charts_LoopTimer_tic(self.ptr)

    def toc(self) -> float:
        """Returns the elapsed time in seconds since the last tic call"""
        return libhebi_charts.hebi_charts_LoopTimer_toc(self.ptr)

    def tic_toc(self) -> float:
        """Calls tic and toc in one call"""
        return libhebi_charts.hebi_charts_LoopTimer_ticToc(self.ptr)

    def wait_for_next_tick(self) -> None:
        """Waits until the next periodic tick counting from the starting point. For
        example, if the start time is 12 with a period of 5, this call will wait
        until the next tick (17, 22, 27, 32, ...) that occurs after the current
        time.
        Note that this is a best effort, and results depends on the OS scheduler
        and load. Missed ticks will not be repeated.
        """
        libhebi_charts.hebi_charts_LoopTimer_waitForNextTick(self.ptr)

    def get_seconds_to_next_tick(self) -> float:
        """Returns the remaining time in seconds until the next periodic tick counting
        from the starting point. For example, if the start time is 12 with a period
        of 5, this call returns the time to the next tick (17, 22, 27, 32, ...) that
        occurs after the current time. Missed ticks will not be repeated. This method
        can never return zero, so  users do not need to special case.
        """
        return libhebi_charts.hebi_charts_LoopTimer_getSecondsToNextTick(self.ptr)

    def get_nanos_to_next_tick(self) -> int:
        """Returns the remaining time in nanoseconds until the next periodic tick counting
        from the starting point. For example, if the start time is 12 with a period
        of 5, this call returns the time to the next tick (17, 22, 27, 32, ...) that
        occurs after the current time. Missed ticks will not be repeated. This method
        can never return zero, so  users do not need to special case.
        """
        return libhebi_charts.hebi_charts_LoopTimer_getNanosToNextTick(self.ptr)

    @classmethod
    def time_seconds(cls) -> float:
        """Seconds since some arbitrary fixed point. Monotonic high-resolution timer for measuring elapsed time deltas."""
        return libhebi_charts.hebi_charts_LoopTimer_timeSeconds()

    @classmethod
    def time_nanos(cls) -> int:
        """Nanoseconds since some arbitrary fixed point. Monotonic high-resolution timer for measuring elapsed time deltas."""
        return libhebi_charts.hebi_charts_LoopTimer_timeNanos()

    @classmethod
    def sleep_nanos(cls, nanos: int) -> None:
        """Experimental (may be removed in the future):
        Attempts to sleep for the given number of nanoseconds using Thread::sleep. Results are
        best effort and depend on the platform. Threads might spuriously wake up early or be late.
        
        Args:
            nanos: Positive number of nanoseconds. Zero returns immediately.
        """
        libhebi_charts.hebi_charts_LoopTimer_sleepNanos(nanos)

    @classmethod
    def park_nanos(cls, nanos: int) -> None:
        """Experimental (may be removed in the future):
        Attempts to sleep for the given number of nanoseconds using LockSupport::park. Results are
        best effort and depend on the platform. Threads might spuriously wake up early or be late.
        
        Args:
            nanos: Positive number of nanoseconds. Zero returns immediately.
        """
        libhebi_charts.hebi_charts_LoopTimer_parkNanos(nanos)

    @classmethod
    def wait_nanos(cls, nanos: int) -> None:
        """Experimental (may be removed in the future):
        Attempts to sleep for the given number of nanoseconds using Object::wait. Results are
        best effort and depend on the platform. Threads might spuriously wake up early or be late.
        
        Args:
            nanos: Positive number of nanoseconds. Zero returns immediately.
        """
        libhebi_charts.hebi_charts_LoopTimer_waitNanos(nanos)

    @classmethod
    def spin_nanos(cls, nanos: int) -> None:
        """Experimental (may be removed in the future):
        Attempts to "sleep" for the given number of nanoseconds using tiered spin methods for a more
        accurate result in exchange for higher CPU cost. Results are best effort and depend on the
        platform. Even busy spins may be late depending on the OS scheduler.
        
        Args:
            nanos: Positive number of nanoseconds. Zero returns immediately.
        """
        libhebi_charts.hebi_charts_LoopTimer_spinNanos(nanos)


class Object3d:
    """Represents a generic 3d object that can be rotated and translated"""

    __slots__ = ('ptr',)

    def __init__(self, ptr: Object3dPtr):
        if not isinstance(ptr, Object3dPtr):
            raise TypeError(f'Expected Object3dPtr, got {type(ptr).__name__}')
        self.ptr = ptr

    def __del__(self):
        if self.ptr is not None:
            libhebi_charts.hebi_charts_Object3d_release(self.ptr)
        self.ptr = None

    def __repr__(self):
        return f"{self.__class__.__name__}(visible={self.visible!r})"

    @property
    def visible(self) -> bool:
        """Gets the current visibility state of this node"""
        return libhebi_charts.hebi_charts_Object3d_isVisible(self.ptr) != 0

    @visible.setter
    def visible(self, visible: bool):
        """Sets visibility for this object. Hidden objects are not removed from the SceneGraph
        
        Args:
            visible:
        """
        libhebi_charts.hebi_charts_Object3d_setVisible(self.ptr, visible)

    def set_orientation(self, qx: float, qy: float, qz: float, qw: float) -> None:
        """Sets the orientation of the object using a unit quaternion (x, y, z, w).
        This follows the ROS/REP-103 convention where the scalar component 'w' is last.
        The input is not verified!
        
        Args:
            qx: quaternion x (vector part)
            qy: quaternion y (vector part)
            qz: quaternion z (vector part)
            qw: quaternion w (scalar part)
        
        Raises:
            Exception: on internal errors
        """
        status_ = libhebi_charts.hebi_charts_Object3d_setOrientation(self.ptr, qx, qy, qz, qw)
        if status_ != 0:
            raise RuntimeError(f'Encountered error in Object3d::set_orientation ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')

    def set_orientation_r_p_y(self, roll: float, pitch: float, yaw: float) -> None:
        """Sets the orientation of the object using Roll, Pitch, and Yaw (radians).
        Follows the ROS/REP-103 convention (Extrinsic / Fixed-Axis XYZ):
        
            orientation = Rz(yaw)*Ry(pitch)*Rx(roll)
        
        This method preserves the current translation. The input is not verified!
        
        Args:
            roll: angle in [rad]
            pitch: angle in [rad]
            yaw: angle in [rad]
        
        Raises:
            Exception: on internal errors
        """
        status_ = libhebi_charts.hebi_charts_Object3d_setOrientationRPY(self.ptr, roll, pitch, yaw)
        if status_ != 0:
            raise RuntimeError(f'Encountered error in Object3d::set_orientation_r_p_y ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')

    def set_translation(self, x: float, y: float, z: float) -> None:
        """Translates the object. The input is not verified!
        
        Args:
            x: position x [m]
            y: position y [m]
            z: position z [m]
        
        Raises:
            Exception: on internal errors
        """
        status_ = libhebi_charts.hebi_charts_Object3d_setTranslation(self.ptr, x, y, z)
        if status_ != 0:
            raise RuntimeError(f'Encountered error in Object3d::set_translation ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')

    def set_pose(self, x: float, y: float, z: float, qx: float, qy: float, qz: float, qw: float) -> None:
        """Sets the full pose (position and orientation) of the object in a
        single atomic update. This follows the ROS/REP-103 convention
        (Position + Quaternion).
        
        Units: Translation in [m], Quaternion (x, y, z, w).
        The input is not verified!
        
        Args:
            x: position x [m]
            y: position y [m]
            z: position z [m]
            qx: quaternion x (vector part)
            qy: quaternion y (vector part)
            qz: quaternion z (vector part)
            qw: quaternion w (scalar part)
        
        Raises:
            Exception: on internal errors
        """
        status_ = libhebi_charts.hebi_charts_Object3d_setPose(self.ptr, x, y, z, qx, qy, qz, qw)
        if status_ != 0:
            raise RuntimeError(f'Encountered error in Object3d::set_pose ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')

    def set_transform4x4(self, matrix: Sequence[float], ordering: MatrixOrdering = MatrixOrdering.ROW_MAJOR) -> None:
        """Sets a 4x4 transform matrix of the form
        
            R R R x
            R R R y
            R R R z
            0 0 0 1
        
        The transform needs to be of size=16 and include the
        bottom row. The translation units are in meters.
        The input is not verified.
        
        Args:
            matrix: pointer to 16 double elements
            ordering: corresponding memory layout of the 4x4 matrix
        
        Raises:
            Exception: on internal errors
        """
        np = _require_numpy()
        if matrix is not None and not isinstance(matrix, POINTER(c_double)):
            matrix = np.ascontiguousarray(matrix, dtype=np.float64).ctypes.data_as(POINTER(c_double))
        status_ = libhebi_charts.hebi_charts_Object3d_setTransform4x4(self.ptr, matrix, ordering)
        if status_ != 0:
            raise RuntimeError(f'Encountered error in Object3d::set_transform4x4 ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')


class Frame(Object3d):
    """A triad that represents a frame"""

    __slots__ = ('ref_Frame',)

    def __init__(self, ref_Frame: FramePtr):
        if not isinstance(ref_Frame, FramePtr):
            raise TypeError(f'Expected FramePtr, got {type(ref_Frame).__name__}')
        self.ref_Frame = ref_Frame
        super().__init__(libhebi_charts.hebi_charts_Frame_to_Object3d(ref_Frame))

    def __del__(self):
        super().__del__()
        self.ref_Frame = None

    def __repr__(self):
        return f"{self.__class__.__name__}(visible={self.visible!r})"


class Mesh(Object3d):
    """Represents a static 3d mesh"""

    __slots__ = ('ref_Mesh',)

    def __init__(self, ref_Mesh: MeshPtr):
        if not isinstance(ref_Mesh, MeshPtr):
            raise TypeError(f'Expected MeshPtr, got {type(ref_Mesh).__name__}')
        self.ref_Mesh = ref_Mesh
        super().__init__(libhebi_charts.hebi_charts_Mesh_to_Object3d(ref_Mesh))

    def __del__(self):
        super().__del__()
        self.ref_Mesh = None

    def __repr__(self):
        return f"{self.__class__.__name__}(centered={self.centered!r}, scale={self.scale!r}, style={self.style!r}, visible={self.visible!r})"

    @property
    def centered(self) -> bool:
        """Returns whether the origin is moved to the center of the mesh"""
        return libhebi_charts.hebi_charts_Mesh_isCentered(self.ref_Mesh) != 0

    @centered.setter
    def centered(self, centered: bool):
        """Moves the origin to the center of the mesh.
        
        Args:
            centered:
        """
        libhebi_charts.hebi_charts_Mesh_setCentered(self.ref_Mesh, centered)

    @property
    def scale(self) -> float:
        """Gets the current scaling factor"""
        return libhebi_charts.hebi_charts_Mesh_getScale(self.ref_Mesh)

    @scale.setter
    def scale(self, scale_units_to_millimeters: float):
        """Sets the scaling factor applied to the mesh. The internal
        units are mm, so a mesh in meters would need to be scaled
        by 1e-3 to render correctly.
        
        Args:
            scale_units_to_millimeters:
        """
        libhebi_charts.hebi_charts_Mesh_setScale(self.ref_Mesh, scale_units_to_millimeters)

    @property
    def style(self) -> DisplayStyle:
        """Gets the current display style (Original, Ghosted, etc.)"""
        return DisplayStyle(libhebi_charts.hebi_charts_Mesh_getDisplayStyle(self.ref_Mesh))

    @style.setter
    def style(self, style: DisplayStyle):
        """Updates the visual representation of the mesh model.
        
        This is typically used to visually distinguish between multiple states of
        the same mesh, such as overlaying a semi-transparent 'Ghosted' target
        pose over the 'Original' pose.
        
        Args:
            style: sets the display style
        """
        libhebi_charts.hebi_charts_Mesh_setDisplayStyle(self.ref_Mesh, style)


class Robot(Object3d):
    """Represents robot kinematics"""

    __slots__ = ('ref_Robot',)

    def __init__(self, ref_Robot: RobotPtr):
        if not isinstance(ref_Robot, RobotPtr):
            raise TypeError(f'Expected RobotPtr, got {type(ref_Robot).__name__}')
        self.ref_Robot = ref_Robot
        super().__init__(libhebi_charts.hebi_charts_Robot_to_Object3d(ref_Robot))

    def __del__(self):
        super().__del__()
        self.ref_Robot = None

    def __repr__(self):
        return f"{self.__class__.__name__}(display_style={self.display_style!r}, dof={self.dof!r}, visible={self.visible!r})"

    @property
    def display_style(self) -> DisplayStyle:
        """Gets the current visual display style (Original, Ghosted, etc.)"""
        return DisplayStyle(libhebi_charts.hebi_charts_Robot_getDisplayStyle(self.ref_Robot))

    @display_style.setter
    def display_style(self, style: DisplayStyle):
        """Updates the visual representation of the robot model.
        
        This is typically used to visually distinguish between multiple states of
        the same robot, such as overlaying a semi-transparent 'Ghosted' target
        pose over the 'Original' pose.
        
        Args:
            style: sets the display style
        """
        libhebi_charts.hebi_charts_Robot_setDisplayStyle(self.ref_Robot, style)

    @property
    def dof(self) -> int:
        """Gets the number of joints (Degrees of Freedom) of this robot"""
        return libhebi_charts.hebi_charts_Robot_getDof(self.ref_Robot)

    def set_positions(self, positions: Sequence[float], length: int = None) -> None:
        """Updates the robot model configuration (kinematics) using a vector of joint positions.
        
        Units:
          - Revolute joints: [rad]
          - Prismatic joints: [m]
        
        The order of the vector must match the joint definitions in the underlying model.
        The 'length' parameter must exactly match the number of degrees of freedom (DOF)
        returned by getDof().
        
        Args:
            positions: pointer to an array of joint positions
            length: number of joints (must match getDof)
        
        Raises:
            Exception: if position vector length does not match number of joints.
        """
        np = _require_numpy()
        if length is None:
            length = 0 if (positions is None) else len(positions)
        if positions is not None and not isinstance(positions, POINTER(c_double)):
            positions = np.ascontiguousarray(positions, dtype=np.float64).ctypes.data_as(POINTER(c_double))
        status_ = libhebi_charts.hebi_charts_Robot_setPositions(self.ref_Robot, positions, length)
        if status_ != 0:
            raise RuntimeError(f'Encountered error in Robot::set_positions ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')


class Series3d(Object3d):
    """A collection of related spatial coordinates"""

    __slots__ = ('ref_Series3d',)

    def __init__(self, ref_Series3d: Series3dPtr):
        if not isinstance(ref_Series3d, Series3dPtr):
            raise TypeError(f'Expected Series3dPtr, got {type(ref_Series3d).__name__}')
        self.ref_Series3d = ref_Series3d
        super().__init__(libhebi_charts.hebi_charts_Series3d_to_Object3d(ref_Series3d))

    def __del__(self):
        super().__del__()
        self.ref_Series3d = None

    def __repr__(self):
        return f"{self.__class__.__name__}(color={self.color!r}, visible={self.visible!r})"

    @property
    def color(self) -> Color:
        """Gets the current dataset color"""
        return Color(libhebi_charts.hebi_charts_Series3d_getColor(self.ref_Series3d))

    @color.setter
    def color(self, color: Color):
        """Sets the dataset color
        
        Args:
            color:
        """
        libhebi_charts.hebi_charts_Series3d_setColor(self.ref_Series3d, color)


class Line3d(Series3d):
    """Represents a line in 3d space. Note that there are currently no
    line primitives, so the rendering is platform dependent and the
    performance is limited.
    """

    __slots__ = ('ref_Line3d',)

    def __init__(self, ref_Line3d: Line3dPtr):
        if not isinstance(ref_Line3d, Line3dPtr):
            raise TypeError(f'Expected Line3dPtr, got {type(ref_Line3d).__name__}')
        self.ref_Line3d = ref_Line3d
        super().__init__(libhebi_charts.hebi_charts_Line3d_to_Series3d(ref_Line3d))

    def __del__(self):
        super().__del__()
        self.ref_Line3d = None

    def __repr__(self):
        return f"{self.__class__.__name__}(color={self.color!r}, visible={self.visible!r})"

    def set_max_point_count(self, count: int) -> None:
        """Sets the internal maximum point count for incrementally
        adding points. May clear existing data.
        
        Args:
            count: maximum number of points
        """
        libhebi_charts.hebi_charts_Line3d_setMaxPointCount(self.ref_Line3d, count)

    def clear(self) -> None:
        """Clears all existing data"""
        libhebi_charts.hebi_charts_Line3d_clear(self.ref_Line3d)

    def set_data(self, x: Sequence[float], y: Sequence[float], z: Sequence[float], length: int = None) -> None:
        """Replaces the entire dataset with the provided X/Y/Z content. This
        operation copies the input data, so the caller retains ownership of
        the memory. Sets the buffer capacity to match the input length and
        clears any previous rolling history.
        
        Args:
            x: points
            y: points
            z: points
            length: number of x/y/z points
        """
        np = _require_numpy()
        if length is None:
            length = (0 if (x is None or y is None or z is None) else min(len(x), len(y), len(z)))
        if x is not None and not isinstance(x, POINTER(c_double)):
            x = np.ascontiguousarray(x, dtype=np.float64).ctypes.data_as(POINTER(c_double))
        if y is not None and not isinstance(y, POINTER(c_double)):
            y = np.ascontiguousarray(y, dtype=np.float64).ctypes.data_as(POINTER(c_double))
        if z is not None and not isinstance(z, POINTER(c_double)):
            z = np.ascontiguousarray(z, dtype=np.float64).ctypes.data_as(POINTER(c_double))
        libhebi_charts.hebi_charts_Line3d_setData(self.ref_Line3d, x, y, z, length)

    def add_points(self, x: Sequence[float], y: Sequence[float], z: Sequence[float], length: int = None) -> None:
        """Appends multiple data points to the end of the internal rolling buffer.
        This operation copies the input data, so the caller retains ownership
        of the memory. If the total number of points exceeds the current capacity,
        the oldest points are overwritten.
        
        Args:
            x: points
            y: points
            z: points
            length: number of x/y/z points
        """
        np = _require_numpy()
        if length is None:
            length = (0 if (x is None or y is None or z is None) else min(len(x), len(y), len(z)))
        if x is not None and not isinstance(x, POINTER(c_double)):
            x = np.ascontiguousarray(x, dtype=np.float64).ctypes.data_as(POINTER(c_double))
        if y is not None and not isinstance(y, POINTER(c_double)):
            y = np.ascontiguousarray(y, dtype=np.float64).ctypes.data_as(POINTER(c_double))
        if z is not None and not isinstance(z, POINTER(c_double)):
            z = np.ascontiguousarray(z, dtype=np.float64).ctypes.data_as(POINTER(c_double))
        libhebi_charts.hebi_charts_Line3d_addPoints(self.ref_Line3d, x, y, z, length)

    def add_point(self, x: float, y: float, z: float) -> None:
        """Adds one point to an internal rolling buffer. Once the maximum
        point count is reached, it will overwrite the earliest data.
        
        Args:
            x: 
            y: 
            z:
        """
        libhebi_charts.hebi_charts_Line3d_addPoint(self.ref_Line3d, x, y, z)


class Points3d(Series3d):
    """Represents points in 3d space. Each point gets rendered as
    the specified marker shape. This is intended for markers
    and is not appropriate for large scale lidar point clouds.
    """

    __slots__ = ('ref_Points3d',)

    def __init__(self, ref_Points3d: Points3dPtr):
        if not isinstance(ref_Points3d, Points3dPtr):
            raise TypeError(f'Expected Points3dPtr, got {type(ref_Points3d).__name__}')
        self.ref_Points3d = ref_Points3d
        super().__init__(libhebi_charts.hebi_charts_Points3d_to_Series3d(ref_Points3d))

    def __del__(self):
        super().__del__()
        self.ref_Points3d = None

    def __repr__(self):
        return f"{self.__class__.__name__}(marker_shape={self.marker_shape!r}, marker_size={self.marker_size!r}, self_illumination={self.self_illumination!r}, vertex_sharing={self.vertex_sharing!r}, color={self.color!r}, visible={self.visible!r})"

    @property
    def marker_shape(self) -> MarkerShape:
        """Gets the geometry used to represent each point in the series"""
        return MarkerShape(libhebi_charts.hebi_charts_Points3d_getMarkerShape(self.ref_Points3d))

    @marker_shape.setter
    def marker_shape(self, shape: MarkerShape):
        """Sets the geometry used to represent each point in the series
        
        Args:
            shape:
        """
        libhebi_charts.hebi_charts_Points3d_setMarkerShape(self.ref_Points3d, shape)

    @property
    def marker_size(self) -> float:
        """Gets the characteristic size (diameter or side length) of the markers in [m]"""
        return libhebi_charts.hebi_charts_Points3d_getMarkerSize(self.ref_Points3d)

    @marker_size.setter
    def marker_size(self, value: float):
        """Sets the characteristic size (diameter or side length) of the markers in [m]
        
        Args:
            value: [m]
        """
        libhebi_charts.hebi_charts_Points3d_setMarkerSize(self.ref_Points3d, value)

    @property
    def self_illumination(self) -> bool:
        """Gets the current self illumination state"""
        return libhebi_charts.hebi_charts_Points3d_isSelfIllumination(self.ref_Points3d) != 0

    @self_illumination.setter
    def self_illumination(self, value: bool):
        """Self illumination makes the shapes glow without an external light source (defaults to true)
        
        Args:
            value:
        """
        libhebi_charts.hebi_charts_Points3d_setSelfIllumination(self.ref_Points3d, value)

    @property
    def vertex_sharing(self) -> bool:
        """Vertex sharing reduces the complexity, but can result in poor lighting"""
        return libhebi_charts.hebi_charts_Points3d_isVertexSharing(self.ref_Points3d) != 0

    @vertex_sharing.setter
    def vertex_sharing(self, value: bool):
        """Vertex sharing reduces the complexity, but can result in poor lighting
        
        Args:
            value:
        """
        libhebi_charts.hebi_charts_Points3d_setVertexSharing(self.ref_Points3d, value)

    def set_max_point_count(self, count: int) -> None:
        """Sets the internal maximum point count for incrementally
        adding points. May clear existing data.
        
        Args:
            count: maximum number of points
        """
        libhebi_charts.hebi_charts_Points3d_setMaxPointCount(self.ref_Points3d, count)

    def clear(self) -> None:
        """Clears all existing data"""
        libhebi_charts.hebi_charts_Points3d_clear(self.ref_Points3d)

    def set_data(self, x: Sequence[float], y: Sequence[float], z: Sequence[float], length: int = None) -> None:
        """Replaces the entire dataset with the provided X/Y/Z content. This
        operation copies the input data, so the caller retains ownership of
        the memory. Sets the buffer capacity to match the input length and
        clears any previous rolling history.
        
        Args:
            x: points
            y: points
            z: points
            length: number of x/y/z points
        """
        np = _require_numpy()
        if length is None:
            length = (0 if (x is None or y is None or z is None) else min(len(x), len(y), len(z)))
        if x is not None and not isinstance(x, POINTER(c_double)):
            x = np.ascontiguousarray(x, dtype=np.float64).ctypes.data_as(POINTER(c_double))
        if y is not None and not isinstance(y, POINTER(c_double)):
            y = np.ascontiguousarray(y, dtype=np.float64).ctypes.data_as(POINTER(c_double))
        if z is not None and not isinstance(z, POINTER(c_double)):
            z = np.ascontiguousarray(z, dtype=np.float64).ctypes.data_as(POINTER(c_double))
        libhebi_charts.hebi_charts_Points3d_setData(self.ref_Points3d, x, y, z, length)

    def add_points(self, x: Sequence[float], y: Sequence[float], z: Sequence[float], length: int = None) -> None:
        """Appends multiple data points to the end of the internal rolling buffer.
        This operation copies the input data, so the caller retains ownership
        of the memory. If the total number of points exceeds the current capacity,
        the oldest points are overwritten.
        
        Args:
            x: points
            y: points
            z: points
            length: number of x/y/z points
        """
        np = _require_numpy()
        if length is None:
            length = (0 if (x is None or y is None or z is None) else min(len(x), len(y), len(z)))
        if x is not None and not isinstance(x, POINTER(c_double)):
            x = np.ascontiguousarray(x, dtype=np.float64).ctypes.data_as(POINTER(c_double))
        if y is not None and not isinstance(y, POINTER(c_double)):
            y = np.ascontiguousarray(y, dtype=np.float64).ctypes.data_as(POINTER(c_double))
        if z is not None and not isinstance(z, POINTER(c_double)):
            z = np.ascontiguousarray(z, dtype=np.float64).ctypes.data_as(POINTER(c_double))
        libhebi_charts.hebi_charts_Points3d_addPoints(self.ref_Points3d, x, y, z, length)

    def add_point(self, x: float, y: float, z: float) -> None:
        """Adds one point to an internal rolling buffer. Once the maximum
        point count is reached, it will overwrite the earliest data.
        
        Args:
            x: 
            y: 
            z:
        """
        libhebi_charts.hebi_charts_Points3d_addPoint(self.ref_Points3d, x, y, z)


class RecordingResult:
    """[EXPERIMENTAL API]
    Represents the result of a recording. Can be used to
    get various statistics and/or trigger FFMpeg.
    """

    __slots__ = ('ptr',)

    def __init__(self, ptr: RecordingResultPtr):
        if not isinstance(ptr, RecordingResultPtr):
            raise TypeError(f'Expected RecordingResultPtr, got {type(ptr).__name__}')
        self.ptr = ptr

    def __del__(self):
        if self.ptr is not None:
            libhebi_charts.hebi_charts_RecordingResult_release(self.ptr)
        self.ptr = None

    def __repr__(self):
        return f"{self.__class__.__name__}(directory={self.directory!r}, dropped={self.dropped!r}, duration={self.duration!r}, fps={self.fps!r}, manifest={self.manifest!r}, recorded={self.recorded!r}, skipped={self.skipped!r}, total_frames={self.total_frames!r})"

    @property
    def directory(self) -> str:
        """The absolute path to the directory containing the PNG frames and manifest.
        
        Raises:
            Exception: on internal errors
        """
        ptr_ = libhebi_charts.hebi_charts_RecordingResult_getDirectory(self.ptr)
        return ptr_.decode("utf-8") if ptr_ is not None else ""

    @property
    def dropped(self) -> int:
        """The number of frames dropped because the hardware (CPU/Disk) couldn't keep up."""
        return libhebi_charts.hebi_charts_RecordingResult_getDroppedCount(self.ptr)

    @property
    def duration(self) -> float:
        """The total duration in seconds."""
        return libhebi_charts.hebi_charts_RecordingResult_getDuration(self.ptr)

    @property
    def fps(self) -> float:
        """The actual average frames per second of the recorded sequence."""
        return libhebi_charts.hebi_charts_RecordingResult_getAverageFps(self.ptr)

    @property
    def manifest(self) -> str:
        """The absolute path to the manifest containing timing metadata.
        
        Raises:
            Exception: on internal errors
        """
        ptr_ = libhebi_charts.hebi_charts_RecordingResult_getManifest(self.ptr)
        return ptr_.decode("utf-8") if ptr_ is not None else ""

    @property
    def recorded(self) -> int:
        """The number of frames successfully written to disk."""
        return libhebi_charts.hebi_charts_RecordingResult_getRecordedCount(self.ptr)

    @property
    def skipped(self) -> int:
        """The number of frames intentionally skipped due to the rate limit."""
        return libhebi_charts.hebi_charts_RecordingResult_getSkippedCount(self.ptr)

    @property
    def total_frames(self) -> int:
        """The total number of hardware pulses/frames encountered during the session."""
        return libhebi_charts.hebi_charts_RecordingResult_getTotalFrames(self.ptr)

    def get_ffmpeg_command(self, output_format: VideoOutputFormat, delete_directory: bool = False) -> str:
        """Generates an FFmpeg command that converts the stored PNG files into the specified video format.
        The file name is the directory name w/ extension one level up. For example, an h264 format would
        map as follows:
        
            input: experiments/test17/*.png
            output: experiments/test17.mp4
        
        The delete directory flag appends a command that delete the input directory after a successful conversion.
        
        Args:
            output_format: target format
            delete_directory: cleanup after conversion
        
        Raises:
            Exception: on internal errors
        """
        ptr_ = libhebi_charts.hebi_charts_RecordingResult_getFfmpegCommand(self.ptr, output_format, delete_directory)
        return ptr_.decode("utf-8") if ptr_ is not None else ""

    def run_ffmpeg(self, output_format: VideoOutputFormat, delete_directory: bool = False) -> str:
        """Runs an FFmpeg command that converts the stored PNG files into the specified video format.
        The file name is the directory name w/ extension one level up. For example, an h264 format would
        map as follows:
        
            input: experiments/test17/*.png
            output: experiments/test17.mp4
        
        The delete directory flag appends a command that delete the input directory after a successful conversion.
        
        Args:
            output_format: target format
            delete_directory: cleanup after conversion
        
        Raises:
            Exception: on internal errors
        """
        ptr_ = libhebi_charts.hebi_charts_RecordingResult_runFfmpeg(self.ptr, output_format, delete_directory)
        return ptr_.decode("utf-8") if ptr_ is not None else ""


class Scene3d:
    """Represents a 3d scene that can render a variety of objects in 3d space"""

    __slots__ = ('ptr',)

    def __init__(self, ptr: Scene3dPtr):
        if not isinstance(ptr, Scene3dPtr):
            raise TypeError(f'Expected Scene3dPtr, got {type(ptr).__name__}')
        self.ptr = ptr

    def __del__(self):
        if self.ptr is not None:
            libhebi_charts.hebi_charts_Scene3d_release(self.ptr)
        self.ptr = None

    def __repr__(self):
        return f"{self.__class__.__name__}(grid_max_x={self.grid_max_x!r}, grid_max_y={self.grid_max_y!r}, grid_max_z={self.grid_max_z!r}, grid_min_x={self.grid_min_x!r}, grid_min_y={self.grid_min_y!r}, grid_min_z={self.grid_min_z!r}, grid_step={self.grid_step!r})"

    @property
    def grid_bounds(self):
        raise AttributeError("Property 'grid_bounds' is write-only")

    @grid_bounds.setter
    def grid_bounds(self, gridBounds: Tuple[float, float, float, float, float, float]):
        """Sets the boundaries of the 3D grid cage in [m]
        
        Args:
            min_x: [m]
            max_x: [m]
            min_y: [m]
            max_y: [m]
            min_z: [m]
            max_z: [m]
        """
        min_x, max_x, min_y, max_y, min_z, max_z = gridBounds
        libhebi_charts.hebi_charts_Scene3d_setGridBounds(self.ptr, min_x, max_x, min_y, max_y, min_z, max_z)

    @property
    def grid_max_x(self) -> float:
        """Gets the maximum X boundary of the 3D grid cage in [m]"""
        return libhebi_charts.hebi_charts_Scene3d_getMaxX(self.ptr)

    @grid_max_x.setter
    def grid_max_x(self, val: float):
        """Sets the maximum X boundary of the 3D grid cage in [m]
        
        Args:
            val:
        """
        libhebi_charts.hebi_charts_Scene3d_setMaxX(self.ptr, val)

    @property
    def grid_max_y(self) -> float:
        """Gets the maximum Y boundary of the 3D grid cage in [m]"""
        return libhebi_charts.hebi_charts_Scene3d_getMaxY(self.ptr)

    @grid_max_y.setter
    def grid_max_y(self, val: float):
        """Sets the maximum Y boundary of the 3D grid cage in [m]
        
        Args:
            val:
        """
        libhebi_charts.hebi_charts_Scene3d_setMaxY(self.ptr, val)

    @property
    def grid_max_z(self) -> float:
        """Gets the maximum Z boundary of the 3D grid cage in [m]"""
        return libhebi_charts.hebi_charts_Scene3d_getMaxZ(self.ptr)

    @grid_max_z.setter
    def grid_max_z(self, val: float):
        """Sets the maximum Z boundary of the 3D grid cage in [m]
        
        Args:
            val:
        """
        libhebi_charts.hebi_charts_Scene3d_setMaxZ(self.ptr, val)

    @property
    def grid_min_x(self) -> float:
        """Gets the minimum X boundary of the 3D grid cage in [m]"""
        return libhebi_charts.hebi_charts_Scene3d_getMinX(self.ptr)

    @grid_min_x.setter
    def grid_min_x(self, val: float):
        """Sets the minimum X boundary of the 3D grid cage in [m]
        
        Args:
            val:
        """
        libhebi_charts.hebi_charts_Scene3d_setMinX(self.ptr, val)

    @property
    def grid_min_y(self) -> float:
        """Gets the minimum Y boundary of the 3D grid cage in [m]"""
        return libhebi_charts.hebi_charts_Scene3d_getMinY(self.ptr)

    @grid_min_y.setter
    def grid_min_y(self, val: float):
        """Sets the minimum Y boundary of the 3D grid cage in [m]
        
        Args:
            val:
        """
        libhebi_charts.hebi_charts_Scene3d_setMinY(self.ptr, val)

    @property
    def grid_min_z(self) -> float:
        """Gets the minimum Z boundary of the 3D grid cage in [m]"""
        return libhebi_charts.hebi_charts_Scene3d_getMinZ(self.ptr)

    @grid_min_z.setter
    def grid_min_z(self, val: float):
        """Sets the minimum Z boundary of the 3D grid cage in [m]
        
        Args:
            val:
        """
        libhebi_charts.hebi_charts_Scene3d_setMinZ(self.ptr, val)

    @property
    def grid_step(self) -> float:
        """Gets the distance between grid lines in [m]"""
        return libhebi_charts.hebi_charts_Scene3d_getGridStep(self.ptr)

    @grid_step.setter
    def grid_step(self, grid_step: float):
        """Sets the distance between grid lines in [m]
        
        Args:
            grid_step: Distance in [m]
        """
        libhebi_charts.hebi_charts_Scene3d_setGridStep(self.ptr, grid_step)

    def get_camera(self) -> Camera:
        """Returns the camera of this 3d chart"""
        ptr_ = libhebi_charts.hebi_charts_Scene3d_getCamera(self.ptr)
        if not ptr_:
            raise RuntimeError(f'Failed to create Camera in Scene3d::get_camera ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')
        return Camera(ptr_)

    def add_robot(self, path_or_url: str, *, display_style: DisplayStyle = None, visible: bool = None) -> Robot:
        """Adds a robot from a description file (.hrdf)
        
        Args:
            path_or_url: file path or web-url to a description file
        
        Raises:
            Exception: on internal errors
        """
        if path_or_url is not None and not isinstance(path_or_url, c_char_p):
            path_or_url = c_char_p(path_or_url.encode('utf-8') if isinstance(path_or_url, str) else path_or_url)
        ptr_ = libhebi_charts.hebi_charts_Scene3d_addRobot(self.ptr, path_or_url)
        if not ptr_:
            raise RuntimeError(f'Failed to create Robot in Scene3d::add_robot ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')
        obj_ = Robot(ptr_)
        if display_style is not None: obj_.display_style = display_style
        if visible is not None: obj_.visible = visible
        return obj_

    def add_mesh(self, path_or_url: str, *, centered: bool = None, scale: float = None, style: DisplayStyle = None, visible: bool = None) -> Mesh:
        """Adds a 3d mesh from a file (.obj)
        
        Args:
            path_or_url: file path or web-url to an .obj file
        
        Raises:
            Exception: on internal errors
        """
        if path_or_url is not None and not isinstance(path_or_url, c_char_p):
            path_or_url = c_char_p(path_or_url.encode('utf-8') if isinstance(path_or_url, str) else path_or_url)
        ptr_ = libhebi_charts.hebi_charts_Scene3d_addMesh(self.ptr, path_or_url)
        if not ptr_:
            raise RuntimeError(f'Failed to create Mesh in Scene3d::add_mesh ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')
        obj_ = Mesh(ptr_)
        if centered is not None: obj_.centered = centered
        if scale is not None: obj_.scale = scale
        if style is not None: obj_.style = style
        if visible is not None: obj_.visible = visible
        return obj_

    def add_frame(self, length_in_meters: float = 0.03, *, visible: bool = None) -> Frame:
        """Adds a triad that represents a right-handed coordinate frame
        
        Args:
            length_in_meters: length of each axis in [m]
        
        Raises:
            Exception: on internal errors
        """
        ptr_ = libhebi_charts.hebi_charts_Scene3d_addFrame(self.ptr, length_in_meters)
        if not ptr_:
            raise RuntimeError(f'Failed to create Frame in Scene3d::add_frame ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')
        obj_ = Frame(ptr_)
        if visible is not None: obj_.visible = visible
        return obj_

    def add_line(self, *, color: Color = None, visible: bool = None) -> Line3d:
        """Adds a 3D data series rendered as a continuous line
        
        Raises:
            Exception: on internal errors
        """
        ptr_ = libhebi_charts.hebi_charts_Scene3d_addLine(self.ptr)
        if not ptr_:
            raise RuntimeError(f'Failed to create Line3d in Scene3d::add_line ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')
        obj_ = Line3d(ptr_)
        if color is not None: obj_.color = color
        if visible is not None: obj_.visible = visible
        return obj_

    def add_points(self, *, marker_shape: MarkerShape = None, marker_size: float = None, self_illumination: bool = None, vertex_sharing: bool = None, color: Color = None, visible: bool = None) -> Points3d:
        """Adds a 3D data series rendered as individual mesh objects
        
        Raises:
            Exception: on internal errors
        """
        ptr_ = libhebi_charts.hebi_charts_Scene3d_addPoints(self.ptr)
        if not ptr_:
            raise RuntimeError(f'Failed to create Points3d in Scene3d::add_points ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')
        obj_ = Points3d(ptr_)
        if marker_shape is not None: obj_.marker_shape = marker_shape
        if marker_size is not None: obj_.marker_size = marker_size
        if self_illumination is not None: obj_.self_illumination = self_illumination
        if vertex_sharing is not None: obj_.vertex_sharing = vertex_sharing
        if color is not None: obj_.color = color
        if visible is not None: obj_.visible = visible
        return obj_


class StreamView:
    """Shows a shared-memory stream generated by hebi-video tools."""

    __slots__ = ('ptr',)

    def __init__(self, ptr: StreamViewPtr):
        if not isinstance(ptr, StreamViewPtr):
            raise TypeError(f'Expected StreamViewPtr, got {type(ptr).__name__}')
        self.ptr = ptr

    def __del__(self):
        if self.ptr is not None:
            libhebi_charts.hebi_charts_StreamView_release(self.ptr)
        self.ptr = None

    def __repr__(self):
        return f"{self.__class__.__name__}()"


class XYChart:
    """Chart for XY series"""

    __slots__ = ('ptr',)

    def __init__(self, ptr: XYChartPtr):
        if not isinstance(ptr, XYChartPtr):
            raise TypeError(f'Expected XYChartPtr, got {type(ptr).__name__}')
        self.ptr = ptr

    def __del__(self):
        if self.ptr is not None:
            libhebi_charts.hebi_charts_XYChart_release(self.ptr)
        self.ptr = None

    def __repr__(self):
        return f"{self.__class__.__name__}(title={self.title!r}, x_assume_sorted={self.x_assume_sorted!r}, xauto={self.xauto!r}, xlabel={self.xlabel!r}, xmax={self.xmax!r}, xmin={self.xmin!r}, xunit={self.xunit!r}, yauto={self.yauto!r}, ylabel={self.ylabel!r}, ymax={self.ymax!r}, ymin={self.ymin!r}, yunit={self.yunit!r})"

    @property
    def title(self) -> str:
        """Gets the title shown in the chart titlebar"""
        ptr_ = libhebi_charts.hebi_charts_XYChart_getTitle(self.ptr)
        return ptr_.decode("utf-8") if ptr_ is not None else ""

    @title.setter
    def title(self, title: str):
        """Sets the title shown in the chart titlebar
        
        Args:
            title:
        """
        if title is not None and not isinstance(title, c_char_p):
            title = c_char_p(title.encode('utf-8') if isinstance(title, str) else title)
        libhebi_charts.hebi_charts_XYChart_setTitle(self.ptr, title)

    @property
    def x_assume_sorted(self) -> bool:
        """Whether the renderer should assume X-axis data is strictly increasing for performance"""
        return libhebi_charts.hebi_charts_XYChart_getXAssumeSorted(self.ptr) != 0

    @x_assume_sorted.setter
    def x_assume_sorted(self, x_assume_sorted: bool):
        """Enable to speed up rendering of large datasets. Must be disabled for paths that 'wrap back' or loops.
        
        Args:
            x_assume_sorted:
        """
        libhebi_charts.hebi_charts_XYChart_setXAssumeSorted(self.ptr, x_assume_sorted)

    @property
    def xauto(self) -> bool:
        """Checks if auto SI-prefix scaling is enabled for the X-axis"""
        return libhebi_charts.hebi_charts_XYChart_isXAutoUnitScaling(self.ptr) != 0

    @xauto.setter
    def xauto(self, enabled: bool):
        """Enables/disables auto SI-prefix scaling for X-axis (e.g., 0.001s -> 1ms)
        
        Args:
            enabled:
        """
        libhebi_charts.hebi_charts_XYChart_setXAutoUnitScaling(self.ptr, enabled)

    @property
    def xlabel(self) -> str:
        """Gets the X-axis label text"""
        ptr_ = libhebi_charts.hebi_charts_XYChart_getXLabel(self.ptr)
        return ptr_.decode("utf-8") if ptr_ is not None else ""

    @xlabel.setter
    def xlabel(self, label: str):
        """Sets the X-axis label text
        
        Args:
            label:
        """
        if label is not None and not isinstance(label, c_char_p):
            label = c_char_p(label.encode('utf-8') if isinstance(label, str) else label)
        libhebi_charts.hebi_charts_XYChart_setXLabel(self.ptr, label)

    @property
    def xlim(self):
        raise AttributeError("Property 'xlim' is write-only")

    @xlim.setter
    def xlim(self, xlim: Tuple[float, float]):
        """Sets the X-axis limits. Set nan for auto-ranging.
        
        Args:
            min: 
            max: 
        
        Raises:
            Exception: on internal errors
        """
        min, max = xlim
        status_ = libhebi_charts.hebi_charts_XYChart_setXLim(self.ptr, min, max)
        if status_ != 0:
            raise RuntimeError(f'Encountered error in XYChart::xlim ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')

    @property
    def xmax(self) -> float:
        """Gets the X-axis max limit"""
        return libhebi_charts.hebi_charts_XYChart_getXMax(self.ptr)

    @xmax.setter
    def xmax(self, max: float):
        """Sets the X-axis maximum limit
        
        Args:
            max:
        """
        libhebi_charts.hebi_charts_XYChart_setXMax(self.ptr, max)

    @property
    def xmin(self) -> float:
        """Gets the X-axis min limit"""
        return libhebi_charts.hebi_charts_XYChart_getXMin(self.ptr)

    @xmin.setter
    def xmin(self, min: float):
        """Sets the X-axis minimum limit
        
        Args:
            min:
        """
        libhebi_charts.hebi_charts_XYChart_setXMin(self.ptr, min)

    @property
    def xunit(self) -> str:
        """Gets the X-axis unit (e.g., 's')"""
        ptr_ = libhebi_charts.hebi_charts_XYChart_getXUnit(self.ptr)
        return ptr_.decode("utf-8") if ptr_ is not None else ""

    @xunit.setter
    def xunit(self, unit: str):
        """Sets the X-axis unit (e.g., 's')
        
        Args:
            unit:
        """
        if unit is not None and not isinstance(unit, c_char_p):
            unit = c_char_p(unit.encode('utf-8') if isinstance(unit, str) else unit)
        libhebi_charts.hebi_charts_XYChart_setXUnit(self.ptr, unit)

    @property
    def yauto(self) -> bool:
        """Checks if auto SI-prefix scaling is enabled for the Y-axis"""
        return libhebi_charts.hebi_charts_XYChart_isYAutoUnitScaling(self.ptr) != 0

    @yauto.setter
    def yauto(self, enabled: bool):
        """Enables/disables auto SI-prefix scaling for Y-axis
        
        Args:
            enabled:
        """
        libhebi_charts.hebi_charts_XYChart_setYAutoUnitScaling(self.ptr, enabled)

    @property
    def ylabel(self) -> str:
        """Gets the Y-axis label text"""
        ptr_ = libhebi_charts.hebi_charts_XYChart_getYLabel(self.ptr)
        return ptr_.decode("utf-8") if ptr_ is not None else ""

    @ylabel.setter
    def ylabel(self, label: str):
        """Sets the Y-axis label text
        
        Args:
            label:
        """
        if label is not None and not isinstance(label, c_char_p):
            label = c_char_p(label.encode('utf-8') if isinstance(label, str) else label)
        libhebi_charts.hebi_charts_XYChart_setYLabel(self.ptr, label)

    @property
    def ylim(self):
        raise AttributeError("Property 'ylim' is write-only")

    @ylim.setter
    def ylim(self, ylim: Tuple[float, float]):
        """Sets the Y-axis limits. Set nan for auto-ranging.
        
        Args:
            min: 
            max:
        """
        min, max = ylim
        libhebi_charts.hebi_charts_XYChart_setYLim(self.ptr, min, max)

    @property
    def ymax(self) -> float:
        """Gets the Y-axis max limit"""
        return libhebi_charts.hebi_charts_XYChart_getYMax(self.ptr)

    @ymax.setter
    def ymax(self, max: float):
        """Sets the Y-axis maximum limit
        
        Args:
            max:
        """
        libhebi_charts.hebi_charts_XYChart_setYMax(self.ptr, max)

    @property
    def ymin(self) -> float:
        """Gets the Y-axis min limit"""
        return libhebi_charts.hebi_charts_XYChart_getYMin(self.ptr)

    @ymin.setter
    def ymin(self, min: float):
        """Sets the Y-axis minimum limit
        
        Args:
            min:
        """
        libhebi_charts.hebi_charts_XYChart_setYMin(self.ptr, min)

    @property
    def yunit(self) -> str:
        """Gets the Y-axis unit (e.g., 'V')"""
        ptr_ = libhebi_charts.hebi_charts_XYChart_getYUnit(self.ptr)
        return ptr_.decode("utf-8") if ptr_ is not None else ""

    @yunit.setter
    def yunit(self, unit: str):
        """Sets the Y-axis unit (e.g., 'V')
        
        Args:
            unit:
        """
        if unit is not None and not isinstance(unit, c_char_p):
            unit = c_char_p(unit.encode('utf-8') if isinstance(unit, str) else unit)
        libhebi_charts.hebi_charts_XYChart_setYUnit(self.ptr, unit)

    def add_x_cursor(self, *, editable: bool = None, label: str = None, value: float = None, visible: bool = None) -> Cursor:
        """Adds a draggable cursor to the X-axis"""
        ptr_ = libhebi_charts.hebi_charts_XYChart_addXCursor(self.ptr)
        if not ptr_:
            raise RuntimeError(f'Failed to create Cursor in XYChart::add_x_cursor ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')
        obj_ = Cursor(ptr_)
        if editable is not None: obj_.editable = editable
        if label is not None: obj_.label = label
        if value is not None: obj_.value = value
        if visible is not None: obj_.visible = visible
        return obj_

    def add_y_cursor(self, *, editable: bool = None, label: str = None, value: float = None, visible: bool = None) -> Cursor:
        """Adds a draggable cursor to the Y-axis"""
        ptr_ = libhebi_charts.hebi_charts_XYChart_addYCursor(self.ptr)
        if not ptr_:
            raise RuntimeError(f'Failed to create Cursor in XYChart::add_y_cursor ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')
        obj_ = Cursor(ptr_)
        if editable is not None: obj_.editable = editable
        if label is not None: obj_.label = label
        if value is not None: obj_.value = value
        if visible is not None: obj_.visible = visible
        return obj_


class LatencyChart(XYChart):
    """Shows latency measurements in HdrHistogram percentile format"""

    __slots__ = ('ref_LatencyChart',)

    def __init__(self, ref_LatencyChart: LatencyChartPtr):
        if not isinstance(ref_LatencyChart, LatencyChartPtr):
            raise TypeError(f'Expected LatencyChartPtr, got {type(ref_LatencyChart).__name__}')
        self.ref_LatencyChart = ref_LatencyChart
        super().__init__(libhebi_charts.hebi_charts_LatencyChart_to_XYChart(ref_LatencyChart))

    def __del__(self):
        super().__del__()
        self.ref_LatencyChart = None

    def __repr__(self):
        return f"{self.__class__.__name__}(title={self.title!r}, x_assume_sorted={self.x_assume_sorted!r}, xauto={self.xauto!r}, xlabel={self.xlabel!r}, xmax={self.xmax!r}, xmin={self.xmin!r}, xunit={self.xunit!r}, yauto={self.yauto!r}, ylabel={self.ylabel!r}, ymax={self.ymax!r}, ymin={self.ymin!r}, yunit={self.yunit!r})"

    def add_trace(self, name: str, *, color: Color = None, label: str = None, line_style: LineStyle = None, line_width: float = None, marker_size: float = None, marker_type: MarkerType = None, show_in_legend: bool = None, visible: bool = None) -> LatencyTrace:
        """Creates a new hdr histogram dataset
        
        Args:
            name: 
        
        Raises:
            Exception: on internal errors
        """
        if name is not None and not isinstance(name, c_char_p):
            name = c_char_p(name.encode('utf-8') if isinstance(name, str) else name)
        ptr_ = libhebi_charts.hebi_charts_LatencyChart_addTrace(self.ref_LatencyChart, name)
        if not ptr_:
            raise RuntimeError(f'Failed to create LatencyTrace in LatencyChart::add_trace ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')
        obj_ = LatencyTrace(ptr_)
        if color is not None: obj_.color = color
        if label is not None: obj_.label = label
        if line_style is not None: obj_.line_style = line_style
        if line_width is not None: obj_.line_width = line_width
        if marker_size is not None: obj_.marker_size = marker_size
        if marker_type is not None: obj_.marker_type = marker_type
        if show_in_legend is not None: obj_.show_in_legend = show_in_legend
        if visible is not None: obj_.visible = visible
        return obj_


class LineChart(XYChart):
    """Represents an XY line chart"""

    __slots__ = ('ref_LineChart',)

    def __init__(self, ref_LineChart: LineChartPtr):
        if not isinstance(ref_LineChart, LineChartPtr):
            raise TypeError(f'Expected LineChartPtr, got {type(ref_LineChart).__name__}')
        self.ref_LineChart = ref_LineChart
        super().__init__(libhebi_charts.hebi_charts_LineChart_to_XYChart(ref_LineChart))

    def __del__(self):
        super().__del__()
        self.ref_LineChart = None

    def __repr__(self):
        return f"{self.__class__.__name__}(title={self.title!r}, x_assume_sorted={self.x_assume_sorted!r}, xauto={self.xauto!r}, xlabel={self.xlabel!r}, xmax={self.xmax!r}, xmin={self.xmin!r}, xunit={self.xunit!r}, yauto={self.yauto!r}, ylabel={self.ylabel!r}, ymax={self.ymax!r}, ymin={self.ymin!r}, yunit={self.yunit!r})"

    def add_line(self, label: str, *, max_point_count: int = None, color: Color = None, line_style: LineStyle = None, line_width: float = None, marker_size: float = None, marker_type: MarkerType = None, show_in_legend: bool = None, visible: bool = None) -> Line:
        """Creates a new line series
        
        Args:
            label: 
        
        Raises:
            Exception: on internal errors
        """
        if label is not None and not isinstance(label, c_char_p):
            label = c_char_p(label.encode('utf-8') if isinstance(label, str) else label)
        ptr_ = libhebi_charts.hebi_charts_LineChart_addLine(self.ref_LineChart, label)
        if not ptr_:
            raise RuntimeError(f'Failed to create Line in LineChart::add_line ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')
        obj_ = Line(ptr_)
        if max_point_count is not None: obj_.max_point_count = max_point_count
        if color is not None: obj_.color = color
        if line_style is not None: obj_.line_style = line_style
        if line_width is not None: obj_.line_width = line_width
        if marker_size is not None: obj_.marker_size = marker_size
        if marker_type is not None: obj_.marker_type = marker_type
        if show_in_legend is not None: obj_.show_in_legend = show_in_legend
        if visible is not None: obj_.visible = visible
        return obj_


class XYSeries:
    """A collection of related coordinates inside an XYChart"""

    __slots__ = ('ptr',)

    def __init__(self, ptr: XYSeriesPtr):
        if not isinstance(ptr, XYSeriesPtr):
            raise TypeError(f'Expected XYSeriesPtr, got {type(ptr).__name__}')
        self.ptr = ptr

    def __del__(self):
        if self.ptr is not None:
            libhebi_charts.hebi_charts_XYSeries_release(self.ptr)
        self.ptr = None

    def __repr__(self):
        return f"{self.__class__.__name__}(color={self.color!r}, label={self.label!r}, line_style={self.line_style!r}, line_width={self.line_width!r}, marker_size={self.marker_size!r}, marker_type={self.marker_type!r}, show_in_legend={self.show_in_legend!r}, visible={self.visible!r})"

    @property
    def color(self) -> Color:
        """Gets the rendering color"""
        return Color(libhebi_charts.hebi_charts_XYSeries_getColor(self.ptr))

    @color.setter
    def color(self, color: Color):
        """Sets the rendering color
        
        Args:
            color:
        """
        libhebi_charts.hebi_charts_XYSeries_setColor(self.ptr, color)

    @property
    def label(self) -> str:
        """Gets the label shown in the chart legend"""
        ptr_ = libhebi_charts.hebi_charts_XYSeries_getLabel(self.ptr)
        return ptr_.decode("utf-8") if ptr_ is not None else ""

    @label.setter
    def label(self, label: str):
        """Sets the label shown in the chart legend
        
        Args:
            label:
        """
        if label is not None and not isinstance(label, c_char_p):
            label = c_char_p(label.encode('utf-8') if isinstance(label, str) else label)
        libhebi_charts.hebi_charts_XYSeries_setLabel(self.ptr, label)

    @property
    def line_style(self) -> LineStyle:
        """Gets the rendering style"""
        return LineStyle(libhebi_charts.hebi_charts_XYSeries_getLineStyle(self.ptr))

    @line_style.setter
    def line_style(self, line_style: LineStyle):
        """Sets the rendering style
        
        Args:
            line_style:
        """
        libhebi_charts.hebi_charts_XYSeries_setLineStyle(self.ptr, line_style)

    @property
    def line_width(self) -> float:
        """Gets the line width in display pixels"""
        return libhebi_charts.hebi_charts_XYSeries_getLineWidth(self.ptr)

    @line_width.setter
    def line_width(self, line_width: float):
        """Sets the line width
        
        Args:
            line_width: width in [px]
        """
        libhebi_charts.hebi_charts_XYSeries_setLineWidth(self.ptr, line_width)

    @property
    def marker_size(self) -> float:
        """Gets the marker size"""
        return libhebi_charts.hebi_charts_XYSeries_getMarkerSize(self.ptr)

    @marker_size.setter
    def marker_size(self, marker_size: float):
        """Sets the marker size
        
        Args:
            marker_size:
        """
        libhebi_charts.hebi_charts_XYSeries_setMarkerSize(self.ptr, marker_size)

    @property
    def marker_type(self) -> MarkerType:
        """Gets the marker type"""
        return MarkerType(libhebi_charts.hebi_charts_XYSeries_getMarkerType(self.ptr))

    @marker_type.setter
    def marker_type(self, marker_type: MarkerType):
        """Sets the marker type
        
        Args:
            marker_type:
        """
        libhebi_charts.hebi_charts_XYSeries_setMarkerType(self.ptr, marker_type)

    @property
    def show_in_legend(self) -> bool:
        """Checks if this data set is shown in the legend"""
        return libhebi_charts.hebi_charts_XYSeries_isShowInLegend(self.ptr) != 0

    @show_in_legend.setter
    def show_in_legend(self, show_in_legend: bool):
        """Shows or hides this data set from the legend
        
        Args:
            show_in_legend:
        """
        libhebi_charts.hebi_charts_XYSeries_setShowInLegend(self.ptr, show_in_legend)

    @property
    def visible(self) -> bool:
        """Gets the visibility in the chart"""
        return libhebi_charts.hebi_charts_XYSeries_isVisible(self.ptr) != 0

    @visible.setter
    def visible(self, visible: bool):
        """Sets the visibility in the chart
        
        Args:
            visible:
        """
        libhebi_charts.hebi_charts_XYSeries_setVisible(self.ptr, visible)


class LatencyTrace(XYSeries):
    """Represents a latency measurement that records latency values in the form of an HdrHistogram"""

    __slots__ = ('ref_LatencyTrace',)

    def __init__(self, ref_LatencyTrace: LatencyTracePtr):
        if not isinstance(ref_LatencyTrace, LatencyTracePtr):
            raise TypeError(f'Expected LatencyTracePtr, got {type(ref_LatencyTrace).__name__}')
        self.ref_LatencyTrace = ref_LatencyTrace
        super().__init__(libhebi_charts.hebi_charts_LatencyTrace_to_XYSeries(ref_LatencyTrace))

    def __del__(self):
        super().__del__()
        self.ref_LatencyTrace = None

    def __repr__(self):
        return f"{self.__class__.__name__}(color={self.color!r}, label={self.label!r}, line_style={self.line_style!r}, line_width={self.line_width!r}, marker_size={self.marker_size!r}, marker_type={self.marker_type!r}, show_in_legend={self.show_in_legend!r}, visible={self.visible!r})"

    def tic(self) -> None:
        """Sets a timestamp for subsequent toc calls"""
        libhebi_charts.hebi_charts_LatencyTrace_tic(self.ref_LatencyTrace)

    def toc(self) -> float:
        """Records the elapsed time since the last tic call. Returns the recorded value in [s]"""
        return libhebi_charts.hebi_charts_LatencyTrace_toc(self.ref_LatencyTrace)

    def tic_toc(self) -> float:
        """Calls tic and toc in one call. Returns the recorded value in [s]"""
        return libhebi_charts.hebi_charts_LatencyTrace_ticToc(self.ref_LatencyTrace)

    def record(self, value: float) -> None:
        """Record a latency value in the histogram
        
        Args:
            value: the value to be recorded in [s]
        """
        libhebi_charts.hebi_charts_LatencyTrace_record(self.ref_LatencyTrace, value)

    def record_with_count(self, value: float, count: int) -> None:
        """Record a value in the histogram (adding to the value's current count)
        
        Args:
            value: the value to be recorded in [s]
            count: the number of occurrences of this value to record
        """
        libhebi_charts.hebi_charts_LatencyTrace_recordWithCount(self.ref_LatencyTrace, value, count)

    def record_compensated(self, value: float, expected_interval_between_value_samples: float) -> None:
        """[EXPERIMENTAL - specific to HdrHistogram]
        Record a value in the histogram.
        To compensate for the loss of sampled values when a recorded value is larger than the expected interval
        between value samples, Histogram will auto-generate an additional series of decreasingly-smaller
        (down to the expectedIntervalBetweenValueSamples) value records.
        
        Args:
            value: the value to be recorded in [s]
            expected_interval_between_value_samples: If expectedIntervalBetweenValueSamples in [s] is larger than
        zero, an auto-generated value records as appropriate if value
        is larger than expectedIntervalBetweenValueSamples
        """
        libhebi_charts.hebi_charts_LatencyTrace_recordCompensated(self.ref_LatencyTrace, value, expected_interval_between_value_samples)

    def reset(self) -> None:
        libhebi_charts.hebi_charts_LatencyTrace_reset(self.ref_LatencyTrace)


class Line(XYSeries):
    """Represents a high-performance 2D line series optimized for real-time
    telemetry.
    Uses a double-buffered architecture with bounded rolling buffers to
    decouple high-frequency data ingestion from the UI rendering pulse.
    """

    __slots__ = ('ref_Line',)

    def __init__(self, ref_Line: LinePtr):
        if not isinstance(ref_Line, LinePtr):
            raise TypeError(f'Expected LinePtr, got {type(ref_Line).__name__}')
        self.ref_Line = ref_Line
        super().__init__(libhebi_charts.hebi_charts_Line_to_XYSeries(ref_Line))

    def __del__(self):
        super().__del__()
        self.ref_Line = None

    def __repr__(self):
        return f"{self.__class__.__name__}(max_point_count={self.max_point_count!r}, color={self.color!r}, label={self.label!r}, line_style={self.line_style!r}, line_width={self.line_width!r}, marker_size={self.marker_size!r}, marker_type={self.marker_type!r}, show_in_legend={self.show_in_legend!r}, visible={self.visible!r})"

    @property
    def max_point_count(self) -> int:
        """Returns the maximum number of points this line can hold before rolling"""
        return libhebi_charts.hebi_charts_Line_getMaxPointCount(self.ref_Line)

    @max_point_count.setter
    def max_point_count(self, count: int):
        """Sets the maximum number of points the rolling buffer can hold.
        Note: Changing the capacity clears all existing data. If the
        new capacity matches the current value, this operation is a
        no-op and data is preserved.
        
        Args:
            count: maximum number of points
        """
        libhebi_charts.hebi_charts_Line_setMaxPointCount(self.ref_Line, count)

    def clear(self) -> None:
        """Clears all existing data"""
        libhebi_charts.hebi_charts_Line_clear(self.ref_Line)

    def set_data(self, x: Sequence[float], y: Sequence[float], length: int = None) -> None:
        """Replaces the entire dataset with the provided X and Y content. This
        operation copies the input data, so the caller retains ownership of
        the memory. Sets the buffer capacity to match the input length and
        clears any previous rolling history.
        
        Args:
            x: points
            y: points
            length: number of x/y points
        """
        np = _require_numpy()
        if length is None:
            length = (0 if (x is None or y is None) else min(len(x), len(y)))
        if x is not None and not isinstance(x, POINTER(c_double)):
            x = np.ascontiguousarray(x, dtype=np.float64).ctypes.data_as(POINTER(c_double))
        if y is not None and not isinstance(y, POINTER(c_double)):
            y = np.ascontiguousarray(y, dtype=np.float64).ctypes.data_as(POINTER(c_double))
        libhebi_charts.hebi_charts_Line_setData(self.ref_Line, x, y, length)

    def add_points(self, x: Sequence[float], y: Sequence[float], length: int = None) -> None:
        """Appends multiple data points to the end of the internal rolling buffer.
        This operation copies the input data, so the caller retains ownership
        of the memory. If the total number of points exceeds the current capacity,
        the oldest points are overwritten.
        
        Args:
            x: points
            y: points
            length: number of x/y points
        """
        np = _require_numpy()
        if length is None:
            length = (0 if (x is None or y is None) else min(len(x), len(y)))
        if x is not None and not isinstance(x, POINTER(c_double)):
            x = np.ascontiguousarray(x, dtype=np.float64).ctypes.data_as(POINTER(c_double))
        if y is not None and not isinstance(y, POINTER(c_double)):
            y = np.ascontiguousarray(y, dtype=np.float64).ctypes.data_as(POINTER(c_double))
        libhebi_charts.hebi_charts_Line_addPoints(self.ref_Line, x, y, length)

    def add_point(self, x: float, y: float) -> None:
        """Adds one point to an internal rolling buffer. Once the maximum
        point count is reached, it will overwrite the earliest data.
        
        Args:
            x: 
            y:
        """
        libhebi_charts.hebi_charts_Line_addPoint(self.ref_Line, x, y)


class Runtime:
    """Contains utility methods for working with the runtime. Some methods
    are experimental and may change or be removed in the future.
    """

    def __new__(cls, *args, **kwargs):
        raise TypeError('Class is not instantiable')

    @classmethod
    def set_option(cls, option: RuntimeOption, value: str) -> None:
        """Applies runtime options. Needs to be done before any other methods.
        
        Args:
            option: 
            value: string value depending on the option (e.g. DpiScale='2.0', VerboseGraphics='true')
        
        Raises:
            Exception: on internal errors
        """
        if value is not None and not isinstance(value, c_char_p):
            value = c_char_p(value.encode('utf-8') if isinstance(value, str) else value)
        status_ = libhebi_charts.hebi_charts_Runtime_setOption(option, value)
        if status_ != 0:
            raise RuntimeError(f'Encountered error in Runtime::set_option ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')

    @classmethod
    def set_theme(cls, theme: Theme) -> None:
        """Sets an AtlantaFX theme for rendering the UI
        
        Args:
            theme:
        """
        libhebi_charts.hebi_charts_Runtime_setTheme(theme)

    @classmethod
    def set_auto_close_windows(cls, auto_close: bool) -> None:
        """Applies a global auto-close behavior, i.e., window::keepOpen
        
        Args:
            auto_close:
        """
        libhebi_charts.hebi_charts_Runtime_setAutoCloseWindows(auto_close)

    @classmethod
    def wait_until_windows_closed(cls) -> None:
        """Waits until all windows were closed by the user
        
        Raises:
            Exception: when called from the FX thread
        """
        status_ = libhebi_charts.hebi_charts_Runtime_waitUntilWindowsClosed()
        if status_ != 0:
            raise RuntimeError(f'Encountered error in Runtime::wait_until_windows_closed ({libhebi_charts.hebi_charts_Runtime_getLastErrorString().decode("utf-8")})')

    @classmethod
    def collect(cls) -> None:
        """Reclaims unused memory and prepares the heap for real-time execution"""
        libhebi_charts.hebi_charts_Runtime_collect()

    @classmethod
    def close_all(cls) -> None:
        """Hides all active windows and dashboards"""
        libhebi_charts.hebi_charts_Runtime_closeAll()

    @classmethod
    def run_on_ui_thread(cls, func: UserCallbackFunction, user_data: Any | bytes | None) -> None:
        """Debug method to run code on the internal UI thread
        
        Args:
            func: 
            user_data:
        """
        libhebi_charts.hebi_charts_Runtime_runOnUiThread(func, user_data)

    @classmethod
    def print_last_error_details(cls) -> None:
        """Debug method that prints the last exception encountered on the current thread"""
        libhebi_charts.hebi_charts_Runtime_printLastErrorDetails()

    @classmethod
    def print_thread_info(cls, name: str) -> None:
        """Debug method to print internal thread information. May be removed in the future.
        
        Args:
            name:
        """
        if name is not None and not isinstance(name, c_char_p):
            name = c_char_p(name.encode('utf-8') if isinstance(name, str) else name)
        libhebi_charts.hebi_charts_Runtime_printThreadInfo(name)

    @classmethod
    def get_last_error_string(cls) -> str:
        """Returns an address to a c string that contains the last error message.
        This address is only valid until the next call to this method from the
        same thread. Never returns nullptr. Do not free the address!
        """
        ptr_ = libhebi_charts.hebi_charts_Runtime_getLastErrorString()
        return ptr_.decode("utf-8") if ptr_ is not None else ""


# ==== Cocoa utilities for supporting macOS ====
# C function signature
hebi_charts_MainCallbackFunction = CFUNCTYPE(c_int, c_int, POINTER(c_char_p))
libhebi_charts.hebi_charts_runApplication.argtypes = [hebi_charts_MainCallbackFunction, c_int, POINTER(c_char_p)]
libhebi_charts.hebi_charts_runApplication.restype = c_int

# Python wrapper. We ignore the C args as Python can just use a global namespace
def run_application(callback_fn, *args, **kwargs):
    """Sets up required system libraries and executes the callback on an appropriate thread
    
    This is technically only needed on macOS as the Cocoa framework for displaying
    windows needs to be run on the main thread. On Windows and Linux this method
    executes the callback directly and otherwise does nothing. However, all platforms
    are supported to enable platform-independent code with the same behavior.
    """
    def c_callback_wrapper(argc, argv):
        callback_fn(*args, **kwargs)
        return 0
    c_callback = hebi_charts_MainCallbackFunction(c_callback_wrapper)
    return libhebi_charts.hebi_charts_runApplication(c_callback, 0, (c_char_p * 0)())
