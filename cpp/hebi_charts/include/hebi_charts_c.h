#pragma once
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MATLAB
typedef void (*UserCallbackFunction)(void* userData);
#else
typedef void* UserCallbackFunction; // Callbacks are not supported
#endif

typedef struct hebi_charts_Camera_* hebi_charts_CameraPtr;
typedef struct hebi_charts_Control_* hebi_charts_ControlPtr;
typedef struct hebi_charts_Button_* hebi_charts_ButtonPtr;
typedef struct hebi_charts_Dropdown_* hebi_charts_DropdownPtr;
typedef struct hebi_charts_Label_* hebi_charts_LabelPtr;
typedef struct hebi_charts_Slider_* hebi_charts_SliderPtr;
typedef struct hebi_charts_Toggle_* hebi_charts_TogglePtr;
typedef struct hebi_charts_ControlPanel_* hebi_charts_ControlPanelPtr;
typedef struct hebi_charts_Cursor_* hebi_charts_CursorPtr;
typedef struct hebi_charts_FxmlView_* hebi_charts_FxmlViewPtr;
typedef struct hebi_charts_GridWindow_* hebi_charts_GridWindowPtr;
typedef struct hebi_charts_HdrHistogramRecorder_* hebi_charts_HdrHistogramRecorderPtr;
typedef struct hebi_charts_HdrHistogramTrace_* hebi_charts_HdrHistogramTracePtr;
typedef struct hebi_charts_ImageStream_* hebi_charts_ImageStreamPtr;
typedef struct hebi_charts_LoopTimer_* hebi_charts_LoopTimerPtr;
typedef struct hebi_charts_Object3d_* hebi_charts_Object3dPtr;
typedef struct hebi_charts_Frame_* hebi_charts_FramePtr;
typedef struct hebi_charts_Mesh_* hebi_charts_MeshPtr;
typedef struct hebi_charts_Robot_* hebi_charts_RobotPtr;
typedef struct hebi_charts_Series3d_* hebi_charts_Series3dPtr;
typedef struct hebi_charts_Line3d_* hebi_charts_Line3dPtr;
typedef struct hebi_charts_Points3d_* hebi_charts_Points3dPtr;
typedef struct hebi_charts_RecordingResult_* hebi_charts_RecordingResultPtr;
typedef struct hebi_charts_Scene3d_* hebi_charts_Scene3dPtr;
typedef struct hebi_charts_StreamView_* hebi_charts_StreamViewPtr;
typedef struct hebi_charts_XYChart_* hebi_charts_XYChartPtr;
typedef struct hebi_charts_HistogramChart_* hebi_charts_HistogramChartPtr;
typedef struct hebi_charts_LineChart_* hebi_charts_LineChartPtr;
typedef struct hebi_charts_XYSeries_* hebi_charts_XYSeriesPtr;
typedef struct hebi_charts_HistogramTrace_* hebi_charts_HistogramTracePtr;
typedef struct hebi_charts_Line_* hebi_charts_LinePtr;

typedef enum hebi_charts_CameraView {
  hebi_charts_CameraView_ISOMETRIC = 0,
  hebi_charts_CameraView_FRONT = 1,
  hebi_charts_CameraView_BACK = 2,
  hebi_charts_CameraView_LEFT = 3,
  hebi_charts_CameraView_RIGHT = 4,
  hebi_charts_CameraView_TOP = 5,
  hebi_charts_CameraView_BOTTOM = 6
} hebi_charts_CameraView;
#if defined(_MSC_VER) || defined(__cplusplus)
static_assert(sizeof(hebi_charts_CameraView) == 4, "error: unexpected size for hebi_charts_CameraView enum");
#else
_Static_assert(sizeof(hebi_charts_CameraView) == 4, "error: unexpected size for hebi_charts_CameraView enum");
#endif

typedef enum hebi_charts_Color {
  hebi_charts_Color_Default = 0,
  hebi_charts_Color_Black = 1,
  hebi_charts_Color_White = 2,
  hebi_charts_Color_Gray = 3,
  hebi_charts_Color_Red = 4,
  hebi_charts_Color_Green = 5,
  hebi_charts_Color_Blue = 6,
  hebi_charts_Color_Yellow = 7,
  hebi_charts_Color_Magenta = 8
} hebi_charts_Color;
#if defined(_MSC_VER) || defined(__cplusplus)
static_assert(sizeof(hebi_charts_Color) == 4, "error: unexpected size for hebi_charts_Color enum");
#else
_Static_assert(sizeof(hebi_charts_Color) == 4, "error: unexpected size for hebi_charts_Color enum");
#endif

typedef enum hebi_charts_DisplayStyle {
  hebi_charts_DisplayStyle_Original = 0,
  hebi_charts_DisplayStyle_Translucent = 1,
  hebi_charts_DisplayStyle_Ghosted = 2
} hebi_charts_DisplayStyle;
#if defined(_MSC_VER) || defined(__cplusplus)
static_assert(sizeof(hebi_charts_DisplayStyle) == 4, "error: unexpected size for hebi_charts_DisplayStyle enum");
#else
_Static_assert(sizeof(hebi_charts_DisplayStyle) == 4, "error: unexpected size for hebi_charts_DisplayStyle enum");
#endif

typedef enum hebi_charts_LineStyle {
  hebi_charts_LineStyle_Default = 0,
  hebi_charts_LineStyle_Solid = 1,
  hebi_charts_LineStyle_Dashed = 2,
  hebi_charts_LineStyle_Points = 3
} hebi_charts_LineStyle;
#if defined(_MSC_VER) || defined(__cplusplus)
static_assert(sizeof(hebi_charts_LineStyle) == 4, "error: unexpected size for hebi_charts_LineStyle enum");
#else
_Static_assert(sizeof(hebi_charts_LineStyle) == 4, "error: unexpected size for hebi_charts_LineStyle enum");
#endif

typedef enum hebi_charts_MarkerShape {
  hebi_charts_MarkerShape_Default = 0,
  hebi_charts_MarkerShape_Tetrahedron = 1,
  hebi_charts_MarkerShape_Cube = 2,
  hebi_charts_MarkerShape_Octahedron = 3,
  hebi_charts_MarkerShape_Icosahedron = 4,
  hebi_charts_MarkerShape_Sphere = 5
} hebi_charts_MarkerShape;
#if defined(_MSC_VER) || defined(__cplusplus)
static_assert(sizeof(hebi_charts_MarkerShape) == 4, "error: unexpected size for hebi_charts_MarkerShape enum");
#else
_Static_assert(sizeof(hebi_charts_MarkerShape) == 4, "error: unexpected size for hebi_charts_MarkerShape enum");
#endif

typedef enum hebi_charts_MarkerType {
  hebi_charts_MarkerType_Default = 0,
  hebi_charts_MarkerType_Rectangle = 1,
  hebi_charts_MarkerType_Rectangle1 = 2,
  hebi_charts_MarkerType_Rectangle2 = 3,
  hebi_charts_MarkerType_Circle = 4,
  hebi_charts_MarkerType_Circle1 = 5,
  hebi_charts_MarkerType_Circle2 = 6,
  hebi_charts_MarkerType_Plus = 7,
  hebi_charts_MarkerType_Cross = 8,
  hebi_charts_MarkerType_Diamond = 9,
  hebi_charts_MarkerType_Diamond1 = 10,
  hebi_charts_MarkerType_Diamond2 = 11
} hebi_charts_MarkerType;
#if defined(_MSC_VER) || defined(__cplusplus)
static_assert(sizeof(hebi_charts_MarkerType) == 4, "error: unexpected size for hebi_charts_MarkerType enum");
#else
_Static_assert(sizeof(hebi_charts_MarkerType) == 4, "error: unexpected size for hebi_charts_MarkerType enum");
#endif

typedef enum hebi_charts_MatrixOrdering {
  hebi_charts_MatrixOrdering_RowMajor = 0,
  hebi_charts_MatrixOrdering_ColumnMajor = 1
} hebi_charts_MatrixOrdering;
#if defined(_MSC_VER) || defined(__cplusplus)
static_assert(sizeof(hebi_charts_MatrixOrdering) == 4, "error: unexpected size for hebi_charts_MatrixOrdering enum");
#else
_Static_assert(sizeof(hebi_charts_MatrixOrdering) == 4, "error: unexpected size for hebi_charts_MatrixOrdering enum");
#endif

typedef enum hebi_charts_PixelFormat {
  hebi_charts_PixelFormat_Unknown = 0,
  hebi_charts_PixelFormat_BGRA_PRE = 1,
  hebi_charts_PixelFormat_BGRA = 2,
  hebi_charts_PixelFormat_ARGB = 3,
  hebi_charts_PixelFormat_RGBA = 4,
  hebi_charts_PixelFormat_RGB = 5,
  hebi_charts_PixelFormat_BGR = 6,
  hebi_charts_PixelFormat_GRAY = 7
} hebi_charts_PixelFormat;
#if defined(_MSC_VER) || defined(__cplusplus)
static_assert(sizeof(hebi_charts_PixelFormat) == 4, "error: unexpected size for hebi_charts_PixelFormat enum");
#else
_Static_assert(sizeof(hebi_charts_PixelFormat) == 4, "error: unexpected size for hebi_charts_PixelFormat enum");
#endif

typedef enum hebi_charts_RuntimeOption {
  hebi_charts_RuntimeOption_DpiScale = 0,
  hebi_charts_RuntimeOption_GraphicsPipeline = 1,
  hebi_charts_RuntimeOption_TextAntialiasing = 2,
  hebi_charts_RuntimeOption_VSync = 3,
  hebi_charts_RuntimeOption_VerboseGraphics = 4,
  hebi_charts_RuntimeOption_VerbosePulse = 5,
  hebi_charts_RuntimeOption_VerboseFonts = 6,
  hebi_charts_RuntimeOption_VerboseWindowing = 7
} hebi_charts_RuntimeOption;
#if defined(_MSC_VER) || defined(__cplusplus)
static_assert(sizeof(hebi_charts_RuntimeOption) == 4, "error: unexpected size for hebi_charts_RuntimeOption enum");
#else
_Static_assert(sizeof(hebi_charts_RuntimeOption) == 4, "error: unexpected size for hebi_charts_RuntimeOption enum");
#endif

typedef enum hebi_charts_Theme {
  hebi_charts_Theme_PrimerLight = 0,
  hebi_charts_Theme_PrimerDark = 1,
  hebi_charts_Theme_NordLight = 2,
  hebi_charts_Theme_NordDark = 3,
  hebi_charts_Theme_CupertinoLight = 4,
  hebi_charts_Theme_CupertinoDark = 5,
  hebi_charts_Theme_Dracula = 6
} hebi_charts_Theme;
#if defined(_MSC_VER) || defined(__cplusplus)
static_assert(sizeof(hebi_charts_Theme) == 4, "error: unexpected size for hebi_charts_Theme enum");
#else
_Static_assert(sizeof(hebi_charts_Theme) == 4, "error: unexpected size for hebi_charts_Theme enum");
#endif

typedef enum hebi_charts_VideoOutputFormat {
  hebi_charts_VideoOutputFormat_H264 = 0,
  hebi_charts_VideoOutputFormat_H265 = 1,
  hebi_charts_VideoOutputFormat_ProRes = 2,
  hebi_charts_VideoOutputFormat_WebM = 3,
  hebi_charts_VideoOutputFormat_Gif = 4
} hebi_charts_VideoOutputFormat;
#if defined(_MSC_VER) || defined(__cplusplus)
static_assert(sizeof(hebi_charts_VideoOutputFormat) == 4, "error: unexpected size for hebi_charts_VideoOutputFormat enum");
#else
_Static_assert(sizeof(hebi_charts_VideoOutputFormat) == 4, "error: unexpected size for hebi_charts_VideoOutputFormat enum");
#endif


// ==== Camera ====
void hebi_charts_Camera_setView(hebi_charts_CameraPtr obj, hebi_charts_CameraView view);
void hebi_charts_Camera_reset(hebi_charts_CameraPtr obj);
int hebi_charts_Camera_applyRotation(hebi_charts_CameraPtr obj, double qx, double qy, double qz, double qw);
int hebi_charts_Camera_setDistance(hebi_charts_CameraPtr obj, double distanceInMeters);
int hebi_charts_Camera_setPan(hebi_charts_CameraPtr obj, double x, double y, double z);
void hebi_charts_Camera_setControlsVisible(hebi_charts_CameraPtr obj, int value);
void hebi_charts_Camera_release(hebi_charts_CameraPtr obj);

// ==== Control ====
int hebi_charts_Control_isEnabled(hebi_charts_ControlPtr obj);
void hebi_charts_Control_setEnabled(hebi_charts_ControlPtr obj, int enabled);
const char* hebi_charts_Control_getLabel(hebi_charts_ControlPtr obj);
void hebi_charts_Control_setLabel(hebi_charts_ControlPtr obj, const char* name);
const char* hebi_charts_Control_getTooltip(hebi_charts_ControlPtr obj);
void hebi_charts_Control_setTooltip(hebi_charts_ControlPtr obj, const char* tooltip);
int hebi_charts_Control_isVisible(hebi_charts_ControlPtr obj);
void hebi_charts_Control_setVisible(hebi_charts_ControlPtr obj, int visible);
void hebi_charts_Control_release(hebi_charts_ControlPtr obj);

// ==== Button ====
const char* hebi_charts_Button_getText(hebi_charts_ButtonPtr obj);
void hebi_charts_Button_setText(hebi_charts_ButtonPtr obj, const char* text);
int hebi_charts_Button_isPressed(hebi_charts_ButtonPtr obj);
int hebi_charts_Button_wasPressed(hebi_charts_ButtonPtr obj);
hebi_charts_ControlPtr hebi_charts_Button_to_Control(hebi_charts_ButtonPtr ptr);

// ==== Dropdown ====
void hebi_charts_Dropdown_setOptions(hebi_charts_DropdownPtr obj, const char** options, size_t count);
int hebi_charts_Dropdown_getSelectedIndex(hebi_charts_DropdownPtr obj);
void hebi_charts_Dropdown_setSelectedIndex(hebi_charts_DropdownPtr obj, int index);
void hebi_charts_Dropdown_addOption(hebi_charts_DropdownPtr obj, const char* option);
int hebi_charts_Dropdown_wasChanged(hebi_charts_DropdownPtr obj);
hebi_charts_ControlPtr hebi_charts_Dropdown_to_Control(hebi_charts_DropdownPtr ptr);

// ==== Label ====
const char* hebi_charts_Label_getText(hebi_charts_LabelPtr obj);
void hebi_charts_Label_setText(hebi_charts_LabelPtr obj, const char* text);
double hebi_charts_Label_getValue(hebi_charts_LabelPtr obj);
void hebi_charts_Label_setValue(hebi_charts_LabelPtr obj, double value);
hebi_charts_ControlPtr hebi_charts_Label_to_Control(hebi_charts_LabelPtr ptr);

// ==== Slider ====
void hebi_charts_Slider_setLimits(hebi_charts_SliderPtr obj, double min, double max);
double hebi_charts_Slider_getMax(hebi_charts_SliderPtr obj);
void hebi_charts_Slider_setMax(hebi_charts_SliderPtr obj, double max);
double hebi_charts_Slider_getMin(hebi_charts_SliderPtr obj);
void hebi_charts_Slider_setMin(hebi_charts_SliderPtr obj, double min);
double hebi_charts_Slider_getValue(hebi_charts_SliderPtr obj);
void hebi_charts_Slider_setValue(hebi_charts_SliderPtr obj, double value);
int hebi_charts_Slider_wasChanged(hebi_charts_SliderPtr obj);
hebi_charts_ControlPtr hebi_charts_Slider_to_Control(hebi_charts_SliderPtr ptr);

// ==== Toggle ====
int hebi_charts_Toggle_isSelected(hebi_charts_TogglePtr obj);
void hebi_charts_Toggle_setSelected(hebi_charts_TogglePtr obj, int selected);
int hebi_charts_Toggle_wasChanged(hebi_charts_TogglePtr obj);
hebi_charts_ControlPtr hebi_charts_Toggle_to_Control(hebi_charts_TogglePtr ptr);

// ==== ControlPanel ====
const char* hebi_charts_ControlPanel_getTitle(hebi_charts_ControlPanelPtr obj);
void hebi_charts_ControlPanel_setTitle(hebi_charts_ControlPanelPtr obj, const char* title);
double hebi_charts_ControlPanel_getWidth(hebi_charts_ControlPanelPtr obj);
void hebi_charts_ControlPanel_setWidth(hebi_charts_ControlPanelPtr obj, double width);
int hebi_charts_ControlPanel_addSection(hebi_charts_ControlPanelPtr obj, const char* title);
hebi_charts_LabelPtr hebi_charts_ControlPanel_addLabel(hebi_charts_ControlPanelPtr obj);
hebi_charts_ButtonPtr hebi_charts_ControlPanel_addButton(hebi_charts_ControlPanelPtr obj);
hebi_charts_ButtonPtr hebi_charts_ControlPanel_addStartButton(hebi_charts_ControlPanelPtr obj);
hebi_charts_ButtonPtr hebi_charts_ControlPanel_addStopButton(hebi_charts_ControlPanelPtr obj);
hebi_charts_SliderPtr hebi_charts_ControlPanel_addSlider(hebi_charts_ControlPanelPtr obj);
hebi_charts_TogglePtr hebi_charts_ControlPanel_addToggle(hebi_charts_ControlPanelPtr obj);
hebi_charts_DropdownPtr hebi_charts_ControlPanel_addDropdown(hebi_charts_ControlPanelPtr obj);
void hebi_charts_ControlPanel_release(hebi_charts_ControlPanelPtr obj);

// ==== Cursor ====
int hebi_charts_Cursor_isEditable(hebi_charts_CursorPtr obj);
void hebi_charts_Cursor_setEditable(hebi_charts_CursorPtr obj, int editable);
const char* hebi_charts_Cursor_getLabel(hebi_charts_CursorPtr obj);
void hebi_charts_Cursor_setLabel(hebi_charts_CursorPtr obj, const char* label);
double hebi_charts_Cursor_getValue(hebi_charts_CursorPtr obj);
void hebi_charts_Cursor_setValue(hebi_charts_CursorPtr obj, double value);
int hebi_charts_Cursor_isVisible(hebi_charts_CursorPtr obj);
void hebi_charts_Cursor_setVisible(hebi_charts_CursorPtr obj, int visible);
void hebi_charts_Cursor_release(hebi_charts_CursorPtr obj);

// ==== FxmlView ====
int hebi_charts_FxmlView_isAutoReload(hebi_charts_FxmlViewPtr obj);
void hebi_charts_FxmlView_setAutoReload(hebi_charts_FxmlViewPtr obj, int enableAutoReload);
const char* hebi_charts_FxmlView_getSource(hebi_charts_FxmlViewPtr obj);
int hebi_charts_FxmlView_setSource(hebi_charts_FxmlViewPtr obj, const char* pathOrUrl);
hebi_charts_LineChartPtr hebi_charts_FxmlView_addLineChart(hebi_charts_FxmlViewPtr obj, const char* fxId);
hebi_charts_LineChartPtr hebi_charts_FxmlView_addScope(hebi_charts_FxmlViewPtr obj, const char* fxId);
hebi_charts_HistogramChartPtr hebi_charts_FxmlView_addPercentileHistogram(hebi_charts_FxmlViewPtr obj, const char* fxId);
hebi_charts_HistogramChartPtr hebi_charts_FxmlView_addLatencyChart(hebi_charts_FxmlViewPtr obj, const char* fxId);
hebi_charts_Scene3dPtr hebi_charts_FxmlView_addScene3d(hebi_charts_FxmlViewPtr obj, const char* fxId);
hebi_charts_StreamViewPtr hebi_charts_FxmlView_addStreamView(hebi_charts_FxmlViewPtr obj, const char* file, const char* fxId);
hebi_charts_FxmlViewPtr hebi_charts_FxmlView_addFxmlView(hebi_charts_FxmlViewPtr obj, const char* fxId);
void hebi_charts_FxmlView_release(hebi_charts_FxmlViewPtr obj);

// ==== GridWindow ====
hebi_charts_GridWindowPtr hebi_charts_GridWindow_create(int rows, int cols);
int hebi_charts_GridWindow_isFullScreen(hebi_charts_GridWindowPtr obj);
void hebi_charts_GridWindow_setFullScreen(hebi_charts_GridWindowPtr obj, int fullScreen);
int hebi_charts_GridWindow_getHeight(hebi_charts_GridWindowPtr obj);
void hebi_charts_GridWindow_setHeight(hebi_charts_GridWindowPtr obj, int height);
int hebi_charts_GridWindow_getKeepOpen(hebi_charts_GridWindowPtr obj);
void hebi_charts_GridWindow_setKeepOpen(hebi_charts_GridWindowPtr obj, int keepOpen);
void hebi_charts_GridWindow_setLocation(hebi_charts_GridWindowPtr obj, int xOffset, int yOffset);
void hebi_charts_GridWindow_setSize(hebi_charts_GridWindowPtr obj, int width, int height);
const char* hebi_charts_GridWindow_getTitle(hebi_charts_GridWindowPtr obj);
void hebi_charts_GridWindow_setTitle(hebi_charts_GridWindowPtr obj, const char* title);
int hebi_charts_GridWindow_getWidth(hebi_charts_GridWindowPtr obj);
void hebi_charts_GridWindow_setWidth(hebi_charts_GridWindowPtr obj, int width);
int hebi_charts_GridWindow_getX(hebi_charts_GridWindowPtr obj);
void hebi_charts_GridWindow_setX(hebi_charts_GridWindowPtr obj, int x);
int hebi_charts_GridWindow_getY(hebi_charts_GridWindowPtr obj);
void hebi_charts_GridWindow_setY(hebi_charts_GridWindowPtr obj, int y);
hebi_charts_LineChartPtr hebi_charts_GridWindow_addLineChart(hebi_charts_GridWindowPtr obj, int row, int col, int rowSpan, int colSpan);
hebi_charts_LineChartPtr hebi_charts_GridWindow_addScope(hebi_charts_GridWindowPtr obj, int row, int col, int rowSpan, int colSpan);
hebi_charts_HistogramChartPtr hebi_charts_GridWindow_addPercentileHistogram(hebi_charts_GridWindowPtr obj, int row, int col, int rowSpan, int colSpan);
hebi_charts_HistogramChartPtr hebi_charts_GridWindow_addLatencyChart(hebi_charts_GridWindowPtr obj, int row, int col, int rowSpan, int colSpan);
hebi_charts_Scene3dPtr hebi_charts_GridWindow_addScene3d(hebi_charts_GridWindowPtr obj, int row, int col, int rowSpan, int colSpan);
hebi_charts_StreamViewPtr hebi_charts_GridWindow_addStreamView(hebi_charts_GridWindowPtr obj, const char* file, int row, int col, int rowSpan, int colSpan);
hebi_charts_FxmlViewPtr hebi_charts_GridWindow_addFxmlView(hebi_charts_GridWindowPtr obj, int row, int col, int rowSpan, int colSpan);
int hebi_charts_GridWindow_addStylesheet(hebi_charts_GridWindowPtr obj, const char* pathOrUrl, int autoReload);
void hebi_charts_GridWindow_show(hebi_charts_GridWindowPtr obj);
int hebi_charts_GridWindow_showOffScreen(hebi_charts_GridWindowPtr obj);
void hebi_charts_GridWindow_hide(hebi_charts_GridWindowPtr obj);
int hebi_charts_GridWindow_isShowing(hebi_charts_GridWindowPtr obj);
int hebi_charts_GridWindow_waitUntilClosed(hebi_charts_GridWindowPtr obj);
hebi_charts_ControlPanelPtr hebi_charts_GridWindow_getControlPanel(hebi_charts_GridWindowPtr obj);
hebi_charts_ImageStreamPtr hebi_charts_GridWindow_createImageStream(hebi_charts_GridWindowPtr obj);
void hebi_charts_GridWindow_dispatchMouseEvent(hebi_charts_GridWindowPtr obj, int action, int button, int downMask, int modifiers, double x, double y);
void hebi_charts_GridWindow_dispatchScrollEvent(hebi_charts_GridWindowPtr obj, double x, double y, double delta_x, double delta_y, int modifiers);
void hebi_charts_GridWindow_release(hebi_charts_GridWindowPtr obj);

// ==== HdrHistogramRecorder ====
hebi_charts_HdrHistogramRecorderPtr hebi_charts_HdrHistogramRecorder_create();
double hebi_charts_HdrHistogramRecorder_getFrequency(hebi_charts_HdrHistogramRecorderPtr obj);
int hebi_charts_HdrHistogramRecorder_setFrequency(hebi_charts_HdrHistogramRecorderPtr obj, double frequency);
double hebi_charts_HdrHistogramRecorder_getMax(hebi_charts_HdrHistogramRecorderPtr obj);
int hebi_charts_HdrHistogramRecorder_setMax(hebi_charts_HdrHistogramRecorderPtr obj, double value);
double hebi_charts_HdrHistogramRecorder_getMin(hebi_charts_HdrHistogramRecorderPtr obj);
int hebi_charts_HdrHistogramRecorder_setMin(hebi_charts_HdrHistogramRecorderPtr obj, double value);
int hebi_charts_HdrHistogramRecorder_getSignificantDigits(hebi_charts_HdrHistogramRecorderPtr obj);
int hebi_charts_HdrHistogramRecorder_setSignificantDigits(hebi_charts_HdrHistogramRecorderPtr obj, int significantDigits);
hebi_charts_HdrHistogramTracePtr hebi_charts_HdrHistogramRecorder_addTrace(hebi_charts_HdrHistogramRecorderPtr obj, const char* tag);
const char* hebi_charts_HdrHistogramRecorder_startRecording(hebi_charts_HdrHistogramRecorderPtr obj, const char* logFile);
void hebi_charts_HdrHistogramRecorder_stopRecording(hebi_charts_HdrHistogramRecorderPtr obj);
void hebi_charts_HdrHistogramRecorder_release(hebi_charts_HdrHistogramRecorderPtr obj);

// ==== HdrHistogramTrace ====
double hebi_charts_HdrHistogramTrace_getMax(hebi_charts_HdrHistogramTracePtr obj);
double hebi_charts_HdrHistogramTrace_getMean(hebi_charts_HdrHistogramTracePtr obj);
double hebi_charts_HdrHistogramTrace_getMin(hebi_charts_HdrHistogramTracePtr obj);
double hebi_charts_HdrHistogramTrace_getStdDeviation(hebi_charts_HdrHistogramTracePtr obj);
const char* hebi_charts_HdrHistogramTrace_getTag(hebi_charts_HdrHistogramTracePtr obj);
size_t hebi_charts_HdrHistogramTrace_getTotalCount(hebi_charts_HdrHistogramTracePtr obj);
hebi_charts_HdrHistogramTracePtr hebi_charts_HdrHistogramTrace_createLocal(int numberOfSignificantDigits, double minSeconds, double maxSeconds);
double hebi_charts_HdrHistogramTrace_getValueAtPercentile(hebi_charts_HdrHistogramTracePtr obj, double percentile);
void hebi_charts_HdrHistogramTrace_tic(hebi_charts_HdrHistogramTracePtr obj);
double hebi_charts_HdrHistogramTrace_toc(hebi_charts_HdrHistogramTracePtr obj);
double hebi_charts_HdrHistogramTrace_ticToc(hebi_charts_HdrHistogramTracePtr obj);
void hebi_charts_HdrHistogramTrace_recordValue(hebi_charts_HdrHistogramTracePtr obj, double value);
void hebi_charts_HdrHistogramTrace_recordValueWithCount(hebi_charts_HdrHistogramTracePtr obj, double value, size_t count);
void hebi_charts_HdrHistogramTrace_recordValueWithExpectedInterval(hebi_charts_HdrHistogramTracePtr obj, double value, double expectedIntervalBetweenSamples);
void hebi_charts_HdrHistogramTrace_reset(hebi_charts_HdrHistogramTracePtr obj);
const char* hebi_charts_HdrHistogramTrace_toHgrmString(hebi_charts_HdrHistogramTracePtr obj, double outputUnitsPerSecond);
const char* hebi_charts_HdrHistogramTrace_saveAsHgrm(hebi_charts_HdrHistogramTracePtr obj, const char* fileName, double outputUnitsPerSecond);
void hebi_charts_HdrHistogramTrace_release(hebi_charts_HdrHistogramTracePtr obj);

// ==== ImageStream ====
void* hebi_charts_ImageStream_getBuffer(hebi_charts_ImageStreamPtr obj);
size_t hebi_charts_ImageStream_getBufferSize(hebi_charts_ImageStreamPtr obj);
int hebi_charts_ImageStream_getChannels(hebi_charts_ImageStreamPtr obj);
int hebi_charts_ImageStream_getHeight(hebi_charts_ImageStreamPtr obj);
hebi_charts_PixelFormat hebi_charts_ImageStream_getPixelFormat(hebi_charts_ImageStreamPtr obj);
void hebi_charts_ImageStream_setPixelFormat(hebi_charts_ImageStreamPtr obj, hebi_charts_PixelFormat pixelFormat);
double hebi_charts_ImageStream_getRateLimit(hebi_charts_ImageStreamPtr obj);
int hebi_charts_ImageStream_setRateLimit(hebi_charts_ImageStreamPtr obj, double maxFramesPerSecond);
size_t hebi_charts_ImageStream_getRecorderThreads(hebi_charts_ImageStreamPtr obj);
int hebi_charts_ImageStream_setRecorderThreads(hebi_charts_ImageStreamPtr obj, size_t numThreads);
int hebi_charts_ImageStream_isRecording(hebi_charts_ImageStreamPtr obj);
double hebi_charts_ImageStream_getRenderScale(hebi_charts_ImageStreamPtr obj);
double hebi_charts_ImageStream_getRenderScaleX(hebi_charts_ImageStreamPtr obj);
double hebi_charts_ImageStream_getRenderScaleY(hebi_charts_ImageStreamPtr obj);
size_t hebi_charts_ImageStream_getSequence(hebi_charts_ImageStreamPtr obj);
int hebi_charts_ImageStream_getStride(hebi_charts_ImageStreamPtr obj);
double hebi_charts_ImageStream_getTimestamp(hebi_charts_ImageStreamPtr obj);
int hebi_charts_ImageStream_getWidth(hebi_charts_ImageStreamPtr obj);
int hebi_charts_ImageStream_setResolution(hebi_charts_ImageStreamPtr obj, int width, int height);
int hebi_charts_ImageStream_waitForNext(hebi_charts_ImageStreamPtr obj, size_t maxTimeoutMillis);
int hebi_charts_ImageStream_tryGetNext(hebi_charts_ImageStreamPtr obj);
int hebi_charts_ImageStream_startRecording(hebi_charts_ImageStreamPtr obj, const char* baseName, int overwrite);
hebi_charts_RecordingResultPtr hebi_charts_ImageStream_stopRecording(hebi_charts_ImageStreamPtr obj);
int hebi_charts_ImageStream_saveToFile(hebi_charts_ImageStreamPtr obj, const char* fileName);
void hebi_charts_ImageStream_release(hebi_charts_ImageStreamPtr obj);

// ==== LoopTimer ====
hebi_charts_LoopTimerPtr hebi_charts_LoopTimer_create();
double hebi_charts_LoopTimer_getElapsedTime(hebi_charts_LoopTimerPtr obj);
double hebi_charts_LoopTimer_getFrequency(hebi_charts_LoopTimerPtr obj);
void hebi_charts_LoopTimer_setFrequency(hebi_charts_LoopTimerPtr obj, double frequency);
double hebi_charts_LoopTimer_getPeriod(hebi_charts_LoopTimerPtr obj);
void hebi_charts_LoopTimer_setPeriod(hebi_charts_LoopTimerPtr obj, double seconds);
void hebi_charts_LoopTimer_restart(hebi_charts_LoopTimerPtr obj);
void hebi_charts_LoopTimer_tic(hebi_charts_LoopTimerPtr obj);
double hebi_charts_LoopTimer_toc(hebi_charts_LoopTimerPtr obj);
double hebi_charts_LoopTimer_ticToc(hebi_charts_LoopTimerPtr obj);
void hebi_charts_LoopTimer_waitForNextTick(hebi_charts_LoopTimerPtr obj);
double hebi_charts_LoopTimer_getSecondsToNextTick(hebi_charts_LoopTimerPtr obj);
size_t hebi_charts_LoopTimer_getNanosToNextTick(hebi_charts_LoopTimerPtr obj);
double hebi_charts_LoopTimer_timeSeconds();
size_t hebi_charts_LoopTimer_timeNanos();
void hebi_charts_LoopTimer_sleepNanos(size_t nanos);
void hebi_charts_LoopTimer_parkNanos(size_t nanos);
void hebi_charts_LoopTimer_waitNanos(size_t nanos);
void hebi_charts_LoopTimer_spinNanos(size_t nanos);
void hebi_charts_LoopTimer_release(hebi_charts_LoopTimerPtr obj);

// ==== Object3d ====
int hebi_charts_Object3d_isVisible(hebi_charts_Object3dPtr obj);
void hebi_charts_Object3d_setVisible(hebi_charts_Object3dPtr obj, int visible);
int hebi_charts_Object3d_setOrientation(hebi_charts_Object3dPtr obj, double qx, double qy, double qz, double qw);
int hebi_charts_Object3d_setOrientationRPY(hebi_charts_Object3dPtr obj, double roll, double pitch, double yaw);
int hebi_charts_Object3d_setTranslation(hebi_charts_Object3dPtr obj, double x, double y, double z);
int hebi_charts_Object3d_setPose(hebi_charts_Object3dPtr obj, double x, double y, double z, double qx, double qy, double qz, double qw);
int hebi_charts_Object3d_setTransform4x4(hebi_charts_Object3dPtr obj, const double* matrix, hebi_charts_MatrixOrdering ordering);
void hebi_charts_Object3d_release(hebi_charts_Object3dPtr obj);

// ==== Frame ====
hebi_charts_Object3dPtr hebi_charts_Frame_to_Object3d(hebi_charts_FramePtr ptr);

// ==== Mesh ====
int hebi_charts_Mesh_isCentered(hebi_charts_MeshPtr obj);
void hebi_charts_Mesh_setCentered(hebi_charts_MeshPtr obj, int centered);
double hebi_charts_Mesh_getScale(hebi_charts_MeshPtr obj);
void hebi_charts_Mesh_setScale(hebi_charts_MeshPtr obj, double scaleUnitsToMillimeters);
hebi_charts_DisplayStyle hebi_charts_Mesh_getDisplayStyle(hebi_charts_MeshPtr obj);
void hebi_charts_Mesh_setDisplayStyle(hebi_charts_MeshPtr obj, hebi_charts_DisplayStyle style);
hebi_charts_Object3dPtr hebi_charts_Mesh_to_Object3d(hebi_charts_MeshPtr ptr);

// ==== Robot ====
hebi_charts_DisplayStyle hebi_charts_Robot_getDisplayStyle(hebi_charts_RobotPtr obj);
void hebi_charts_Robot_setDisplayStyle(hebi_charts_RobotPtr obj, hebi_charts_DisplayStyle style);
size_t hebi_charts_Robot_getDof(hebi_charts_RobotPtr obj);
int hebi_charts_Robot_setPositions(hebi_charts_RobotPtr obj, const double* positions, size_t length);
hebi_charts_Object3dPtr hebi_charts_Robot_to_Object3d(hebi_charts_RobotPtr ptr);

// ==== Series3d ====
hebi_charts_Color hebi_charts_Series3d_getColor(hebi_charts_Series3dPtr obj);
void hebi_charts_Series3d_setColor(hebi_charts_Series3dPtr obj, hebi_charts_Color color);
hebi_charts_Object3dPtr hebi_charts_Series3d_to_Object3d(hebi_charts_Series3dPtr ptr);

// ==== Line3d ====
void hebi_charts_Line3d_setMaxPointCount(hebi_charts_Line3dPtr obj, size_t count);
void hebi_charts_Line3d_clear(hebi_charts_Line3dPtr obj);
void hebi_charts_Line3d_setData(hebi_charts_Line3dPtr obj, const double* x, const double* y, const double* z, size_t length);
void hebi_charts_Line3d_addPoints(hebi_charts_Line3dPtr obj, const double* x, const double* y, const double* z, size_t length);
void hebi_charts_Line3d_addPoint(hebi_charts_Line3dPtr obj, double x, double y, double z);
hebi_charts_Series3dPtr hebi_charts_Line3d_to_Series3d(hebi_charts_Line3dPtr ptr);

// ==== Points3d ====
hebi_charts_MarkerShape hebi_charts_Points3d_getMarkerShape(hebi_charts_Points3dPtr obj);
void hebi_charts_Points3d_setMarkerShape(hebi_charts_Points3dPtr obj, hebi_charts_MarkerShape shape);
double hebi_charts_Points3d_getMarkerSize(hebi_charts_Points3dPtr obj);
void hebi_charts_Points3d_setMarkerSize(hebi_charts_Points3dPtr obj, double value);
int hebi_charts_Points3d_isSelfIllumination(hebi_charts_Points3dPtr obj);
void hebi_charts_Points3d_setSelfIllumination(hebi_charts_Points3dPtr obj, int value);
int hebi_charts_Points3d_isVertexSharing(hebi_charts_Points3dPtr obj);
void hebi_charts_Points3d_setVertexSharing(hebi_charts_Points3dPtr obj, int value);
void hebi_charts_Points3d_setMaxPointCount(hebi_charts_Points3dPtr obj, size_t count);
void hebi_charts_Points3d_clear(hebi_charts_Points3dPtr obj);
void hebi_charts_Points3d_setData(hebi_charts_Points3dPtr obj, const double* x, const double* y, const double* z, size_t length);
void hebi_charts_Points3d_addPoints(hebi_charts_Points3dPtr obj, const double* x, const double* y, const double* z, size_t length);
void hebi_charts_Points3d_addPoint(hebi_charts_Points3dPtr obj, double x, double y, double z);
hebi_charts_Series3dPtr hebi_charts_Points3d_to_Series3d(hebi_charts_Points3dPtr ptr);

// ==== RecordingResult ====
const char* hebi_charts_RecordingResult_getDirectory(hebi_charts_RecordingResultPtr obj);
size_t hebi_charts_RecordingResult_getDroppedCount(hebi_charts_RecordingResultPtr obj);
double hebi_charts_RecordingResult_getDuration(hebi_charts_RecordingResultPtr obj);
double hebi_charts_RecordingResult_getAverageFps(hebi_charts_RecordingResultPtr obj);
const char* hebi_charts_RecordingResult_getManifest(hebi_charts_RecordingResultPtr obj);
size_t hebi_charts_RecordingResult_getRecordedCount(hebi_charts_RecordingResultPtr obj);
size_t hebi_charts_RecordingResult_getSkippedCount(hebi_charts_RecordingResultPtr obj);
size_t hebi_charts_RecordingResult_getTotalFrames(hebi_charts_RecordingResultPtr obj);
const char* hebi_charts_RecordingResult_getFfmpegCommand(hebi_charts_RecordingResultPtr obj, hebi_charts_VideoOutputFormat outputFormat, int deleteDirectory);
const char* hebi_charts_RecordingResult_runFfmpeg(hebi_charts_RecordingResultPtr obj, hebi_charts_VideoOutputFormat outputFormat, int deleteDirectory);
void hebi_charts_RecordingResult_release(hebi_charts_RecordingResultPtr obj);

// ==== Scene3d ====
void hebi_charts_Scene3d_setGridBounds(hebi_charts_Scene3dPtr obj, double minX, double maxX, double minY, double maxY, double minZ, double maxZ);
double hebi_charts_Scene3d_getMaxX(hebi_charts_Scene3dPtr obj);
void hebi_charts_Scene3d_setMaxX(hebi_charts_Scene3dPtr obj, double val);
double hebi_charts_Scene3d_getMaxY(hebi_charts_Scene3dPtr obj);
void hebi_charts_Scene3d_setMaxY(hebi_charts_Scene3dPtr obj, double val);
double hebi_charts_Scene3d_getMaxZ(hebi_charts_Scene3dPtr obj);
void hebi_charts_Scene3d_setMaxZ(hebi_charts_Scene3dPtr obj, double val);
double hebi_charts_Scene3d_getMinX(hebi_charts_Scene3dPtr obj);
void hebi_charts_Scene3d_setMinX(hebi_charts_Scene3dPtr obj, double val);
double hebi_charts_Scene3d_getMinY(hebi_charts_Scene3dPtr obj);
void hebi_charts_Scene3d_setMinY(hebi_charts_Scene3dPtr obj, double val);
double hebi_charts_Scene3d_getMinZ(hebi_charts_Scene3dPtr obj);
void hebi_charts_Scene3d_setMinZ(hebi_charts_Scene3dPtr obj, double val);
double hebi_charts_Scene3d_getGridStep(hebi_charts_Scene3dPtr obj);
void hebi_charts_Scene3d_setGridStep(hebi_charts_Scene3dPtr obj, double gridStep);
hebi_charts_CameraPtr hebi_charts_Scene3d_getCamera(hebi_charts_Scene3dPtr obj);
hebi_charts_RobotPtr hebi_charts_Scene3d_addRobot(hebi_charts_Scene3dPtr obj, const char* pathOrUrl);
hebi_charts_MeshPtr hebi_charts_Scene3d_addMesh(hebi_charts_Scene3dPtr obj, const char* pathOrUrl);
hebi_charts_FramePtr hebi_charts_Scene3d_addFrame(hebi_charts_Scene3dPtr obj, double lengthInMeters);
hebi_charts_Line3dPtr hebi_charts_Scene3d_addLine(hebi_charts_Scene3dPtr obj);
hebi_charts_Points3dPtr hebi_charts_Scene3d_addPoints(hebi_charts_Scene3dPtr obj);
void hebi_charts_Scene3d_release(hebi_charts_Scene3dPtr obj);

// ==== StreamView ====
void hebi_charts_StreamView_release(hebi_charts_StreamViewPtr obj);

// ==== XYChart ====
const char* hebi_charts_XYChart_getTitle(hebi_charts_XYChartPtr obj);
void hebi_charts_XYChart_setTitle(hebi_charts_XYChartPtr obj, const char* title);
int hebi_charts_XYChart_getXAssumeSorted(hebi_charts_XYChartPtr obj);
void hebi_charts_XYChart_setXAssumeSorted(hebi_charts_XYChartPtr obj, int xAssumeSorted);
int hebi_charts_XYChart_isXAutoUnitScaling(hebi_charts_XYChartPtr obj);
void hebi_charts_XYChart_setXAutoUnitScaling(hebi_charts_XYChartPtr obj, int enabled);
const char* hebi_charts_XYChart_getXLabel(hebi_charts_XYChartPtr obj);
void hebi_charts_XYChart_setXLabel(hebi_charts_XYChartPtr obj, const char* label);
int hebi_charts_XYChart_setXLim(hebi_charts_XYChartPtr obj, double min, double max);
double hebi_charts_XYChart_getXMax(hebi_charts_XYChartPtr obj);
void hebi_charts_XYChart_setXMax(hebi_charts_XYChartPtr obj, double max);
double hebi_charts_XYChart_getXMin(hebi_charts_XYChartPtr obj);
void hebi_charts_XYChart_setXMin(hebi_charts_XYChartPtr obj, double min);
const char* hebi_charts_XYChart_getXUnit(hebi_charts_XYChartPtr obj);
void hebi_charts_XYChart_setXUnit(hebi_charts_XYChartPtr obj, const char* unit);
int hebi_charts_XYChart_isYAutoUnitScaling(hebi_charts_XYChartPtr obj);
void hebi_charts_XYChart_setYAutoUnitScaling(hebi_charts_XYChartPtr obj, int enabled);
const char* hebi_charts_XYChart_getYLabel(hebi_charts_XYChartPtr obj);
void hebi_charts_XYChart_setYLabel(hebi_charts_XYChartPtr obj, const char* label);
void hebi_charts_XYChart_setYLim(hebi_charts_XYChartPtr obj, double min, double max);
double hebi_charts_XYChart_getYMax(hebi_charts_XYChartPtr obj);
void hebi_charts_XYChart_setYMax(hebi_charts_XYChartPtr obj, double max);
double hebi_charts_XYChart_getYMin(hebi_charts_XYChartPtr obj);
void hebi_charts_XYChart_setYMin(hebi_charts_XYChartPtr obj, double min);
const char* hebi_charts_XYChart_getYUnit(hebi_charts_XYChartPtr obj);
void hebi_charts_XYChart_setYUnit(hebi_charts_XYChartPtr obj, const char* unit);
hebi_charts_CursorPtr hebi_charts_XYChart_addXCursor(hebi_charts_XYChartPtr obj);
hebi_charts_CursorPtr hebi_charts_XYChart_addYCursor(hebi_charts_XYChartPtr obj);
void hebi_charts_XYChart_release(hebi_charts_XYChartPtr obj);

// ==== HistogramChart ====
hebi_charts_HistogramTracePtr hebi_charts_HistogramChart_addTrace(hebi_charts_HistogramChartPtr obj, const char* name);
hebi_charts_XYChartPtr hebi_charts_HistogramChart_to_XYChart(hebi_charts_HistogramChartPtr ptr);

// ==== LineChart ====
hebi_charts_LinePtr hebi_charts_LineChart_addLine(hebi_charts_LineChartPtr obj, const char* label);
hebi_charts_XYChartPtr hebi_charts_LineChart_to_XYChart(hebi_charts_LineChartPtr ptr);

// ==== XYSeries ====
hebi_charts_Color hebi_charts_XYSeries_getColor(hebi_charts_XYSeriesPtr obj);
void hebi_charts_XYSeries_setColor(hebi_charts_XYSeriesPtr obj, hebi_charts_Color color);
const char* hebi_charts_XYSeries_getLabel(hebi_charts_XYSeriesPtr obj);
void hebi_charts_XYSeries_setLabel(hebi_charts_XYSeriesPtr obj, const char* label);
hebi_charts_LineStyle hebi_charts_XYSeries_getLineStyle(hebi_charts_XYSeriesPtr obj);
void hebi_charts_XYSeries_setLineStyle(hebi_charts_XYSeriesPtr obj, hebi_charts_LineStyle lineStyle);
double hebi_charts_XYSeries_getLineWidth(hebi_charts_XYSeriesPtr obj);
void hebi_charts_XYSeries_setLineWidth(hebi_charts_XYSeriesPtr obj, double lineWidth);
double hebi_charts_XYSeries_getMarkerSize(hebi_charts_XYSeriesPtr obj);
void hebi_charts_XYSeries_setMarkerSize(hebi_charts_XYSeriesPtr obj, double markerSize);
hebi_charts_MarkerType hebi_charts_XYSeries_getMarkerType(hebi_charts_XYSeriesPtr obj);
void hebi_charts_XYSeries_setMarkerType(hebi_charts_XYSeriesPtr obj, hebi_charts_MarkerType markerType);
int hebi_charts_XYSeries_isShowInLegend(hebi_charts_XYSeriesPtr obj);
void hebi_charts_XYSeries_setShowInLegend(hebi_charts_XYSeriesPtr obj, int showInLegend);
int hebi_charts_XYSeries_isVisible(hebi_charts_XYSeriesPtr obj);
void hebi_charts_XYSeries_setVisible(hebi_charts_XYSeriesPtr obj, int visible);
void hebi_charts_XYSeries_release(hebi_charts_XYSeriesPtr obj);

// ==== HistogramTrace ====
void hebi_charts_HistogramTrace_record(hebi_charts_HistogramTracePtr obj, double value);
void hebi_charts_HistogramTrace_recordWithCount(hebi_charts_HistogramTracePtr obj, double value, size_t count);
void hebi_charts_HistogramTrace_recordCompensated(hebi_charts_HistogramTracePtr obj, double value, double expectedIntervalBetweenValueSamples);
void hebi_charts_HistogramTrace_reset(hebi_charts_HistogramTracePtr obj);
hebi_charts_XYSeriesPtr hebi_charts_HistogramTrace_to_XYSeries(hebi_charts_HistogramTracePtr ptr);

// ==== Line ====
size_t hebi_charts_Line_getMaxPointCount(hebi_charts_LinePtr obj);
void hebi_charts_Line_setMaxPointCount(hebi_charts_LinePtr obj, size_t count);
void hebi_charts_Line_clear(hebi_charts_LinePtr obj);
void hebi_charts_Line_setData(hebi_charts_LinePtr obj, const double* x, const double* y, size_t length);
void hebi_charts_Line_addPoints(hebi_charts_LinePtr obj, const double* x, const double* y, size_t length);
void hebi_charts_Line_addPoint(hebi_charts_LinePtr obj, double x, double y);
hebi_charts_XYSeriesPtr hebi_charts_Line_to_XYSeries(hebi_charts_LinePtr ptr);

// ==== Runtime ====
int hebi_charts_Runtime_setOption(hebi_charts_RuntimeOption option, const char* value);
void hebi_charts_Runtime_setTheme(hebi_charts_Theme theme);
void hebi_charts_Runtime_setAutoCloseWindows(int autoClose);
int hebi_charts_Runtime_waitUntilWindowsClosed();
void hebi_charts_Runtime_collect();
void hebi_charts_Runtime_closeAll();
void hebi_charts_Runtime_runOnUiThread(UserCallbackFunction func, void* userData);
void hebi_charts_Runtime_printLastErrorDetails();
void hebi_charts_Runtime_printThreadInfo(const char* name);
const char* hebi_charts_Runtime_getLastErrorString();

void hebi_charts_getLibraryVersion(int* major, int* minor, int* patch, int* build);
void hebi_charts_getHeaderVersion(int* major, int* minor, int* patch, int* build) {
    *major = 0;
    *minor = 9;
    *patch = 2;
    *build = 118;
}


// ==== Cocoa utilities for supporting macOS ====
#ifndef MATLAB
typedef int (*hebi_charts_MainCallbackFunction)(int argc, char** argv);
int hebi_charts_runApplication(hebi_charts_MainCallbackFunction callback, int argc, char** argv);
#endif


#ifdef __cplusplus
}
#endif
