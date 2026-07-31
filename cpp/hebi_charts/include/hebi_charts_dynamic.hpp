#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <iostream>
#include <string>
#include <vector>

#ifdef WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

namespace hebi {
namespace charts {

class Exception : public std::exception {
public:
  Exception(const char* message) noexcept : msg_(message) {}
  Exception(const std::string& message) noexcept : msg_(message) {}
  virtual ~Exception() noexcept {}
  const char* what() const noexcept override {
    return msg_.c_str();
  }
protected:
  std::string msg_;
};

enum class CameraView : int32_t {
  ISOMETRIC = 0,
  FRONT = 1,
  BACK = 2,
  LEFT = 3,
  RIGHT = 4,
  TOP = 5,
  BOTTOM = 6
};

enum class Color : int32_t {
  Default = 0,
  Black = 1,
  White = 2,
  Gray = 3,
  Red = 4,
  Green = 5,
  Blue = 6,
  Yellow = 7,
  Magenta = 8
};

enum class DisplayStyle : int32_t {
  Original = 0,
  Translucent = 1,
  Ghosted = 2
};

enum class LineStyle : int32_t {
  Default = 0,
  Solid = 1,
  Dashed = 2,
  Points = 3
};

enum class MarkerShape : int32_t {
  Default = 0,
  Tetrahedron = 1,
  Cube = 2,
  Octahedron = 3,
  Icosahedron = 4,
  Sphere = 5
};

enum class MarkerType : int32_t {
  Default = 0,
  Rectangle = 1,
  Rectangle1 = 2,
  Rectangle2 = 3,
  Circle = 4,
  Circle1 = 5,
  Circle2 = 6,
  Plus = 7,
  Cross = 8,
  Diamond = 9,
  Diamond1 = 10,
  Diamond2 = 11
};

enum class MatrixOrdering : int32_t {
  RowMajor = 0,
  ColumnMajor = 1
};

enum class PixelFormat : int32_t {
  Unknown = 0,
  BGRA_PRE = 1,
  BGRA = 2,
  ARGB = 3,
  RGBA = 4,
  RGB = 5,
  BGR = 6,
  GRAY = 7
};

enum class RuntimeOption : int32_t {
  DpiScale = 0,
  GraphicsPipeline = 1,
  TextAntialiasing = 2,
  VSync = 3,
  VerboseGraphics = 4,
  VerbosePulse = 5,
  VerboseFonts = 6,
  VerboseWindowing = 7
};

enum class Theme : int32_t {
  PrimerLight = 0,
  PrimerDark = 1,
  NordLight = 2,
  NordDark = 3,
  CupertinoLight = 4,
  CupertinoDark = 5,
  Dracula = 6
};

enum class VideoOutputFormat : int32_t {
  H264 = 0,
  H265 = 1,
  ProRes = 2,
  WebM = 3,
  Gif = 4
};

// ==== Forward Declarations ====
class Camera;
class Control;
class Button;
class Dropdown;
class Label;
class Slider;
class Toggle;
class ControlPanel;
class Cursor;
class FxmlView;
class GridWindow;
class HdrHistogramRecorder;
class HdrHistogramTrace;
class ImageStream;
class LoopTimer;
class Object3d;
class Frame;
class Mesh;
class Robot;
class Series3d;
class Line3d;
class Points3d;
class RecordingResult;
class Scene3d;
class StreamView;
class XYChart;
class LatencyChart;
class LineChart;
class XYSeries;
class LatencyTrace;
class Line;
using UserCallbackFunction = void (*)(void* userData);

namespace internal {
using CameraPtr = struct Camera_*;
using ControlPtr = struct Control_*;
using ButtonPtr = struct Button_*;
using DropdownPtr = struct Dropdown_*;
using LabelPtr = struct Label_*;
using SliderPtr = struct Slider_*;
using TogglePtr = struct Toggle_*;
using ControlPanelPtr = struct ControlPanel_*;
using CursorPtr = struct Cursor_*;
using FxmlViewPtr = struct FxmlView_*;
using GridWindowPtr = struct GridWindow_*;
using HdrHistogramRecorderPtr = struct HdrHistogramRecorder_*;
using HdrHistogramTracePtr = struct HdrHistogramTrace_*;
using ImageStreamPtr = struct ImageStream_*;
using LoopTimerPtr = struct LoopTimer_*;
using Object3dPtr = struct Object3d_*;
using FramePtr = struct Frame_*;
using MeshPtr = struct Mesh_*;
using RobotPtr = struct Robot_*;
using Series3dPtr = struct Series3d_*;
using Line3dPtr = struct Line3d_*;
using Points3dPtr = struct Points3d_*;
using RecordingResultPtr = struct RecordingResult_*;
using Scene3dPtr = struct Scene3d_*;
using StreamViewPtr = struct StreamView_*;
using XYChartPtr = struct XYChart_*;
using LatencyChartPtr = struct LatencyChart_*;
using LineChartPtr = struct LineChart_*;
using XYSeriesPtr = struct XYSeries_*;
using LatencyTracePtr = struct LatencyTrace_*;
using LinePtr = struct Line_*;
}

// ==== Class Wrappers ====

/**
 * @brief Represents a view point looking at a 3d scene
 */
class Camera {
  friend class Scene3d;
public:

  /**
   * @brief Sets the view point to a predefined standard view
   *
   * @param view 
   */
  void setView(CameraView view) noexcept;

  /**
   * @brief Resets the camera orientation, zoom, and pan to defaults
   */
  void reset() noexcept;

  /**
   * @details 
   * Applies an incremental rotation to the current camera view using a unit quaternion (x, y, z, w).
   * This follows the ROS/REP-103 convention where the scalar component 'w' is last.
   * 
   * This rotation is multiplied by the current camera orientation.
   * The input is not verified!
   *
   * @param qx quaternion x (vector part)
   * @param qy quaternion y (vector part)
   * @param qz quaternion z (vector part)
   * @param qw quaternion w (scalar part)
   * @throw on internal errors
   */
  void applyRotation(double qx, double qy, double qz, double qw);

  /**
   * @brief Sets the distance (zoom) from the camera to the pivot center. Out of range values are clamped.
   *
   * @param distanceInMeters distance [m]
   * @throw on internal errors
   */
  void setDistance(double distanceInMeters);

  /**
   * @details 
   * Sets the 3D pan offset (panning) of the camera.
   * This slides the entire scene relative to the camera view.
   * 
   * To center the camera on a specific object, use this to offset
   * the world origin.
   *
   * @param x position x [m]
   * @param y position y [m]
   * @param z position z [m]
   * @throw on internal errors
   */
  void setPan(double x, double y, double z);

  /**
   * @brief Shows or hides the on-screen navigation UI controls (buttons/overlays).
   *
   * @param value 
   */
  void setControlsVisible(bool value) noexcept;
  Camera(Camera&& from) noexcept;
  Camera& operator=(Camera&& from) noexcept;
  ~Camera() noexcept;
private:
  void cleanup() noexcept;
  explicit Camera(internal::CameraPtr cPointer) noexcept : ptr_(cPointer) {}
  internal::CameraPtr ptr_{};
};


/**
 * @brief Shared control functionality
 */
class Control {
public:

  /**
   * @brief Gets the current disabled state of this node
   */
  bool isEnabled() const noexcept;

  /**
   * @brief Sets the disabled state of this node
   *
   * @param enabled 
   */
  void setEnabled(bool enabled) noexcept;

  /**
   * @brief Gets the name or descriptor of the control in the left column
   */
  std::string getLabel() const noexcept;

  /**
   * @brief Sets the name or descriptor of the control in the left column
   *
   * @param name 
   */
  void setLabel(const char* name) noexcept;
  void setLabel(const std::string& name) noexcept;

  /**
   * @brief Gets the tooltip
   */
  std::string getTooltip() const noexcept;

  /**
   * @brief Sets the tooltip
   *
   * @param tooltip 
   */
  void setTooltip(const char* tooltip) noexcept;
  void setTooltip(const std::string& tooltip) noexcept;

  /**
   * @brief Gets the current visibility state of this node
   */
  bool isVisible() const noexcept;

  /**
   * @brief Sets the visibility of this node
   *
   * @param visible 
   */
  void setVisible(bool visible) noexcept;
  Control(Control&& from) noexcept;
  Control& operator=(Control&& from) noexcept;
  virtual ~Control() noexcept;
protected:
  explicit Control(internal::ControlPtr cPointer) noexcept : ptr_(cPointer) {}
private:
  void cleanup() noexcept;
  internal::ControlPtr ptr_{};
};


/**
 * @brief A button control
 */
class Button : public Control {
  friend class ControlPanel;
public:

  /**
   * @brief Gets the text of this button
   */
  std::string getText() const noexcept;

  /**
   * @brief Sets the text of this button
   *
   * @param text 
   */
  void setText(const char* text) noexcept;
  void setText(const std::string& text) noexcept;

  /**
   * @brief Returns the current pressed state
   */
  bool isPressed() noexcept;

  /**
   * @details 
   * Returns whether the button was pressed at least once since the last call and
   * resets the flag. Toggling visibility or enabled states also reset the flag.
   */
  bool wasPressed() noexcept;
  Button(Button&& from) noexcept;
  Button& operator=(Button&& from) noexcept;
private:
  explicit Button(internal::ButtonPtr cPointer) noexcept;
  static internal::ControlPtr getControlPointer(internal::ButtonPtr cPointer) noexcept;
  internal::ButtonPtr ptr_{};
};


/**
 * @brief A dropdown selection control for switching between discrete modes
 */
class Dropdown : public Control {
  friend class ControlPanel;
public:

  /**
   * @brief Sets the list of available options
   *
   * @param options 
   * @param count 
   */
  void setOptions(const char** options, size_t count) noexcept;
  void setOptions(const std::vector<std::string>& options) noexcept;

  /**
   * @brief Returns the currently selected index
   */
  int getSelectedIndex() const noexcept;

  /**
   * @brief Sets the selected index
   *
   * @param index 
   */
  void setSelectedIndex(int index) noexcept;

  /**
   * @brief Adds an option to the list
   *
   * @param option 
   */
  void addOption(const char* option) noexcept;
  void addOption(const std::string& option) noexcept;

  /**
   * @details 
   * Returns whether the control value has changed since the last call and
   * resets the flag. Toggling visibility or enabled states also reset the flag.
   */
  bool wasChanged() noexcept;
  Dropdown(Dropdown&& from) noexcept;
  Dropdown& operator=(Dropdown&& from) noexcept;
private:
  explicit Dropdown(internal::DropdownPtr cPointer) noexcept;
  static internal::ControlPtr getControlPointer(internal::DropdownPtr cPointer) noexcept;
  internal::DropdownPtr ptr_{};
};


/**
 * @brief A control that displays some text or value
 */
class Label : public Control {
  friend class ControlPanel;
public:

  /**
   * @brief Gets the text of this status label
   */
  std::string getText() const noexcept;

  /**
   * @brief Sets the text of this status label
   *
   * @param text 
   */
  void setText(const char* text) noexcept;
  void setText(const std::string& text) noexcept;

  /**
   * @brief Gets the current numeric value (may be NaN if text was set last)
   */
  double getValue() const noexcept;

  /**
   * @brief Sets the numeric value of this label using a standard engineering format
   *
   * @param value 
   */
  void setValue(double value) noexcept;
  Label(Label&& from) noexcept;
  Label& operator=(Label&& from) noexcept;
private:
  explicit Label(internal::LabelPtr cPointer) noexcept;
  static internal::ControlPtr getControlPointer(internal::LabelPtr cPointer) noexcept;
  internal::LabelPtr ptr_{};
};


/**
 * @brief A slider control for numeric input
 */
class Slider : public Control {
  friend class ControlPanel;
public:

  /**
   * @brief Sets the possible range of the slider [min, max]
   *
   * @param min 
   * @param max 
   */
  void setLimits(double min, double max) noexcept;

  /**
   * @brief Gets the maximum value of the slider range
   */
  double getMax() const noexcept;

  /**
   * @brief Sets the maximum value of the slider range. Must be greater than min.
   *
   * @param max 
   */
  void setMax(double max) noexcept;

  /**
   * @brief Gets the minimum value of the slider range
   */
  double getMin() const noexcept;

  /**
   * @brief Sets the minimum value of the slider range. Must be less than max.
   *
   * @param min 
   */
  void setMin(double min) noexcept;

  /**
   * @brief Gets the current value of the slider
   */
  double getValue() const noexcept;

  /**
   * @brief Sets the value of this slider
   *
   * @param value 
   */
  void setValue(double value) noexcept;

  /**
   * @details 
   * Returns whether the control value has changed since the last call and
   * resets the flag. Toggling visibility or enabled states also reset the flag.
   */
  bool wasChanged() noexcept;
  Slider(Slider&& from) noexcept;
  Slider& operator=(Slider&& from) noexcept;
private:
  explicit Slider(internal::SliderPtr cPointer) noexcept;
  static internal::ControlPtr getControlPointer(internal::SliderPtr cPointer) noexcept;
  internal::SliderPtr ptr_{};
};


/**
 * @brief A boolean toggle switch for on/off states
 */
class Toggle : public Control {
  friend class ControlPanel;
public:

  /**
   * @brief Returns the current toggle state
   */
  bool isSelected() const noexcept;

  /**
   * @brief Sets the toggle state
   *
   * @param selected 
   */
  void setSelected(bool selected) noexcept;

  /**
   * @details 
   * Returns whether the control value has changed since the last call and
   * resets the flag. Toggling visibility or enabled states also reset the flag.
   */
  bool wasChanged() noexcept;
  Toggle(Toggle&& from) noexcept;
  Toggle& operator=(Toggle&& from) noexcept;
private:
  explicit Toggle(internal::TogglePtr cPointer) noexcept;
  static internal::ControlPtr getControlPointer(internal::TogglePtr cPointer) noexcept;
  internal::TogglePtr ptr_{};
};


/**
 * @brief A panel containing a list of interactive controls like buttons, sliders, and labels
 */
class ControlPanel {
  friend class GridWindow;
public:

  /**
   * @brief Gets the title
   */
  std::string getTitle() const noexcept;

  /**
   * @brief Sets the title
   *
   * @param title 
   */
  void setTitle(const char* title) noexcept;
  void setTitle(const std::string& title) noexcept;

  /**
   * @brief Gets the current width
   */
  double getWidth() const noexcept;

  /**
   * @brief Sets the desired width
   *
   * @param width 
   */
  void setWidth(double width) noexcept;

  /**
   * @brief Starts a new section with the given header
   *
   * @param title 
   * @throw on internal errors
   */
  void addSection(const char* title);
  void addSection(const std::string& title);

  /**
   * @brief Adds a label for displaying text to the control panel
   * @throw on internal errors
   */
  Label addLabel();

  /**
   * @brief Adds a button to the control panel
   * @throw on internal errors
   */
  Button addButton();

  /**
   * @brief Adds a 'start'-style button to the control panel
   * @throw on internal errors
   */
  Button addStartButton();

  /**
   * @brief Adds a 'stop'-style (red) button to the control panel
   * @throw on internal errors
   */
  Button addStopButton();

  /**
   * @brief Adds a slider to the control panel
   * @throw on internal errors
   */
  Slider addSlider();

  /**
   * @brief Adds a toggle to the control panel
   * @throw on internal errors
   */
  Toggle addToggle();

  /**
   * @brief Adds a dropdown choice selector to the control panel
   * @throw on internal errors
   */
  Dropdown addDropdown();
  ControlPanel(ControlPanel&& from) noexcept;
  ControlPanel& operator=(ControlPanel&& from) noexcept;
  ~ControlPanel() noexcept;
private:
  void cleanup() noexcept;
  explicit ControlPanel(internal::ControlPanelPtr cPointer) noexcept : ptr_(cPointer) {}
  internal::ControlPanelPtr ptr_{};
};


/**
 * @brief A vertical or horizontal cursor to measure or mark an axis value. Can be draggable.
 */
class Cursor {
  friend class XYChart;
public:

  /**
   * @brief Gets the editable state of the indicator
   */
  bool isEditable() const noexcept;

  /**
   * @brief Editable indicators can be dragged around by users
   *
   * @param editable 
   */
  void setEditable(bool editable) noexcept;

  /**
   * @brief Gets the indicator label
   */
  std::string getLabel() const noexcept;

  /**
   * @brief Sets the indicator label. Empty or null hides the label.
   *
   * @param label 
   */
  void setLabel(const char* label) noexcept;
  void setLabel(const std::string& label) noexcept;

  /**
   * @brief Gets the current value
   */
  double getValue() noexcept;

  /**
   * @brief Sets the indicated value. NaN hides the indicator.
   *
   * @param value 
   */
  void setValue(double value) noexcept;

  /**
   * @brief Gets the visibility in the chart
   */
  bool isVisible() const noexcept;

  /**
   * @brief Sets the visibility in the chart
   *
   * @param visible 
   */
  void setVisible(bool visible) noexcept;
  Cursor(Cursor&& from) noexcept;
  Cursor& operator=(Cursor&& from) noexcept;
  ~Cursor() noexcept;
private:
  void cleanup() noexcept;
  explicit Cursor(internal::CursorPtr cPointer) noexcept : ptr_(cPointer) {}
  internal::CursorPtr ptr_{};
};


/**
 * @brief A completely customizable view that is defined by FXML.
 */
class FxmlView {
  friend class GridWindow;
public:

  /**
   * @brief Gets the auto-reload state
   */
  bool isAutoReload() const noexcept;

  /**
   * @brief Sets the auto-reload state
   *
   * @param enableAutoReload 
   */
  void setAutoReload(bool enableAutoReload) noexcept;

  /**
   * @brief Gets the FXML source location
   */
  std::string getSource() const noexcept;

  /**
   * @brief Sets the file path or web-url to a .fxml file
   *
   * @param pathOrUrl path or url to an FXML file
   * @throw on internal errors
   */
  void setSource(const char* pathOrUrl);
  void setSource(const std::string& pathOrUrl);

  /**
   * @brief Creates a 2d line chart with the given size
   *
   * @param fxId 
   * @throw on internal errors
   */
  LineChart addLineChart(const char* fxId);
  LineChart addLineChart(const std::string& fxId);

  /**
   * @brief Creates a line chart with a pre-set time axis in [s]
   *
   * @param fxId 
   * @throw on internal errors
   */
  LineChart addScope(const char* fxId);
  LineChart addScope(const std::string& fxId);

  /**
   * @brief Creates a latency chart for displaying latency measurements [s]
   *
   * @param fxId 
   * @throw on internal errors
   */
  LatencyChart addLatencyChart(const char* fxId);
  LatencyChart addLatencyChart(const std::string& fxId);

  /**
   * @brief Creates a 3d chart with the given size
   *
   * @param fxId 
   * @throw on internal errors
   */
  Scene3d addScene3d(const char* fxId);
  Scene3d addScene3d(const std::string& fxId);

  /**
   * @brief Shows a shared-memory stream generated by hebi-video tools
   *
   * @param file 
   * @param fxId 
   * @throw on internal errors
   */
  StreamView addStreamView(const char* file, const char* fxId);
  StreamView addStreamView(const std::string& file, const std::string& fxId);

  /**
   * @brief Shows a panel for interactive controls
   *
   * @param fxId 
   * @throw on internal errors
   */
  FxmlView addFxmlView(const char* fxId);
  FxmlView addFxmlView(const std::string& fxId);
  FxmlView(FxmlView&& from) noexcept;
  FxmlView& operator=(FxmlView&& from) noexcept;
  ~FxmlView() noexcept;
private:
  void cleanup() noexcept;
  explicit FxmlView(internal::FxmlViewPtr cPointer) noexcept : ptr_(cPointer) {}
  internal::FxmlViewPtr ptr_{};
};


/**
 * @brief Represents a window containing an equally sized row/col grid
 */
class GridWindow {
public:

  /**
   * @brief Creates a grid of equally sized rows and columns
   *
   * @param rows 
   * @param cols 
   * @throw on internal errors
   */
  GridWindow(int rows = 1, int cols = 1);

  /**
   * @brief Gets the current full screen mode
   */
  bool isFullScreen() const noexcept;

  /**
   * @brief Enters or exits fullscreen mode. Does not apply to off screen windows
   *
   * @param fullScreen 
   */
  void setFullScreen(bool fullScreen) noexcept;

  /**
   * @brief Gets the content height in display points
   */
  int getHeight() const noexcept;

  /**
   * @brief Sets the content height in display points
   *
   * @param height 
   */
  void setHeight(int height) noexcept;

  /**
   * @brief Gets the current keepOpen state
   */
  bool getKeepOpen() const noexcept;

  /**
   * @brief Keeps the window open after the destructor gets called
   *
   * @param keepOpen 
   */
  void setKeepOpen(bool keepOpen) noexcept;

  /**
   * @brief Sets the window's screen location in pixels (x, y)
   *
   * @param xOffset 
   * @param yOffset 
   */
  void setLocation(int xOffset, int yOffset) noexcept;

  /**
   * @brief Sets the content size in display points (width, height), excluding the title bar.
   *
   * @param width 
   * @param height 
   */
  void setSize(int width, int height) noexcept;

  /**
   * @brief Gets the title of the window header bar
   */
  std::string getTitle() const noexcept;

  /**
   * @brief Sets the title of the window header bar
   *
   * @param title 
   */
  void setTitle(const char* title) noexcept;
  void setTitle(const std::string& title) noexcept;

  /**
   * @brief Gets the content width in display points
   */
  int getWidth() const noexcept;

  /**
   * @brief Sets the content width in display points
   *
   * @param width 
   */
  void setWidth(int width) noexcept;

  /**
   * @brief Gets the window's horizontal screen location in pixels
   */
  int getX() const noexcept;

  /**
   * @brief Sets the window's horizontal screen location in pixels
   *
   * @param x 
   */
  void setX(int x) noexcept;

  /**
   * @brief Gets the window's vertical screen location in pixels
   */
  int getY() const noexcept;

  /**
   * @brief Sets the window's vertical screen location in pixels
   *
   * @param y 
   */
  void setY(int y) noexcept;

  /**
   * @brief Creates a 2d line chart with the given size
   *
   * @param row 
   * @param col 
   * @param rowSpan 
   * @param colSpan 
   * @throw on internal errors
   */
  LineChart addLineChart(int row = 0, int col = 0, int rowSpan = 1, int colSpan = 1);

  /**
   * @brief Creates a line chart with a pre-set time axis in [s]
   *
   * @param row 
   * @param col 
   * @param rowSpan 
   * @param colSpan 
   * @throw on internal errors
   */
  LineChart addScope(int row = 0, int col = 0, int rowSpan = 1, int colSpan = 1);

  /**
   * @brief Creates a latency chart for recording latency measurements in [s]
   *
   * @param row 
   * @param col 
   * @param rowSpan 
   * @param colSpan 
   * @throw on internal errors
   */
  LatencyChart addLatencyChart(int row = 0, int col = 0, int rowSpan = 1, int colSpan = 1);

  /**
   * @brief Creates a 3d chart with the given size
   *
   * @param row 
   * @param col 
   * @param rowSpan 
   * @param colSpan 
   * @throw on internal errors
   */
  Scene3d addScene3d(int row = 0, int col = 0, int rowSpan = 1, int colSpan = 1);

  /**
   * @brief Shows a shared-memory stream generated by hebi-video tools
   *
   * @param file 
   * @param row 
   * @param col 
   * @param rowSpan 
   * @param colSpan 
   * @throw on internal errors
   */
  StreamView addStreamView(const char* file, int row = 0, int col = 0, int rowSpan = 1, int colSpan = 1);
  StreamView addStreamView(const std::string& file, int row = 0, int col = 0, int rowSpan = 1, int colSpan = 1);

  /**
   * @brief Shows a panel for interactive controls
   *
   * @param row 
   * @param col 
   * @param rowSpan 
   * @param colSpan 
   * @throw on internal errors
   */
  FxmlView addFxmlView(int row = 0, int col = 0, int rowSpan = 1, int colSpan = 1);

  /**
   * @brief Adds a CSS file to the root scene
   *
   * @param pathOrUrl path to a CSS file
   * @param autoReload true adds a hot-reload capability on file change
   * @throw on internal errors
   */
  void addStylesheet(const char* pathOrUrl, bool autoReload = false);
  void addStylesheet(const std::string& pathOrUrl, bool autoReload = false);

  /**
   * @brief Shows the window. May be called multiple times
   */
  void show() noexcept;

  /**
   * @brief [EXPERIMENTAL] Shows the window in a hidden utility window. Use this if you encounter issues with headless rendering.
   * @throw on internal errors
   */
  void showOffScreen();

  /**
   * @brief Hides the window. May be called multiple times. Hidden windows are not destroyed
   */
  void hide() noexcept;

  /**
   * @brief Returns whether the window is currently showing
   */
  bool isShowing() const noexcept;

  /**
   * @brief Waits until this window gets closed by the user
   * @throw when called from the FX thread
   */
  void waitUntilClosed() const;

  /**
   * @brief Returns a fixed-size panel on the side of the window that can be used for interactive controls
   * @throw on internal errors
   */
  ControlPanel getControlPanel();

  /**
   * @details 
   * [EXPERIMENTAL API]
   * Creates an image stream that continuously stores the content as images.
   * @throw on internal errors
   */
  ImageStream createImageStream();

  /**
   * @details 
   * [EXPERIMENTAL API]
   * Injects a mouse event into this window
   *
   * @param action 
   * @param button 
   * @param downMask 
   * @param modifiers 
   * @param x 
   * @param y 
   */
  void dispatchMouseEvent(int action, int button, int downMask, int modifiers, double x, double y) noexcept;

  /**
   * @details 
   * [EXPERIMENTAL API]
   * Injects a mouse event into this window
   *
   * @param x 
   * @param y 
   * @param delta_x 
   * @param delta_y 
   * @param modifiers 
   */
  void dispatchScrollEvent(double x, double y, double delta_x, double delta_y, int modifiers) noexcept;
  GridWindow(GridWindow&& from) noexcept;
  GridWindow& operator=(GridWindow&& from) noexcept;
  ~GridWindow() noexcept;
private:
  void cleanup() noexcept;
  internal::GridWindowPtr ptr_{};
};


/**
 * @brief Manages background logging of high-dynamic-range histograms to disk.
 */
class HdrHistogramRecorder {
public:
  HdrHistogramRecorder();
  double getFrequency() const noexcept;

  /**
   * @brief Sets the logging frequency (Hz) for background file updates.
   *
   * @param frequency hz
   * @throw on internal errors
   */
  void setFrequency(double frequency);
  double getMax() const noexcept;

  /**
   * @brief Sets the highest trackable value [s]. Values above this will throw an error.
   *
   * @param value seconds
   * @throw on internal errors
   */
  void setMax(double value);
  double getMin() const noexcept;

  /**
   * @brief Sets the lowest trackable value [s]. Values below this will be rounded up.
   *
   * @param value seconds
   * @throw on internal errors
   */
  void setMin(double value);
  int getSignificantDigits() const noexcept;

  /**
   * @brief Sets the number of significant decimal digits to maintain (1-5).
   *
   * @param significantDigits 
   * @throw on internal errors
   */
  void setSignificantDigits(int significantDigits);

  /**
   * @brief Creates a new single-writer trace that gets recorded in intervals.
   *
   * @param tag 
   */
  HdrHistogramTrace addTrace(const char* tag) noexcept;
  HdrHistogramTrace addTrace(const std::string& tag) noexcept;

  /**
   * @brief Starts the background recording thread. Returns the absolute normalized path.
   *
   * @param logFile Path to the .hlog file
   * @throw on internal errors
   */
  std::string startRecording(const char* logFile);
  std::string startRecording(const std::string& logFile);

  /**
   * @brief Stops the recording thread.
   */
  void stopRecording() noexcept;
  HdrHistogramRecorder(HdrHistogramRecorder&& from) noexcept;
  HdrHistogramRecorder& operator=(HdrHistogramRecorder&& from) noexcept;
  ~HdrHistogramRecorder() noexcept;
private:
  void cleanup() noexcept;
  internal::HdrHistogramRecorderPtr ptr_{};
};


/**
 * @brief A wait-free single-writer HdrHistogram record
 */
class HdrHistogramTrace {
  friend class HdrHistogramRecorder;
public:

  /**
   * @brief Maximum recorded value in [s].
   */
  double getMax() const noexcept;

  /**
   * @brief Arithmetic mean of recorded values in [s].
   */
  double getMean() const noexcept;

  /**
   * @brief Minimum recorded value in [s].
   */
  double getMin() const noexcept;

  /**
   * @brief Standard deviation of recorded values in [s].
   */
  double getStdDeviation() const noexcept;

  /**
   * @brief The internal tag
   */
  std::string getTag() const noexcept;

  /**
   * @brief The total number of samples recorded.
   */
  size_t getTotalCount() const noexcept;

  /**
   * @details 
   * Creates an unattached and untagged histogram. The local version removes
   * all synchronization overhead, but is not thread-safe.
   *
   * @param numberOfSignificantDigits 
   * @param minSeconds 
   * @param maxSeconds 
   */
  static HdrHistogramTrace createLocal(int numberOfSignificantDigits = 2, double minSeconds = 1e-9, double maxSeconds = 3600) noexcept;

  /**
   * @brief Returns the value at a specific percentile (0-100) in [s].
   *
   * @param percentile 
   */
  double getValueAtPercentile(double percentile) const noexcept;

  /**
   * @brief Sets a timestamp for subsequent toc calls
   */
  void tic() noexcept;

  /**
   * @brief Records the elapsed time since the last tic call. Returns the recorded value in [s]
   */
  double toc() noexcept;

  /**
   * @brief Calls tic and toc in one call. Returns the recorded value in [s]
   */
  double ticToc() noexcept;

  /**
   * @details 
   * Records a single latency value in seconds. Values outside the
   * min/max range are clamped. Returns the recorded value in [s]
   *
   * @param value latency in [s]
   */
  void recordValue(double value) noexcept;

  /**
   * @brief Records a latency value in seconds with a specific occurrence count.
   *
   * @param value latency in [s]
   * @param count occurrence count
   */
  void recordValueWithCount(double value, size_t count) noexcept;

  /**
   * @details 
   * Records a value in seconds with Coordinated Omission compensation.
   * If the value is larger than the expected interval, additional samples
   * are auto-generated to fill the gap.
   *
   * @param value latency in [s]
   * @param expectedIntervalBetweenSamples expected interval in [s]
   */
  void recordValueWithExpectedInterval(double value, double expectedIntervalBetweenSamples) noexcept;

  /**
   * @brief Reset the contents and stats of this histogram
   */
  void reset() noexcept;

  /**
   * @brief A string of the percentile distribution for plotting .hgrm files
   *
   * @param outputUnitsPerSecond output scale (ms=1e3, us=1e6, ns=1e9
   */
  std::string toHgrmString(double outputUnitsPerSecond = 1e6) const noexcept;

  /**
   * @details 
   * Saves the percentile distribution as an .hgrm file in the desired output units. This
   * can be loaded into standard hgrm plotting tools. Returns the absolute path to the output.
   *
   * @param fileName 
   * @param outputUnitsPerSecond output scale (ms=1e3, us=1e6, ns=1e9
   * @throw on internal errors
   */
  std::string saveAsHgrm(const char* fileName, double outputUnitsPerSecond = 1e6) const;
  std::string saveAsHgrm(const std::string& fileName, double outputUnitsPerSecond = 1e6) const;
  HdrHistogramTrace(HdrHistogramTrace&& from) noexcept;
  HdrHistogramTrace& operator=(HdrHistogramTrace&& from) noexcept;
  ~HdrHistogramTrace() noexcept;
private:
  void cleanup() noexcept;
  explicit HdrHistogramTrace(internal::HdrHistogramTracePtr cPointer) noexcept : ptr_(cPointer) {}
  internal::HdrHistogramTracePtr ptr_{};
};


/**
 * @details 
 * [EXPERIMENTAL API]
 * Represents a stream of images with an accessible pixel buffer. This class
 * is not thread-safe and should only be used from one thread. Buffers and metadata
 * for an image are only valid in between successful next() calls.
 * 
 * The stream reuses multiple buffers internally and provides efficient access to the raw memory.
 */
class ImageStream {
  friend class GridWindow;
public:

  /**
   * @brief @return the raw internal pixel buffer. Only valid between getNext calls.
   */
  void* getBuffer() const noexcept;

  /**
   * @brief @return the raw buffer size in bytes
   */
  size_t getBufferSize() const noexcept;

  /**
   * @brief @return the number of channels of the current frame (e.g. 4 for BGRA)
   */
  int getChannels() const noexcept;

  /**
   * @brief @return the height of the current frame in pixels
   */
  int getHeight() const noexcept;

  /**
   * @details 
   * Represents the pixel format of the current buffer. The native and most
   * performant format is BGRA_PRE, which stores pixels in adjacent bytes
   * with premultiplied alpha components.
   * 
   * Other convenience formats may be added in the future, but as of this point
   * the others are all experimental.
   * 
   * [Memory Layout (BGRA_PRE)]
   * Bytes are stored in order of increasing index: Blue, Green, Red, Alpha
   * 
   * [Compatibility]
   * With an Alpha of 255 BGRA_PRE is identical to BGRA and is binary-compatible with the following:
   * 
   *     OpenCV:    CV_8UC4
   * 
   *     wxWidgets: BitmapBufferFormat_ARGB32
   *                BitmapBufferFormat_RGB32
   * 
   *     Qt:        Format_ARGB32 (on little endian)
   *                Format_BGRA8888
   * 
   * Alpha less than 255 would show the image as darker or distorted, in which case
   * the channels would need to be un-multiplied first.
   * 
   * [Usage]
   * Pixels in this format can be decoded using the following sample code:
   * 
   *     int i = rowstart + x * 4;
   *     int blue  = buffer[i + 0] & 0xff;
   *     int green = buffer[i + 1] & 0xff;
   *     int red   = buffer[i + 2] & 0xff;
   *     int alpha = buffer[i + 3] & 0xff;
   * 
   * @return the pixel format of the current frame
   */
  PixelFormat getPixelFormat() const noexcept;

  /**
   * @details 
   * Sets the desired pixel format for future frames. The stream starts
   * with the default of BGRA_PRE. Setting Unknown also reverts back to
   * the default.
   * 
   * All other formats are considered experimental.
   *
   * @param pixelFormat 
   */
  void setPixelFormat(PixelFormat pixelFormat) const noexcept;

  /**
   * @brief An artificial rate limit above which frames get ignored
   */
  double getRateLimit() const noexcept;

  /**
   * @brief Sets the artificial rate limit above which frames get ignored.
   *
   * @param maxFramesPerSecond Rate limit in fps, e.g., 30.0
   * @throw on internal errors
   */
  void setRateLimit(double maxFramesPerSecond);

  /**
   * @brief Gets the number of threads used for encoding individual frames
   * @throw on internal errors
   */
  size_t getRecorderThreads();

  /**
   * @brief Sets the number of threads used for encoding individual frames
   *
   * @param numThreads 
   * @throw on internal errors
   */
  void setRecorderThreads(size_t numThreads);

  /**
   * @brief [EXPERIMENTAL API] Checks whether the stream is currently recording.
   * @throw on internal errors
   */
  bool isRecording();

  /**
   * @brief @return the logical to actual pixel scale that was applied to this snapshot
   */
  double getRenderScale() const noexcept;

  /**
   * @brief @return the logical to actual pixel scale that was applied to this snapshot in x
   */
  double getRenderScaleX() const noexcept;

  /**
   * @brief @return the logical to actual pixel scale that was applied to this snapshot in y
   */
  double getRenderScaleY() const noexcept;

  /**
   * @brief @return the sequence number of the current frame
   */
  size_t getSequence() const noexcept;

  /**
   * @details 
   * @return the number of bytes from the start of one row to the next.
   * This includes any padding for memory alignment.
   */
  int getStride() const noexcept;

  /**
   * @brief @return the timestamp of the current frame in seconds
   */
  double getTimestamp() const noexcept;

  /**
   * @brief @return the width of the current frame in pixels
   */
  int getWidth() const noexcept;

  /**
   * @brief Sets the resolution for future snapshots. Defaults to the initial resolution. Set 0 to auto-size.
   *
   * @param width 
   * @param height 
   * @throw on internal errors
   */
  void setResolution(int width, int height);

  /**
   * @details 
   * Waits until there is a new image, and flips internal buffers
   * as needed. Similar in behavior, but more efficient than.
   * 
   *     while (!tryGetNext() && !timeout) yield();
   * 
   * @return true if a new image is available
   *
   * @param maxTimeoutMillis zero waits forever
   * @throw on internal errors
   */
  bool waitForNext(size_t maxTimeoutMillis);

  /**
   * @details 
   * Checks whether there is a new image, and flips
   * internal buffers as needed. Any metadata is only
   * valid until the next call.
   * 
   * @return true if a new image is available
   */
  bool tryGetNext() noexcept;

  /**
   * @details 
   * [EXPERIMENTAL API]
   * Losslessly records individual frames to disk in a way that ffmpeg can convert.
   * 
   * The base name represents the file name without the extension. Images get stored
   * in dir/<base>/*.png and the result will be in dir/<base>.<extension>.
   *
   * @param baseName The target base name
   * @param overwrite Whether the recorder is allowed to overwrite existing files
   * @throw IO errors when creating the directory
   */
  void startRecording(const char* baseName, bool overwrite = false);
  void startRecording(const std::string& baseName, bool overwrite = false);

  /**
   * @brief [EXPERIMENTAL API] Blocking call that stops recording and returns the result.
   * @throw on internal errors
   */
  RecordingResult stopRecording();

  /**
   * @brief Saves the image to a file
   *
   * @param fileName 
   * @throw on internal errors
   */
  void saveToFile(const char* fileName) const;
  void saveToFile(const std::string& fileName) const;
  ImageStream(ImageStream&& from) noexcept;
  ImageStream& operator=(ImageStream&& from) noexcept;
  ~ImageStream() noexcept;
private:
  void cleanup() noexcept;
  explicit ImageStream(internal::ImageStreamPtr cPointer) noexcept : ptr_(cPointer) {}
  internal::ImageStreamPtr ptr_{};
};


/**
 * @details 
 * Provides various time related functionality for timing, benchmarking,
 * and scheduling loops. On Windows, using any method will request an
 * interrupt timer of 1ms - beginTimePeriod(1).
 */
class LoopTimer {
public:
  LoopTimer();

  /**
   * @brief Returns the elapsed time in seconds since the starting point.
   */
  double getElapsedTime() const noexcept;
  double getFrequency() const noexcept;

  /**
   * @brief Sets the desired frequency for waitForNextTick
   *
   * @param frequency hz
   */
  void setFrequency(double frequency) noexcept;
  double getPeriod() const noexcept;

  /**
   * @brief Sets the desired period of waitForNextTick
   *
   * @param seconds seconds
   */
  void setPeriod(double seconds) noexcept;

  /**
   * @details 
   * Resets the internal start time offset to now. This gets
   * used to determine the elapsed time and the starting point
   * for periodic ticks.
   */
  void restart() noexcept;

  /**
   * @brief Sets a timestamp for subsequent toc calls
   */
  void tic() noexcept;

  /**
   * @brief Returns the elapsed time in seconds since the last tic call
   */
  double toc() const noexcept;

  /**
   * @brief Calls tic and toc in one call
   */
  double ticToc() noexcept;

  /**
   * @details 
   * Waits until the next periodic tick counting from the starting point. For
   * example, if the start time is 12 with a period of 5, this call will wait
   * until the next tick (17, 22, 27, 32, ...) that occurs after the current
   * time.
   * Note that this is a best effort, and results depends on the OS scheduler
   * and load. Missed ticks will not be repeated.
   */
  void waitForNextTick() const noexcept;

  /**
   * @details 
   * Returns the remaining time in seconds until the next periodic tick counting
   * from the starting point. For example, if the start time is 12 with a period
   * of 5, this call returns the time to the next tick (17, 22, 27, 32, ...) that
   * occurs after the current time. Missed ticks will not be repeated. This method
   * can never return zero, so  users do not need to special case.
   */
  double getSecondsToNextTick() const noexcept;

  /**
   * @details 
   * Returns the remaining time in nanoseconds until the next periodic tick counting
   * from the starting point. For example, if the start time is 12 with a period
   * of 5, this call returns the time to the next tick (17, 22, 27, 32, ...) that
   * occurs after the current time. Missed ticks will not be repeated. This method
   * can never return zero, so  users do not need to special case.
   */
  size_t getNanosToNextTick() const noexcept;

  /**
   * @brief Seconds since some arbitrary fixed point. Monotonic high-resolution timer for measuring elapsed time deltas.
   */
  static double timeSeconds() noexcept;

  /**
   * @brief Nanoseconds since some arbitrary fixed point. Monotonic high-resolution timer for measuring elapsed time deltas.
   */
  static size_t timeNanos() noexcept;

  /**
   * @details 
   * Experimental (may be removed in the future):
   * Attempts to sleep for the given number of nanoseconds using Thread::sleep. Results are
   * best effort and depend on the platform. Threads might spuriously wake up early or be late.
   *
   * @param nanos Positive number of nanoseconds. Zero returns immediately.
   */
  static void sleepNanos(size_t nanos) noexcept;

  /**
   * @details 
   * Experimental (may be removed in the future):
   * Attempts to sleep for the given number of nanoseconds using LockSupport::park. Results are
   * best effort and depend on the platform. Threads might spuriously wake up early or be late.
   *
   * @param nanos Positive number of nanoseconds. Zero returns immediately.
   */
  static void parkNanos(size_t nanos) noexcept;

  /**
   * @details 
   * Experimental (may be removed in the future):
   * Attempts to sleep for the given number of nanoseconds using Object::wait. Results are
   * best effort and depend on the platform. Threads might spuriously wake up early or be late.
   *
   * @param nanos Positive number of nanoseconds. Zero returns immediately.
   */
  static void waitNanos(size_t nanos) noexcept;

  /**
   * @details 
   * Experimental (may be removed in the future):
   * Attempts to "sleep" for the given number of nanoseconds using tiered spin methods for a more
   * accurate result in exchange for higher CPU cost. Results are best effort and depend on the
   * platform. Even busy spins may be late depending on the OS scheduler.
   *
   * @param nanos Positive number of nanoseconds. Zero returns immediately.
   */
  static void spinNanos(size_t nanos) noexcept;
  LoopTimer(LoopTimer&& from) noexcept;
  LoopTimer& operator=(LoopTimer&& from) noexcept;
  ~LoopTimer() noexcept;
private:
  void cleanup() noexcept;
  internal::LoopTimerPtr ptr_{};
};


/**
 * @brief Represents a generic 3d object that can be rotated and translated
 */
class Object3d {
public:

  /**
   * @brief Gets the current visibility state of this node
   */
  bool isVisible() const noexcept;

  /**
   * @brief Sets visibility for this object. Hidden objects are not removed from the SceneGraph
   *
   * @param visible 
   */
  void setVisible(bool visible) noexcept;

  /**
   * @details 
   * Sets the orientation of the object using a unit quaternion (x, y, z, w).
   * This follows the ROS/REP-103 convention where the scalar component 'w' is last.
   * The input is not verified!
   *
   * @param qx quaternion x (vector part)
   * @param qy quaternion y (vector part)
   * @param qz quaternion z (vector part)
   * @param qw quaternion w (scalar part)
   * @throw on internal errors
   */
  void setOrientation(double qx, double qy, double qz, double qw);

  /**
   * @details 
   * Sets the orientation of the object using Roll, Pitch, and Yaw (radians).
   * Follows the ROS/REP-103 convention (Extrinsic / Fixed-Axis XYZ):
   * 
   *     orientation = Rz(yaw)*Ry(pitch)*Rx(roll)
   * 
   * This method preserves the current translation. The input is not verified!
   *
   * @param roll angle in [rad]
   * @param pitch angle in [rad]
   * @param yaw angle in [rad]
   * @throw on internal errors
   */
  void setOrientationRPY(double roll, double pitch, double yaw);

  /**
   * @brief Translates the object. The input is not verified!
   *
   * @param x position x [m]
   * @param y position y [m]
   * @param z position z [m]
   * @throw on internal errors
   */
  void setTranslation(double x, double y, double z);

  /**
   * @details 
   * Sets the full pose (position and orientation) of the object in a
   * single atomic update. This follows the ROS/REP-103 convention
   * (Position + Quaternion).
   * 
   * Units: Translation in [m], Quaternion (x, y, z, w).
   * The input is not verified!
   *
   * @param x position x [m]
   * @param y position y [m]
   * @param z position z [m]
   * @param qx quaternion x (vector part)
   * @param qy quaternion y (vector part)
   * @param qz quaternion z (vector part)
   * @param qw quaternion w (scalar part)
   * @throw on internal errors
   */
  void setPose(double x, double y, double z, double qx, double qy, double qz, double qw);

  /**
   * @details 
   * Sets a 4x4 transform matrix of the form
   * 
   *     R R R x
   *     R R R y
   *     R R R z
   *     0 0 0 1
   * 
   * The transform needs to be of size=16 and include the
   * bottom row. The translation units are in meters.
   * The input is not verified.
   *
   * @param matrix pointer to 16 double elements
   * @param ordering corresponding memory layout of the 4x4 matrix
   * @throw on internal errors
   */
  void setTransform4x4(const double* matrix, MatrixOrdering ordering);
  void setTransform4x4(const std::vector<double>& matrix, MatrixOrdering ordering = MatrixOrdering::RowMajor);
  Object3d(Object3d&& from) noexcept;
  Object3d& operator=(Object3d&& from) noexcept;
  virtual ~Object3d() noexcept;
protected:
  explicit Object3d(internal::Object3dPtr cPointer) noexcept : ptr_(cPointer) {}
private:
  void cleanup() noexcept;
  internal::Object3dPtr ptr_{};
};


/**
 * @brief A triad that represents a frame
 */
class Frame : public Object3d {
  friend class Scene3d;
public:
  Frame(Frame&& from) noexcept;
  Frame& operator=(Frame&& from) noexcept;
private:
  explicit Frame(internal::FramePtr cPointer) noexcept;
  static internal::Object3dPtr getObject3dPointer(internal::FramePtr cPointer) noexcept;
  internal::FramePtr ptr_{};
};


/**
 * @brief Represents a static 3d mesh
 */
class Mesh : public Object3d {
  friend class Scene3d;
public:

  /**
   * @brief Returns whether the origin is moved to the center of the mesh
   */
  bool isCentered() const noexcept;

  /**
   * @brief Moves the origin to the center of the mesh.
   *
   * @param centered 
   */
  void setCentered(bool centered) noexcept;

  /**
   * @brief Gets the current scaling factor
   */
  double getScale() const noexcept;

  /**
   * @details 
   * Sets the scaling factor applied to the mesh. The internal
   * units are mm, so a mesh in meters would need to be scaled
   * by 1e-3 to render correctly.
   *
   * @param scaleUnitsToMillimeters 
   */
  void setScale(double scaleUnitsToMillimeters) noexcept;

  /**
   * @brief Gets the current display style (Original, Ghosted, etc.)
   */
  DisplayStyle getDisplayStyle() const noexcept;

  /**
   * @details 
   * Updates the visual representation of the mesh model.
   * 
   * This is typically used to visually distinguish between multiple states of
   * the same mesh, such as overlaying a semi-transparent 'Ghosted' target
   * pose over the 'Original' pose.
   *
   * @param style sets the display style
   */
  void setDisplayStyle(DisplayStyle style) noexcept;
  Mesh(Mesh&& from) noexcept;
  Mesh& operator=(Mesh&& from) noexcept;
private:
  explicit Mesh(internal::MeshPtr cPointer) noexcept;
  static internal::Object3dPtr getObject3dPointer(internal::MeshPtr cPointer) noexcept;
  internal::MeshPtr ptr_{};
};


/**
 * @brief Represents robot kinematics
 */
class Robot : public Object3d {
  friend class Scene3d;
public:

  /**
   * @brief Gets the current visual display style (Original, Ghosted, etc.)
   */
  DisplayStyle getDisplayStyle() const noexcept;

  /**
   * @details 
   * Updates the visual representation of the robot model.
   * 
   * This is typically used to visually distinguish between multiple states of
   * the same robot, such as overlaying a semi-transparent 'Ghosted' target
   * pose over the 'Original' pose.
   *
   * @param style sets the display style
   */
  void setDisplayStyle(DisplayStyle style) noexcept;

  /**
   * @brief Gets the number of joints (Degrees of Freedom) of this robot
   */
  size_t getDof() const noexcept;

  /**
   * @details 
   * Updates the robot model configuration (kinematics) using a vector of joint positions.
   * 
   * Units:
   *   - Revolute joints: [rad]
   *   - Prismatic joints: [m]
   * 
   * The order of the vector must match the joint definitions in the underlying model.
   * The 'length' parameter must exactly match the number of degrees of freedom (DOF)
   * returned by getDof().
   *
   * @param positions pointer to an array of joint positions
   * @param length number of joints (must match getDof)
   * @throw if position vector length does not match number of joints.
   */
  void setPositions(const double* positions, size_t length);
  void setPositions(const std::vector<double>& positions);
  Robot(Robot&& from) noexcept;
  Robot& operator=(Robot&& from) noexcept;
private:
  explicit Robot(internal::RobotPtr cPointer) noexcept;
  static internal::Object3dPtr getObject3dPointer(internal::RobotPtr cPointer) noexcept;
  internal::RobotPtr ptr_{};
};


/**
 * @brief A collection of related spatial coordinates
 */
class Series3d : public Object3d {
public:

  /**
   * @brief Gets the current dataset color
   */
  Color getColor() const noexcept;

  /**
   * @brief Sets the dataset color
   *
   * @param color 
   */
  void setColor(Color color) noexcept;
  Series3d(Series3d&& from) noexcept;
  Series3d& operator=(Series3d&& from) noexcept;
protected:
  explicit Series3d(internal::Series3dPtr cPointer) noexcept;
private:
  static internal::Object3dPtr getObject3dPointer(internal::Series3dPtr cPointer) noexcept;
  internal::Series3dPtr ptr_{};
};


/**
 * @details 
 * Represents a line in 3d space. Note that there are currently no
 * line primitives, so the rendering is platform dependent and the
 * performance is limited.
 */
class Line3d : public Series3d {
  friend class Scene3d;
public:

  /**
   * @details 
   * Sets the internal maximum point count for incrementally
   * adding points. May clear existing data.
   *
   * @param count maximum number of points
   */
  void setMaxPointCount(size_t count) noexcept;

  /**
   * @brief Clears all existing data
   */
  void clear() noexcept;

  /**
   * @details 
   * Replaces the entire dataset with the provided X/Y/Z content. This
   * operation copies the input data, so the caller retains ownership of
   * the memory. Sets the buffer capacity to match the input length and
   * clears any previous rolling history.
   *
   * @param x points
   * @param y points
   * @param z points
   * @param length number of x/y/z points
   */
  void setData(const double* x, const double* y, const double* z, size_t length) noexcept;
  void setData(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z) noexcept;

  /**
   * @details 
   * Appends multiple data points to the end of the internal rolling buffer.
   * This operation copies the input data, so the caller retains ownership
   * of the memory. If the total number of points exceeds the current capacity,
   * the oldest points are overwritten.
   *
   * @param x points
   * @param y points
   * @param z points
   * @param length number of x/y/z points
   */
  void addPoints(const double* x, const double* y, const double* z, size_t length) noexcept;
  void addPoints(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z) noexcept;

  /**
   * @details 
   * Adds one point to an internal rolling buffer. Once the maximum
   * point count is reached, it will overwrite the earliest data.
   *
   * @param x 
   * @param y 
   * @param z 
   */
  void addPoint(double x, double y, double z) noexcept;
  Line3d(Line3d&& from) noexcept;
  Line3d& operator=(Line3d&& from) noexcept;
private:
  explicit Line3d(internal::Line3dPtr cPointer) noexcept;
  static internal::Series3dPtr getSeries3dPointer(internal::Line3dPtr cPointer) noexcept;
  internal::Line3dPtr ptr_{};
};


/**
 * @details 
 * Represents points in 3d space. Each point gets rendered as
 * the specified marker shape. This is intended for markers
 * and is not appropriate for large scale lidar point clouds.
 */
class Points3d : public Series3d {
  friend class Scene3d;
public:

  /**
   * @brief Gets the geometry used to represent each point in the series
   */
  MarkerShape getMarkerShape() const noexcept;

  /**
   * @brief Sets the geometry used to represent each point in the series
   *
   * @param shape 
   */
  void setMarkerShape(MarkerShape shape) noexcept;

  /**
   * @brief Gets the characteristic size (diameter or side length) of the markers in [m]
   */
  double getMarkerSize() const noexcept;

  /**
   * @brief Sets the characteristic size (diameter or side length) of the markers in [m]
   *
   * @param value [m]
   */
  void setMarkerSize(double value) noexcept;

  /**
   * @brief Gets the current self illumination state
   */
  bool isSelfIllumination() const noexcept;

  /**
   * @brief Self illumination makes the shapes glow without an external light source (defaults to true)
   *
   * @param value 
   */
  void setSelfIllumination(bool value) noexcept;

  /**
   * @brief Vertex sharing reduces the complexity, but can result in poor lighting
   */
  bool isVertexSharing() const noexcept;

  /**
   * @brief Vertex sharing reduces the complexity, but can result in poor lighting
   *
   * @param value 
   */
  void setVertexSharing(bool value) noexcept;

  /**
   * @details 
   * Sets the internal maximum point count for incrementally
   * adding points. May clear existing data.
   *
   * @param count maximum number of points
   */
  void setMaxPointCount(size_t count) noexcept;

  /**
   * @brief Clears all existing data
   */
  void clear() noexcept;

  /**
   * @details 
   * Replaces the entire dataset with the provided X/Y/Z content. This
   * operation copies the input data, so the caller retains ownership of
   * the memory. Sets the buffer capacity to match the input length and
   * clears any previous rolling history.
   *
   * @param x points
   * @param y points
   * @param z points
   * @param length number of x/y/z points
   */
  void setData(const double* x, const double* y, const double* z, size_t length) noexcept;
  void setData(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z) noexcept;

  /**
   * @details 
   * Appends multiple data points to the end of the internal rolling buffer.
   * This operation copies the input data, so the caller retains ownership
   * of the memory. If the total number of points exceeds the current capacity,
   * the oldest points are overwritten.
   *
   * @param x points
   * @param y points
   * @param z points
   * @param length number of x/y/z points
   */
  void addPoints(const double* x, const double* y, const double* z, size_t length) noexcept;
  void addPoints(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z) noexcept;

  /**
   * @details 
   * Adds one point to an internal rolling buffer. Once the maximum
   * point count is reached, it will overwrite the earliest data.
   *
   * @param x 
   * @param y 
   * @param z 
   */
  void addPoint(double x, double y, double z) noexcept;
  Points3d(Points3d&& from) noexcept;
  Points3d& operator=(Points3d&& from) noexcept;
private:
  explicit Points3d(internal::Points3dPtr cPointer) noexcept;
  static internal::Series3dPtr getSeries3dPointer(internal::Points3dPtr cPointer) noexcept;
  internal::Points3dPtr ptr_{};
};


/**
 * @details 
 * [EXPERIMENTAL API]
 * Represents the result of a recording. Can be used to
 * get various statistics and/or trigger FFMpeg.
 */
class RecordingResult {
  friend class ImageStream;
public:

  /**
   * @brief The absolute path to the directory containing the PNG frames and manifest.
   * @throw on internal errors
   */
  std::string getDirectory() const;

  /**
   * @brief The number of frames dropped because the hardware (CPU/Disk) couldn't keep up.
   */
  size_t getDroppedCount() const noexcept;

  /**
   * @brief The total duration in seconds.
   */
  double getDuration() const noexcept;

  /**
   * @brief The actual average frames per second of the recorded sequence.
   */
  double getAverageFps() const noexcept;

  /**
   * @brief The absolute path to the manifest containing timing metadata.
   * @throw on internal errors
   */
  std::string getManifest() const;

  /**
   * @brief The number of frames successfully written to disk.
   */
  size_t getRecordedCount() const noexcept;

  /**
   * @brief The number of frames intentionally skipped due to the rate limit.
   */
  size_t getSkippedCount() const noexcept;

  /**
   * @brief The total number of hardware pulses/frames encountered during the session.
   */
  size_t getTotalFrames() const noexcept;

  /**
   * @details 
   * Generates an FFmpeg command that converts the stored PNG files into the specified video format.
   * The file name is the directory name w/ extension one level up. For example, an h264 format would
   * map as follows:
   * 
   *     input: experiments/test17/*.png
   *     output: experiments/test17.mp4
   * 
   * The delete directory flag appends a command that delete the input directory after a successful conversion.
   *
   * @param outputFormat target format
   * @param deleteDirectory cleanup after conversion
   * @throw on internal errors
   */
  std::string getFfmpegCommand(VideoOutputFormat outputFormat, bool deleteDirectory = false) const;

  /**
   * @details 
   * Runs an FFmpeg command that converts the stored PNG files into the specified video format.
   * The file name is the directory name w/ extension one level up. For example, an h264 format would
   * map as follows:
   * 
   *     input: experiments/test17/*.png
   *     output: experiments/test17.mp4
   * 
   * The delete directory flag appends a command that delete the input directory after a successful conversion.
   *
   * @param outputFormat target format
   * @param deleteDirectory cleanup after conversion
   * @throw on internal errors
   */
  std::string runFfmpeg(VideoOutputFormat outputFormat, bool deleteDirectory = false);
  RecordingResult(RecordingResult&& from) noexcept;
  RecordingResult& operator=(RecordingResult&& from) noexcept;
  ~RecordingResult() noexcept;
private:
  void cleanup() noexcept;
  explicit RecordingResult(internal::RecordingResultPtr cPointer) noexcept : ptr_(cPointer) {}
  internal::RecordingResultPtr ptr_{};
};


/**
 * @brief Represents a 3d scene that can render a variety of objects in 3d space
 */
class Scene3d {
  friend class FxmlView;
  friend class GridWindow;
public:

  /**
   * @brief Sets the boundaries of the 3D grid cage in [m]
   *
   * @param minX [m]
   * @param maxX [m]
   * @param minY [m]
   * @param maxY [m]
   * @param minZ [m]
   * @param maxZ [m]
   */
  void setGridBounds(double minX, double maxX, double minY, double maxY, double minZ, double maxZ) noexcept;

  /**
   * @brief Gets the maximum X boundary of the 3D grid cage in [m]
   */
  double getMaxX() const noexcept;

  /**
   * @brief Sets the maximum X boundary of the 3D grid cage in [m]
   *
   * @param val 
   */
  void setMaxX(double val) noexcept;

  /**
   * @brief Gets the maximum Y boundary of the 3D grid cage in [m]
   */
  double getMaxY() const noexcept;

  /**
   * @brief Sets the maximum Y boundary of the 3D grid cage in [m]
   *
   * @param val 
   */
  void setMaxY(double val) noexcept;

  /**
   * @brief Gets the maximum Z boundary of the 3D grid cage in [m]
   */
  double getMaxZ() const noexcept;

  /**
   * @brief Sets the maximum Z boundary of the 3D grid cage in [m]
   *
   * @param val 
   */
  void setMaxZ(double val) noexcept;

  /**
   * @brief Gets the minimum X boundary of the 3D grid cage in [m]
   */
  double getMinX() const noexcept;

  /**
   * @brief Sets the minimum X boundary of the 3D grid cage in [m]
   *
   * @param val 
   */
  void setMinX(double val) noexcept;

  /**
   * @brief Gets the minimum Y boundary of the 3D grid cage in [m]
   */
  double getMinY() const noexcept;

  /**
   * @brief Sets the minimum Y boundary of the 3D grid cage in [m]
   *
   * @param val 
   */
  void setMinY(double val) noexcept;

  /**
   * @brief Gets the minimum Z boundary of the 3D grid cage in [m]
   */
  double getMinZ() const noexcept;

  /**
   * @brief Sets the minimum Z boundary of the 3D grid cage in [m]
   *
   * @param val 
   */
  void setMinZ(double val) noexcept;

  /**
   * @brief Gets the distance between grid lines in [m]
   */
  double getGridStep() const noexcept;

  /**
   * @brief Sets the distance between grid lines in [m]
   *
   * @param gridStep Distance in [m]
   */
  void setGridStep(double gridStep) noexcept;

  /**
   * @brief Returns the camera of this 3d chart
   */
  Camera getCamera() noexcept;

  /**
   * @brief Adds a robot from a description file (.hrdf)
   *
   * @param pathOrUrl file path or web-url to a description file
   * @throw on internal errors
   */
  Robot addRobot(const char* pathOrUrl);
  Robot addRobot(const std::string& pathOrUrl);

  /**
   * @brief Adds a 3d mesh from a file (.obj)
   *
   * @param pathOrUrl file path or web-url to an .obj file
   * @throw on internal errors
   */
  Mesh addMesh(const char* pathOrUrl);
  Mesh addMesh(const std::string& pathOrUrl);

  /**
   * @brief Adds a triad that represents a right-handed coordinate frame
   *
   * @param lengthInMeters length of each axis in [m]
   * @throw on internal errors
   */
  Frame addFrame(double lengthInMeters = 0.03);

  /**
   * @brief Adds a 3D data series rendered as a continuous line
   * @throw on internal errors
   */
  Line3d addLine();

  /**
   * @brief Adds a 3D data series rendered as individual mesh objects
   * @throw on internal errors
   */
  Points3d addPoints();
  Scene3d(Scene3d&& from) noexcept;
  Scene3d& operator=(Scene3d&& from) noexcept;
  ~Scene3d() noexcept;
private:
  void cleanup() noexcept;
  explicit Scene3d(internal::Scene3dPtr cPointer) noexcept : ptr_(cPointer) {}
  internal::Scene3dPtr ptr_{};
};


/**
 * @brief Shows a shared-memory stream generated by hebi-video tools.
 */
class StreamView {
  friend class FxmlView;
  friend class GridWindow;
public:
  StreamView(StreamView&& from) noexcept;
  StreamView& operator=(StreamView&& from) noexcept;
  ~StreamView() noexcept;
private:
  void cleanup() noexcept;
  explicit StreamView(internal::StreamViewPtr cPointer) noexcept : ptr_(cPointer) {}
  internal::StreamViewPtr ptr_{};
};


/**
 * @brief Chart for XY series
 */
class XYChart {
public:

  /**
   * @brief Gets the title shown in the chart titlebar
   */
  std::string getTitle() const noexcept;

  /**
   * @brief Sets the title shown in the chart titlebar
   *
   * @param title 
   */
  void setTitle(const char* title) noexcept;
  void setTitle(const std::string& title) noexcept;

  /**
   * @brief Whether the renderer should assume X-axis data is strictly increasing for performance
   */
  bool getXAssumeSorted() const noexcept;

  /**
   * @brief Enable to speed up rendering of large datasets. Must be disabled for paths that 'wrap back' or loops.
   *
   * @param xAssumeSorted 
   */
  void setXAssumeSorted(bool xAssumeSorted) noexcept;

  /**
   * @brief Checks if auto SI-prefix scaling is enabled for the X-axis
   */
  bool isXAutoUnitScaling() const noexcept;

  /**
   * @brief Enables/disables auto SI-prefix scaling for X-axis (e.g., 0.001s -> 1ms)
   *
   * @param enabled 
   */
  void setXAutoUnitScaling(bool enabled) noexcept;

  /**
   * @brief Gets the X-axis label text
   */
  std::string getXLabel() const noexcept;

  /**
   * @brief Sets the X-axis label text
   *
   * @param label 
   */
  void setXLabel(const char* label) noexcept;
  void setXLabel(const std::string& label) noexcept;

  /**
   * @brief Sets the X-axis limits. Set nan for auto-ranging.
   *
   * @param min 
   * @param max 
   * @throw on internal errors
   */
  void setXLim(double min, double max);

  /**
   * @brief Gets the X-axis max limit
   */
  double getXMax() const noexcept;

  /**
   * @brief Sets the X-axis maximum limit
   *
   * @param max 
   */
  void setXMax(double max) noexcept;

  /**
   * @brief Gets the X-axis min limit
   */
  double getXMin() const noexcept;

  /**
   * @brief Sets the X-axis minimum limit
   *
   * @param min 
   */
  void setXMin(double min) noexcept;

  /**
   * @brief Gets the X-axis unit (e.g., 's')
   */
  std::string getXUnit() const noexcept;

  /**
   * @brief Sets the X-axis unit (e.g., 's')
   *
   * @param unit 
   */
  void setXUnit(const char* unit) noexcept;
  void setXUnit(const std::string& unit) noexcept;

  /**
   * @brief Checks if auto SI-prefix scaling is enabled for the Y-axis
   */
  bool isYAutoUnitScaling() const noexcept;

  /**
   * @brief Enables/disables auto SI-prefix scaling for Y-axis
   *
   * @param enabled 
   */
  void setYAutoUnitScaling(bool enabled) noexcept;

  /**
   * @brief Gets the Y-axis label text
   */
  std::string getYLabel() const noexcept;

  /**
   * @brief Sets the Y-axis label text
   *
   * @param label 
   */
  void setYLabel(const char* label) noexcept;
  void setYLabel(const std::string& label) noexcept;

  /**
   * @brief Sets the Y-axis limits. Set nan for auto-ranging.
   *
   * @param min 
   * @param max 
   */
  void setYLim(double min, double max) noexcept;

  /**
   * @brief Gets the Y-axis max limit
   */
  double getYMax() const noexcept;

  /**
   * @brief Sets the Y-axis maximum limit
   *
   * @param max 
   */
  void setYMax(double max) noexcept;

  /**
   * @brief Gets the Y-axis min limit
   */
  double getYMin() const noexcept;

  /**
   * @brief Sets the Y-axis minimum limit
   *
   * @param min 
   */
  void setYMin(double min) noexcept;

  /**
   * @brief Gets the Y-axis unit (e.g., 'V')
   */
  std::string getYUnit() const noexcept;

  /**
   * @brief Sets the Y-axis unit (e.g., 'V')
   *
   * @param unit 
   */
  void setYUnit(const char* unit) noexcept;
  void setYUnit(const std::string& unit) noexcept;

  /**
   * @brief Adds a draggable cursor to the X-axis
   */
  Cursor addXCursor() noexcept;

  /**
   * @brief Adds a draggable cursor to the Y-axis
   */
  Cursor addYCursor() noexcept;
  XYChart(XYChart&& from) noexcept;
  XYChart& operator=(XYChart&& from) noexcept;
  virtual ~XYChart() noexcept;
protected:
  explicit XYChart(internal::XYChartPtr cPointer) noexcept : ptr_(cPointer) {}
private:
  void cleanup() noexcept;
  internal::XYChartPtr ptr_{};
};


/**
 * @brief Shows latency measurements in HdrHistogram percentile format
 */
class LatencyChart : public XYChart {
  friend class FxmlView;
  friend class GridWindow;
public:

  /**
   * @brief Creates a new hdr histogram dataset
   *
   * @param name 
   * @throw on internal errors
   */
  LatencyTrace addTrace(const char* name);
  LatencyTrace addTrace(const std::string& name);
  LatencyChart(LatencyChart&& from) noexcept;
  LatencyChart& operator=(LatencyChart&& from) noexcept;
private:
  explicit LatencyChart(internal::LatencyChartPtr cPointer) noexcept;
  static internal::XYChartPtr getXYChartPointer(internal::LatencyChartPtr cPointer) noexcept;
  internal::LatencyChartPtr ptr_{};
};


/**
 * @brief Represents an XY line chart
 */
class LineChart : public XYChart {
  friend class FxmlView;
  friend class GridWindow;
public:

  /**
   * @brief Creates a new line series
   *
   * @param label 
   * @throw on internal errors
   */
  Line addLine(const char* label);
  Line addLine(const std::string& label);
  LineChart(LineChart&& from) noexcept;
  LineChart& operator=(LineChart&& from) noexcept;
private:
  explicit LineChart(internal::LineChartPtr cPointer) noexcept;
  static internal::XYChartPtr getXYChartPointer(internal::LineChartPtr cPointer) noexcept;
  internal::LineChartPtr ptr_{};
};


/**
 * @brief A collection of related coordinates inside an XYChart
 */
class XYSeries {
public:

  /**
   * @brief Gets the rendering color
   */
  Color getColor() const noexcept;

  /**
   * @brief Sets the rendering color
   *
   * @param color 
   */
  void setColor(Color color) noexcept;

  /**
   * @brief Gets the label shown in the chart legend
   */
  std::string getLabel() const noexcept;

  /**
   * @brief Sets the label shown in the chart legend
   *
   * @param label 
   */
  void setLabel(const char* label) noexcept;
  void setLabel(const std::string& label) noexcept;

  /**
   * @brief Gets the rendering style
   */
  LineStyle getLineStyle() const noexcept;

  /**
   * @brief Sets the rendering style
   *
   * @param lineStyle 
   */
  void setLineStyle(LineStyle lineStyle) noexcept;

  /**
   * @brief Gets the line width in display pixels
   */
  double getLineWidth() const noexcept;

  /**
   * @brief Sets the line width
   *
   * @param lineWidth width in [px]
   */
  void setLineWidth(double lineWidth) noexcept;

  /**
   * @brief Gets the marker size
   */
  double getMarkerSize() const noexcept;

  /**
   * @brief Sets the marker size
   *
   * @param markerSize 
   */
  void setMarkerSize(double markerSize) noexcept;

  /**
   * @brief Gets the marker type
   */
  MarkerType getMarkerType() const noexcept;

  /**
   * @brief Sets the marker type
   *
   * @param markerType 
   */
  void setMarkerType(MarkerType markerType) noexcept;

  /**
   * @brief Checks if this data set is shown in the legend
   */
  bool isShowInLegend() const noexcept;

  /**
   * @brief Shows or hides this data set from the legend
   *
   * @param showInLegend 
   */
  void setShowInLegend(bool showInLegend) noexcept;

  /**
   * @brief Gets the visibility in the chart
   */
  bool isVisible() const noexcept;

  /**
   * @brief Sets the visibility in the chart
   *
   * @param visible 
   */
  void setVisible(bool visible) noexcept;
  XYSeries(XYSeries&& from) noexcept;
  XYSeries& operator=(XYSeries&& from) noexcept;
  virtual ~XYSeries() noexcept;
protected:
  explicit XYSeries(internal::XYSeriesPtr cPointer) noexcept : ptr_(cPointer) {}
private:
  void cleanup() noexcept;
  internal::XYSeriesPtr ptr_{};
};


/**
 * @brief Represents a latency measurement that records latency values in the form of an HdrHistogram
 */
class LatencyTrace : public XYSeries {
  friend class LatencyChart;
public:

  /**
   * @brief Sets a timestamp for subsequent toc calls
   */
  void tic() noexcept;

  /**
   * @brief Records the elapsed time since the last tic call. Returns the recorded value in [s]
   */
  double toc() noexcept;

  /**
   * @brief Calls tic and toc in one call. Returns the recorded value in [s]
   */
  double ticToc() noexcept;

  /**
   * @brief Record a latency value in the histogram
   *
   * @param value the value to be recorded in [s]
   */
  void record(double value) noexcept;

  /**
   * @brief Record a value in the histogram (adding to the value's current count)
   *
   * @param value the value to be recorded in [s]
   * @param count the number of occurrences of this value to record
   */
  void recordWithCount(double value, size_t count) noexcept;

  /**
   * @details 
   * [EXPERIMENTAL - specific to HdrHistogram]
   * Record a value in the histogram.
   * To compensate for the loss of sampled values when a recorded value is larger than the expected interval
   * between value samples, Histogram will auto-generate an additional series of decreasingly-smaller
   * (down to the expectedIntervalBetweenValueSamples) value records.
   *
   * @param value the value to be recorded in [s]
   * @param expectedIntervalBetweenValueSamples If expectedIntervalBetweenValueSamples in [s] is larger than
  zero, an auto-generated value records as appropriate if value
  is larger than expectedIntervalBetweenValueSamples
  
   */
  void recordCompensated(double value, double expectedIntervalBetweenValueSamples) noexcept;
  void reset() noexcept;
  LatencyTrace(LatencyTrace&& from) noexcept;
  LatencyTrace& operator=(LatencyTrace&& from) noexcept;
private:
  explicit LatencyTrace(internal::LatencyTracePtr cPointer) noexcept;
  static internal::XYSeriesPtr getXYSeriesPointer(internal::LatencyTracePtr cPointer) noexcept;
  internal::LatencyTracePtr ptr_{};
};


/**
 * @details 
 * Represents a high-performance 2D line series optimized for real-time
 * telemetry.
 * Uses a double-buffered architecture with bounded rolling buffers to
 * decouple high-frequency data ingestion from the UI rendering pulse.
 */
class Line : public XYSeries {
  friend class LineChart;
public:

  /**
   * @brief Returns the maximum number of points this line can hold before rolling
   */
  size_t getMaxPointCount() const noexcept;

  /**
   * @details 
   * Sets the maximum number of points the rolling buffer can hold.
   * Note: Changing the capacity clears all existing data. If the
   * new capacity matches the current value, this operation is a
   * no-op and data is preserved.
   *
   * @param count maximum number of points
   */
  void setMaxPointCount(size_t count) noexcept;

  /**
   * @brief Clears all existing data
   */
  void clear() noexcept;

  /**
   * @details 
   * Replaces the entire dataset with the provided X and Y content. This
   * operation copies the input data, so the caller retains ownership of
   * the memory. Sets the buffer capacity to match the input length and
   * clears any previous rolling history.
   *
   * @param x points
   * @param y points
   * @param length number of x/y points
   */
  void setData(const double* x, const double* y, size_t length) noexcept;
  void setData(const std::vector<double>& x, const std::vector<double>& y) noexcept;

  /**
   * @details 
   * Appends multiple data points to the end of the internal rolling buffer.
   * This operation copies the input data, so the caller retains ownership
   * of the memory. If the total number of points exceeds the current capacity,
   * the oldest points are overwritten.
   *
   * @param x points
   * @param y points
   * @param length number of x/y points
   */
  void addPoints(const double* x, const double* y, size_t length) noexcept;
  void addPoints(const std::vector<double>& x, const std::vector<double>& y) noexcept;

  /**
   * @details 
   * Adds one point to an internal rolling buffer. Once the maximum
   * point count is reached, it will overwrite the earliest data.
   *
   * @param x 
   * @param y 
   */
  void addPoint(double x, double y) noexcept;
  Line(Line&& from) noexcept;
  Line& operator=(Line&& from) noexcept;
private:
  explicit Line(internal::LinePtr cPointer) noexcept;
  static internal::XYSeriesPtr getXYSeriesPointer(internal::LinePtr cPointer) noexcept;
  internal::LinePtr ptr_{};
};


/**
 * @details 
 * Contains utility methods for working with the runtime. Some methods
 * are experimental and may change or be removed in the future.
 */
namespace runtime {

/**
 * @brief Applies runtime options. Needs to be done before any other methods.
 *
 * @param option 
 * @param value string value depending on the option (e.g. DpiScale='2.0', VerboseGraphics='true')
 * @throw on internal errors
 */
void setOption(RuntimeOption option, const char* value);
void setOption(RuntimeOption option, const std::string& value);

/**
 * @brief Sets an AtlantaFX theme for rendering the UI
 *
 * @param theme 
 */
void setTheme(Theme theme) noexcept;

/**
 * @brief Applies a global auto-close behavior, i.e., window::keepOpen
 *
 * @param autoClose 
 */
void setAutoCloseWindows(bool autoClose) noexcept;

/**
 * @brief Waits until all windows were closed by the user
 * @throw when called from the FX thread
 */
void waitUntilWindowsClosed();

/**
 * @brief Reclaims unused memory and prepares the heap for real-time execution
 */
void collect() noexcept;

/**
 * @brief Hides all active windows and dashboards
 */
void closeAll() noexcept;

/**
 * @brief Debug method to run code on the internal UI thread
 *
 * @param func 
 * @param userData 
 */
void runOnUiThread(UserCallbackFunction func, void* userData) noexcept;

/**
 * @brief Debug method that prints the last exception encountered on the current thread
 */
void printLastErrorDetails() noexcept;

/**
 * @brief Debug method to print internal thread information. May be removed in the future.
 *
 * @param name 
 */
void printThreadInfo(const char* name) noexcept;
void printThreadInfo(const std::string& name) noexcept;

/**
 * @details 
 * Returns an address to a c string that contains the last error message.
 * This address is only valid until the next call to this method from the
 * same thread. Never returns nullptr. Do not free the address!
 */
std::string getLastErrorString() noexcept;
} // namespace runtime

// ==== C Library Lookup ====
namespace {
struct DynamicLookup {
public:
  static DynamicLookup& instance() {
#ifdef WIN32
    static DynamicLookup lib("hebi_charts.dll");
#elif __APPLE__
    static DynamicLookup lib("libhebi_charts.dylib");
#else
    static DynamicLookup lib("./libhebi_charts.so", "libhebi_charts.so");
#endif
    return lib;
  }

  bool isLoaded() const { return lib_ != nullptr; };

  template<typename FuncType> FuncType getFunc(const char* func) const;
  DynamicLookup(const DynamicLookup&) = delete;
  DynamicLookup &operator = (const DynamicLookup&) = delete;

private:
  explicit DynamicLookup(const std::string& path, const std::string& alt_path = "");
  void* lib_{};
  std::string path_{};
};
} // namespace

// ==== C++ Implementations ====
// Camera
inline void Camera::setView(CameraView view) noexcept {
  static auto hebi_charts_Camera_setView = DynamicLookup::instance().getFunc<void(*)(internal::CameraPtr, CameraView)>("hebi_charts_Camera_setView");
  hebi_charts_Camera_setView(ptr_, view);
}
inline void Camera::reset() noexcept {
  static auto hebi_charts_Camera_reset = DynamicLookup::instance().getFunc<void(*)(internal::CameraPtr)>("hebi_charts_Camera_reset");
  hebi_charts_Camera_reset(ptr_);
}
inline void Camera::applyRotation(double qx, double qy, double qz, double qw) {
  static auto hebi_charts_Camera_applyRotation = DynamicLookup::instance().getFunc<int(*)(internal::CameraPtr, double, double, double, double)>("hebi_charts_Camera_applyRotation");
  int status_ = hebi_charts_Camera_applyRotation(ptr_, qx, qy, qz, qw);
  if (status_ != 0) {
    throw Exception("Encountered error in Camera::applyRotation");
  }
}
inline void Camera::setDistance(double distanceInMeters) {
  static auto hebi_charts_Camera_setDistance = DynamicLookup::instance().getFunc<int(*)(internal::CameraPtr, double)>("hebi_charts_Camera_setDistance");
  int status_ = hebi_charts_Camera_setDistance(ptr_, distanceInMeters);
  if (status_ != 0) {
    throw Exception("Encountered error in Camera::setDistance");
  }
}
inline void Camera::setPan(double x, double y, double z) {
  static auto hebi_charts_Camera_setPan = DynamicLookup::instance().getFunc<int(*)(internal::CameraPtr, double, double, double)>("hebi_charts_Camera_setPan");
  int status_ = hebi_charts_Camera_setPan(ptr_, x, y, z);
  if (status_ != 0) {
    throw Exception("Encountered error in Camera::setPan");
  }
}
inline void Camera::setControlsVisible(bool value) noexcept {
  static auto hebi_charts_Camera_setControlsVisible = DynamicLookup::instance().getFunc<void(*)(internal::CameraPtr, int)>("hebi_charts_Camera_setControlsVisible");
  hebi_charts_Camera_setControlsVisible(ptr_, value);
}
inline void Camera::cleanup() noexcept {
  if (ptr_ != nullptr) {
    static auto hebi_charts_Camera_release = DynamicLookup::instance().getFunc<void(*)(internal::CameraPtr)>("hebi_charts_Camera_release");
    hebi_charts_Camera_release(ptr_);
  }
}
inline Camera::Camera(Camera&& from) noexcept : ptr_(from.ptr_) {
  from.ptr_ = nullptr;
};
inline Camera& Camera::operator=(Camera&& from) noexcept {
  cleanup();
  ptr_ = from.ptr_;
  from.ptr_ = nullptr;
  return *this;
};
inline Camera::~Camera() noexcept {
  cleanup();
}

// Control
inline bool Control::isEnabled() const noexcept {
  static auto hebi_charts_Control_isEnabled = DynamicLookup::instance().getFunc<int(*)(internal::ControlPtr)>("hebi_charts_Control_isEnabled");
  return hebi_charts_Control_isEnabled(ptr_);
}
inline void Control::setEnabled(bool enabled) noexcept {
  static auto hebi_charts_Control_setEnabled = DynamicLookup::instance().getFunc<void(*)(internal::ControlPtr, int)>("hebi_charts_Control_setEnabled");
  hebi_charts_Control_setEnabled(ptr_, enabled);
}
inline std::string Control::getLabel() const noexcept {
  static auto hebi_charts_Control_getLabel = DynamicLookup::instance().getFunc<const char*(*)(internal::ControlPtr)>("hebi_charts_Control_getLabel");
  auto ptr = hebi_charts_Control_getLabel(ptr_);
  return !ptr ? std::string() : std::string(ptr); // copy utf8
}
inline void Control::setLabel(const char* name) noexcept {
  static auto hebi_charts_Control_setLabel = DynamicLookup::instance().getFunc<void(*)(internal::ControlPtr, const char*)>("hebi_charts_Control_setLabel");
  hebi_charts_Control_setLabel(ptr_, name);
}
inline void Control::setLabel(const std::string& name) noexcept {
  setLabel(name.c_str());
}
inline std::string Control::getTooltip() const noexcept {
  static auto hebi_charts_Control_getTooltip = DynamicLookup::instance().getFunc<const char*(*)(internal::ControlPtr)>("hebi_charts_Control_getTooltip");
  auto ptr = hebi_charts_Control_getTooltip(ptr_);
  return !ptr ? std::string() : std::string(ptr); // copy utf8
}
inline void Control::setTooltip(const char* tooltip) noexcept {
  static auto hebi_charts_Control_setTooltip = DynamicLookup::instance().getFunc<void(*)(internal::ControlPtr, const char*)>("hebi_charts_Control_setTooltip");
  hebi_charts_Control_setTooltip(ptr_, tooltip);
}
inline void Control::setTooltip(const std::string& tooltip) noexcept {
  setTooltip(tooltip.c_str());
}
inline bool Control::isVisible() const noexcept {
  static auto hebi_charts_Control_isVisible = DynamicLookup::instance().getFunc<int(*)(internal::ControlPtr)>("hebi_charts_Control_isVisible");
  return hebi_charts_Control_isVisible(ptr_);
}
inline void Control::setVisible(bool visible) noexcept {
  static auto hebi_charts_Control_setVisible = DynamicLookup::instance().getFunc<void(*)(internal::ControlPtr, int)>("hebi_charts_Control_setVisible");
  hebi_charts_Control_setVisible(ptr_, visible);
}
inline void Control::cleanup() noexcept {
  if (ptr_ != nullptr) {
    static auto hebi_charts_Control_release = DynamicLookup::instance().getFunc<void(*)(internal::ControlPtr)>("hebi_charts_Control_release");
    hebi_charts_Control_release(ptr_);
  }
}
inline Control::Control(Control&& from) noexcept : ptr_(from.ptr_) {
  from.ptr_ = nullptr;
};
inline Control& Control::operator=(Control&& from) noexcept {
  cleanup();
  ptr_ = from.ptr_;
  from.ptr_ = nullptr;
  return *this;
};
inline Control::~Control() noexcept {
  cleanup();
}

// Button
inline std::string Button::getText() const noexcept {
  static auto hebi_charts_Button_getText = DynamicLookup::instance().getFunc<const char*(*)(internal::ButtonPtr)>("hebi_charts_Button_getText");
  auto ptr = hebi_charts_Button_getText(ptr_);
  return !ptr ? std::string() : std::string(ptr); // copy utf8
}
inline void Button::setText(const char* text) noexcept {
  static auto hebi_charts_Button_setText = DynamicLookup::instance().getFunc<void(*)(internal::ButtonPtr, const char*)>("hebi_charts_Button_setText");
  hebi_charts_Button_setText(ptr_, text);
}
inline void Button::setText(const std::string& text) noexcept {
  setText(text.c_str());
}
inline bool Button::isPressed() noexcept {
  static auto hebi_charts_Button_isPressed = DynamicLookup::instance().getFunc<int(*)(internal::ButtonPtr)>("hebi_charts_Button_isPressed");
  return hebi_charts_Button_isPressed(ptr_);
}
inline bool Button::wasPressed() noexcept {
  static auto hebi_charts_Button_wasPressed = DynamicLookup::instance().getFunc<int(*)(internal::ButtonPtr)>("hebi_charts_Button_wasPressed");
  return hebi_charts_Button_wasPressed(ptr_);
}
inline internal::ControlPtr Button::getControlPointer(internal::ButtonPtr cPointer) noexcept {
  static auto hebi_charts_Button_to_Control = DynamicLookup::instance().getFunc<internal::ControlPtr(*)(internal::ButtonPtr)>("hebi_charts_Button_to_Control");
  return hebi_charts_Button_to_Control(cPointer);
}
inline Button::Button(internal::ButtonPtr cPointer) noexcept : Control(getControlPointer(cPointer)), ptr_(cPointer) {}
inline Button::Button(Button&& from) noexcept : Control(std::move(from)), ptr_(from.ptr_) {
  from.ptr_ = nullptr;
};
inline Button& Button::operator=(Button&& from) noexcept {
  Control::operator=(std::move(from));
  ptr_ = from.ptr_;
  from.ptr_ = nullptr;
  return *this;
};

// Dropdown
inline void Dropdown::setOptions(const char** options, size_t count) noexcept {
  static auto hebi_charts_Dropdown_setOptions = DynamicLookup::instance().getFunc<void(*)(internal::DropdownPtr, const char**, size_t)>("hebi_charts_Dropdown_setOptions");
  hebi_charts_Dropdown_setOptions(ptr_, options, count);
}
inline void Dropdown::setOptions(const std::vector<std::string>& options) noexcept {
  std::vector<const char*> cstrings;
  cstrings.reserve(options.size());
  for (const auto& s : options) cstrings.push_back(s.c_str());
  setOptions(cstrings.data(), cstrings.size());
}
inline int Dropdown::getSelectedIndex() const noexcept {
  static auto hebi_charts_Dropdown_getSelectedIndex = DynamicLookup::instance().getFunc<int(*)(internal::DropdownPtr)>("hebi_charts_Dropdown_getSelectedIndex");
  return hebi_charts_Dropdown_getSelectedIndex(ptr_);
}
inline void Dropdown::setSelectedIndex(int index) noexcept {
  static auto hebi_charts_Dropdown_setSelectedIndex = DynamicLookup::instance().getFunc<void(*)(internal::DropdownPtr, int)>("hebi_charts_Dropdown_setSelectedIndex");
  hebi_charts_Dropdown_setSelectedIndex(ptr_, index);
}
inline void Dropdown::addOption(const char* option) noexcept {
  static auto hebi_charts_Dropdown_addOption = DynamicLookup::instance().getFunc<void(*)(internal::DropdownPtr, const char*)>("hebi_charts_Dropdown_addOption");
  hebi_charts_Dropdown_addOption(ptr_, option);
}
inline void Dropdown::addOption(const std::string& option) noexcept {
  addOption(option.c_str());
}
inline bool Dropdown::wasChanged() noexcept {
  static auto hebi_charts_Dropdown_wasChanged = DynamicLookup::instance().getFunc<int(*)(internal::DropdownPtr)>("hebi_charts_Dropdown_wasChanged");
  return hebi_charts_Dropdown_wasChanged(ptr_);
}
inline internal::ControlPtr Dropdown::getControlPointer(internal::DropdownPtr cPointer) noexcept {
  static auto hebi_charts_Dropdown_to_Control = DynamicLookup::instance().getFunc<internal::ControlPtr(*)(internal::DropdownPtr)>("hebi_charts_Dropdown_to_Control");
  return hebi_charts_Dropdown_to_Control(cPointer);
}
inline Dropdown::Dropdown(internal::DropdownPtr cPointer) noexcept : Control(getControlPointer(cPointer)), ptr_(cPointer) {}
inline Dropdown::Dropdown(Dropdown&& from) noexcept : Control(std::move(from)), ptr_(from.ptr_) {
  from.ptr_ = nullptr;
};
inline Dropdown& Dropdown::operator=(Dropdown&& from) noexcept {
  Control::operator=(std::move(from));
  ptr_ = from.ptr_;
  from.ptr_ = nullptr;
  return *this;
};

// Label
inline std::string Label::getText() const noexcept {
  static auto hebi_charts_Label_getText = DynamicLookup::instance().getFunc<const char*(*)(internal::LabelPtr)>("hebi_charts_Label_getText");
  auto ptr = hebi_charts_Label_getText(ptr_);
  return !ptr ? std::string() : std::string(ptr); // copy utf8
}
inline void Label::setText(const char* text) noexcept {
  static auto hebi_charts_Label_setText = DynamicLookup::instance().getFunc<void(*)(internal::LabelPtr, const char*)>("hebi_charts_Label_setText");
  hebi_charts_Label_setText(ptr_, text);
}
inline void Label::setText(const std::string& text) noexcept {
  setText(text.c_str());
}
inline double Label::getValue() const noexcept {
  static auto hebi_charts_Label_getValue = DynamicLookup::instance().getFunc<double(*)(internal::LabelPtr)>("hebi_charts_Label_getValue");
  return hebi_charts_Label_getValue(ptr_);
}
inline void Label::setValue(double value) noexcept {
  static auto hebi_charts_Label_setValue = DynamicLookup::instance().getFunc<void(*)(internal::LabelPtr, double)>("hebi_charts_Label_setValue");
  hebi_charts_Label_setValue(ptr_, value);
}
inline internal::ControlPtr Label::getControlPointer(internal::LabelPtr cPointer) noexcept {
  static auto hebi_charts_Label_to_Control = DynamicLookup::instance().getFunc<internal::ControlPtr(*)(internal::LabelPtr)>("hebi_charts_Label_to_Control");
  return hebi_charts_Label_to_Control(cPointer);
}
inline Label::Label(internal::LabelPtr cPointer) noexcept : Control(getControlPointer(cPointer)), ptr_(cPointer) {}
inline Label::Label(Label&& from) noexcept : Control(std::move(from)), ptr_(from.ptr_) {
  from.ptr_ = nullptr;
};
inline Label& Label::operator=(Label&& from) noexcept {
  Control::operator=(std::move(from));
  ptr_ = from.ptr_;
  from.ptr_ = nullptr;
  return *this;
};

// Slider
inline void Slider::setLimits(double min, double max) noexcept {
  static auto hebi_charts_Slider_setLimits = DynamicLookup::instance().getFunc<void(*)(internal::SliderPtr, double, double)>("hebi_charts_Slider_setLimits");
  hebi_charts_Slider_setLimits(ptr_, min, max);
}
inline double Slider::getMax() const noexcept {
  static auto hebi_charts_Slider_getMax = DynamicLookup::instance().getFunc<double(*)(internal::SliderPtr)>("hebi_charts_Slider_getMax");
  return hebi_charts_Slider_getMax(ptr_);
}
inline void Slider::setMax(double max) noexcept {
  static auto hebi_charts_Slider_setMax = DynamicLookup::instance().getFunc<void(*)(internal::SliderPtr, double)>("hebi_charts_Slider_setMax");
  hebi_charts_Slider_setMax(ptr_, max);
}
inline double Slider::getMin() const noexcept {
  static auto hebi_charts_Slider_getMin = DynamicLookup::instance().getFunc<double(*)(internal::SliderPtr)>("hebi_charts_Slider_getMin");
  return hebi_charts_Slider_getMin(ptr_);
}
inline void Slider::setMin(double min) noexcept {
  static auto hebi_charts_Slider_setMin = DynamicLookup::instance().getFunc<void(*)(internal::SliderPtr, double)>("hebi_charts_Slider_setMin");
  hebi_charts_Slider_setMin(ptr_, min);
}
inline double Slider::getValue() const noexcept {
  static auto hebi_charts_Slider_getValue = DynamicLookup::instance().getFunc<double(*)(internal::SliderPtr)>("hebi_charts_Slider_getValue");
  return hebi_charts_Slider_getValue(ptr_);
}
inline void Slider::setValue(double value) noexcept {
  static auto hebi_charts_Slider_setValue = DynamicLookup::instance().getFunc<void(*)(internal::SliderPtr, double)>("hebi_charts_Slider_setValue");
  hebi_charts_Slider_setValue(ptr_, value);
}
inline bool Slider::wasChanged() noexcept {
  static auto hebi_charts_Slider_wasChanged = DynamicLookup::instance().getFunc<int(*)(internal::SliderPtr)>("hebi_charts_Slider_wasChanged");
  return hebi_charts_Slider_wasChanged(ptr_);
}
inline internal::ControlPtr Slider::getControlPointer(internal::SliderPtr cPointer) noexcept {
  static auto hebi_charts_Slider_to_Control = DynamicLookup::instance().getFunc<internal::ControlPtr(*)(internal::SliderPtr)>("hebi_charts_Slider_to_Control");
  return hebi_charts_Slider_to_Control(cPointer);
}
inline Slider::Slider(internal::SliderPtr cPointer) noexcept : Control(getControlPointer(cPointer)), ptr_(cPointer) {}
inline Slider::Slider(Slider&& from) noexcept : Control(std::move(from)), ptr_(from.ptr_) {
  from.ptr_ = nullptr;
};
inline Slider& Slider::operator=(Slider&& from) noexcept {
  Control::operator=(std::move(from));
  ptr_ = from.ptr_;
  from.ptr_ = nullptr;
  return *this;
};

// Toggle
inline bool Toggle::isSelected() const noexcept {
  static auto hebi_charts_Toggle_isSelected = DynamicLookup::instance().getFunc<int(*)(internal::TogglePtr)>("hebi_charts_Toggle_isSelected");
  return hebi_charts_Toggle_isSelected(ptr_);
}
inline void Toggle::setSelected(bool selected) noexcept {
  static auto hebi_charts_Toggle_setSelected = DynamicLookup::instance().getFunc<void(*)(internal::TogglePtr, int)>("hebi_charts_Toggle_setSelected");
  hebi_charts_Toggle_setSelected(ptr_, selected);
}
inline bool Toggle::wasChanged() noexcept {
  static auto hebi_charts_Toggle_wasChanged = DynamicLookup::instance().getFunc<int(*)(internal::TogglePtr)>("hebi_charts_Toggle_wasChanged");
  return hebi_charts_Toggle_wasChanged(ptr_);
}
inline internal::ControlPtr Toggle::getControlPointer(internal::TogglePtr cPointer) noexcept {
  static auto hebi_charts_Toggle_to_Control = DynamicLookup::instance().getFunc<internal::ControlPtr(*)(internal::TogglePtr)>("hebi_charts_Toggle_to_Control");
  return hebi_charts_Toggle_to_Control(cPointer);
}
inline Toggle::Toggle(internal::TogglePtr cPointer) noexcept : Control(getControlPointer(cPointer)), ptr_(cPointer) {}
inline Toggle::Toggle(Toggle&& from) noexcept : Control(std::move(from)), ptr_(from.ptr_) {
  from.ptr_ = nullptr;
};
inline Toggle& Toggle::operator=(Toggle&& from) noexcept {
  Control::operator=(std::move(from));
  ptr_ = from.ptr_;
  from.ptr_ = nullptr;
  return *this;
};

// ControlPanel
inline std::string ControlPanel::getTitle() const noexcept {
  static auto hebi_charts_ControlPanel_getTitle = DynamicLookup::instance().getFunc<const char*(*)(internal::ControlPanelPtr)>("hebi_charts_ControlPanel_getTitle");
  auto ptr = hebi_charts_ControlPanel_getTitle(ptr_);
  return !ptr ? std::string() : std::string(ptr); // copy utf8
}
inline void ControlPanel::setTitle(const char* title) noexcept {
  static auto hebi_charts_ControlPanel_setTitle = DynamicLookup::instance().getFunc<void(*)(internal::ControlPanelPtr, const char*)>("hebi_charts_ControlPanel_setTitle");
  hebi_charts_ControlPanel_setTitle(ptr_, title);
}
inline void ControlPanel::setTitle(const std::string& title) noexcept {
  setTitle(title.c_str());
}
inline double ControlPanel::getWidth() const noexcept {
  static auto hebi_charts_ControlPanel_getWidth = DynamicLookup::instance().getFunc<double(*)(internal::ControlPanelPtr)>("hebi_charts_ControlPanel_getWidth");
  return hebi_charts_ControlPanel_getWidth(ptr_);
}
inline void ControlPanel::setWidth(double width) noexcept {
  static auto hebi_charts_ControlPanel_setWidth = DynamicLookup::instance().getFunc<void(*)(internal::ControlPanelPtr, double)>("hebi_charts_ControlPanel_setWidth");
  hebi_charts_ControlPanel_setWidth(ptr_, width);
}
inline void ControlPanel::addSection(const char* title) {
  static auto hebi_charts_ControlPanel_addSection = DynamicLookup::instance().getFunc<int(*)(internal::ControlPanelPtr, const char*)>("hebi_charts_ControlPanel_addSection");
  int status_ = hebi_charts_ControlPanel_addSection(ptr_, title);
  if (status_ != 0) {
    throw Exception("Encountered error in ControlPanel::addSection");
  }
}
inline void ControlPanel::addSection(const std::string& title) {
  addSection(title.c_str());
}
inline Label ControlPanel::addLabel() {
  static auto hebi_charts_ControlPanel_addLabel = DynamicLookup::instance().getFunc<internal::LabelPtr(*)(internal::ControlPanelPtr)>("hebi_charts_ControlPanel_addLabel");
  auto ptr = hebi_charts_ControlPanel_addLabel(ptr_);
  if (!ptr) {
    throw Exception("Could not create Label in ControlPanel::addLabel");
  }
  return Label(ptr);
}
inline Button ControlPanel::addButton() {
  static auto hebi_charts_ControlPanel_addButton = DynamicLookup::instance().getFunc<internal::ButtonPtr(*)(internal::ControlPanelPtr)>("hebi_charts_ControlPanel_addButton");
  auto ptr = hebi_charts_ControlPanel_addButton(ptr_);
  if (!ptr) {
    throw Exception("Could not create Button in ControlPanel::addButton");
  }
  return Button(ptr);
}
inline Button ControlPanel::addStartButton() {
  static auto hebi_charts_ControlPanel_addStartButton = DynamicLookup::instance().getFunc<internal::ButtonPtr(*)(internal::ControlPanelPtr)>("hebi_charts_ControlPanel_addStartButton");
  auto ptr = hebi_charts_ControlPanel_addStartButton(ptr_);
  if (!ptr) {
    throw Exception("Could not create Button in ControlPanel::addStartButton");
  }
  return Button(ptr);
}
inline Button ControlPanel::addStopButton() {
  static auto hebi_charts_ControlPanel_addStopButton = DynamicLookup::instance().getFunc<internal::ButtonPtr(*)(internal::ControlPanelPtr)>("hebi_charts_ControlPanel_addStopButton");
  auto ptr = hebi_charts_ControlPanel_addStopButton(ptr_);
  if (!ptr) {
    throw Exception("Could not create Button in ControlPanel::addStopButton");
  }
  return Button(ptr);
}
inline Slider ControlPanel::addSlider() {
  static auto hebi_charts_ControlPanel_addSlider = DynamicLookup::instance().getFunc<internal::SliderPtr(*)(internal::ControlPanelPtr)>("hebi_charts_ControlPanel_addSlider");
  auto ptr = hebi_charts_ControlPanel_addSlider(ptr_);
  if (!ptr) {
    throw Exception("Could not create Slider in ControlPanel::addSlider");
  }
  return Slider(ptr);
}
inline Toggle ControlPanel::addToggle() {
  static auto hebi_charts_ControlPanel_addToggle = DynamicLookup::instance().getFunc<internal::TogglePtr(*)(internal::ControlPanelPtr)>("hebi_charts_ControlPanel_addToggle");
  auto ptr = hebi_charts_ControlPanel_addToggle(ptr_);
  if (!ptr) {
    throw Exception("Could not create Toggle in ControlPanel::addToggle");
  }
  return Toggle(ptr);
}
inline Dropdown ControlPanel::addDropdown() {
  static auto hebi_charts_ControlPanel_addDropdown = DynamicLookup::instance().getFunc<internal::DropdownPtr(*)(internal::ControlPanelPtr)>("hebi_charts_ControlPanel_addDropdown");
  auto ptr = hebi_charts_ControlPanel_addDropdown(ptr_);
  if (!ptr) {
    throw Exception("Could not create Dropdown in ControlPanel::addDropdown");
  }
  return Dropdown(ptr);
}
inline void ControlPanel::cleanup() noexcept {
  if (ptr_ != nullptr) {
    static auto hebi_charts_ControlPanel_release = DynamicLookup::instance().getFunc<void(*)(internal::ControlPanelPtr)>("hebi_charts_ControlPanel_release");
    hebi_charts_ControlPanel_release(ptr_);
  }
}
inline ControlPanel::ControlPanel(ControlPanel&& from) noexcept : ptr_(from.ptr_) {
  from.ptr_ = nullptr;
};
inline ControlPanel& ControlPanel::operator=(ControlPanel&& from) noexcept {
  cleanup();
  ptr_ = from.ptr_;
  from.ptr_ = nullptr;
  return *this;
};
inline ControlPanel::~ControlPanel() noexcept {
  cleanup();
}

// Cursor
inline bool Cursor::isEditable() const noexcept {
  static auto hebi_charts_Cursor_isEditable = DynamicLookup::instance().getFunc<int(*)(internal::CursorPtr)>("hebi_charts_Cursor_isEditable");
  return hebi_charts_Cursor_isEditable(ptr_);
}
inline void Cursor::setEditable(bool editable) noexcept {
  static auto hebi_charts_Cursor_setEditable = DynamicLookup::instance().getFunc<void(*)(internal::CursorPtr, int)>("hebi_charts_Cursor_setEditable");
  hebi_charts_Cursor_setEditable(ptr_, editable);
}
inline std::string Cursor::getLabel() const noexcept {
  static auto hebi_charts_Cursor_getLabel = DynamicLookup::instance().getFunc<const char*(*)(internal::CursorPtr)>("hebi_charts_Cursor_getLabel");
  auto ptr = hebi_charts_Cursor_getLabel(ptr_);
  return !ptr ? std::string() : std::string(ptr); // copy utf8
}
inline void Cursor::setLabel(const char* label) noexcept {
  static auto hebi_charts_Cursor_setLabel = DynamicLookup::instance().getFunc<void(*)(internal::CursorPtr, const char*)>("hebi_charts_Cursor_setLabel");
  hebi_charts_Cursor_setLabel(ptr_, label);
}
inline void Cursor::setLabel(const std::string& label) noexcept {
  setLabel(label.c_str());
}
inline double Cursor::getValue() noexcept {
  static auto hebi_charts_Cursor_getValue = DynamicLookup::instance().getFunc<double(*)(internal::CursorPtr)>("hebi_charts_Cursor_getValue");
  return hebi_charts_Cursor_getValue(ptr_);
}
inline void Cursor::setValue(double value) noexcept {
  static auto hebi_charts_Cursor_setValue = DynamicLookup::instance().getFunc<void(*)(internal::CursorPtr, double)>("hebi_charts_Cursor_setValue");
  hebi_charts_Cursor_setValue(ptr_, value);
}
inline bool Cursor::isVisible() const noexcept {
  static auto hebi_charts_Cursor_isVisible = DynamicLookup::instance().getFunc<int(*)(internal::CursorPtr)>("hebi_charts_Cursor_isVisible");
  return hebi_charts_Cursor_isVisible(ptr_);
}
inline void Cursor::setVisible(bool visible) noexcept {
  static auto hebi_charts_Cursor_setVisible = DynamicLookup::instance().getFunc<void(*)(internal::CursorPtr, int)>("hebi_charts_Cursor_setVisible");
  hebi_charts_Cursor_setVisible(ptr_, visible);
}
inline void Cursor::cleanup() noexcept {
  if (ptr_ != nullptr) {
    static auto hebi_charts_Cursor_release = DynamicLookup::instance().getFunc<void(*)(internal::CursorPtr)>("hebi_charts_Cursor_release");
    hebi_charts_Cursor_release(ptr_);
  }
}
inline Cursor::Cursor(Cursor&& from) noexcept : ptr_(from.ptr_) {
  from.ptr_ = nullptr;
};
inline Cursor& Cursor::operator=(Cursor&& from) noexcept {
  cleanup();
  ptr_ = from.ptr_;
  from.ptr_ = nullptr;
  return *this;
};
inline Cursor::~Cursor() noexcept {
  cleanup();
}

// FxmlView
inline bool FxmlView::isAutoReload() const noexcept {
  static auto hebi_charts_FxmlView_isAutoReload = DynamicLookup::instance().getFunc<int(*)(internal::FxmlViewPtr)>("hebi_charts_FxmlView_isAutoReload");
  return hebi_charts_FxmlView_isAutoReload(ptr_);
}
inline void FxmlView::setAutoReload(bool enableAutoReload) noexcept {
  static auto hebi_charts_FxmlView_setAutoReload = DynamicLookup::instance().getFunc<void(*)(internal::FxmlViewPtr, int)>("hebi_charts_FxmlView_setAutoReload");
  hebi_charts_FxmlView_setAutoReload(ptr_, enableAutoReload);
}
inline std::string FxmlView::getSource() const noexcept {
  static auto hebi_charts_FxmlView_getSource = DynamicLookup::instance().getFunc<const char*(*)(internal::FxmlViewPtr)>("hebi_charts_FxmlView_getSource");
  auto ptr = hebi_charts_FxmlView_getSource(ptr_);
  return !ptr ? std::string() : std::string(ptr); // copy utf8
}
inline void FxmlView::setSource(const char* pathOrUrl) {
  static auto hebi_charts_FxmlView_setSource = DynamicLookup::instance().getFunc<int(*)(internal::FxmlViewPtr, const char*)>("hebi_charts_FxmlView_setSource");
  int status_ = hebi_charts_FxmlView_setSource(ptr_, pathOrUrl);
  if (status_ != 0) {
    throw Exception("Encountered error in FxmlView::setSource");
  }
}
inline void FxmlView::setSource(const std::string& pathOrUrl) {
  setSource(pathOrUrl.c_str());
}
inline LineChart FxmlView::addLineChart(const char* fxId) {
  static auto hebi_charts_FxmlView_addLineChart = DynamicLookup::instance().getFunc<internal::LineChartPtr(*)(internal::FxmlViewPtr, const char*)>("hebi_charts_FxmlView_addLineChart");
  auto ptr = hebi_charts_FxmlView_addLineChart(ptr_, fxId);
  if (!ptr) {
    throw Exception("Could not create LineChart in FxmlView::addLineChart");
  }
  return LineChart(ptr);
}
inline LineChart FxmlView::addLineChart(const std::string& fxId) {
  return addLineChart(fxId.c_str());
}
inline LineChart FxmlView::addScope(const char* fxId) {
  static auto hebi_charts_FxmlView_addScope = DynamicLookup::instance().getFunc<internal::LineChartPtr(*)(internal::FxmlViewPtr, const char*)>("hebi_charts_FxmlView_addScope");
  auto ptr = hebi_charts_FxmlView_addScope(ptr_, fxId);
  if (!ptr) {
    throw Exception("Could not create LineChart in FxmlView::addScope");
  }
  return LineChart(ptr);
}
inline LineChart FxmlView::addScope(const std::string& fxId) {
  return addScope(fxId.c_str());
}
inline LatencyChart FxmlView::addLatencyChart(const char* fxId) {
  static auto hebi_charts_FxmlView_addLatencyChart = DynamicLookup::instance().getFunc<internal::LatencyChartPtr(*)(internal::FxmlViewPtr, const char*)>("hebi_charts_FxmlView_addLatencyChart");
  auto ptr = hebi_charts_FxmlView_addLatencyChart(ptr_, fxId);
  if (!ptr) {
    throw Exception("Could not create LatencyChart in FxmlView::addLatencyChart");
  }
  return LatencyChart(ptr);
}
inline LatencyChart FxmlView::addLatencyChart(const std::string& fxId) {
  return addLatencyChart(fxId.c_str());
}
inline Scene3d FxmlView::addScene3d(const char* fxId) {
  static auto hebi_charts_FxmlView_addScene3d = DynamicLookup::instance().getFunc<internal::Scene3dPtr(*)(internal::FxmlViewPtr, const char*)>("hebi_charts_FxmlView_addScene3d");
  auto ptr = hebi_charts_FxmlView_addScene3d(ptr_, fxId);
  if (!ptr) {
    throw Exception("Could not create Scene3d in FxmlView::addScene3d");
  }
  return Scene3d(ptr);
}
inline Scene3d FxmlView::addScene3d(const std::string& fxId) {
  return addScene3d(fxId.c_str());
}
inline StreamView FxmlView::addStreamView(const char* file, const char* fxId) {
  static auto hebi_charts_FxmlView_addStreamView = DynamicLookup::instance().getFunc<internal::StreamViewPtr(*)(internal::FxmlViewPtr, const char*, const char*)>("hebi_charts_FxmlView_addStreamView");
  auto ptr = hebi_charts_FxmlView_addStreamView(ptr_, file, fxId);
  if (!ptr) {
    throw Exception("Could not create StreamView in FxmlView::addStreamView");
  }
  return StreamView(ptr);
}
inline StreamView FxmlView::addStreamView(const std::string& file, const std::string& fxId) {
  return addStreamView(file.c_str(), fxId.c_str());
}
inline FxmlView FxmlView::addFxmlView(const char* fxId) {
  static auto hebi_charts_FxmlView_addFxmlView = DynamicLookup::instance().getFunc<internal::FxmlViewPtr(*)(internal::FxmlViewPtr, const char*)>("hebi_charts_FxmlView_addFxmlView");
  auto ptr = hebi_charts_FxmlView_addFxmlView(ptr_, fxId);
  if (!ptr) {
    throw Exception("Could not create FxmlView in FxmlView::addFxmlView");
  }
  return FxmlView(ptr);
}
inline FxmlView FxmlView::addFxmlView(const std::string& fxId) {
  return addFxmlView(fxId.c_str());
}
inline void FxmlView::cleanup() noexcept {
  if (ptr_ != nullptr) {
    static auto hebi_charts_FxmlView_release = DynamicLookup::instance().getFunc<void(*)(internal::FxmlViewPtr)>("hebi_charts_FxmlView_release");
    hebi_charts_FxmlView_release(ptr_);
  }
}
inline FxmlView::FxmlView(FxmlView&& from) noexcept : ptr_(from.ptr_) {
  from.ptr_ = nullptr;
};
inline FxmlView& FxmlView::operator=(FxmlView&& from) noexcept {
  cleanup();
  ptr_ = from.ptr_;
  from.ptr_ = nullptr;
  return *this;
};
inline FxmlView::~FxmlView() noexcept {
  cleanup();
}

// GridWindow
inline GridWindow::GridWindow(int rows, int cols) {
  static auto hebi_charts_GridWindow_create = DynamicLookup::instance().getFunc<internal::GridWindowPtr(*)(int, int)>("hebi_charts_GridWindow_create");
  ptr_ = hebi_charts_GridWindow_create(rows, cols);
  if (!ptr_) {
    throw Exception("Could not create GridWindow");
  }
}
inline bool GridWindow::isFullScreen() const noexcept {
  static auto hebi_charts_GridWindow_isFullScreen = DynamicLookup::instance().getFunc<int(*)(internal::GridWindowPtr)>("hebi_charts_GridWindow_isFullScreen");
  return hebi_charts_GridWindow_isFullScreen(ptr_);
}
inline void GridWindow::setFullScreen(bool fullScreen) noexcept {
  static auto hebi_charts_GridWindow_setFullScreen = DynamicLookup::instance().getFunc<void(*)(internal::GridWindowPtr, int)>("hebi_charts_GridWindow_setFullScreen");
  hebi_charts_GridWindow_setFullScreen(ptr_, fullScreen);
}
inline int GridWindow::getHeight() const noexcept {
  static auto hebi_charts_GridWindow_getHeight = DynamicLookup::instance().getFunc<int(*)(internal::GridWindowPtr)>("hebi_charts_GridWindow_getHeight");
  return hebi_charts_GridWindow_getHeight(ptr_);
}
inline void GridWindow::setHeight(int height) noexcept {
  static auto hebi_charts_GridWindow_setHeight = DynamicLookup::instance().getFunc<void(*)(internal::GridWindowPtr, int)>("hebi_charts_GridWindow_setHeight");
  hebi_charts_GridWindow_setHeight(ptr_, height);
}
inline bool GridWindow::getKeepOpen() const noexcept {
  static auto hebi_charts_GridWindow_getKeepOpen = DynamicLookup::instance().getFunc<int(*)(internal::GridWindowPtr)>("hebi_charts_GridWindow_getKeepOpen");
  return hebi_charts_GridWindow_getKeepOpen(ptr_);
}
inline void GridWindow::setKeepOpen(bool keepOpen) noexcept {
  static auto hebi_charts_GridWindow_setKeepOpen = DynamicLookup::instance().getFunc<void(*)(internal::GridWindowPtr, int)>("hebi_charts_GridWindow_setKeepOpen");
  hebi_charts_GridWindow_setKeepOpen(ptr_, keepOpen);
}
inline void GridWindow::setLocation(int xOffset, int yOffset) noexcept {
  static auto hebi_charts_GridWindow_setLocation = DynamicLookup::instance().getFunc<void(*)(internal::GridWindowPtr, int, int)>("hebi_charts_GridWindow_setLocation");
  hebi_charts_GridWindow_setLocation(ptr_, xOffset, yOffset);
}
inline void GridWindow::setSize(int width, int height) noexcept {
  static auto hebi_charts_GridWindow_setSize = DynamicLookup::instance().getFunc<void(*)(internal::GridWindowPtr, int, int)>("hebi_charts_GridWindow_setSize");
  hebi_charts_GridWindow_setSize(ptr_, width, height);
}
inline std::string GridWindow::getTitle() const noexcept {
  static auto hebi_charts_GridWindow_getTitle = DynamicLookup::instance().getFunc<const char*(*)(internal::GridWindowPtr)>("hebi_charts_GridWindow_getTitle");
  auto ptr = hebi_charts_GridWindow_getTitle(ptr_);
  return !ptr ? std::string() : std::string(ptr); // copy utf8
}
inline void GridWindow::setTitle(const char* title) noexcept {
  static auto hebi_charts_GridWindow_setTitle = DynamicLookup::instance().getFunc<void(*)(internal::GridWindowPtr, const char*)>("hebi_charts_GridWindow_setTitle");
  hebi_charts_GridWindow_setTitle(ptr_, title);
}
inline void GridWindow::setTitle(const std::string& title) noexcept {
  setTitle(title.c_str());
}
inline int GridWindow::getWidth() const noexcept {
  static auto hebi_charts_GridWindow_getWidth = DynamicLookup::instance().getFunc<int(*)(internal::GridWindowPtr)>("hebi_charts_GridWindow_getWidth");
  return hebi_charts_GridWindow_getWidth(ptr_);
}
inline void GridWindow::setWidth(int width) noexcept {
  static auto hebi_charts_GridWindow_setWidth = DynamicLookup::instance().getFunc<void(*)(internal::GridWindowPtr, int)>("hebi_charts_GridWindow_setWidth");
  hebi_charts_GridWindow_setWidth(ptr_, width);
}
inline int GridWindow::getX() const noexcept {
  static auto hebi_charts_GridWindow_getX = DynamicLookup::instance().getFunc<int(*)(internal::GridWindowPtr)>("hebi_charts_GridWindow_getX");
  return hebi_charts_GridWindow_getX(ptr_);
}
inline void GridWindow::setX(int x) noexcept {
  static auto hebi_charts_GridWindow_setX = DynamicLookup::instance().getFunc<void(*)(internal::GridWindowPtr, int)>("hebi_charts_GridWindow_setX");
  hebi_charts_GridWindow_setX(ptr_, x);
}
inline int GridWindow::getY() const noexcept {
  static auto hebi_charts_GridWindow_getY = DynamicLookup::instance().getFunc<int(*)(internal::GridWindowPtr)>("hebi_charts_GridWindow_getY");
  return hebi_charts_GridWindow_getY(ptr_);
}
inline void GridWindow::setY(int y) noexcept {
  static auto hebi_charts_GridWindow_setY = DynamicLookup::instance().getFunc<void(*)(internal::GridWindowPtr, int)>("hebi_charts_GridWindow_setY");
  hebi_charts_GridWindow_setY(ptr_, y);
}
inline LineChart GridWindow::addLineChart(int row, int col, int rowSpan, int colSpan) {
  static auto hebi_charts_GridWindow_addLineChart = DynamicLookup::instance().getFunc<internal::LineChartPtr(*)(internal::GridWindowPtr, int, int, int, int)>("hebi_charts_GridWindow_addLineChart");
  auto ptr = hebi_charts_GridWindow_addLineChart(ptr_, row, col, rowSpan, colSpan);
  if (!ptr) {
    throw Exception("Could not create LineChart in GridWindow::addLineChart");
  }
  return LineChart(ptr);
}
inline LineChart GridWindow::addScope(int row, int col, int rowSpan, int colSpan) {
  static auto hebi_charts_GridWindow_addScope = DynamicLookup::instance().getFunc<internal::LineChartPtr(*)(internal::GridWindowPtr, int, int, int, int)>("hebi_charts_GridWindow_addScope");
  auto ptr = hebi_charts_GridWindow_addScope(ptr_, row, col, rowSpan, colSpan);
  if (!ptr) {
    throw Exception("Could not create LineChart in GridWindow::addScope");
  }
  return LineChart(ptr);
}
inline LatencyChart GridWindow::addLatencyChart(int row, int col, int rowSpan, int colSpan) {
  static auto hebi_charts_GridWindow_addLatencyChart = DynamicLookup::instance().getFunc<internal::LatencyChartPtr(*)(internal::GridWindowPtr, int, int, int, int)>("hebi_charts_GridWindow_addLatencyChart");
  auto ptr = hebi_charts_GridWindow_addLatencyChart(ptr_, row, col, rowSpan, colSpan);
  if (!ptr) {
    throw Exception("Could not create LatencyChart in GridWindow::addLatencyChart");
  }
  return LatencyChart(ptr);
}
inline Scene3d GridWindow::addScene3d(int row, int col, int rowSpan, int colSpan) {
  static auto hebi_charts_GridWindow_addScene3d = DynamicLookup::instance().getFunc<internal::Scene3dPtr(*)(internal::GridWindowPtr, int, int, int, int)>("hebi_charts_GridWindow_addScene3d");
  auto ptr = hebi_charts_GridWindow_addScene3d(ptr_, row, col, rowSpan, colSpan);
  if (!ptr) {
    throw Exception("Could not create Scene3d in GridWindow::addScene3d");
  }
  return Scene3d(ptr);
}
inline StreamView GridWindow::addStreamView(const char* file, int row, int col, int rowSpan, int colSpan) {
  static auto hebi_charts_GridWindow_addStreamView = DynamicLookup::instance().getFunc<internal::StreamViewPtr(*)(internal::GridWindowPtr, const char*, int, int, int, int)>("hebi_charts_GridWindow_addStreamView");
  auto ptr = hebi_charts_GridWindow_addStreamView(ptr_, file, row, col, rowSpan, colSpan);
  if (!ptr) {
    throw Exception("Could not create StreamView in GridWindow::addStreamView");
  }
  return StreamView(ptr);
}
inline StreamView GridWindow::addStreamView(const std::string& file, int row, int col, int rowSpan, int colSpan) {
  return addStreamView(file.c_str(), row, col, rowSpan, colSpan);
}
inline FxmlView GridWindow::addFxmlView(int row, int col, int rowSpan, int colSpan) {
  static auto hebi_charts_GridWindow_addFxmlView = DynamicLookup::instance().getFunc<internal::FxmlViewPtr(*)(internal::GridWindowPtr, int, int, int, int)>("hebi_charts_GridWindow_addFxmlView");
  auto ptr = hebi_charts_GridWindow_addFxmlView(ptr_, row, col, rowSpan, colSpan);
  if (!ptr) {
    throw Exception("Could not create FxmlView in GridWindow::addFxmlView");
  }
  return FxmlView(ptr);
}
inline void GridWindow::addStylesheet(const char* pathOrUrl, bool autoReload) {
  static auto hebi_charts_GridWindow_addStylesheet = DynamicLookup::instance().getFunc<int(*)(internal::GridWindowPtr, const char*, int)>("hebi_charts_GridWindow_addStylesheet");
  int status_ = hebi_charts_GridWindow_addStylesheet(ptr_, pathOrUrl, autoReload);
  if (status_ != 0) {
    throw Exception("Encountered error in GridWindow::addStylesheet");
  }
}
inline void GridWindow::addStylesheet(const std::string& pathOrUrl, bool autoReload) {
  addStylesheet(pathOrUrl.c_str(), autoReload);
}
inline void GridWindow::show() noexcept {
  static auto hebi_charts_GridWindow_show = DynamicLookup::instance().getFunc<void(*)(internal::GridWindowPtr)>("hebi_charts_GridWindow_show");
  hebi_charts_GridWindow_show(ptr_);
}
inline void GridWindow::showOffScreen() {
  static auto hebi_charts_GridWindow_showOffScreen = DynamicLookup::instance().getFunc<int(*)(internal::GridWindowPtr)>("hebi_charts_GridWindow_showOffScreen");
  int status_ = hebi_charts_GridWindow_showOffScreen(ptr_);
  if (status_ != 0) {
    throw Exception("Encountered error in GridWindow::showOffScreen");
  }
}
inline void GridWindow::hide() noexcept {
  static auto hebi_charts_GridWindow_hide = DynamicLookup::instance().getFunc<void(*)(internal::GridWindowPtr)>("hebi_charts_GridWindow_hide");
  hebi_charts_GridWindow_hide(ptr_);
}
inline bool GridWindow::isShowing() const noexcept {
  static auto hebi_charts_GridWindow_isShowing = DynamicLookup::instance().getFunc<int(*)(internal::GridWindowPtr)>("hebi_charts_GridWindow_isShowing");
  return hebi_charts_GridWindow_isShowing(ptr_);
}
inline void GridWindow::waitUntilClosed() const {
  static auto hebi_charts_GridWindow_waitUntilClosed = DynamicLookup::instance().getFunc<int(*)(internal::GridWindowPtr)>("hebi_charts_GridWindow_waitUntilClosed");
  int status_ = hebi_charts_GridWindow_waitUntilClosed(ptr_);
  if (status_ != 0) {
    throw Exception("Encountered error in GridWindow::waitUntilClosed");
  }
}
inline ControlPanel GridWindow::getControlPanel() {
  static auto hebi_charts_GridWindow_getControlPanel = DynamicLookup::instance().getFunc<internal::ControlPanelPtr(*)(internal::GridWindowPtr)>("hebi_charts_GridWindow_getControlPanel");
  auto ptr = hebi_charts_GridWindow_getControlPanel(ptr_);
  if (!ptr) {
    throw Exception("Could not create ControlPanel in GridWindow::getControlPanel");
  }
  return ControlPanel(ptr);
}
inline ImageStream GridWindow::createImageStream() {
  static auto hebi_charts_GridWindow_createImageStream = DynamicLookup::instance().getFunc<internal::ImageStreamPtr(*)(internal::GridWindowPtr)>("hebi_charts_GridWindow_createImageStream");
  auto ptr = hebi_charts_GridWindow_createImageStream(ptr_);
  if (!ptr) {
    throw Exception("Could not create ImageStream in GridWindow::createImageStream");
  }
  return ImageStream(ptr);
}
inline void GridWindow::dispatchMouseEvent(int action, int button, int downMask, int modifiers, double x, double y) noexcept {
  static auto hebi_charts_GridWindow_dispatchMouseEvent = DynamicLookup::instance().getFunc<void(*)(internal::GridWindowPtr, int, int, int, int, double, double)>("hebi_charts_GridWindow_dispatchMouseEvent");
  hebi_charts_GridWindow_dispatchMouseEvent(ptr_, action, button, downMask, modifiers, x, y);
}
inline void GridWindow::dispatchScrollEvent(double x, double y, double delta_x, double delta_y, int modifiers) noexcept {
  static auto hebi_charts_GridWindow_dispatchScrollEvent = DynamicLookup::instance().getFunc<void(*)(internal::GridWindowPtr, double, double, double, double, int)>("hebi_charts_GridWindow_dispatchScrollEvent");
  hebi_charts_GridWindow_dispatchScrollEvent(ptr_, x, y, delta_x, delta_y, modifiers);
}
inline void GridWindow::cleanup() noexcept {
  if (ptr_ != nullptr) {
    static auto hebi_charts_GridWindow_release = DynamicLookup::instance().getFunc<void(*)(internal::GridWindowPtr)>("hebi_charts_GridWindow_release");
    hebi_charts_GridWindow_release(ptr_);
  }
}
inline GridWindow::GridWindow(GridWindow&& from) noexcept : ptr_(from.ptr_) {
  from.ptr_ = nullptr;
};
inline GridWindow& GridWindow::operator=(GridWindow&& from) noexcept {
  cleanup();
  ptr_ = from.ptr_;
  from.ptr_ = nullptr;
  return *this;
};
inline GridWindow::~GridWindow() noexcept {
  cleanup();
}

// HdrHistogramRecorder
inline HdrHistogramRecorder::HdrHistogramRecorder() {
  static auto hebi_charts_HdrHistogramRecorder_create = DynamicLookup::instance().getFunc<internal::HdrHistogramRecorderPtr(*)()>("hebi_charts_HdrHistogramRecorder_create");
  ptr_ = hebi_charts_HdrHistogramRecorder_create();
  if (!ptr_) {
    throw Exception("Could not create HdrHistogramRecorder");
  }
}
inline double HdrHistogramRecorder::getFrequency() const noexcept {
  static auto hebi_charts_HdrHistogramRecorder_getFrequency = DynamicLookup::instance().getFunc<double(*)(internal::HdrHistogramRecorderPtr)>("hebi_charts_HdrHistogramRecorder_getFrequency");
  return hebi_charts_HdrHistogramRecorder_getFrequency(ptr_);
}
inline void HdrHistogramRecorder::setFrequency(double frequency) {
  static auto hebi_charts_HdrHistogramRecorder_setFrequency = DynamicLookup::instance().getFunc<int(*)(internal::HdrHistogramRecorderPtr, double)>("hebi_charts_HdrHistogramRecorder_setFrequency");
  int status_ = hebi_charts_HdrHistogramRecorder_setFrequency(ptr_, frequency);
  if (status_ != 0) {
    throw Exception("Encountered error in HdrHistogramRecorder::setFrequency");
  }
}
inline double HdrHistogramRecorder::getMax() const noexcept {
  static auto hebi_charts_HdrHistogramRecorder_getMax = DynamicLookup::instance().getFunc<double(*)(internal::HdrHistogramRecorderPtr)>("hebi_charts_HdrHistogramRecorder_getMax");
  return hebi_charts_HdrHistogramRecorder_getMax(ptr_);
}
inline void HdrHistogramRecorder::setMax(double value) {
  static auto hebi_charts_HdrHistogramRecorder_setMax = DynamicLookup::instance().getFunc<int(*)(internal::HdrHistogramRecorderPtr, double)>("hebi_charts_HdrHistogramRecorder_setMax");
  int status_ = hebi_charts_HdrHistogramRecorder_setMax(ptr_, value);
  if (status_ != 0) {
    throw Exception("Encountered error in HdrHistogramRecorder::setMax");
  }
}
inline double HdrHistogramRecorder::getMin() const noexcept {
  static auto hebi_charts_HdrHistogramRecorder_getMin = DynamicLookup::instance().getFunc<double(*)(internal::HdrHistogramRecorderPtr)>("hebi_charts_HdrHistogramRecorder_getMin");
  return hebi_charts_HdrHistogramRecorder_getMin(ptr_);
}
inline void HdrHistogramRecorder::setMin(double value) {
  static auto hebi_charts_HdrHistogramRecorder_setMin = DynamicLookup::instance().getFunc<int(*)(internal::HdrHistogramRecorderPtr, double)>("hebi_charts_HdrHistogramRecorder_setMin");
  int status_ = hebi_charts_HdrHistogramRecorder_setMin(ptr_, value);
  if (status_ != 0) {
    throw Exception("Encountered error in HdrHistogramRecorder::setMin");
  }
}
inline int HdrHistogramRecorder::getSignificantDigits() const noexcept {
  static auto hebi_charts_HdrHistogramRecorder_getSignificantDigits = DynamicLookup::instance().getFunc<int(*)(internal::HdrHistogramRecorderPtr)>("hebi_charts_HdrHistogramRecorder_getSignificantDigits");
  return hebi_charts_HdrHistogramRecorder_getSignificantDigits(ptr_);
}
inline void HdrHistogramRecorder::setSignificantDigits(int significantDigits) {
  static auto hebi_charts_HdrHistogramRecorder_setSignificantDigits = DynamicLookup::instance().getFunc<int(*)(internal::HdrHistogramRecorderPtr, int)>("hebi_charts_HdrHistogramRecorder_setSignificantDigits");
  int status_ = hebi_charts_HdrHistogramRecorder_setSignificantDigits(ptr_, significantDigits);
  if (status_ != 0) {
    throw Exception("Encountered error in HdrHistogramRecorder::setSignificantDigits");
  }
}
inline HdrHistogramTrace HdrHistogramRecorder::addTrace(const char* tag) noexcept {
  static auto hebi_charts_HdrHistogramRecorder_addTrace = DynamicLookup::instance().getFunc<internal::HdrHistogramTracePtr(*)(internal::HdrHistogramRecorderPtr, const char*)>("hebi_charts_HdrHistogramRecorder_addTrace");
  auto ptr = hebi_charts_HdrHistogramRecorder_addTrace(ptr_, tag);
  return HdrHistogramTrace(ptr);
}
inline HdrHistogramTrace HdrHistogramRecorder::addTrace(const std::string& tag) noexcept {
  return addTrace(tag.c_str());
}
inline std::string HdrHistogramRecorder::startRecording(const char* logFile) {
  static auto hebi_charts_HdrHistogramRecorder_startRecording = DynamicLookup::instance().getFunc<const char*(*)(internal::HdrHistogramRecorderPtr, const char*)>("hebi_charts_HdrHistogramRecorder_startRecording");
  auto ptr = hebi_charts_HdrHistogramRecorder_startRecording(ptr_, logFile);
  return !ptr ? std::string() : std::string(ptr); // copy utf8
}
inline std::string HdrHistogramRecorder::startRecording(const std::string& logFile) {
  return startRecording(logFile.c_str());
}
inline void HdrHistogramRecorder::stopRecording() noexcept {
  static auto hebi_charts_HdrHistogramRecorder_stopRecording = DynamicLookup::instance().getFunc<void(*)(internal::HdrHistogramRecorderPtr)>("hebi_charts_HdrHistogramRecorder_stopRecording");
  hebi_charts_HdrHistogramRecorder_stopRecording(ptr_);
}
inline void HdrHistogramRecorder::cleanup() noexcept {
  if (ptr_ != nullptr) {
    static auto hebi_charts_HdrHistogramRecorder_release = DynamicLookup::instance().getFunc<void(*)(internal::HdrHistogramRecorderPtr)>("hebi_charts_HdrHistogramRecorder_release");
    hebi_charts_HdrHistogramRecorder_release(ptr_);
  }
}
inline HdrHistogramRecorder::HdrHistogramRecorder(HdrHistogramRecorder&& from) noexcept : ptr_(from.ptr_) {
  from.ptr_ = nullptr;
};
inline HdrHistogramRecorder& HdrHistogramRecorder::operator=(HdrHistogramRecorder&& from) noexcept {
  cleanup();
  ptr_ = from.ptr_;
  from.ptr_ = nullptr;
  return *this;
};
inline HdrHistogramRecorder::~HdrHistogramRecorder() noexcept {
  cleanup();
}

// HdrHistogramTrace
inline double HdrHistogramTrace::getMax() const noexcept {
  static auto hebi_charts_HdrHistogramTrace_getMax = DynamicLookup::instance().getFunc<double(*)(internal::HdrHistogramTracePtr)>("hebi_charts_HdrHistogramTrace_getMax");
  return hebi_charts_HdrHistogramTrace_getMax(ptr_);
}
inline double HdrHistogramTrace::getMean() const noexcept {
  static auto hebi_charts_HdrHistogramTrace_getMean = DynamicLookup::instance().getFunc<double(*)(internal::HdrHistogramTracePtr)>("hebi_charts_HdrHistogramTrace_getMean");
  return hebi_charts_HdrHistogramTrace_getMean(ptr_);
}
inline double HdrHistogramTrace::getMin() const noexcept {
  static auto hebi_charts_HdrHistogramTrace_getMin = DynamicLookup::instance().getFunc<double(*)(internal::HdrHistogramTracePtr)>("hebi_charts_HdrHistogramTrace_getMin");
  return hebi_charts_HdrHistogramTrace_getMin(ptr_);
}
inline double HdrHistogramTrace::getStdDeviation() const noexcept {
  static auto hebi_charts_HdrHistogramTrace_getStdDeviation = DynamicLookup::instance().getFunc<double(*)(internal::HdrHistogramTracePtr)>("hebi_charts_HdrHistogramTrace_getStdDeviation");
  return hebi_charts_HdrHistogramTrace_getStdDeviation(ptr_);
}
inline std::string HdrHistogramTrace::getTag() const noexcept {
  static auto hebi_charts_HdrHistogramTrace_getTag = DynamicLookup::instance().getFunc<const char*(*)(internal::HdrHistogramTracePtr)>("hebi_charts_HdrHistogramTrace_getTag");
  auto ptr = hebi_charts_HdrHistogramTrace_getTag(ptr_);
  return !ptr ? std::string() : std::string(ptr); // copy utf8
}
inline size_t HdrHistogramTrace::getTotalCount() const noexcept {
  static auto hebi_charts_HdrHistogramTrace_getTotalCount = DynamicLookup::instance().getFunc<size_t(*)(internal::HdrHistogramTracePtr)>("hebi_charts_HdrHistogramTrace_getTotalCount");
  return hebi_charts_HdrHistogramTrace_getTotalCount(ptr_);
}
inline HdrHistogramTrace HdrHistogramTrace::createLocal(int numberOfSignificantDigits, double minSeconds, double maxSeconds) noexcept {
  static auto hebi_charts_HdrHistogramTrace_createLocal = DynamicLookup::instance().getFunc<internal::HdrHistogramTracePtr(*)(int, double, double)>("hebi_charts_HdrHistogramTrace_createLocal");
  auto ptr = hebi_charts_HdrHistogramTrace_createLocal(numberOfSignificantDigits, minSeconds, maxSeconds);
  return HdrHistogramTrace(ptr);
}
inline double HdrHistogramTrace::getValueAtPercentile(double percentile) const noexcept {
  static auto hebi_charts_HdrHistogramTrace_getValueAtPercentile = DynamicLookup::instance().getFunc<double(*)(internal::HdrHistogramTracePtr, double)>("hebi_charts_HdrHistogramTrace_getValueAtPercentile");
  return hebi_charts_HdrHistogramTrace_getValueAtPercentile(ptr_, percentile);
}
inline void HdrHistogramTrace::tic() noexcept {
  static auto hebi_charts_HdrHistogramTrace_tic = DynamicLookup::instance().getFunc<void(*)(internal::HdrHistogramTracePtr)>("hebi_charts_HdrHistogramTrace_tic");
  hebi_charts_HdrHistogramTrace_tic(ptr_);
}
inline double HdrHistogramTrace::toc() noexcept {
  static auto hebi_charts_HdrHistogramTrace_toc = DynamicLookup::instance().getFunc<double(*)(internal::HdrHistogramTracePtr)>("hebi_charts_HdrHistogramTrace_toc");
  return hebi_charts_HdrHistogramTrace_toc(ptr_);
}
inline double HdrHistogramTrace::ticToc() noexcept {
  static auto hebi_charts_HdrHistogramTrace_ticToc = DynamicLookup::instance().getFunc<double(*)(internal::HdrHistogramTracePtr)>("hebi_charts_HdrHistogramTrace_ticToc");
  return hebi_charts_HdrHistogramTrace_ticToc(ptr_);
}
inline void HdrHistogramTrace::recordValue(double value) noexcept {
  static auto hebi_charts_HdrHistogramTrace_recordValue = DynamicLookup::instance().getFunc<void(*)(internal::HdrHistogramTracePtr, double)>("hebi_charts_HdrHistogramTrace_recordValue");
  hebi_charts_HdrHistogramTrace_recordValue(ptr_, value);
}
inline void HdrHistogramTrace::recordValueWithCount(double value, size_t count) noexcept {
  static auto hebi_charts_HdrHistogramTrace_recordValueWithCount = DynamicLookup::instance().getFunc<void(*)(internal::HdrHistogramTracePtr, double, size_t)>("hebi_charts_HdrHistogramTrace_recordValueWithCount");
  hebi_charts_HdrHistogramTrace_recordValueWithCount(ptr_, value, count);
}
inline void HdrHistogramTrace::recordValueWithExpectedInterval(double value, double expectedIntervalBetweenSamples) noexcept {
  static auto hebi_charts_HdrHistogramTrace_recordValueWithExpectedInterval = DynamicLookup::instance().getFunc<void(*)(internal::HdrHistogramTracePtr, double, double)>("hebi_charts_HdrHistogramTrace_recordValueWithExpectedInterval");
  hebi_charts_HdrHistogramTrace_recordValueWithExpectedInterval(ptr_, value, expectedIntervalBetweenSamples);
}
inline void HdrHistogramTrace::reset() noexcept {
  static auto hebi_charts_HdrHistogramTrace_reset = DynamicLookup::instance().getFunc<void(*)(internal::HdrHistogramTracePtr)>("hebi_charts_HdrHistogramTrace_reset");
  hebi_charts_HdrHistogramTrace_reset(ptr_);
}
inline std::string HdrHistogramTrace::toHgrmString(double outputUnitsPerSecond) const noexcept {
  static auto hebi_charts_HdrHistogramTrace_toHgrmString = DynamicLookup::instance().getFunc<const char*(*)(internal::HdrHistogramTracePtr, double)>("hebi_charts_HdrHistogramTrace_toHgrmString");
  auto ptr = hebi_charts_HdrHistogramTrace_toHgrmString(ptr_, outputUnitsPerSecond);
  return !ptr ? std::string() : std::string(ptr); // copy utf8
}
inline std::string HdrHistogramTrace::saveAsHgrm(const char* fileName, double outputUnitsPerSecond) const {
  static auto hebi_charts_HdrHistogramTrace_saveAsHgrm = DynamicLookup::instance().getFunc<const char*(*)(internal::HdrHistogramTracePtr, const char*, double)>("hebi_charts_HdrHistogramTrace_saveAsHgrm");
  auto ptr = hebi_charts_HdrHistogramTrace_saveAsHgrm(ptr_, fileName, outputUnitsPerSecond);
  return !ptr ? std::string() : std::string(ptr); // copy utf8
}
inline std::string HdrHistogramTrace::saveAsHgrm(const std::string& fileName, double outputUnitsPerSecond) const {
  return saveAsHgrm(fileName.c_str(), outputUnitsPerSecond);
}
inline void HdrHistogramTrace::cleanup() noexcept {
  if (ptr_ != nullptr) {
    static auto hebi_charts_HdrHistogramTrace_release = DynamicLookup::instance().getFunc<void(*)(internal::HdrHistogramTracePtr)>("hebi_charts_HdrHistogramTrace_release");
    hebi_charts_HdrHistogramTrace_release(ptr_);
  }
}
inline HdrHistogramTrace::HdrHistogramTrace(HdrHistogramTrace&& from) noexcept : ptr_(from.ptr_) {
  from.ptr_ = nullptr;
};
inline HdrHistogramTrace& HdrHistogramTrace::operator=(HdrHistogramTrace&& from) noexcept {
  cleanup();
  ptr_ = from.ptr_;
  from.ptr_ = nullptr;
  return *this;
};
inline HdrHistogramTrace::~HdrHistogramTrace() noexcept {
  cleanup();
}

// ImageStream
inline void* ImageStream::getBuffer() const noexcept {
  static auto hebi_charts_ImageStream_getBuffer = DynamicLookup::instance().getFunc<void*(*)(internal::ImageStreamPtr)>("hebi_charts_ImageStream_getBuffer");
  return hebi_charts_ImageStream_getBuffer(ptr_);
}
inline size_t ImageStream::getBufferSize() const noexcept {
  static auto hebi_charts_ImageStream_getBufferSize = DynamicLookup::instance().getFunc<size_t(*)(internal::ImageStreamPtr)>("hebi_charts_ImageStream_getBufferSize");
  return hebi_charts_ImageStream_getBufferSize(ptr_);
}
inline int ImageStream::getChannels() const noexcept {
  static auto hebi_charts_ImageStream_getChannels = DynamicLookup::instance().getFunc<int(*)(internal::ImageStreamPtr)>("hebi_charts_ImageStream_getChannels");
  return hebi_charts_ImageStream_getChannels(ptr_);
}
inline int ImageStream::getHeight() const noexcept {
  static auto hebi_charts_ImageStream_getHeight = DynamicLookup::instance().getFunc<int(*)(internal::ImageStreamPtr)>("hebi_charts_ImageStream_getHeight");
  return hebi_charts_ImageStream_getHeight(ptr_);
}
inline PixelFormat ImageStream::getPixelFormat() const noexcept {
  static auto hebi_charts_ImageStream_getPixelFormat = DynamicLookup::instance().getFunc<PixelFormat(*)(internal::ImageStreamPtr)>("hebi_charts_ImageStream_getPixelFormat");
  return hebi_charts_ImageStream_getPixelFormat(ptr_);
}
inline void ImageStream::setPixelFormat(PixelFormat pixelFormat) const noexcept {
  static auto hebi_charts_ImageStream_setPixelFormat = DynamicLookup::instance().getFunc<void(*)(internal::ImageStreamPtr, PixelFormat)>("hebi_charts_ImageStream_setPixelFormat");
  hebi_charts_ImageStream_setPixelFormat(ptr_, pixelFormat);
}
inline double ImageStream::getRateLimit() const noexcept {
  static auto hebi_charts_ImageStream_getRateLimit = DynamicLookup::instance().getFunc<double(*)(internal::ImageStreamPtr)>("hebi_charts_ImageStream_getRateLimit");
  return hebi_charts_ImageStream_getRateLimit(ptr_);
}
inline void ImageStream::setRateLimit(double maxFramesPerSecond) {
  static auto hebi_charts_ImageStream_setRateLimit = DynamicLookup::instance().getFunc<int(*)(internal::ImageStreamPtr, double)>("hebi_charts_ImageStream_setRateLimit");
  int status_ = hebi_charts_ImageStream_setRateLimit(ptr_, maxFramesPerSecond);
  if (status_ != 0) {
    throw Exception("Encountered error in ImageStream::setRateLimit");
  }
}
inline size_t ImageStream::getRecorderThreads() {
  static auto hebi_charts_ImageStream_getRecorderThreads = DynamicLookup::instance().getFunc<size_t(*)(internal::ImageStreamPtr)>("hebi_charts_ImageStream_getRecorderThreads");
  return hebi_charts_ImageStream_getRecorderThreads(ptr_);
}
inline void ImageStream::setRecorderThreads(size_t numThreads) {
  static auto hebi_charts_ImageStream_setRecorderThreads = DynamicLookup::instance().getFunc<int(*)(internal::ImageStreamPtr, size_t)>("hebi_charts_ImageStream_setRecorderThreads");
  int status_ = hebi_charts_ImageStream_setRecorderThreads(ptr_, numThreads);
  if (status_ != 0) {
    throw Exception("Encountered error in ImageStream::setRecorderThreads");
  }
}
inline bool ImageStream::isRecording() {
  static auto hebi_charts_ImageStream_isRecording = DynamicLookup::instance().getFunc<int(*)(internal::ImageStreamPtr)>("hebi_charts_ImageStream_isRecording");
  return hebi_charts_ImageStream_isRecording(ptr_);
}
inline double ImageStream::getRenderScale() const noexcept {
  static auto hebi_charts_ImageStream_getRenderScale = DynamicLookup::instance().getFunc<double(*)(internal::ImageStreamPtr)>("hebi_charts_ImageStream_getRenderScale");
  return hebi_charts_ImageStream_getRenderScale(ptr_);
}
inline double ImageStream::getRenderScaleX() const noexcept {
  static auto hebi_charts_ImageStream_getRenderScaleX = DynamicLookup::instance().getFunc<double(*)(internal::ImageStreamPtr)>("hebi_charts_ImageStream_getRenderScaleX");
  return hebi_charts_ImageStream_getRenderScaleX(ptr_);
}
inline double ImageStream::getRenderScaleY() const noexcept {
  static auto hebi_charts_ImageStream_getRenderScaleY = DynamicLookup::instance().getFunc<double(*)(internal::ImageStreamPtr)>("hebi_charts_ImageStream_getRenderScaleY");
  return hebi_charts_ImageStream_getRenderScaleY(ptr_);
}
inline size_t ImageStream::getSequence() const noexcept {
  static auto hebi_charts_ImageStream_getSequence = DynamicLookup::instance().getFunc<size_t(*)(internal::ImageStreamPtr)>("hebi_charts_ImageStream_getSequence");
  return hebi_charts_ImageStream_getSequence(ptr_);
}
inline int ImageStream::getStride() const noexcept {
  static auto hebi_charts_ImageStream_getStride = DynamicLookup::instance().getFunc<int(*)(internal::ImageStreamPtr)>("hebi_charts_ImageStream_getStride");
  return hebi_charts_ImageStream_getStride(ptr_);
}
inline double ImageStream::getTimestamp() const noexcept {
  static auto hebi_charts_ImageStream_getTimestamp = DynamicLookup::instance().getFunc<double(*)(internal::ImageStreamPtr)>("hebi_charts_ImageStream_getTimestamp");
  return hebi_charts_ImageStream_getTimestamp(ptr_);
}
inline int ImageStream::getWidth() const noexcept {
  static auto hebi_charts_ImageStream_getWidth = DynamicLookup::instance().getFunc<int(*)(internal::ImageStreamPtr)>("hebi_charts_ImageStream_getWidth");
  return hebi_charts_ImageStream_getWidth(ptr_);
}
inline void ImageStream::setResolution(int width, int height) {
  static auto hebi_charts_ImageStream_setResolution = DynamicLookup::instance().getFunc<int(*)(internal::ImageStreamPtr, int, int)>("hebi_charts_ImageStream_setResolution");
  int status_ = hebi_charts_ImageStream_setResolution(ptr_, width, height);
  if (status_ != 0) {
    throw Exception("Encountered error in ImageStream::setResolution");
  }
}
inline bool ImageStream::waitForNext(size_t maxTimeoutMillis) {
  static auto hebi_charts_ImageStream_waitForNext = DynamicLookup::instance().getFunc<int(*)(internal::ImageStreamPtr, size_t)>("hebi_charts_ImageStream_waitForNext");
  return hebi_charts_ImageStream_waitForNext(ptr_, maxTimeoutMillis);
}
inline bool ImageStream::tryGetNext() noexcept {
  static auto hebi_charts_ImageStream_tryGetNext = DynamicLookup::instance().getFunc<int(*)(internal::ImageStreamPtr)>("hebi_charts_ImageStream_tryGetNext");
  return hebi_charts_ImageStream_tryGetNext(ptr_);
}
inline void ImageStream::startRecording(const char* baseName, bool overwrite) {
  static auto hebi_charts_ImageStream_startRecording = DynamicLookup::instance().getFunc<int(*)(internal::ImageStreamPtr, const char*, int)>("hebi_charts_ImageStream_startRecording");
  int status_ = hebi_charts_ImageStream_startRecording(ptr_, baseName, overwrite);
  if (status_ != 0) {
    throw Exception("Encountered error in ImageStream::startRecording");
  }
}
inline void ImageStream::startRecording(const std::string& baseName, bool overwrite) {
  startRecording(baseName.c_str(), overwrite);
}
inline RecordingResult ImageStream::stopRecording() {
  static auto hebi_charts_ImageStream_stopRecording = DynamicLookup::instance().getFunc<internal::RecordingResultPtr(*)(internal::ImageStreamPtr)>("hebi_charts_ImageStream_stopRecording");
  auto ptr = hebi_charts_ImageStream_stopRecording(ptr_);
  if (!ptr) {
    throw Exception("Could not create RecordingResult in ImageStream::stopRecording");
  }
  return RecordingResult(ptr);
}
inline void ImageStream::saveToFile(const char* fileName) const {
  static auto hebi_charts_ImageStream_saveToFile = DynamicLookup::instance().getFunc<int(*)(internal::ImageStreamPtr, const char*)>("hebi_charts_ImageStream_saveToFile");
  int status_ = hebi_charts_ImageStream_saveToFile(ptr_, fileName);
  if (status_ != 0) {
    throw Exception("Encountered error in ImageStream::saveToFile");
  }
}
inline void ImageStream::saveToFile(const std::string& fileName) const {
  saveToFile(fileName.c_str());
}
inline void ImageStream::cleanup() noexcept {
  if (ptr_ != nullptr) {
    static auto hebi_charts_ImageStream_release = DynamicLookup::instance().getFunc<void(*)(internal::ImageStreamPtr)>("hebi_charts_ImageStream_release");
    hebi_charts_ImageStream_release(ptr_);
  }
}
inline ImageStream::ImageStream(ImageStream&& from) noexcept : ptr_(from.ptr_) {
  from.ptr_ = nullptr;
};
inline ImageStream& ImageStream::operator=(ImageStream&& from) noexcept {
  cleanup();
  ptr_ = from.ptr_;
  from.ptr_ = nullptr;
  return *this;
};
inline ImageStream::~ImageStream() noexcept {
  cleanup();
}

// LoopTimer
inline LoopTimer::LoopTimer() {
  static auto hebi_charts_LoopTimer_create = DynamicLookup::instance().getFunc<internal::LoopTimerPtr(*)()>("hebi_charts_LoopTimer_create");
  ptr_ = hebi_charts_LoopTimer_create();
  if (!ptr_) {
    throw Exception("Could not create LoopTimer");
  }
}
inline double LoopTimer::getElapsedTime() const noexcept {
  static auto hebi_charts_LoopTimer_getElapsedTime = DynamicLookup::instance().getFunc<double(*)(internal::LoopTimerPtr)>("hebi_charts_LoopTimer_getElapsedTime");
  return hebi_charts_LoopTimer_getElapsedTime(ptr_);
}
inline double LoopTimer::getFrequency() const noexcept {
  static auto hebi_charts_LoopTimer_getFrequency = DynamicLookup::instance().getFunc<double(*)(internal::LoopTimerPtr)>("hebi_charts_LoopTimer_getFrequency");
  return hebi_charts_LoopTimer_getFrequency(ptr_);
}
inline void LoopTimer::setFrequency(double frequency) noexcept {
  static auto hebi_charts_LoopTimer_setFrequency = DynamicLookup::instance().getFunc<void(*)(internal::LoopTimerPtr, double)>("hebi_charts_LoopTimer_setFrequency");
  hebi_charts_LoopTimer_setFrequency(ptr_, frequency);
}
inline double LoopTimer::getPeriod() const noexcept {
  static auto hebi_charts_LoopTimer_getPeriod = DynamicLookup::instance().getFunc<double(*)(internal::LoopTimerPtr)>("hebi_charts_LoopTimer_getPeriod");
  return hebi_charts_LoopTimer_getPeriod(ptr_);
}
inline void LoopTimer::setPeriod(double seconds) noexcept {
  static auto hebi_charts_LoopTimer_setPeriod = DynamicLookup::instance().getFunc<void(*)(internal::LoopTimerPtr, double)>("hebi_charts_LoopTimer_setPeriod");
  hebi_charts_LoopTimer_setPeriod(ptr_, seconds);
}
inline void LoopTimer::restart() noexcept {
  static auto hebi_charts_LoopTimer_restart = DynamicLookup::instance().getFunc<void(*)(internal::LoopTimerPtr)>("hebi_charts_LoopTimer_restart");
  hebi_charts_LoopTimer_restart(ptr_);
}
inline void LoopTimer::tic() noexcept {
  static auto hebi_charts_LoopTimer_tic = DynamicLookup::instance().getFunc<void(*)(internal::LoopTimerPtr)>("hebi_charts_LoopTimer_tic");
  hebi_charts_LoopTimer_tic(ptr_);
}
inline double LoopTimer::toc() const noexcept {
  static auto hebi_charts_LoopTimer_toc = DynamicLookup::instance().getFunc<double(*)(internal::LoopTimerPtr)>("hebi_charts_LoopTimer_toc");
  return hebi_charts_LoopTimer_toc(ptr_);
}
inline double LoopTimer::ticToc() noexcept {
  static auto hebi_charts_LoopTimer_ticToc = DynamicLookup::instance().getFunc<double(*)(internal::LoopTimerPtr)>("hebi_charts_LoopTimer_ticToc");
  return hebi_charts_LoopTimer_ticToc(ptr_);
}
inline void LoopTimer::waitForNextTick() const noexcept {
  static auto hebi_charts_LoopTimer_waitForNextTick = DynamicLookup::instance().getFunc<void(*)(internal::LoopTimerPtr)>("hebi_charts_LoopTimer_waitForNextTick");
  hebi_charts_LoopTimer_waitForNextTick(ptr_);
}
inline double LoopTimer::getSecondsToNextTick() const noexcept {
  static auto hebi_charts_LoopTimer_getSecondsToNextTick = DynamicLookup::instance().getFunc<double(*)(internal::LoopTimerPtr)>("hebi_charts_LoopTimer_getSecondsToNextTick");
  return hebi_charts_LoopTimer_getSecondsToNextTick(ptr_);
}
inline size_t LoopTimer::getNanosToNextTick() const noexcept {
  static auto hebi_charts_LoopTimer_getNanosToNextTick = DynamicLookup::instance().getFunc<size_t(*)(internal::LoopTimerPtr)>("hebi_charts_LoopTimer_getNanosToNextTick");
  return hebi_charts_LoopTimer_getNanosToNextTick(ptr_);
}
inline double LoopTimer::timeSeconds() noexcept {
  static auto hebi_charts_LoopTimer_timeSeconds = DynamicLookup::instance().getFunc<double(*)()>("hebi_charts_LoopTimer_timeSeconds");
  return hebi_charts_LoopTimer_timeSeconds();
}
inline size_t LoopTimer::timeNanos() noexcept {
  static auto hebi_charts_LoopTimer_timeNanos = DynamicLookup::instance().getFunc<size_t(*)()>("hebi_charts_LoopTimer_timeNanos");
  return hebi_charts_LoopTimer_timeNanos();
}
inline void LoopTimer::sleepNanos(size_t nanos) noexcept {
  static auto hebi_charts_LoopTimer_sleepNanos = DynamicLookup::instance().getFunc<void(*)(size_t)>("hebi_charts_LoopTimer_sleepNanos");
  hebi_charts_LoopTimer_sleepNanos(nanos);
}
inline void LoopTimer::parkNanos(size_t nanos) noexcept {
  static auto hebi_charts_LoopTimer_parkNanos = DynamicLookup::instance().getFunc<void(*)(size_t)>("hebi_charts_LoopTimer_parkNanos");
  hebi_charts_LoopTimer_parkNanos(nanos);
}
inline void LoopTimer::waitNanos(size_t nanos) noexcept {
  static auto hebi_charts_LoopTimer_waitNanos = DynamicLookup::instance().getFunc<void(*)(size_t)>("hebi_charts_LoopTimer_waitNanos");
  hebi_charts_LoopTimer_waitNanos(nanos);
}
inline void LoopTimer::spinNanos(size_t nanos) noexcept {
  static auto hebi_charts_LoopTimer_spinNanos = DynamicLookup::instance().getFunc<void(*)(size_t)>("hebi_charts_LoopTimer_spinNanos");
  hebi_charts_LoopTimer_spinNanos(nanos);
}
inline void LoopTimer::cleanup() noexcept {
  if (ptr_ != nullptr) {
    static auto hebi_charts_LoopTimer_release = DynamicLookup::instance().getFunc<void(*)(internal::LoopTimerPtr)>("hebi_charts_LoopTimer_release");
    hebi_charts_LoopTimer_release(ptr_);
  }
}
inline LoopTimer::LoopTimer(LoopTimer&& from) noexcept : ptr_(from.ptr_) {
  from.ptr_ = nullptr;
};
inline LoopTimer& LoopTimer::operator=(LoopTimer&& from) noexcept {
  cleanup();
  ptr_ = from.ptr_;
  from.ptr_ = nullptr;
  return *this;
};
inline LoopTimer::~LoopTimer() noexcept {
  cleanup();
}

// Object3d
inline bool Object3d::isVisible() const noexcept {
  static auto hebi_charts_Object3d_isVisible = DynamicLookup::instance().getFunc<int(*)(internal::Object3dPtr)>("hebi_charts_Object3d_isVisible");
  return hebi_charts_Object3d_isVisible(ptr_);
}
inline void Object3d::setVisible(bool visible) noexcept {
  static auto hebi_charts_Object3d_setVisible = DynamicLookup::instance().getFunc<void(*)(internal::Object3dPtr, int)>("hebi_charts_Object3d_setVisible");
  hebi_charts_Object3d_setVisible(ptr_, visible);
}
inline void Object3d::setOrientation(double qx, double qy, double qz, double qw) {
  static auto hebi_charts_Object3d_setOrientation = DynamicLookup::instance().getFunc<int(*)(internal::Object3dPtr, double, double, double, double)>("hebi_charts_Object3d_setOrientation");
  int status_ = hebi_charts_Object3d_setOrientation(ptr_, qx, qy, qz, qw);
  if (status_ != 0) {
    throw Exception("Encountered error in Object3d::setOrientation");
  }
}
inline void Object3d::setOrientationRPY(double roll, double pitch, double yaw) {
  static auto hebi_charts_Object3d_setOrientationRPY = DynamicLookup::instance().getFunc<int(*)(internal::Object3dPtr, double, double, double)>("hebi_charts_Object3d_setOrientationRPY");
  int status_ = hebi_charts_Object3d_setOrientationRPY(ptr_, roll, pitch, yaw);
  if (status_ != 0) {
    throw Exception("Encountered error in Object3d::setOrientationRPY");
  }
}
inline void Object3d::setTranslation(double x, double y, double z) {
  static auto hebi_charts_Object3d_setTranslation = DynamicLookup::instance().getFunc<int(*)(internal::Object3dPtr, double, double, double)>("hebi_charts_Object3d_setTranslation");
  int status_ = hebi_charts_Object3d_setTranslation(ptr_, x, y, z);
  if (status_ != 0) {
    throw Exception("Encountered error in Object3d::setTranslation");
  }
}
inline void Object3d::setPose(double x, double y, double z, double qx, double qy, double qz, double qw) {
  static auto hebi_charts_Object3d_setPose = DynamicLookup::instance().getFunc<int(*)(internal::Object3dPtr, double, double, double, double, double, double, double)>("hebi_charts_Object3d_setPose");
  int status_ = hebi_charts_Object3d_setPose(ptr_, x, y, z, qx, qy, qz, qw);
  if (status_ != 0) {
    throw Exception("Encountered error in Object3d::setPose");
  }
}
inline void Object3d::setTransform4x4(const double* matrix, MatrixOrdering ordering) {
  static auto hebi_charts_Object3d_setTransform4x4 = DynamicLookup::instance().getFunc<int(*)(internal::Object3dPtr, const double*, MatrixOrdering)>("hebi_charts_Object3d_setTransform4x4");
  int status_ = hebi_charts_Object3d_setTransform4x4(ptr_, matrix, ordering);
  if (status_ != 0) {
    throw Exception("Encountered error in Object3d::setTransform4x4");
  }
}
inline void Object3d::setTransform4x4(const std::vector<double>& matrix, MatrixOrdering ordering) {
  if (matrix.size() != 16) {
    throw Exception("Transform matrix incorrect size");
  }
  setTransform4x4(matrix.data(), ordering);
}
inline void Object3d::cleanup() noexcept {
  if (ptr_ != nullptr) {
    static auto hebi_charts_Object3d_release = DynamicLookup::instance().getFunc<void(*)(internal::Object3dPtr)>("hebi_charts_Object3d_release");
    hebi_charts_Object3d_release(ptr_);
  }
}
inline Object3d::Object3d(Object3d&& from) noexcept : ptr_(from.ptr_) {
  from.ptr_ = nullptr;
};
inline Object3d& Object3d::operator=(Object3d&& from) noexcept {
  cleanup();
  ptr_ = from.ptr_;
  from.ptr_ = nullptr;
  return *this;
};
inline Object3d::~Object3d() noexcept {
  cleanup();
}

// Frame
inline internal::Object3dPtr Frame::getObject3dPointer(internal::FramePtr cPointer) noexcept {
  static auto hebi_charts_Frame_to_Object3d = DynamicLookup::instance().getFunc<internal::Object3dPtr(*)(internal::FramePtr)>("hebi_charts_Frame_to_Object3d");
  return hebi_charts_Frame_to_Object3d(cPointer);
}
inline Frame::Frame(internal::FramePtr cPointer) noexcept : Object3d(getObject3dPointer(cPointer)), ptr_(cPointer) {}
inline Frame::Frame(Frame&& from) noexcept : Object3d(std::move(from)), ptr_(from.ptr_) {
  from.ptr_ = nullptr;
};
inline Frame& Frame::operator=(Frame&& from) noexcept {
  Object3d::operator=(std::move(from));
  ptr_ = from.ptr_;
  from.ptr_ = nullptr;
  return *this;
};

// Mesh
inline bool Mesh::isCentered() const noexcept {
  static auto hebi_charts_Mesh_isCentered = DynamicLookup::instance().getFunc<int(*)(internal::MeshPtr)>("hebi_charts_Mesh_isCentered");
  return hebi_charts_Mesh_isCentered(ptr_);
}
inline void Mesh::setCentered(bool centered) noexcept {
  static auto hebi_charts_Mesh_setCentered = DynamicLookup::instance().getFunc<void(*)(internal::MeshPtr, int)>("hebi_charts_Mesh_setCentered");
  hebi_charts_Mesh_setCentered(ptr_, centered);
}
inline double Mesh::getScale() const noexcept {
  static auto hebi_charts_Mesh_getScale = DynamicLookup::instance().getFunc<double(*)(internal::MeshPtr)>("hebi_charts_Mesh_getScale");
  return hebi_charts_Mesh_getScale(ptr_);
}
inline void Mesh::setScale(double scaleUnitsToMillimeters) noexcept {
  static auto hebi_charts_Mesh_setScale = DynamicLookup::instance().getFunc<void(*)(internal::MeshPtr, double)>("hebi_charts_Mesh_setScale");
  hebi_charts_Mesh_setScale(ptr_, scaleUnitsToMillimeters);
}
inline DisplayStyle Mesh::getDisplayStyle() const noexcept {
  static auto hebi_charts_Mesh_getDisplayStyle = DynamicLookup::instance().getFunc<DisplayStyle(*)(internal::MeshPtr)>("hebi_charts_Mesh_getDisplayStyle");
  return hebi_charts_Mesh_getDisplayStyle(ptr_);
}
inline void Mesh::setDisplayStyle(DisplayStyle style) noexcept {
  static auto hebi_charts_Mesh_setDisplayStyle = DynamicLookup::instance().getFunc<void(*)(internal::MeshPtr, DisplayStyle)>("hebi_charts_Mesh_setDisplayStyle");
  hebi_charts_Mesh_setDisplayStyle(ptr_, style);
}
inline internal::Object3dPtr Mesh::getObject3dPointer(internal::MeshPtr cPointer) noexcept {
  static auto hebi_charts_Mesh_to_Object3d = DynamicLookup::instance().getFunc<internal::Object3dPtr(*)(internal::MeshPtr)>("hebi_charts_Mesh_to_Object3d");
  return hebi_charts_Mesh_to_Object3d(cPointer);
}
inline Mesh::Mesh(internal::MeshPtr cPointer) noexcept : Object3d(getObject3dPointer(cPointer)), ptr_(cPointer) {}
inline Mesh::Mesh(Mesh&& from) noexcept : Object3d(std::move(from)), ptr_(from.ptr_) {
  from.ptr_ = nullptr;
};
inline Mesh& Mesh::operator=(Mesh&& from) noexcept {
  Object3d::operator=(std::move(from));
  ptr_ = from.ptr_;
  from.ptr_ = nullptr;
  return *this;
};

// Robot
inline DisplayStyle Robot::getDisplayStyle() const noexcept {
  static auto hebi_charts_Robot_getDisplayStyle = DynamicLookup::instance().getFunc<DisplayStyle(*)(internal::RobotPtr)>("hebi_charts_Robot_getDisplayStyle");
  return hebi_charts_Robot_getDisplayStyle(ptr_);
}
inline void Robot::setDisplayStyle(DisplayStyle style) noexcept {
  static auto hebi_charts_Robot_setDisplayStyle = DynamicLookup::instance().getFunc<void(*)(internal::RobotPtr, DisplayStyle)>("hebi_charts_Robot_setDisplayStyle");
  hebi_charts_Robot_setDisplayStyle(ptr_, style);
}
inline size_t Robot::getDof() const noexcept {
  static auto hebi_charts_Robot_getDof = DynamicLookup::instance().getFunc<size_t(*)(internal::RobotPtr)>("hebi_charts_Robot_getDof");
  return hebi_charts_Robot_getDof(ptr_);
}
inline void Robot::setPositions(const double* positions, size_t length) {
  static auto hebi_charts_Robot_setPositions = DynamicLookup::instance().getFunc<int(*)(internal::RobotPtr, const double*, size_t)>("hebi_charts_Robot_setPositions");
  if (length != getDof()) {
    throw Exception("Position vector length does not match number of joints");
  }
  int status_ = hebi_charts_Robot_setPositions(ptr_, positions, length);
  if (status_ != 0) {
    throw Exception("Encountered error in Robot::setPositions");
  }
}
inline void Robot::setPositions(const std::vector<double>& positions) {
  setPositions(positions.data(), positions.size());
}
inline internal::Object3dPtr Robot::getObject3dPointer(internal::RobotPtr cPointer) noexcept {
  static auto hebi_charts_Robot_to_Object3d = DynamicLookup::instance().getFunc<internal::Object3dPtr(*)(internal::RobotPtr)>("hebi_charts_Robot_to_Object3d");
  return hebi_charts_Robot_to_Object3d(cPointer);
}
inline Robot::Robot(internal::RobotPtr cPointer) noexcept : Object3d(getObject3dPointer(cPointer)), ptr_(cPointer) {}
inline Robot::Robot(Robot&& from) noexcept : Object3d(std::move(from)), ptr_(from.ptr_) {
  from.ptr_ = nullptr;
};
inline Robot& Robot::operator=(Robot&& from) noexcept {
  Object3d::operator=(std::move(from));
  ptr_ = from.ptr_;
  from.ptr_ = nullptr;
  return *this;
};

// Series3d
inline Color Series3d::getColor() const noexcept {
  static auto hebi_charts_Series3d_getColor = DynamicLookup::instance().getFunc<Color(*)(internal::Series3dPtr)>("hebi_charts_Series3d_getColor");
  return hebi_charts_Series3d_getColor(ptr_);
}
inline void Series3d::setColor(Color color) noexcept {
  static auto hebi_charts_Series3d_setColor = DynamicLookup::instance().getFunc<void(*)(internal::Series3dPtr, Color)>("hebi_charts_Series3d_setColor");
  hebi_charts_Series3d_setColor(ptr_, color);
}
inline internal::Object3dPtr Series3d::getObject3dPointer(internal::Series3dPtr cPointer) noexcept {
  static auto hebi_charts_Series3d_to_Object3d = DynamicLookup::instance().getFunc<internal::Object3dPtr(*)(internal::Series3dPtr)>("hebi_charts_Series3d_to_Object3d");
  return hebi_charts_Series3d_to_Object3d(cPointer);
}
inline Series3d::Series3d(internal::Series3dPtr cPointer) noexcept : Object3d(getObject3dPointer(cPointer)), ptr_(cPointer) {}
inline Series3d::Series3d(Series3d&& from) noexcept : Object3d(std::move(from)), ptr_(from.ptr_) {
  from.ptr_ = nullptr;
};
inline Series3d& Series3d::operator=(Series3d&& from) noexcept {
  Object3d::operator=(std::move(from));
  ptr_ = from.ptr_;
  from.ptr_ = nullptr;
  return *this;
};

// Line3d
inline void Line3d::setMaxPointCount(size_t count) noexcept {
  static auto hebi_charts_Line3d_setMaxPointCount = DynamicLookup::instance().getFunc<void(*)(internal::Line3dPtr, size_t)>("hebi_charts_Line3d_setMaxPointCount");
  hebi_charts_Line3d_setMaxPointCount(ptr_, count);
}
inline void Line3d::clear() noexcept {
  static auto hebi_charts_Line3d_clear = DynamicLookup::instance().getFunc<void(*)(internal::Line3dPtr)>("hebi_charts_Line3d_clear");
  hebi_charts_Line3d_clear(ptr_);
}
inline void Line3d::setData(const double* x, const double* y, const double* z, size_t length) noexcept {
  static auto hebi_charts_Line3d_setData = DynamicLookup::instance().getFunc<void(*)(internal::Line3dPtr, const double*, const double*, const double*, size_t)>("hebi_charts_Line3d_setData");
  hebi_charts_Line3d_setData(ptr_, x, y, z, length);
}
inline void Line3d::setData(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z) noexcept {
  setData(x.data(), y.data(), z.data(), (std::min)({x.size(), y.size(), z.size()}));
}
inline void Line3d::addPoints(const double* x, const double* y, const double* z, size_t length) noexcept {
  static auto hebi_charts_Line3d_addPoints = DynamicLookup::instance().getFunc<void(*)(internal::Line3dPtr, const double*, const double*, const double*, size_t)>("hebi_charts_Line3d_addPoints");
  hebi_charts_Line3d_addPoints(ptr_, x, y, z, length);
}
inline void Line3d::addPoints(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z) noexcept {
  addPoints(x.data(), y.data(), z.data(), (std::min)({x.size(), y.size(), z.size()}));
}
inline void Line3d::addPoint(double x, double y, double z) noexcept {
  static auto hebi_charts_Line3d_addPoint = DynamicLookup::instance().getFunc<void(*)(internal::Line3dPtr, double, double, double)>("hebi_charts_Line3d_addPoint");
  hebi_charts_Line3d_addPoint(ptr_, x, y, z);
}
inline internal::Series3dPtr Line3d::getSeries3dPointer(internal::Line3dPtr cPointer) noexcept {
  static auto hebi_charts_Line3d_to_Series3d = DynamicLookup::instance().getFunc<internal::Series3dPtr(*)(internal::Line3dPtr)>("hebi_charts_Line3d_to_Series3d");
  return hebi_charts_Line3d_to_Series3d(cPointer);
}
inline Line3d::Line3d(internal::Line3dPtr cPointer) noexcept : Series3d(getSeries3dPointer(cPointer)), ptr_(cPointer) {}
inline Line3d::Line3d(Line3d&& from) noexcept : Series3d(std::move(from)), ptr_(from.ptr_) {
  from.ptr_ = nullptr;
};
inline Line3d& Line3d::operator=(Line3d&& from) noexcept {
  Series3d::operator=(std::move(from));
  ptr_ = from.ptr_;
  from.ptr_ = nullptr;
  return *this;
};

// Points3d
inline MarkerShape Points3d::getMarkerShape() const noexcept {
  static auto hebi_charts_Points3d_getMarkerShape = DynamicLookup::instance().getFunc<MarkerShape(*)(internal::Points3dPtr)>("hebi_charts_Points3d_getMarkerShape");
  return hebi_charts_Points3d_getMarkerShape(ptr_);
}
inline void Points3d::setMarkerShape(MarkerShape shape) noexcept {
  static auto hebi_charts_Points3d_setMarkerShape = DynamicLookup::instance().getFunc<void(*)(internal::Points3dPtr, MarkerShape)>("hebi_charts_Points3d_setMarkerShape");
  hebi_charts_Points3d_setMarkerShape(ptr_, shape);
}
inline double Points3d::getMarkerSize() const noexcept {
  static auto hebi_charts_Points3d_getMarkerSize = DynamicLookup::instance().getFunc<double(*)(internal::Points3dPtr)>("hebi_charts_Points3d_getMarkerSize");
  return hebi_charts_Points3d_getMarkerSize(ptr_);
}
inline void Points3d::setMarkerSize(double value) noexcept {
  static auto hebi_charts_Points3d_setMarkerSize = DynamicLookup::instance().getFunc<void(*)(internal::Points3dPtr, double)>("hebi_charts_Points3d_setMarkerSize");
  hebi_charts_Points3d_setMarkerSize(ptr_, value);
}
inline bool Points3d::isSelfIllumination() const noexcept {
  static auto hebi_charts_Points3d_isSelfIllumination = DynamicLookup::instance().getFunc<int(*)(internal::Points3dPtr)>("hebi_charts_Points3d_isSelfIllumination");
  return hebi_charts_Points3d_isSelfIllumination(ptr_);
}
inline void Points3d::setSelfIllumination(bool value) noexcept {
  static auto hebi_charts_Points3d_setSelfIllumination = DynamicLookup::instance().getFunc<void(*)(internal::Points3dPtr, int)>("hebi_charts_Points3d_setSelfIllumination");
  hebi_charts_Points3d_setSelfIllumination(ptr_, value);
}
inline bool Points3d::isVertexSharing() const noexcept {
  static auto hebi_charts_Points3d_isVertexSharing = DynamicLookup::instance().getFunc<int(*)(internal::Points3dPtr)>("hebi_charts_Points3d_isVertexSharing");
  return hebi_charts_Points3d_isVertexSharing(ptr_);
}
inline void Points3d::setVertexSharing(bool value) noexcept {
  static auto hebi_charts_Points3d_setVertexSharing = DynamicLookup::instance().getFunc<void(*)(internal::Points3dPtr, int)>("hebi_charts_Points3d_setVertexSharing");
  hebi_charts_Points3d_setVertexSharing(ptr_, value);
}
inline void Points3d::setMaxPointCount(size_t count) noexcept {
  static auto hebi_charts_Points3d_setMaxPointCount = DynamicLookup::instance().getFunc<void(*)(internal::Points3dPtr, size_t)>("hebi_charts_Points3d_setMaxPointCount");
  hebi_charts_Points3d_setMaxPointCount(ptr_, count);
}
inline void Points3d::clear() noexcept {
  static auto hebi_charts_Points3d_clear = DynamicLookup::instance().getFunc<void(*)(internal::Points3dPtr)>("hebi_charts_Points3d_clear");
  hebi_charts_Points3d_clear(ptr_);
}
inline void Points3d::setData(const double* x, const double* y, const double* z, size_t length) noexcept {
  static auto hebi_charts_Points3d_setData = DynamicLookup::instance().getFunc<void(*)(internal::Points3dPtr, const double*, const double*, const double*, size_t)>("hebi_charts_Points3d_setData");
  hebi_charts_Points3d_setData(ptr_, x, y, z, length);
}
inline void Points3d::setData(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z) noexcept {
  setData(x.data(), y.data(), z.data(), (std::min)({x.size(), y.size(), z.size()}));
}
inline void Points3d::addPoints(const double* x, const double* y, const double* z, size_t length) noexcept {
  static auto hebi_charts_Points3d_addPoints = DynamicLookup::instance().getFunc<void(*)(internal::Points3dPtr, const double*, const double*, const double*, size_t)>("hebi_charts_Points3d_addPoints");
  hebi_charts_Points3d_addPoints(ptr_, x, y, z, length);
}
inline void Points3d::addPoints(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z) noexcept {
  addPoints(x.data(), y.data(), z.data(), (std::min)({x.size(), y.size(), z.size()}));
}
inline void Points3d::addPoint(double x, double y, double z) noexcept {
  static auto hebi_charts_Points3d_addPoint = DynamicLookup::instance().getFunc<void(*)(internal::Points3dPtr, double, double, double)>("hebi_charts_Points3d_addPoint");
  hebi_charts_Points3d_addPoint(ptr_, x, y, z);
}
inline internal::Series3dPtr Points3d::getSeries3dPointer(internal::Points3dPtr cPointer) noexcept {
  static auto hebi_charts_Points3d_to_Series3d = DynamicLookup::instance().getFunc<internal::Series3dPtr(*)(internal::Points3dPtr)>("hebi_charts_Points3d_to_Series3d");
  return hebi_charts_Points3d_to_Series3d(cPointer);
}
inline Points3d::Points3d(internal::Points3dPtr cPointer) noexcept : Series3d(getSeries3dPointer(cPointer)), ptr_(cPointer) {}
inline Points3d::Points3d(Points3d&& from) noexcept : Series3d(std::move(from)), ptr_(from.ptr_) {
  from.ptr_ = nullptr;
};
inline Points3d& Points3d::operator=(Points3d&& from) noexcept {
  Series3d::operator=(std::move(from));
  ptr_ = from.ptr_;
  from.ptr_ = nullptr;
  return *this;
};

// RecordingResult
inline std::string RecordingResult::getDirectory() const {
  static auto hebi_charts_RecordingResult_getDirectory = DynamicLookup::instance().getFunc<const char*(*)(internal::RecordingResultPtr)>("hebi_charts_RecordingResult_getDirectory");
  auto ptr = hebi_charts_RecordingResult_getDirectory(ptr_);
  return !ptr ? std::string() : std::string(ptr); // copy utf8
}
inline size_t RecordingResult::getDroppedCount() const noexcept {
  static auto hebi_charts_RecordingResult_getDroppedCount = DynamicLookup::instance().getFunc<size_t(*)(internal::RecordingResultPtr)>("hebi_charts_RecordingResult_getDroppedCount");
  return hebi_charts_RecordingResult_getDroppedCount(ptr_);
}
inline double RecordingResult::getDuration() const noexcept {
  static auto hebi_charts_RecordingResult_getDuration = DynamicLookup::instance().getFunc<double(*)(internal::RecordingResultPtr)>("hebi_charts_RecordingResult_getDuration");
  return hebi_charts_RecordingResult_getDuration(ptr_);
}
inline double RecordingResult::getAverageFps() const noexcept {
  static auto hebi_charts_RecordingResult_getAverageFps = DynamicLookup::instance().getFunc<double(*)(internal::RecordingResultPtr)>("hebi_charts_RecordingResult_getAverageFps");
  return hebi_charts_RecordingResult_getAverageFps(ptr_);
}
inline std::string RecordingResult::getManifest() const {
  static auto hebi_charts_RecordingResult_getManifest = DynamicLookup::instance().getFunc<const char*(*)(internal::RecordingResultPtr)>("hebi_charts_RecordingResult_getManifest");
  auto ptr = hebi_charts_RecordingResult_getManifest(ptr_);
  return !ptr ? std::string() : std::string(ptr); // copy utf8
}
inline size_t RecordingResult::getRecordedCount() const noexcept {
  static auto hebi_charts_RecordingResult_getRecordedCount = DynamicLookup::instance().getFunc<size_t(*)(internal::RecordingResultPtr)>("hebi_charts_RecordingResult_getRecordedCount");
  return hebi_charts_RecordingResult_getRecordedCount(ptr_);
}
inline size_t RecordingResult::getSkippedCount() const noexcept {
  static auto hebi_charts_RecordingResult_getSkippedCount = DynamicLookup::instance().getFunc<size_t(*)(internal::RecordingResultPtr)>("hebi_charts_RecordingResult_getSkippedCount");
  return hebi_charts_RecordingResult_getSkippedCount(ptr_);
}
inline size_t RecordingResult::getTotalFrames() const noexcept {
  static auto hebi_charts_RecordingResult_getTotalFrames = DynamicLookup::instance().getFunc<size_t(*)(internal::RecordingResultPtr)>("hebi_charts_RecordingResult_getTotalFrames");
  return hebi_charts_RecordingResult_getTotalFrames(ptr_);
}
inline std::string RecordingResult::getFfmpegCommand(VideoOutputFormat outputFormat, bool deleteDirectory) const {
  static auto hebi_charts_RecordingResult_getFfmpegCommand = DynamicLookup::instance().getFunc<const char*(*)(internal::RecordingResultPtr, VideoOutputFormat, int)>("hebi_charts_RecordingResult_getFfmpegCommand");
  auto ptr = hebi_charts_RecordingResult_getFfmpegCommand(ptr_, outputFormat, deleteDirectory);
  return !ptr ? std::string() : std::string(ptr); // copy utf8
}
inline std::string RecordingResult::runFfmpeg(VideoOutputFormat outputFormat, bool deleteDirectory) {
  static auto hebi_charts_RecordingResult_runFfmpeg = DynamicLookup::instance().getFunc<const char*(*)(internal::RecordingResultPtr, VideoOutputFormat, int)>("hebi_charts_RecordingResult_runFfmpeg");
  auto ptr = hebi_charts_RecordingResult_runFfmpeg(ptr_, outputFormat, deleteDirectory);
  return !ptr ? std::string() : std::string(ptr); // copy utf8
}
inline void RecordingResult::cleanup() noexcept {
  if (ptr_ != nullptr) {
    static auto hebi_charts_RecordingResult_release = DynamicLookup::instance().getFunc<void(*)(internal::RecordingResultPtr)>("hebi_charts_RecordingResult_release");
    hebi_charts_RecordingResult_release(ptr_);
  }
}
inline RecordingResult::RecordingResult(RecordingResult&& from) noexcept : ptr_(from.ptr_) {
  from.ptr_ = nullptr;
};
inline RecordingResult& RecordingResult::operator=(RecordingResult&& from) noexcept {
  cleanup();
  ptr_ = from.ptr_;
  from.ptr_ = nullptr;
  return *this;
};
inline RecordingResult::~RecordingResult() noexcept {
  cleanup();
}

// Scene3d
inline void Scene3d::setGridBounds(double minX, double maxX, double minY, double maxY, double minZ, double maxZ) noexcept {
  static auto hebi_charts_Scene3d_setGridBounds = DynamicLookup::instance().getFunc<void(*)(internal::Scene3dPtr, double, double, double, double, double, double)>("hebi_charts_Scene3d_setGridBounds");
  hebi_charts_Scene3d_setGridBounds(ptr_, minX, maxX, minY, maxY, minZ, maxZ);
}
inline double Scene3d::getMaxX() const noexcept {
  static auto hebi_charts_Scene3d_getMaxX = DynamicLookup::instance().getFunc<double(*)(internal::Scene3dPtr)>("hebi_charts_Scene3d_getMaxX");
  return hebi_charts_Scene3d_getMaxX(ptr_);
}
inline void Scene3d::setMaxX(double val) noexcept {
  static auto hebi_charts_Scene3d_setMaxX = DynamicLookup::instance().getFunc<void(*)(internal::Scene3dPtr, double)>("hebi_charts_Scene3d_setMaxX");
  hebi_charts_Scene3d_setMaxX(ptr_, val);
}
inline double Scene3d::getMaxY() const noexcept {
  static auto hebi_charts_Scene3d_getMaxY = DynamicLookup::instance().getFunc<double(*)(internal::Scene3dPtr)>("hebi_charts_Scene3d_getMaxY");
  return hebi_charts_Scene3d_getMaxY(ptr_);
}
inline void Scene3d::setMaxY(double val) noexcept {
  static auto hebi_charts_Scene3d_setMaxY = DynamicLookup::instance().getFunc<void(*)(internal::Scene3dPtr, double)>("hebi_charts_Scene3d_setMaxY");
  hebi_charts_Scene3d_setMaxY(ptr_, val);
}
inline double Scene3d::getMaxZ() const noexcept {
  static auto hebi_charts_Scene3d_getMaxZ = DynamicLookup::instance().getFunc<double(*)(internal::Scene3dPtr)>("hebi_charts_Scene3d_getMaxZ");
  return hebi_charts_Scene3d_getMaxZ(ptr_);
}
inline void Scene3d::setMaxZ(double val) noexcept {
  static auto hebi_charts_Scene3d_setMaxZ = DynamicLookup::instance().getFunc<void(*)(internal::Scene3dPtr, double)>("hebi_charts_Scene3d_setMaxZ");
  hebi_charts_Scene3d_setMaxZ(ptr_, val);
}
inline double Scene3d::getMinX() const noexcept {
  static auto hebi_charts_Scene3d_getMinX = DynamicLookup::instance().getFunc<double(*)(internal::Scene3dPtr)>("hebi_charts_Scene3d_getMinX");
  return hebi_charts_Scene3d_getMinX(ptr_);
}
inline void Scene3d::setMinX(double val) noexcept {
  static auto hebi_charts_Scene3d_setMinX = DynamicLookup::instance().getFunc<void(*)(internal::Scene3dPtr, double)>("hebi_charts_Scene3d_setMinX");
  hebi_charts_Scene3d_setMinX(ptr_, val);
}
inline double Scene3d::getMinY() const noexcept {
  static auto hebi_charts_Scene3d_getMinY = DynamicLookup::instance().getFunc<double(*)(internal::Scene3dPtr)>("hebi_charts_Scene3d_getMinY");
  return hebi_charts_Scene3d_getMinY(ptr_);
}
inline void Scene3d::setMinY(double val) noexcept {
  static auto hebi_charts_Scene3d_setMinY = DynamicLookup::instance().getFunc<void(*)(internal::Scene3dPtr, double)>("hebi_charts_Scene3d_setMinY");
  hebi_charts_Scene3d_setMinY(ptr_, val);
}
inline double Scene3d::getMinZ() const noexcept {
  static auto hebi_charts_Scene3d_getMinZ = DynamicLookup::instance().getFunc<double(*)(internal::Scene3dPtr)>("hebi_charts_Scene3d_getMinZ");
  return hebi_charts_Scene3d_getMinZ(ptr_);
}
inline void Scene3d::setMinZ(double val) noexcept {
  static auto hebi_charts_Scene3d_setMinZ = DynamicLookup::instance().getFunc<void(*)(internal::Scene3dPtr, double)>("hebi_charts_Scene3d_setMinZ");
  hebi_charts_Scene3d_setMinZ(ptr_, val);
}
inline double Scene3d::getGridStep() const noexcept {
  static auto hebi_charts_Scene3d_getGridStep = DynamicLookup::instance().getFunc<double(*)(internal::Scene3dPtr)>("hebi_charts_Scene3d_getGridStep");
  return hebi_charts_Scene3d_getGridStep(ptr_);
}
inline void Scene3d::setGridStep(double gridStep) noexcept {
  static auto hebi_charts_Scene3d_setGridStep = DynamicLookup::instance().getFunc<void(*)(internal::Scene3dPtr, double)>("hebi_charts_Scene3d_setGridStep");
  hebi_charts_Scene3d_setGridStep(ptr_, gridStep);
}
inline Camera Scene3d::getCamera() noexcept {
  static auto hebi_charts_Scene3d_getCamera = DynamicLookup::instance().getFunc<internal::CameraPtr(*)(internal::Scene3dPtr)>("hebi_charts_Scene3d_getCamera");
  auto ptr = hebi_charts_Scene3d_getCamera(ptr_);
  return Camera(ptr);
}
inline Robot Scene3d::addRobot(const char* pathOrUrl) {
  static auto hebi_charts_Scene3d_addRobot = DynamicLookup::instance().getFunc<internal::RobotPtr(*)(internal::Scene3dPtr, const char*)>("hebi_charts_Scene3d_addRobot");
  auto ptr = hebi_charts_Scene3d_addRobot(ptr_, pathOrUrl);
  if (!ptr) {
    throw Exception("Could not create Robot in Scene3d::addRobot");
  }
  return Robot(ptr);
}
inline Robot Scene3d::addRobot(const std::string& pathOrUrl) {
  return addRobot(pathOrUrl.c_str());
}
inline Mesh Scene3d::addMesh(const char* pathOrUrl) {
  static auto hebi_charts_Scene3d_addMesh = DynamicLookup::instance().getFunc<internal::MeshPtr(*)(internal::Scene3dPtr, const char*)>("hebi_charts_Scene3d_addMesh");
  auto ptr = hebi_charts_Scene3d_addMesh(ptr_, pathOrUrl);
  if (!ptr) {
    throw Exception("Could not create Mesh in Scene3d::addMesh");
  }
  return Mesh(ptr);
}
inline Mesh Scene3d::addMesh(const std::string& pathOrUrl) {
  return addMesh(pathOrUrl.c_str());
}
inline Frame Scene3d::addFrame(double lengthInMeters) {
  static auto hebi_charts_Scene3d_addFrame = DynamicLookup::instance().getFunc<internal::FramePtr(*)(internal::Scene3dPtr, double)>("hebi_charts_Scene3d_addFrame");
  auto ptr = hebi_charts_Scene3d_addFrame(ptr_, lengthInMeters);
  if (!ptr) {
    throw Exception("Could not create Frame in Scene3d::addFrame");
  }
  return Frame(ptr);
}
inline Line3d Scene3d::addLine() {
  static auto hebi_charts_Scene3d_addLine = DynamicLookup::instance().getFunc<internal::Line3dPtr(*)(internal::Scene3dPtr)>("hebi_charts_Scene3d_addLine");
  auto ptr = hebi_charts_Scene3d_addLine(ptr_);
  if (!ptr) {
    throw Exception("Could not create Line3d in Scene3d::addLine");
  }
  return Line3d(ptr);
}
inline Points3d Scene3d::addPoints() {
  static auto hebi_charts_Scene3d_addPoints = DynamicLookup::instance().getFunc<internal::Points3dPtr(*)(internal::Scene3dPtr)>("hebi_charts_Scene3d_addPoints");
  auto ptr = hebi_charts_Scene3d_addPoints(ptr_);
  if (!ptr) {
    throw Exception("Could not create Points3d in Scene3d::addPoints");
  }
  return Points3d(ptr);
}
inline void Scene3d::cleanup() noexcept {
  if (ptr_ != nullptr) {
    static auto hebi_charts_Scene3d_release = DynamicLookup::instance().getFunc<void(*)(internal::Scene3dPtr)>("hebi_charts_Scene3d_release");
    hebi_charts_Scene3d_release(ptr_);
  }
}
inline Scene3d::Scene3d(Scene3d&& from) noexcept : ptr_(from.ptr_) {
  from.ptr_ = nullptr;
};
inline Scene3d& Scene3d::operator=(Scene3d&& from) noexcept {
  cleanup();
  ptr_ = from.ptr_;
  from.ptr_ = nullptr;
  return *this;
};
inline Scene3d::~Scene3d() noexcept {
  cleanup();
}

// StreamView
inline void StreamView::cleanup() noexcept {
  if (ptr_ != nullptr) {
    static auto hebi_charts_StreamView_release = DynamicLookup::instance().getFunc<void(*)(internal::StreamViewPtr)>("hebi_charts_StreamView_release");
    hebi_charts_StreamView_release(ptr_);
  }
}
inline StreamView::StreamView(StreamView&& from) noexcept : ptr_(from.ptr_) {
  from.ptr_ = nullptr;
};
inline StreamView& StreamView::operator=(StreamView&& from) noexcept {
  cleanup();
  ptr_ = from.ptr_;
  from.ptr_ = nullptr;
  return *this;
};
inline StreamView::~StreamView() noexcept {
  cleanup();
}

// XYChart
inline std::string XYChart::getTitle() const noexcept {
  static auto hebi_charts_XYChart_getTitle = DynamicLookup::instance().getFunc<const char*(*)(internal::XYChartPtr)>("hebi_charts_XYChart_getTitle");
  auto ptr = hebi_charts_XYChart_getTitle(ptr_);
  return !ptr ? std::string() : std::string(ptr); // copy utf8
}
inline void XYChart::setTitle(const char* title) noexcept {
  static auto hebi_charts_XYChart_setTitle = DynamicLookup::instance().getFunc<void(*)(internal::XYChartPtr, const char*)>("hebi_charts_XYChart_setTitle");
  hebi_charts_XYChart_setTitle(ptr_, title);
}
inline void XYChart::setTitle(const std::string& title) noexcept {
  setTitle(title.c_str());
}
inline bool XYChart::getXAssumeSorted() const noexcept {
  static auto hebi_charts_XYChart_getXAssumeSorted = DynamicLookup::instance().getFunc<int(*)(internal::XYChartPtr)>("hebi_charts_XYChart_getXAssumeSorted");
  return hebi_charts_XYChart_getXAssumeSorted(ptr_);
}
inline void XYChart::setXAssumeSorted(bool xAssumeSorted) noexcept {
  static auto hebi_charts_XYChart_setXAssumeSorted = DynamicLookup::instance().getFunc<void(*)(internal::XYChartPtr, int)>("hebi_charts_XYChart_setXAssumeSorted");
  hebi_charts_XYChart_setXAssumeSorted(ptr_, xAssumeSorted);
}
inline bool XYChart::isXAutoUnitScaling() const noexcept {
  static auto hebi_charts_XYChart_isXAutoUnitScaling = DynamicLookup::instance().getFunc<int(*)(internal::XYChartPtr)>("hebi_charts_XYChart_isXAutoUnitScaling");
  return hebi_charts_XYChart_isXAutoUnitScaling(ptr_);
}
inline void XYChart::setXAutoUnitScaling(bool enabled) noexcept {
  static auto hebi_charts_XYChart_setXAutoUnitScaling = DynamicLookup::instance().getFunc<void(*)(internal::XYChartPtr, int)>("hebi_charts_XYChart_setXAutoUnitScaling");
  hebi_charts_XYChart_setXAutoUnitScaling(ptr_, enabled);
}
inline std::string XYChart::getXLabel() const noexcept {
  static auto hebi_charts_XYChart_getXLabel = DynamicLookup::instance().getFunc<const char*(*)(internal::XYChartPtr)>("hebi_charts_XYChart_getXLabel");
  auto ptr = hebi_charts_XYChart_getXLabel(ptr_);
  return !ptr ? std::string() : std::string(ptr); // copy utf8
}
inline void XYChart::setXLabel(const char* label) noexcept {
  static auto hebi_charts_XYChart_setXLabel = DynamicLookup::instance().getFunc<void(*)(internal::XYChartPtr, const char*)>("hebi_charts_XYChart_setXLabel");
  hebi_charts_XYChart_setXLabel(ptr_, label);
}
inline void XYChart::setXLabel(const std::string& label) noexcept {
  setXLabel(label.c_str());
}
inline void XYChart::setXLim(double min, double max) {
  static auto hebi_charts_XYChart_setXLim = DynamicLookup::instance().getFunc<int(*)(internal::XYChartPtr, double, double)>("hebi_charts_XYChart_setXLim");
  int status_ = hebi_charts_XYChart_setXLim(ptr_, min, max);
  if (status_ != 0) {
    throw Exception("Encountered error in XYChart::setXLim");
  }
}
inline double XYChart::getXMax() const noexcept {
  static auto hebi_charts_XYChart_getXMax = DynamicLookup::instance().getFunc<double(*)(internal::XYChartPtr)>("hebi_charts_XYChart_getXMax");
  return hebi_charts_XYChart_getXMax(ptr_);
}
inline void XYChart::setXMax(double max) noexcept {
  static auto hebi_charts_XYChart_setXMax = DynamicLookup::instance().getFunc<void(*)(internal::XYChartPtr, double)>("hebi_charts_XYChart_setXMax");
  hebi_charts_XYChart_setXMax(ptr_, max);
}
inline double XYChart::getXMin() const noexcept {
  static auto hebi_charts_XYChart_getXMin = DynamicLookup::instance().getFunc<double(*)(internal::XYChartPtr)>("hebi_charts_XYChart_getXMin");
  return hebi_charts_XYChart_getXMin(ptr_);
}
inline void XYChart::setXMin(double min) noexcept {
  static auto hebi_charts_XYChart_setXMin = DynamicLookup::instance().getFunc<void(*)(internal::XYChartPtr, double)>("hebi_charts_XYChart_setXMin");
  hebi_charts_XYChart_setXMin(ptr_, min);
}
inline std::string XYChart::getXUnit() const noexcept {
  static auto hebi_charts_XYChart_getXUnit = DynamicLookup::instance().getFunc<const char*(*)(internal::XYChartPtr)>("hebi_charts_XYChart_getXUnit");
  auto ptr = hebi_charts_XYChart_getXUnit(ptr_);
  return !ptr ? std::string() : std::string(ptr); // copy utf8
}
inline void XYChart::setXUnit(const char* unit) noexcept {
  static auto hebi_charts_XYChart_setXUnit = DynamicLookup::instance().getFunc<void(*)(internal::XYChartPtr, const char*)>("hebi_charts_XYChart_setXUnit");
  hebi_charts_XYChart_setXUnit(ptr_, unit);
}
inline void XYChart::setXUnit(const std::string& unit) noexcept {
  setXUnit(unit.c_str());
}
inline bool XYChart::isYAutoUnitScaling() const noexcept {
  static auto hebi_charts_XYChart_isYAutoUnitScaling = DynamicLookup::instance().getFunc<int(*)(internal::XYChartPtr)>("hebi_charts_XYChart_isYAutoUnitScaling");
  return hebi_charts_XYChart_isYAutoUnitScaling(ptr_);
}
inline void XYChart::setYAutoUnitScaling(bool enabled) noexcept {
  static auto hebi_charts_XYChart_setYAutoUnitScaling = DynamicLookup::instance().getFunc<void(*)(internal::XYChartPtr, int)>("hebi_charts_XYChart_setYAutoUnitScaling");
  hebi_charts_XYChart_setYAutoUnitScaling(ptr_, enabled);
}
inline std::string XYChart::getYLabel() const noexcept {
  static auto hebi_charts_XYChart_getYLabel = DynamicLookup::instance().getFunc<const char*(*)(internal::XYChartPtr)>("hebi_charts_XYChart_getYLabel");
  auto ptr = hebi_charts_XYChart_getYLabel(ptr_);
  return !ptr ? std::string() : std::string(ptr); // copy utf8
}
inline void XYChart::setYLabel(const char* label) noexcept {
  static auto hebi_charts_XYChart_setYLabel = DynamicLookup::instance().getFunc<void(*)(internal::XYChartPtr, const char*)>("hebi_charts_XYChart_setYLabel");
  hebi_charts_XYChart_setYLabel(ptr_, label);
}
inline void XYChart::setYLabel(const std::string& label) noexcept {
  setYLabel(label.c_str());
}
inline void XYChart::setYLim(double min, double max) noexcept {
  static auto hebi_charts_XYChart_setYLim = DynamicLookup::instance().getFunc<void(*)(internal::XYChartPtr, double, double)>("hebi_charts_XYChart_setYLim");
  hebi_charts_XYChart_setYLim(ptr_, min, max);
}
inline double XYChart::getYMax() const noexcept {
  static auto hebi_charts_XYChart_getYMax = DynamicLookup::instance().getFunc<double(*)(internal::XYChartPtr)>("hebi_charts_XYChart_getYMax");
  return hebi_charts_XYChart_getYMax(ptr_);
}
inline void XYChart::setYMax(double max) noexcept {
  static auto hebi_charts_XYChart_setYMax = DynamicLookup::instance().getFunc<void(*)(internal::XYChartPtr, double)>("hebi_charts_XYChart_setYMax");
  hebi_charts_XYChart_setYMax(ptr_, max);
}
inline double XYChart::getYMin() const noexcept {
  static auto hebi_charts_XYChart_getYMin = DynamicLookup::instance().getFunc<double(*)(internal::XYChartPtr)>("hebi_charts_XYChart_getYMin");
  return hebi_charts_XYChart_getYMin(ptr_);
}
inline void XYChart::setYMin(double min) noexcept {
  static auto hebi_charts_XYChart_setYMin = DynamicLookup::instance().getFunc<void(*)(internal::XYChartPtr, double)>("hebi_charts_XYChart_setYMin");
  hebi_charts_XYChart_setYMin(ptr_, min);
}
inline std::string XYChart::getYUnit() const noexcept {
  static auto hebi_charts_XYChart_getYUnit = DynamicLookup::instance().getFunc<const char*(*)(internal::XYChartPtr)>("hebi_charts_XYChart_getYUnit");
  auto ptr = hebi_charts_XYChart_getYUnit(ptr_);
  return !ptr ? std::string() : std::string(ptr); // copy utf8
}
inline void XYChart::setYUnit(const char* unit) noexcept {
  static auto hebi_charts_XYChart_setYUnit = DynamicLookup::instance().getFunc<void(*)(internal::XYChartPtr, const char*)>("hebi_charts_XYChart_setYUnit");
  hebi_charts_XYChart_setYUnit(ptr_, unit);
}
inline void XYChart::setYUnit(const std::string& unit) noexcept {
  setYUnit(unit.c_str());
}
inline Cursor XYChart::addXCursor() noexcept {
  static auto hebi_charts_XYChart_addXCursor = DynamicLookup::instance().getFunc<internal::CursorPtr(*)(internal::XYChartPtr)>("hebi_charts_XYChart_addXCursor");
  auto ptr = hebi_charts_XYChart_addXCursor(ptr_);
  return Cursor(ptr);
}
inline Cursor XYChart::addYCursor() noexcept {
  static auto hebi_charts_XYChart_addYCursor = DynamicLookup::instance().getFunc<internal::CursorPtr(*)(internal::XYChartPtr)>("hebi_charts_XYChart_addYCursor");
  auto ptr = hebi_charts_XYChart_addYCursor(ptr_);
  return Cursor(ptr);
}
inline void XYChart::cleanup() noexcept {
  if (ptr_ != nullptr) {
    static auto hebi_charts_XYChart_release = DynamicLookup::instance().getFunc<void(*)(internal::XYChartPtr)>("hebi_charts_XYChart_release");
    hebi_charts_XYChart_release(ptr_);
  }
}
inline XYChart::XYChart(XYChart&& from) noexcept : ptr_(from.ptr_) {
  from.ptr_ = nullptr;
};
inline XYChart& XYChart::operator=(XYChart&& from) noexcept {
  cleanup();
  ptr_ = from.ptr_;
  from.ptr_ = nullptr;
  return *this;
};
inline XYChart::~XYChart() noexcept {
  cleanup();
}

// LatencyChart
inline LatencyTrace LatencyChart::addTrace(const char* name) {
  static auto hebi_charts_LatencyChart_addTrace = DynamicLookup::instance().getFunc<internal::LatencyTracePtr(*)(internal::LatencyChartPtr, const char*)>("hebi_charts_LatencyChart_addTrace");
  auto ptr = hebi_charts_LatencyChart_addTrace(ptr_, name);
  if (!ptr) {
    throw Exception("Could not create LatencyTrace in LatencyChart::addTrace");
  }
  return LatencyTrace(ptr);
}
inline LatencyTrace LatencyChart::addTrace(const std::string& name) {
  return addTrace(name.c_str());
}
inline internal::XYChartPtr LatencyChart::getXYChartPointer(internal::LatencyChartPtr cPointer) noexcept {
  static auto hebi_charts_LatencyChart_to_XYChart = DynamicLookup::instance().getFunc<internal::XYChartPtr(*)(internal::LatencyChartPtr)>("hebi_charts_LatencyChart_to_XYChart");
  return hebi_charts_LatencyChart_to_XYChart(cPointer);
}
inline LatencyChart::LatencyChart(internal::LatencyChartPtr cPointer) noexcept : XYChart(getXYChartPointer(cPointer)), ptr_(cPointer) {}
inline LatencyChart::LatencyChart(LatencyChart&& from) noexcept : XYChart(std::move(from)), ptr_(from.ptr_) {
  from.ptr_ = nullptr;
};
inline LatencyChart& LatencyChart::operator=(LatencyChart&& from) noexcept {
  XYChart::operator=(std::move(from));
  ptr_ = from.ptr_;
  from.ptr_ = nullptr;
  return *this;
};

// LineChart
inline Line LineChart::addLine(const char* label) {
  static auto hebi_charts_LineChart_addLine = DynamicLookup::instance().getFunc<internal::LinePtr(*)(internal::LineChartPtr, const char*)>("hebi_charts_LineChart_addLine");
  auto ptr = hebi_charts_LineChart_addLine(ptr_, label);
  if (!ptr) {
    throw Exception("Could not create Line in LineChart::addLine");
  }
  return Line(ptr);
}
inline Line LineChart::addLine(const std::string& label) {
  return addLine(label.c_str());
}
inline internal::XYChartPtr LineChart::getXYChartPointer(internal::LineChartPtr cPointer) noexcept {
  static auto hebi_charts_LineChart_to_XYChart = DynamicLookup::instance().getFunc<internal::XYChartPtr(*)(internal::LineChartPtr)>("hebi_charts_LineChart_to_XYChart");
  return hebi_charts_LineChart_to_XYChart(cPointer);
}
inline LineChart::LineChart(internal::LineChartPtr cPointer) noexcept : XYChart(getXYChartPointer(cPointer)), ptr_(cPointer) {}
inline LineChart::LineChart(LineChart&& from) noexcept : XYChart(std::move(from)), ptr_(from.ptr_) {
  from.ptr_ = nullptr;
};
inline LineChart& LineChart::operator=(LineChart&& from) noexcept {
  XYChart::operator=(std::move(from));
  ptr_ = from.ptr_;
  from.ptr_ = nullptr;
  return *this;
};

// XYSeries
inline Color XYSeries::getColor() const noexcept {
  static auto hebi_charts_XYSeries_getColor = DynamicLookup::instance().getFunc<Color(*)(internal::XYSeriesPtr)>("hebi_charts_XYSeries_getColor");
  return hebi_charts_XYSeries_getColor(ptr_);
}
inline void XYSeries::setColor(Color color) noexcept {
  static auto hebi_charts_XYSeries_setColor = DynamicLookup::instance().getFunc<void(*)(internal::XYSeriesPtr, Color)>("hebi_charts_XYSeries_setColor");
  hebi_charts_XYSeries_setColor(ptr_, color);
}
inline std::string XYSeries::getLabel() const noexcept {
  static auto hebi_charts_XYSeries_getLabel = DynamicLookup::instance().getFunc<const char*(*)(internal::XYSeriesPtr)>("hebi_charts_XYSeries_getLabel");
  auto ptr = hebi_charts_XYSeries_getLabel(ptr_);
  return !ptr ? std::string() : std::string(ptr); // copy utf8
}
inline void XYSeries::setLabel(const char* label) noexcept {
  static auto hebi_charts_XYSeries_setLabel = DynamicLookup::instance().getFunc<void(*)(internal::XYSeriesPtr, const char*)>("hebi_charts_XYSeries_setLabel");
  hebi_charts_XYSeries_setLabel(ptr_, label);
}
inline void XYSeries::setLabel(const std::string& label) noexcept {
  setLabel(label.c_str());
}
inline LineStyle XYSeries::getLineStyle() const noexcept {
  static auto hebi_charts_XYSeries_getLineStyle = DynamicLookup::instance().getFunc<LineStyle(*)(internal::XYSeriesPtr)>("hebi_charts_XYSeries_getLineStyle");
  return hebi_charts_XYSeries_getLineStyle(ptr_);
}
inline void XYSeries::setLineStyle(LineStyle lineStyle) noexcept {
  static auto hebi_charts_XYSeries_setLineStyle = DynamicLookup::instance().getFunc<void(*)(internal::XYSeriesPtr, LineStyle)>("hebi_charts_XYSeries_setLineStyle");
  hebi_charts_XYSeries_setLineStyle(ptr_, lineStyle);
}
inline double XYSeries::getLineWidth() const noexcept {
  static auto hebi_charts_XYSeries_getLineWidth = DynamicLookup::instance().getFunc<double(*)(internal::XYSeriesPtr)>("hebi_charts_XYSeries_getLineWidth");
  return hebi_charts_XYSeries_getLineWidth(ptr_);
}
inline void XYSeries::setLineWidth(double lineWidth) noexcept {
  static auto hebi_charts_XYSeries_setLineWidth = DynamicLookup::instance().getFunc<void(*)(internal::XYSeriesPtr, double)>("hebi_charts_XYSeries_setLineWidth");
  hebi_charts_XYSeries_setLineWidth(ptr_, lineWidth);
}
inline double XYSeries::getMarkerSize() const noexcept {
  static auto hebi_charts_XYSeries_getMarkerSize = DynamicLookup::instance().getFunc<double(*)(internal::XYSeriesPtr)>("hebi_charts_XYSeries_getMarkerSize");
  return hebi_charts_XYSeries_getMarkerSize(ptr_);
}
inline void XYSeries::setMarkerSize(double markerSize) noexcept {
  static auto hebi_charts_XYSeries_setMarkerSize = DynamicLookup::instance().getFunc<void(*)(internal::XYSeriesPtr, double)>("hebi_charts_XYSeries_setMarkerSize");
  hebi_charts_XYSeries_setMarkerSize(ptr_, markerSize);
}
inline MarkerType XYSeries::getMarkerType() const noexcept {
  static auto hebi_charts_XYSeries_getMarkerType = DynamicLookup::instance().getFunc<MarkerType(*)(internal::XYSeriesPtr)>("hebi_charts_XYSeries_getMarkerType");
  return hebi_charts_XYSeries_getMarkerType(ptr_);
}
inline void XYSeries::setMarkerType(MarkerType markerType) noexcept {
  static auto hebi_charts_XYSeries_setMarkerType = DynamicLookup::instance().getFunc<void(*)(internal::XYSeriesPtr, MarkerType)>("hebi_charts_XYSeries_setMarkerType");
  hebi_charts_XYSeries_setMarkerType(ptr_, markerType);
}
inline bool XYSeries::isShowInLegend() const noexcept {
  static auto hebi_charts_XYSeries_isShowInLegend = DynamicLookup::instance().getFunc<int(*)(internal::XYSeriesPtr)>("hebi_charts_XYSeries_isShowInLegend");
  return hebi_charts_XYSeries_isShowInLegend(ptr_);
}
inline void XYSeries::setShowInLegend(bool showInLegend) noexcept {
  static auto hebi_charts_XYSeries_setShowInLegend = DynamicLookup::instance().getFunc<void(*)(internal::XYSeriesPtr, int)>("hebi_charts_XYSeries_setShowInLegend");
  hebi_charts_XYSeries_setShowInLegend(ptr_, showInLegend);
}
inline bool XYSeries::isVisible() const noexcept {
  static auto hebi_charts_XYSeries_isVisible = DynamicLookup::instance().getFunc<int(*)(internal::XYSeriesPtr)>("hebi_charts_XYSeries_isVisible");
  return hebi_charts_XYSeries_isVisible(ptr_);
}
inline void XYSeries::setVisible(bool visible) noexcept {
  static auto hebi_charts_XYSeries_setVisible = DynamicLookup::instance().getFunc<void(*)(internal::XYSeriesPtr, int)>("hebi_charts_XYSeries_setVisible");
  hebi_charts_XYSeries_setVisible(ptr_, visible);
}
inline void XYSeries::cleanup() noexcept {
  if (ptr_ != nullptr) {
    static auto hebi_charts_XYSeries_release = DynamicLookup::instance().getFunc<void(*)(internal::XYSeriesPtr)>("hebi_charts_XYSeries_release");
    hebi_charts_XYSeries_release(ptr_);
  }
}
inline XYSeries::XYSeries(XYSeries&& from) noexcept : ptr_(from.ptr_) {
  from.ptr_ = nullptr;
};
inline XYSeries& XYSeries::operator=(XYSeries&& from) noexcept {
  cleanup();
  ptr_ = from.ptr_;
  from.ptr_ = nullptr;
  return *this;
};
inline XYSeries::~XYSeries() noexcept {
  cleanup();
}

// LatencyTrace
inline void LatencyTrace::tic() noexcept {
  static auto hebi_charts_LatencyTrace_tic = DynamicLookup::instance().getFunc<void(*)(internal::LatencyTracePtr)>("hebi_charts_LatencyTrace_tic");
  hebi_charts_LatencyTrace_tic(ptr_);
}
inline double LatencyTrace::toc() noexcept {
  static auto hebi_charts_LatencyTrace_toc = DynamicLookup::instance().getFunc<double(*)(internal::LatencyTracePtr)>("hebi_charts_LatencyTrace_toc");
  return hebi_charts_LatencyTrace_toc(ptr_);
}
inline double LatencyTrace::ticToc() noexcept {
  static auto hebi_charts_LatencyTrace_ticToc = DynamicLookup::instance().getFunc<double(*)(internal::LatencyTracePtr)>("hebi_charts_LatencyTrace_ticToc");
  return hebi_charts_LatencyTrace_ticToc(ptr_);
}
inline void LatencyTrace::record(double value) noexcept {
  static auto hebi_charts_LatencyTrace_record = DynamicLookup::instance().getFunc<void(*)(internal::LatencyTracePtr, double)>("hebi_charts_LatencyTrace_record");
  hebi_charts_LatencyTrace_record(ptr_, value);
}
inline void LatencyTrace::recordWithCount(double value, size_t count) noexcept {
  static auto hebi_charts_LatencyTrace_recordWithCount = DynamicLookup::instance().getFunc<void(*)(internal::LatencyTracePtr, double, size_t)>("hebi_charts_LatencyTrace_recordWithCount");
  hebi_charts_LatencyTrace_recordWithCount(ptr_, value, count);
}
inline void LatencyTrace::recordCompensated(double value, double expectedIntervalBetweenValueSamples) noexcept {
  static auto hebi_charts_LatencyTrace_recordCompensated = DynamicLookup::instance().getFunc<void(*)(internal::LatencyTracePtr, double, double)>("hebi_charts_LatencyTrace_recordCompensated");
  hebi_charts_LatencyTrace_recordCompensated(ptr_, value, expectedIntervalBetweenValueSamples);
}
inline void LatencyTrace::reset() noexcept {
  static auto hebi_charts_LatencyTrace_reset = DynamicLookup::instance().getFunc<void(*)(internal::LatencyTracePtr)>("hebi_charts_LatencyTrace_reset");
  hebi_charts_LatencyTrace_reset(ptr_);
}
inline internal::XYSeriesPtr LatencyTrace::getXYSeriesPointer(internal::LatencyTracePtr cPointer) noexcept {
  static auto hebi_charts_LatencyTrace_to_XYSeries = DynamicLookup::instance().getFunc<internal::XYSeriesPtr(*)(internal::LatencyTracePtr)>("hebi_charts_LatencyTrace_to_XYSeries");
  return hebi_charts_LatencyTrace_to_XYSeries(cPointer);
}
inline LatencyTrace::LatencyTrace(internal::LatencyTracePtr cPointer) noexcept : XYSeries(getXYSeriesPointer(cPointer)), ptr_(cPointer) {}
inline LatencyTrace::LatencyTrace(LatencyTrace&& from) noexcept : XYSeries(std::move(from)), ptr_(from.ptr_) {
  from.ptr_ = nullptr;
};
inline LatencyTrace& LatencyTrace::operator=(LatencyTrace&& from) noexcept {
  XYSeries::operator=(std::move(from));
  ptr_ = from.ptr_;
  from.ptr_ = nullptr;
  return *this;
};

// Line
inline size_t Line::getMaxPointCount() const noexcept {
  static auto hebi_charts_Line_getMaxPointCount = DynamicLookup::instance().getFunc<size_t(*)(internal::LinePtr)>("hebi_charts_Line_getMaxPointCount");
  return hebi_charts_Line_getMaxPointCount(ptr_);
}
inline void Line::setMaxPointCount(size_t count) noexcept {
  static auto hebi_charts_Line_setMaxPointCount = DynamicLookup::instance().getFunc<void(*)(internal::LinePtr, size_t)>("hebi_charts_Line_setMaxPointCount");
  hebi_charts_Line_setMaxPointCount(ptr_, count);
}
inline void Line::clear() noexcept {
  static auto hebi_charts_Line_clear = DynamicLookup::instance().getFunc<void(*)(internal::LinePtr)>("hebi_charts_Line_clear");
  hebi_charts_Line_clear(ptr_);
}
inline void Line::setData(const double* x, const double* y, size_t length) noexcept {
  static auto hebi_charts_Line_setData = DynamicLookup::instance().getFunc<void(*)(internal::LinePtr, const double*, const double*, size_t)>("hebi_charts_Line_setData");
  hebi_charts_Line_setData(ptr_, x, y, length);
}
inline void Line::setData(const std::vector<double>& x, const std::vector<double>& y) noexcept {
  setData(x.data(), y.data(), (std::min)(x.size(), y.size()));
}
inline void Line::addPoints(const double* x, const double* y, size_t length) noexcept {
  static auto hebi_charts_Line_addPoints = DynamicLookup::instance().getFunc<void(*)(internal::LinePtr, const double*, const double*, size_t)>("hebi_charts_Line_addPoints");
  hebi_charts_Line_addPoints(ptr_, x, y, length);
}
inline void Line::addPoints(const std::vector<double>& x, const std::vector<double>& y) noexcept {
  addPoints(x.data(), y.data(), (std::min)(x.size(), y.size()));
}
inline void Line::addPoint(double x, double y) noexcept {
  static auto hebi_charts_Line_addPoint = DynamicLookup::instance().getFunc<void(*)(internal::LinePtr, double, double)>("hebi_charts_Line_addPoint");
  hebi_charts_Line_addPoint(ptr_, x, y);
}
inline internal::XYSeriesPtr Line::getXYSeriesPointer(internal::LinePtr cPointer) noexcept {
  static auto hebi_charts_Line_to_XYSeries = DynamicLookup::instance().getFunc<internal::XYSeriesPtr(*)(internal::LinePtr)>("hebi_charts_Line_to_XYSeries");
  return hebi_charts_Line_to_XYSeries(cPointer);
}
inline Line::Line(internal::LinePtr cPointer) noexcept : XYSeries(getXYSeriesPointer(cPointer)), ptr_(cPointer) {}
inline Line::Line(Line&& from) noexcept : XYSeries(std::move(from)), ptr_(from.ptr_) {
  from.ptr_ = nullptr;
};
inline Line& Line::operator=(Line&& from) noexcept {
  XYSeries::operator=(std::move(from));
  ptr_ = from.ptr_;
  from.ptr_ = nullptr;
  return *this;
};

// Runtime
inline void runtime::setOption(RuntimeOption option, const char* value) {
  static auto hebi_charts_Runtime_setOption = DynamicLookup::instance().getFunc<int(*)(RuntimeOption, const char*)>("hebi_charts_Runtime_setOption");
  int status_ = hebi_charts_Runtime_setOption(option, value);
  if (status_ != 0) {
    throw Exception("Encountered error in Runtime::setOption");
  }
}
inline void runtime::setOption(RuntimeOption option, const std::string& value) {
  setOption(option, value.c_str());
}
inline void runtime::setTheme(Theme theme) noexcept {
  static auto hebi_charts_Runtime_setTheme = DynamicLookup::instance().getFunc<void(*)(Theme)>("hebi_charts_Runtime_setTheme");
  hebi_charts_Runtime_setTheme(theme);
}
inline void runtime::setAutoCloseWindows(bool autoClose) noexcept {
  static auto hebi_charts_Runtime_setAutoCloseWindows = DynamicLookup::instance().getFunc<void(*)(int)>("hebi_charts_Runtime_setAutoCloseWindows");
  hebi_charts_Runtime_setAutoCloseWindows(autoClose);
}
inline void runtime::waitUntilWindowsClosed() {
  static auto hebi_charts_Runtime_waitUntilWindowsClosed = DynamicLookup::instance().getFunc<int(*)()>("hebi_charts_Runtime_waitUntilWindowsClosed");
  int status_ = hebi_charts_Runtime_waitUntilWindowsClosed();
  if (status_ != 0) {
    throw Exception("Encountered error in Runtime::waitUntilWindowsClosed");
  }
}
inline void runtime::collect() noexcept {
  static auto hebi_charts_Runtime_collect = DynamicLookup::instance().getFunc<void(*)()>("hebi_charts_Runtime_collect");
  hebi_charts_Runtime_collect();
}
inline void runtime::closeAll() noexcept {
  static auto hebi_charts_Runtime_closeAll = DynamicLookup::instance().getFunc<void(*)()>("hebi_charts_Runtime_closeAll");
  hebi_charts_Runtime_closeAll();
}
inline void runtime::runOnUiThread(UserCallbackFunction func, void* userData) noexcept {
  static auto hebi_charts_Runtime_runOnUiThread = DynamicLookup::instance().getFunc<void(*)(UserCallbackFunction, void*)>("hebi_charts_Runtime_runOnUiThread");
  hebi_charts_Runtime_runOnUiThread(func, userData);
}
inline void runtime::printLastErrorDetails() noexcept {
  static auto hebi_charts_Runtime_printLastErrorDetails = DynamicLookup::instance().getFunc<void(*)()>("hebi_charts_Runtime_printLastErrorDetails");
  hebi_charts_Runtime_printLastErrorDetails();
}
inline void runtime::printThreadInfo(const char* name) noexcept {
  static auto hebi_charts_Runtime_printThreadInfo = DynamicLookup::instance().getFunc<void(*)(const char*)>("hebi_charts_Runtime_printThreadInfo");
  hebi_charts_Runtime_printThreadInfo(name);
}
inline void runtime::printThreadInfo(const std::string& name) noexcept {
  printThreadInfo(name.c_str());
}
inline std::string runtime::getLastErrorString() noexcept {
  static auto hebi_charts_Runtime_getLastErrorString = DynamicLookup::instance().getFunc<const char*(*)()>("hebi_charts_Runtime_getLastErrorString");
  auto ptr = hebi_charts_Runtime_getLastErrorString();
  return !ptr ? std::string() : std::string(ptr); // copy utf8
}

// ==== Cocoa utilities for supporting macOS ====
typedef int (*hebi_charts_MainCallbackFunction)(int argc, char** argv);
/**
 * @details 
 * Sets up required system libraries and executes the callback on an appropriate thread
 * 
 * This is technically only needed on macOS as the Cocoa framework for displaying
 * windows needs to be run on the main thread. On Windows and Linux this method
 * executes the callback directly and otherwise does nothing. However, all platforms
 * are supported to enable platform-independent code with the same behavior.
 */
inline int runApplication(hebi_charts_MainCallbackFunction callback, int argc, char** argv) {
  if (!DynamicLookup::instance().isLoaded()) {
    return callback(argc, argv);
  }
  static auto hebi_charts_runApplication = DynamicLookup::instance().getFunc<int(*)(hebi_charts_MainCallbackFunction, int, char**)>("hebi_charts_runApplication");
  return hebi_charts_runApplication(callback, argc, argv);
}

// Library Information
namespace lib {

struct Version {
  Version(int major, int minor, int patch, int build) : major_(major), minor_(minor), patch_(patch), build_(build) {}
  int major_{};
  int minor_{};
  int patch_{};
  int build_{};
};

inline Version getHeaderVersion() {
  return {0, 9, 4, 127};
}

inline Version getLibraryVersion() {
  Version v(0,0,0,0);
  static auto hebi_charts_getLibraryVersion = DynamicLookup::instance().getFunc<void(*)(int*, int*, int*, int*)>("hebi_charts_getLibraryVersion");
  hebi_charts_getLibraryVersion(&v.major_, &v.minor_, &v.patch_, &v.build_);
  return v;
}

inline bool isAvailable() { return DynamicLookup::instance().isLoaded(); }

} // namespace lib


template<typename FuncType> FuncType DynamicLookup::getFunc(const char* func) const {
  assert(lib_ != nullptr && "Fatal error -- cannot call hebi::charts functions on unloaded library");
  if (!lib_) {
    std::cerr << "Fatal error -- cannot call hebi::charts functions on unloaded library\n";
    return nullptr;
  }
#ifdef WIN32
  FARPROC symbol = ::GetProcAddress((HMODULE) lib_, func);
#else
  void* symbol = dlsym(lib_, func);
#endif
  if (!symbol) {
    std::cerr << "Fatal error -- hebi::charts library is missing expected symbol: " << func << "\n";
  }
  assert(symbol != nullptr);
  return reinterpret_cast<FuncType>(symbol);
}

DynamicLookup:: DynamicLookup(const std::string& path, const std::string& alt_path) {
  path_ = path;
#ifdef WIN32
  // convert to wide string to support non-ascii paths
  int size_needed = MultiByteToWideChar(CP_UTF8, 0, path.c_str(), (int)path.size(), NULL, 0);
  std::wstring wstrTo(size_needed, 0);
  MultiByteToWideChar(CP_UTF8, 0, path.c_str(), (int)path.size(), &wstrTo[0], size_needed);
  lib_ = ::LoadLibraryW(wstrTo.c_str());
  if (!lib_ && !alt_path.empty()) {
    size_needed = MultiByteToWideChar(CP_UTF8, 0, alt_path.c_str(), (int)alt_path.size(), NULL, 0);
    wstrTo = std::wstring(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, alt_path.c_str(), (int)alt_path.size(), &wstrTo[0], size_needed);
    lib_ = ::LoadLibraryW(wstrTo.c_str());
  }
#else
  lib_ = dlopen(path.c_str(), RTLD_LAZY);
  if (!lib_ && !alt_path.empty()) {
    lib_ = dlopen(alt_path.c_str(), RTLD_LAZY);
  }
#endif
  if (!lib_) {
    std::cerr << "hebi::charts library not found! Include \""<< path <<"\" in library search path.\n";
    return;
  }

  // Check version compatibility. Note that we haven't finished the constructor, so we can't
  // just use the API method as it would cause a recursive call stack.
  lib::Version header_version = lib::getHeaderVersion();
  lib::Version lib_version(-1,0,0,0);
  auto hebi_charts_getLibraryVersion = getFunc<void(*)(int*, int*, int*, int*)>("hebi_charts_getLibraryVersion");
  hebi_charts_getLibraryVersion(&lib_version.major_, &lib_version.minor_, &lib_version.patch_, &lib_version.build_);

  bool is_compatible = (lib_version.major_ == header_version.major_ &&
                        lib_version.minor_ >= header_version.minor_ &&
                        (lib_version.minor_ > header_version.minor_ || lib_version.patch_ >= header_version.patch_));
  if (!is_compatible) {
    std::cerr << "Incompatible hebi::charts library version found!\n";
#ifdef WIN32
    if (!::FreeLibrary((HMODULE)lib_))
      std::cerr << "Error closing hebi::charts library!\n";
#else
    if (dlclose(lib_) != 0)
      std::cerr << "Error closing hebi::charts library!\n";
#endif
    lib_ = nullptr;
  }

}

} // namespace hebi
} // namespace charts
