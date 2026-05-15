#include "mex.h"
#include <cstddef>
#include <cstdint>
#include <string>

#ifdef _WIN32
  #include <windows.h>
#else
  #include <dlfcn.h>
#endif

// We set symbols to be hidden by default, but there are some macros in mex.h that break the
// MSVC macro. Thus, we manually fix MSVC in the cmake, and use the gnu macro for mingw.
#if defined(__GNUC__) || defined(__clang__)
#define MEX_EXPORT __attribute__((visibility("default")))
#else
#define MEX_EXPORT // msvc: __declspec(dllexport)
#endif

// Hashing function for turning names to integers (FNV-1a). The seed and prime are chosen
// such that there are no hash collisions within the known set.
// The hash is calculated directly on the raw UTF-16 data. C functions are limited to ASCII,
// so the upper bits are always zero and can be ignored.
inline uint32_t hash32_fnv1a(const mxChar* str, size_t len) {
    uint32_t hash = 0x811C9DC5;
    for (size_t i = 0; i < len; ++i) {
        hash = (hash ^ (uint32_t)((uint16_t) str[i] & 0xFF)) * 0x01000193;
    }
    return hash;
}

template <typename T>
inline T getScalar(const mxArray* arg) {
    return static_cast<T>(mxGetScalar(arg));
}

template <typename T, typename S>
inline void copyData(T* dest, const void* src, size_t n) {
    auto s = static_cast<const S*>(src);
    for (size_t i = 0; i < n; ++i) dest[i] = static_cast<T>(s[i]);
}

template <typename T, mxClassID Category>
struct MatlabArray {
    T* ptr;
    mxArray* tmp = nullptr;

    explicit MatlabArray(const mxArray* arg) {
        if (mxIsComplex(arg)) {
            mexErrMsgIdAndTxt("hebi_charts:TypeError", "Complex arrays are not supported.");
        }

        if (mxGetClassID(arg) == Category) {
            // Perfect match: Zero-copy
            ptr = (T*)mxGetData(arg);
        } else {
            // Mismatch: Create a converted copy (like calllib does)
            tmp = mxCreateNumericMatrix(mxGetM(arg), mxGetN(arg), Category, mxREAL);
            ptr = (T*)mxGetData(tmp);
            size_t n = mxGetNumberOfElements(arg);

            // Convert types individually
            const void* srcData = mxGetData(arg);
            switch (mxGetClassID(arg)) {
                case mxDOUBLE_CLASS:  copyData<T, double>(ptr, srcData, n); break;
                case mxSINGLE_CLASS:  copyData<T, float>(ptr, srcData, n);  break;
                case mxINT8_CLASS:    copyData<T, int8_t>(ptr, srcData, n); break;
                case mxUINT8_CLASS:   copyData<T, uint8_t>(ptr, srcData, n); break;
                case mxINT16_CLASS:   copyData<T, int16_t>(ptr, srcData, n); break;
                case mxUINT16_CLASS:  copyData<T, uint16_t>(ptr, srcData, n); break;
                case mxINT32_CLASS:   copyData<T, int32_t>(ptr, srcData, n); break;
                case mxUINT32_CLASS:  copyData<T, uint32_t>(ptr, srcData, n); break;
                case mxINT64_CLASS:   copyData<T, int64_t>(ptr, srcData, n); break;
                case mxUINT64_CLASS:  copyData<T, uint64_t>(ptr, srcData, n); break;
                case mxLOGICAL_CLASS: copyData<T, bool>(ptr, srcData, n);    break;
                default: mexErrMsgTxt("Unsupported conversion type");
            }

        }
    }

    ~MatlabArray() { if (tmp) mxDestroyArray(tmp); }
    operator T*() { return ptr; }
};

inline char* charsToUtf8String(const mxArray* arg) {
    if (mxIsClass(arg, "string")) {
        mexErrMsgIdAndTxt("hebi_charts:TypeError", "Double-quoted strings are not supported. Use single quotes 'char' arrays.");
    } else if(!mxIsChar(arg)) {
        mexErrMsgIdAndTxt("hebi_charts:TypeError", "Expected single quotes 'char' array");
    }
    return mxArrayToUTF8String(arg);
}

// RAII cleanup for temporary utf8 strings
struct MatlabString {
    char* ptr;
    explicit MatlabString(const mxArray* arg) : ptr(charsToUtf8String(arg)) {}
    ~MatlabString() { if (ptr) mxFree(ptr); }
    operator const char*() const { return ptr; }
    MatlabString(const MatlabString&) = delete;
};

// RAII cleanup for temporary utf8 string arrays
struct MatlabStringArray {
    char** ptrs;
    size_t count;
    explicit MatlabStringArray(const mxArray* arg) {
        if (!mxIsCell(arg)) mexErrMsgIdAndTxt("hebi_charts:InvalidInput", "Expected cell array of strings.");
        count = mxGetNumberOfElements(arg);
        if (count == 0) {
            ptrs = nullptr;
            return;
        }
        ptrs = (char**)mxCalloc(count, sizeof(char*));
        for (size_t i = 0; i < count; ++i) {
            ptrs[i] = charsToUtf8String(mxGetCell(arg, i));
        }
    }
    ~MatlabStringArray() {
        if (ptrs) {
            for (size_t i = 0; i < count; ++i) if (ptrs[i]) mxFree(ptrs[i]);
            mxFree(ptrs);
        }
    }
    operator const char**() const { return (const char**)ptrs; }
    MatlabStringArray(const MatlabStringArray&) = delete;
};

inline mxArray* createEmpty() {
    // Returns a 0x0 double array, which is the standard MATLAB 'null'
    return mxCreateDoubleMatrix(0, 0, mxREAL);
}

inline mxArray* createMexString(const char* c_str) {
    return c_str ? mxCreateString(c_str) : createEmpty();
}

template <typename T>
inline T getPtr(const mxArray* array) {
    if (mxIsEmpty(array)) {
        return nullptr; // C-Style nullptr
    }
    if (!mxIsUint64(array)) {
        mexErrMsgIdAndTxt("hebi_charts:TypeError", "Pointer must be a uint64 scalar.");
    }
    return reinterpret_cast<T>(((uint64_t*)mxGetData(array))[0]);
}

inline mxArray* createPtr(uint64_t value) {
    auto array = mxCreateNumericMatrix(1, 1, mxUINT64_CLASS, mxREAL);
    ((uint64_t*)mxGetData(array))[0] = value;
    return array;
}

inline mxArray* createMexPointer(const void* c_ptr) {
    // Cast pointer to uintptr_t to safely convert to double without precision loss
    return c_ptr ? createPtr((uint64_t)(uintptr_t)c_ptr) : createEmpty();
}

template <typename T>
inline mxArray* createMexScalar(T value) {
    // MATLAB saves all numbers as double by default. Going with something else can
    // cerate issues. This gives us 2^53 bits for integers before loss of precision,
    // which is fine for everything but pointers.
    return mxCreateDoubleScalar(static_cast<double>(value));
}

// Determine the path to the library by calling into MATLAB (may auto-download)
std::string getLibraryPath() {
    mxArray* infoStruct = nullptr;
    if (mexCallMATLAB(1, &infoStruct, 0, nullptr, "locate_binary") != 0) {
        mexErrMsgIdAndTxt("hebi_charts:PathError", "Failed to call locate_binary");
    }

    mxArray* pathArray = mxGetField(infoStruct, 0, "binPath");
    if (!pathArray) {
        mxDestroyArray(infoStruct);
        mexErrMsgIdAndTxt("hebi_charts:PathError", "locate_binary did not return 'binPath'");
    }

    char* pathCStr = charsToUtf8String(pathArray);
    std::string path(pathCStr);

    mxFree(pathCStr);
    mxDestroyArray(infoStruct);
    return path;
}

class LibLoader {
public:
    explicit LibLoader(const std::string& path) {
#ifdef _WIN32
        // Support UTF-8 paths on Windows
        int wsize = MultiByteToWideChar(CP_UTF8, 0, path.c_str(), (int)path.size(), NULL, 0);
        std::wstring wpath(wsize, 0);
        MultiByteToWideChar(CP_UTF8, 0, path.c_str(), (int)path.size(), &wpath[0], wsize);
        handle_ = LoadLibraryW(wpath.c_str());
#else
        handle_ = dlopen(path.c_str(), RTLD_LAZY | RTLD_GLOBAL);
#endif
        if (!handle_) mexErrMsgIdAndTxt("hebi_charts:LoadError", "Could not load library: %s", path.c_str());
    }

    template<typename T>
    T getFunc(const char* name) const {
#ifdef _WIN32
        FARPROC symbol = GetProcAddress((HMODULE)handle_, name);
#else
        void* symbol = dlsym(handle_, name);
#endif
        if(!symbol) {
            mexErrMsgIdAndTxt("hebi_charts:LoadError", "Could not load symbol: %s", name);
        }
        return reinterpret_cast<T>(symbol);
    }

    ~LibLoader() {
        if (handle_) {
#ifdef _WIN32
            FreeLibrary((HMODULE)handle_);
#else
            dlclose(handle_);
#endif
        }
    }

private:
    void* handle_ = nullptr;
};

#ifdef __cplusplus
extern "C" {
#endif

MEX_EXPORT void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
     uint32_t funcHash;
     if (nrhs >= 1 && mxIsChar(prhs[0])) {
         // Zero-allocation hash MATLAB's 16 bit chars.
         const mxChar *funcName = (const mxChar *) mxGetData(prhs[0]);
         size_t nameLen = mxGetNumberOfElements(prhs[0]);
         funcHash = hash32_fnv1a(funcName, nameLen);
     } else if (nrhs >= 1 && mxGetClassID(prhs[0]) == mxUINT32_CLASS && mxGetNumberOfElements(prhs[0]) == 1) {
         // Raw function hash
         funcHash = ((const uint32_t *) mxGetData(prhs[0]))[0];
     } else {
         mexErrMsgIdAndTxt("hebi_charts:InvalidInput", "First argument must be a function identifier.");
     }

    // Bind to the binary once per MATLAB session/clear
    static std::string binPath = getLibraryPath();
    static LibLoader libLoader(binPath);

    // Switch on a Perfect Hash (seeds are chosen until there are no collisions)
    switch (funcHash) {

        // Camera
        case /* hash32_fnv1a("hebi_charts_Camera_setView") */ 3801818395: {
            static auto hebi_charts_Camera_setView = libLoader.getFunc<void(*)(void*, int)>("hebi_charts_Camera_setView");
            int view = getScalar<int>(prhs[2]);
            hebi_charts_Camera_setView(getPtr<void*>(prhs[1]), view);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Camera_reset") */ 1415684501: {
            static auto hebi_charts_Camera_reset = libLoader.getFunc<void(*)(void*)>("hebi_charts_Camera_reset");
            hebi_charts_Camera_reset(getPtr<void*>(prhs[1]));
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Camera_applyRotation") */ 3985810136: {
            static auto hebi_charts_Camera_applyRotation = libLoader.getFunc<int(*)(void*, double, double, double, double)>("hebi_charts_Camera_applyRotation");
            double qx = getScalar<double>(prhs[2]);
            double qy = getScalar<double>(prhs[3]);
            double qz = getScalar<double>(prhs[4]);
            double qw = getScalar<double>(prhs[5]);
            int _value = hebi_charts_Camera_applyRotation(getPtr<void*>(prhs[1]), qx, qy, qz, qw);
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Camera_setDistance") */ 1243202457: {
            static auto hebi_charts_Camera_setDistance = libLoader.getFunc<int(*)(void*, double)>("hebi_charts_Camera_setDistance");
            double distanceInMeters = getScalar<double>(prhs[2]);
            int _value = hebi_charts_Camera_setDistance(getPtr<void*>(prhs[1]), distanceInMeters);
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Camera_setPan") */ 2602784433: {
            static auto hebi_charts_Camera_setPan = libLoader.getFunc<int(*)(void*, double, double, double)>("hebi_charts_Camera_setPan");
            double x = getScalar<double>(prhs[2]);
            double y = getScalar<double>(prhs[3]);
            double z = getScalar<double>(prhs[4]);
            int _value = hebi_charts_Camera_setPan(getPtr<void*>(prhs[1]), x, y, z);
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Camera_setControlsVisible") */ 3987743262: {
            static auto hebi_charts_Camera_setControlsVisible = libLoader.getFunc<void(*)(void*, int)>("hebi_charts_Camera_setControlsVisible");
            int value = getScalar<int>(prhs[2]);
            hebi_charts_Camera_setControlsVisible(getPtr<void*>(prhs[1]), value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Camera_release") */ 2037833667: {
            static auto hebi_charts_Camera_release = libLoader.getFunc<void(*)(void*)>("hebi_charts_Camera_release");
            hebi_charts_Camera_release(getPtr<void*>(prhs[1]));
            break;
        }

        // Control
        case /* hash32_fnv1a("hebi_charts_Control_isEnabled") */ 3777718777: {
            static auto hebi_charts_Control_isEnabled = libLoader.getFunc<int(*)(void*)>("hebi_charts_Control_isEnabled");
            int _value = hebi_charts_Control_isEnabled(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Control_setEnabled") */ 3290542769: {
            static auto hebi_charts_Control_setEnabled = libLoader.getFunc<void(*)(void*, int)>("hebi_charts_Control_setEnabled");
            int enabled = getScalar<int>(prhs[2]);
            hebi_charts_Control_setEnabled(getPtr<void*>(prhs[1]), enabled);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Control_getLabel") */ 4292032106: {
            static auto hebi_charts_Control_getLabel = libLoader.getFunc<const char*(*)(void*)>("hebi_charts_Control_getLabel");
            const char* _value = hebi_charts_Control_getLabel(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexString(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Control_setLabel") */ 2485366894: {
            static auto hebi_charts_Control_setLabel = libLoader.getFunc<void(*)(void*, const char*)>("hebi_charts_Control_setLabel");
            MatlabString name(prhs[2]);
            hebi_charts_Control_setLabel(getPtr<void*>(prhs[1]), name);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Control_getTooltip") */ 1933368807: {
            static auto hebi_charts_Control_getTooltip = libLoader.getFunc<const char*(*)(void*)>("hebi_charts_Control_getTooltip");
            const char* _value = hebi_charts_Control_getTooltip(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexString(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Control_setTooltip") */ 871102075: {
            static auto hebi_charts_Control_setTooltip = libLoader.getFunc<void(*)(void*, const char*)>("hebi_charts_Control_setTooltip");
            MatlabString tooltip(prhs[2]);
            hebi_charts_Control_setTooltip(getPtr<void*>(prhs[1]), tooltip);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Control_isVisible") */ 633637578: {
            static auto hebi_charts_Control_isVisible = libLoader.getFunc<int(*)(void*)>("hebi_charts_Control_isVisible");
            int _value = hebi_charts_Control_isVisible(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Control_setVisible") */ 264986930: {
            static auto hebi_charts_Control_setVisible = libLoader.getFunc<void(*)(void*, int)>("hebi_charts_Control_setVisible");
            int visible = getScalar<int>(prhs[2]);
            hebi_charts_Control_setVisible(getPtr<void*>(prhs[1]), visible);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Control_release") */ 795521629: {
            static auto hebi_charts_Control_release = libLoader.getFunc<void(*)(void*)>("hebi_charts_Control_release");
            hebi_charts_Control_release(getPtr<void*>(prhs[1]));
            break;
        }

        // Button
        case /* hash32_fnv1a("hebi_charts_Button_getText") */ 236848194: {
            static auto hebi_charts_Button_getText = libLoader.getFunc<const char*(*)(void*)>("hebi_charts_Button_getText");
            const char* _value = hebi_charts_Button_getText(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexString(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Button_setText") */ 578248390: {
            static auto hebi_charts_Button_setText = libLoader.getFunc<void(*)(void*, const char*)>("hebi_charts_Button_setText");
            MatlabString text(prhs[2]);
            hebi_charts_Button_setText(getPtr<void*>(prhs[1]), text);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Button_isPressed") */ 3638190771: {
            static auto hebi_charts_Button_isPressed = libLoader.getFunc<int(*)(void*)>("hebi_charts_Button_isPressed");
            int _value = hebi_charts_Button_isPressed(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Button_wasPressed") */ 2864445990: {
            static auto hebi_charts_Button_wasPressed = libLoader.getFunc<int(*)(void*)>("hebi_charts_Button_wasPressed");
            int _value = hebi_charts_Button_wasPressed(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Button_to_Control") */ 3682425080: {
            static auto hebi_charts_Button_to_Control = libLoader.getFunc<void*(*)(void*)>("hebi_charts_Button_to_Control");
            if (nlhs > 0) plhs[0] = createMexPointer(hebi_charts_Button_to_Control(getPtr<void*>(prhs[1])));
            break;
        }

        // Dropdown
        case /* hash32_fnv1a("hebi_charts_Dropdown_setOptions") */ 1446832170: {
            static auto hebi_charts_Dropdown_setOptions = libLoader.getFunc<void(*)(void*, const char**, size_t)>("hebi_charts_Dropdown_setOptions");
            MatlabStringArray options(prhs[2]);
            size_t count = getScalar<size_t>(prhs[3]);
            hebi_charts_Dropdown_setOptions(getPtr<void*>(prhs[1]), options, count);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Dropdown_getSelectedIndex") */ 2533218167: {
            static auto hebi_charts_Dropdown_getSelectedIndex = libLoader.getFunc<int(*)(void*)>("hebi_charts_Dropdown_getSelectedIndex");
            int _value = hebi_charts_Dropdown_getSelectedIndex(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Dropdown_setSelectedIndex") */ 1323411971: {
            static auto hebi_charts_Dropdown_setSelectedIndex = libLoader.getFunc<void(*)(void*, int)>("hebi_charts_Dropdown_setSelectedIndex");
            int index = getScalar<int>(prhs[2]);
            hebi_charts_Dropdown_setSelectedIndex(getPtr<void*>(prhs[1]), index);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Dropdown_addOption") */ 3317650418: {
            static auto hebi_charts_Dropdown_addOption = libLoader.getFunc<void(*)(void*, const char*)>("hebi_charts_Dropdown_addOption");
            MatlabString option(prhs[2]);
            hebi_charts_Dropdown_addOption(getPtr<void*>(prhs[1]), option);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Dropdown_wasChanged") */ 1278658951: {
            static auto hebi_charts_Dropdown_wasChanged = libLoader.getFunc<int(*)(void*)>("hebi_charts_Dropdown_wasChanged");
            int _value = hebi_charts_Dropdown_wasChanged(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Dropdown_to_Control") */ 3115808627: {
            static auto hebi_charts_Dropdown_to_Control = libLoader.getFunc<void*(*)(void*)>("hebi_charts_Dropdown_to_Control");
            if (nlhs > 0) plhs[0] = createMexPointer(hebi_charts_Dropdown_to_Control(getPtr<void*>(prhs[1])));
            break;
        }

        // Label
        case /* hash32_fnv1a("hebi_charts_Label_getText") */ 4217140664: {
            static auto hebi_charts_Label_getText = libLoader.getFunc<const char*(*)(void*)>("hebi_charts_Label_getText");
            const char* _value = hebi_charts_Label_getText(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexString(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Label_setText") */ 2021464236: {
            static auto hebi_charts_Label_setText = libLoader.getFunc<void(*)(void*, const char*)>("hebi_charts_Label_setText");
            MatlabString text(prhs[2]);
            hebi_charts_Label_setText(getPtr<void*>(prhs[1]), text);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Label_getValue") */ 4162135716: {
            static auto hebi_charts_Label_getValue = libLoader.getFunc<double(*)(void*)>("hebi_charts_Label_getValue");
            double _value = hebi_charts_Label_getValue(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Label_setValue") */ 3763610232: {
            static auto hebi_charts_Label_setValue = libLoader.getFunc<void(*)(void*, double)>("hebi_charts_Label_setValue");
            double value = getScalar<double>(prhs[2]);
            hebi_charts_Label_setValue(getPtr<void*>(prhs[1]), value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Label_to_Control") */ 2160563954: {
            static auto hebi_charts_Label_to_Control = libLoader.getFunc<void*(*)(void*)>("hebi_charts_Label_to_Control");
            if (nlhs > 0) plhs[0] = createMexPointer(hebi_charts_Label_to_Control(getPtr<void*>(prhs[1])));
            break;
        }

        // Slider
        case /* hash32_fnv1a("hebi_charts_Slider_setLimits") */ 358947112: {
            static auto hebi_charts_Slider_setLimits = libLoader.getFunc<void(*)(void*, double, double)>("hebi_charts_Slider_setLimits");
            double min = getScalar<double>(prhs[2]);
            double max = getScalar<double>(prhs[3]);
            hebi_charts_Slider_setLimits(getPtr<void*>(prhs[1]), min, max);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Slider_getMax") */ 443624594: {
            static auto hebi_charts_Slider_getMax = libLoader.getFunc<double(*)(void*)>("hebi_charts_Slider_getMax");
            double _value = hebi_charts_Slider_getMax(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Slider_setMax") */ 1275740086: {
            static auto hebi_charts_Slider_setMax = libLoader.getFunc<void(*)(void*, double)>("hebi_charts_Slider_setMax");
            double max = getScalar<double>(prhs[2]);
            hebi_charts_Slider_setMax(getPtr<void*>(prhs[1]), max);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Slider_getMin") */ 73237048: {
            static auto hebi_charts_Slider_getMin = libLoader.getFunc<double(*)(void*)>("hebi_charts_Slider_getMin");
            double _value = hebi_charts_Slider_getMin(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Slider_setMin") */ 1576457300: {
            static auto hebi_charts_Slider_setMin = libLoader.getFunc<void(*)(void*, double)>("hebi_charts_Slider_setMin");
            double min = getScalar<double>(prhs[2]);
            hebi_charts_Slider_setMin(getPtr<void*>(prhs[1]), min);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Slider_getValue") */ 2102644573: {
            static auto hebi_charts_Slider_getValue = libLoader.getFunc<double(*)(void*)>("hebi_charts_Slider_getValue");
            double _value = hebi_charts_Slider_getValue(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Slider_setValue") */ 3045260073: {
            static auto hebi_charts_Slider_setValue = libLoader.getFunc<void(*)(void*, double)>("hebi_charts_Slider_setValue");
            double value = getScalar<double>(prhs[2]);
            hebi_charts_Slider_setValue(getPtr<void*>(prhs[1]), value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Slider_wasChanged") */ 2651043067: {
            static auto hebi_charts_Slider_wasChanged = libLoader.getFunc<int(*)(void*)>("hebi_charts_Slider_wasChanged");
            int _value = hebi_charts_Slider_wasChanged(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Slider_to_Control") */ 474184687: {
            static auto hebi_charts_Slider_to_Control = libLoader.getFunc<void*(*)(void*)>("hebi_charts_Slider_to_Control");
            if (nlhs > 0) plhs[0] = createMexPointer(hebi_charts_Slider_to_Control(getPtr<void*>(prhs[1])));
            break;
        }

        // Toggle
        case /* hash32_fnv1a("hebi_charts_Toggle_isSelected") */ 436557226: {
            static auto hebi_charts_Toggle_isSelected = libLoader.getFunc<int(*)(void*)>("hebi_charts_Toggle_isSelected");
            int _value = hebi_charts_Toggle_isSelected(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Toggle_setSelected") */ 4261780332: {
            static auto hebi_charts_Toggle_setSelected = libLoader.getFunc<void(*)(void*, int)>("hebi_charts_Toggle_setSelected");
            int selected = getScalar<int>(prhs[2]);
            hebi_charts_Toggle_setSelected(getPtr<void*>(prhs[1]), selected);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Toggle_wasChanged") */ 822243956: {
            static auto hebi_charts_Toggle_wasChanged = libLoader.getFunc<int(*)(void*)>("hebi_charts_Toggle_wasChanged");
            int _value = hebi_charts_Toggle_wasChanged(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Toggle_to_Control") */ 660545256: {
            static auto hebi_charts_Toggle_to_Control = libLoader.getFunc<void*(*)(void*)>("hebi_charts_Toggle_to_Control");
            if (nlhs > 0) plhs[0] = createMexPointer(hebi_charts_Toggle_to_Control(getPtr<void*>(prhs[1])));
            break;
        }

        // ControlPanel
        case /* hash32_fnv1a("hebi_charts_ControlPanel_getTitle") */ 1927630722: {
            static auto hebi_charts_ControlPanel_getTitle = libLoader.getFunc<const char*(*)(void*)>("hebi_charts_ControlPanel_getTitle");
            const char* _value = hebi_charts_ControlPanel_getTitle(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexString(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_ControlPanel_setTitle") */ 3220079598: {
            static auto hebi_charts_ControlPanel_setTitle = libLoader.getFunc<void(*)(void*, const char*)>("hebi_charts_ControlPanel_setTitle");
            MatlabString title(prhs[2]);
            hebi_charts_ControlPanel_setTitle(getPtr<void*>(prhs[1]), title);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_ControlPanel_getWidth") */ 877042768: {
            static auto hebi_charts_ControlPanel_getWidth = libLoader.getFunc<double(*)(void*)>("hebi_charts_ControlPanel_getWidth");
            double _value = hebi_charts_ControlPanel_getWidth(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_ControlPanel_setWidth") */ 2288203524: {
            static auto hebi_charts_ControlPanel_setWidth = libLoader.getFunc<void(*)(void*, double)>("hebi_charts_ControlPanel_setWidth");
            double width = getScalar<double>(prhs[2]);
            hebi_charts_ControlPanel_setWidth(getPtr<void*>(prhs[1]), width);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_ControlPanel_addSection") */ 557108654: {
            static auto hebi_charts_ControlPanel_addSection = libLoader.getFunc<int(*)(void*, const char*)>("hebi_charts_ControlPanel_addSection");
            MatlabString title(prhs[2]);
            int _value = hebi_charts_ControlPanel_addSection(getPtr<void*>(prhs[1]), title);
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_ControlPanel_addLabel") */ 4162567507: {
            static auto hebi_charts_ControlPanel_addLabel = libLoader.getFunc<void*(*)(void*)>("hebi_charts_ControlPanel_addLabel");
            void* _value = hebi_charts_ControlPanel_addLabel(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexPointer(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_ControlPanel_addButton") */ 348400099: {
            static auto hebi_charts_ControlPanel_addButton = libLoader.getFunc<void*(*)(void*)>("hebi_charts_ControlPanel_addButton");
            void* _value = hebi_charts_ControlPanel_addButton(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexPointer(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_ControlPanel_addStartButton") */ 404094125: {
            static auto hebi_charts_ControlPanel_addStartButton = libLoader.getFunc<void*(*)(void*)>("hebi_charts_ControlPanel_addStartButton");
            void* _value = hebi_charts_ControlPanel_addStartButton(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexPointer(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_ControlPanel_addStopButton") */ 391801435: {
            static auto hebi_charts_ControlPanel_addStopButton = libLoader.getFunc<void*(*)(void*)>("hebi_charts_ControlPanel_addStopButton");
            void* _value = hebi_charts_ControlPanel_addStopButton(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexPointer(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_ControlPanel_addSlider") */ 1637676682: {
            static auto hebi_charts_ControlPanel_addSlider = libLoader.getFunc<void*(*)(void*)>("hebi_charts_ControlPanel_addSlider");
            void* _value = hebi_charts_ControlPanel_addSlider(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexPointer(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_ControlPanel_addToggle") */ 1616396831: {
            static auto hebi_charts_ControlPanel_addToggle = libLoader.getFunc<void*(*)(void*)>("hebi_charts_ControlPanel_addToggle");
            void* _value = hebi_charts_ControlPanel_addToggle(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexPointer(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_ControlPanel_addDropdown") */ 2499277402: {
            static auto hebi_charts_ControlPanel_addDropdown = libLoader.getFunc<void*(*)(void*)>("hebi_charts_ControlPanel_addDropdown");
            void* _value = hebi_charts_ControlPanel_addDropdown(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexPointer(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_ControlPanel_release") */ 4196057477: {
            static auto hebi_charts_ControlPanel_release = libLoader.getFunc<void(*)(void*)>("hebi_charts_ControlPanel_release");
            hebi_charts_ControlPanel_release(getPtr<void*>(prhs[1]));
            break;
        }

        // Cursor
        case /* hash32_fnv1a("hebi_charts_Cursor_isEditable") */ 3545168017: {
            static auto hebi_charts_Cursor_isEditable = libLoader.getFunc<int(*)(void*)>("hebi_charts_Cursor_isEditable");
            int _value = hebi_charts_Cursor_isEditable(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Cursor_setEditable") */ 1606003675: {
            static auto hebi_charts_Cursor_setEditable = libLoader.getFunc<void(*)(void*, int)>("hebi_charts_Cursor_setEditable");
            int editable = getScalar<int>(prhs[2]);
            hebi_charts_Cursor_setEditable(getPtr<void*>(prhs[1]), editable);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Cursor_getLabel") */ 2513275231: {
            static auto hebi_charts_Cursor_getLabel = libLoader.getFunc<const char*(*)(void*)>("hebi_charts_Cursor_getLabel");
            const char* _value = hebi_charts_Cursor_getLabel(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexString(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Cursor_setLabel") */ 2912272059: {
            static auto hebi_charts_Cursor_setLabel = libLoader.getFunc<void(*)(void*, const char*)>("hebi_charts_Cursor_setLabel");
            MatlabString label(prhs[2]);
            hebi_charts_Cursor_setLabel(getPtr<void*>(prhs[1]), label);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Cursor_getValue") */ 1660839612: {
            static auto hebi_charts_Cursor_getValue = libLoader.getFunc<double(*)(void*)>("hebi_charts_Cursor_getValue");
            double _value = hebi_charts_Cursor_getValue(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Cursor_setValue") */ 3942528016: {
            static auto hebi_charts_Cursor_setValue = libLoader.getFunc<void(*)(void*, double)>("hebi_charts_Cursor_setValue");
            double value = getScalar<double>(prhs[2]);
            hebi_charts_Cursor_setValue(getPtr<void*>(prhs[1]), value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Cursor_isVisible") */ 2958376865: {
            static auto hebi_charts_Cursor_isVisible = libLoader.getFunc<int(*)(void*)>("hebi_charts_Cursor_isVisible");
            int _value = hebi_charts_Cursor_isVisible(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Cursor_setVisible") */ 3195068407: {
            static auto hebi_charts_Cursor_setVisible = libLoader.getFunc<void(*)(void*, int)>("hebi_charts_Cursor_setVisible");
            int visible = getScalar<int>(prhs[2]);
            hebi_charts_Cursor_setVisible(getPtr<void*>(prhs[1]), visible);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Cursor_release") */ 1029829710: {
            static auto hebi_charts_Cursor_release = libLoader.getFunc<void(*)(void*)>("hebi_charts_Cursor_release");
            hebi_charts_Cursor_release(getPtr<void*>(prhs[1]));
            break;
        }

        // FxmlView
        case /* hash32_fnv1a("hebi_charts_FxmlView_isAutoReload") */ 3201681499: {
            static auto hebi_charts_FxmlView_isAutoReload = libLoader.getFunc<int(*)(void*)>("hebi_charts_FxmlView_isAutoReload");
            int _value = hebi_charts_FxmlView_isAutoReload(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_FxmlView_setAutoReload") */ 3760019613: {
            static auto hebi_charts_FxmlView_setAutoReload = libLoader.getFunc<void(*)(void*, int)>("hebi_charts_FxmlView_setAutoReload");
            int enableAutoReload = getScalar<int>(prhs[2]);
            hebi_charts_FxmlView_setAutoReload(getPtr<void*>(prhs[1]), enableAutoReload);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_FxmlView_getSource") */ 109946018: {
            static auto hebi_charts_FxmlView_getSource = libLoader.getFunc<const char*(*)(void*)>("hebi_charts_FxmlView_getSource");
            const char* _value = hebi_charts_FxmlView_getSource(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexString(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_FxmlView_setSource") */ 3778040214: {
            static auto hebi_charts_FxmlView_setSource = libLoader.getFunc<int(*)(void*, const char*)>("hebi_charts_FxmlView_setSource");
            MatlabString pathOrUrl(prhs[2]);
            int _value = hebi_charts_FxmlView_setSource(getPtr<void*>(prhs[1]), pathOrUrl);
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_FxmlView_addLineChart") */ 576685690: {
            static auto hebi_charts_FxmlView_addLineChart = libLoader.getFunc<void*(*)(void*, const char*)>("hebi_charts_FxmlView_addLineChart");
            MatlabString fxId(prhs[2]);
            void* _value = hebi_charts_FxmlView_addLineChart(getPtr<void*>(prhs[1]), fxId);
            if (nlhs > 0) plhs[0] = createMexPointer(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_FxmlView_addScope") */ 288888368: {
            static auto hebi_charts_FxmlView_addScope = libLoader.getFunc<void*(*)(void*, const char*)>("hebi_charts_FxmlView_addScope");
            MatlabString fxId(prhs[2]);
            void* _value = hebi_charts_FxmlView_addScope(getPtr<void*>(prhs[1]), fxId);
            if (nlhs > 0) plhs[0] = createMexPointer(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_FxmlView_addLatencyChart") */ 2361709490: {
            static auto hebi_charts_FxmlView_addLatencyChart = libLoader.getFunc<void*(*)(void*, const char*)>("hebi_charts_FxmlView_addLatencyChart");
            MatlabString fxId(prhs[2]);
            void* _value = hebi_charts_FxmlView_addLatencyChart(getPtr<void*>(prhs[1]), fxId);
            if (nlhs > 0) plhs[0] = createMexPointer(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_FxmlView_addScene3d") */ 3960459659: {
            static auto hebi_charts_FxmlView_addScene3d = libLoader.getFunc<void*(*)(void*, const char*)>("hebi_charts_FxmlView_addScene3d");
            MatlabString fxId(prhs[2]);
            void* _value = hebi_charts_FxmlView_addScene3d(getPtr<void*>(prhs[1]), fxId);
            if (nlhs > 0) plhs[0] = createMexPointer(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_FxmlView_addStreamView") */ 3468796213: {
            static auto hebi_charts_FxmlView_addStreamView = libLoader.getFunc<void*(*)(void*, const char*, const char*)>("hebi_charts_FxmlView_addStreamView");
            MatlabString file(prhs[2]);
            MatlabString fxId(prhs[3]);
            void* _value = hebi_charts_FxmlView_addStreamView(getPtr<void*>(prhs[1]), file, fxId);
            if (nlhs > 0) plhs[0] = createMexPointer(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_FxmlView_addFxmlView") */ 3279362014: {
            static auto hebi_charts_FxmlView_addFxmlView = libLoader.getFunc<void*(*)(void*, const char*)>("hebi_charts_FxmlView_addFxmlView");
            MatlabString fxId(prhs[2]);
            void* _value = hebi_charts_FxmlView_addFxmlView(getPtr<void*>(prhs[1]), fxId);
            if (nlhs > 0) plhs[0] = createMexPointer(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_FxmlView_release") */ 1439298174: {
            static auto hebi_charts_FxmlView_release = libLoader.getFunc<void(*)(void*)>("hebi_charts_FxmlView_release");
            hebi_charts_FxmlView_release(getPtr<void*>(prhs[1]));
            break;
        }

        // GridWindow
        case /* hash32_fnv1a("hebi_charts_GridWindow_create") */ 481105715: {
            static auto hebi_charts_GridWindow_create = libLoader.getFunc<void*(*)(int, int)>("hebi_charts_GridWindow_create");
            int rows = getScalar<int>(prhs[1]);
            int cols = getScalar<int>(prhs[2]);
            void* _value = hebi_charts_GridWindow_create(rows, cols);
            if (nlhs > 0) plhs[0] = createMexPointer(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_GridWindow_isFullScreen") */ 1721034094: {
            static auto hebi_charts_GridWindow_isFullScreen = libLoader.getFunc<int(*)(void*)>("hebi_charts_GridWindow_isFullScreen");
            int _value = hebi_charts_GridWindow_isFullScreen(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_GridWindow_setFullScreen") */ 1125370908: {
            static auto hebi_charts_GridWindow_setFullScreen = libLoader.getFunc<void(*)(void*, int)>("hebi_charts_GridWindow_setFullScreen");
            int fullScreen = getScalar<int>(prhs[2]);
            hebi_charts_GridWindow_setFullScreen(getPtr<void*>(prhs[1]), fullScreen);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_GridWindow_getHeight") */ 1740977914: {
            static auto hebi_charts_GridWindow_getHeight = libLoader.getFunc<int(*)(void*)>("hebi_charts_GridWindow_getHeight");
            int _value = hebi_charts_GridWindow_getHeight(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_GridWindow_setHeight") */ 2254829894: {
            static auto hebi_charts_GridWindow_setHeight = libLoader.getFunc<void(*)(void*, int)>("hebi_charts_GridWindow_setHeight");
            int height = getScalar<int>(prhs[2]);
            hebi_charts_GridWindow_setHeight(getPtr<void*>(prhs[1]), height);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_GridWindow_getKeepOpen") */ 2380182568: {
            static auto hebi_charts_GridWindow_getKeepOpen = libLoader.getFunc<int(*)(void*)>("hebi_charts_GridWindow_getKeepOpen");
            int _value = hebi_charts_GridWindow_getKeepOpen(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_GridWindow_setKeepOpen") */ 140990388: {
            static auto hebi_charts_GridWindow_setKeepOpen = libLoader.getFunc<void(*)(void*, int)>("hebi_charts_GridWindow_setKeepOpen");
            int keepOpen = getScalar<int>(prhs[2]);
            hebi_charts_GridWindow_setKeepOpen(getPtr<void*>(prhs[1]), keepOpen);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_GridWindow_setLocation") */ 1129240730: {
            static auto hebi_charts_GridWindow_setLocation = libLoader.getFunc<void(*)(void*, int, int)>("hebi_charts_GridWindow_setLocation");
            int xOffset = getScalar<int>(prhs[2]);
            int yOffset = getScalar<int>(prhs[3]);
            hebi_charts_GridWindow_setLocation(getPtr<void*>(prhs[1]), xOffset, yOffset);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_GridWindow_setSize") */ 1704531216: {
            static auto hebi_charts_GridWindow_setSize = libLoader.getFunc<void(*)(void*, int, int)>("hebi_charts_GridWindow_setSize");
            int width = getScalar<int>(prhs[2]);
            int height = getScalar<int>(prhs[3]);
            hebi_charts_GridWindow_setSize(getPtr<void*>(prhs[1]), width, height);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_GridWindow_getTitle") */ 1153585057: {
            static auto hebi_charts_GridWindow_getTitle = libLoader.getFunc<const char*(*)(void*)>("hebi_charts_GridWindow_getTitle");
            const char* _value = hebi_charts_GridWindow_getTitle(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexString(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_GridWindow_setTitle") */ 2959599629: {
            static auto hebi_charts_GridWindow_setTitle = libLoader.getFunc<void(*)(void*, const char*)>("hebi_charts_GridWindow_setTitle");
            MatlabString title(prhs[2]);
            hebi_charts_GridWindow_setTitle(getPtr<void*>(prhs[1]), title);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_GridWindow_getWidth") */ 1311524919: {
            static auto hebi_charts_GridWindow_getWidth = libLoader.getFunc<int(*)(void*)>("hebi_charts_GridWindow_getWidth");
            int _value = hebi_charts_GridWindow_getWidth(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_GridWindow_setWidth") */ 782365675: {
            static auto hebi_charts_GridWindow_setWidth = libLoader.getFunc<void(*)(void*, int)>("hebi_charts_GridWindow_setWidth");
            int width = getScalar<int>(prhs[2]);
            hebi_charts_GridWindow_setWidth(getPtr<void*>(prhs[1]), width);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_GridWindow_getX") */ 2878897391: {
            static auto hebi_charts_GridWindow_getX = libLoader.getFunc<int(*)(void*)>("hebi_charts_GridWindow_getX");
            int _value = hebi_charts_GridWindow_getX(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_GridWindow_setX") */ 4248629307: {
            static auto hebi_charts_GridWindow_setX = libLoader.getFunc<void(*)(void*, int)>("hebi_charts_GridWindow_setX");
            int x = getScalar<int>(prhs[2]);
            hebi_charts_GridWindow_setX(getPtr<void*>(prhs[1]), x);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_GridWindow_getY") */ 2862119772: {
            static auto hebi_charts_GridWindow_getY = libLoader.getFunc<int(*)(void*)>("hebi_charts_GridWindow_getY");
            int _value = hebi_charts_GridWindow_getY(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_GridWindow_setY") */ 4231851688: {
            static auto hebi_charts_GridWindow_setY = libLoader.getFunc<void(*)(void*, int)>("hebi_charts_GridWindow_setY");
            int y = getScalar<int>(prhs[2]);
            hebi_charts_GridWindow_setY(getPtr<void*>(prhs[1]), y);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_GridWindow_addLineChart") */ 3022891796: {
            static auto hebi_charts_GridWindow_addLineChart = libLoader.getFunc<void*(*)(void*, int, int, int, int)>("hebi_charts_GridWindow_addLineChart");
            int row = getScalar<int>(prhs[2]);
            int col = getScalar<int>(prhs[3]);
            int rowSpan = getScalar<int>(prhs[4]);
            int colSpan = getScalar<int>(prhs[5]);
            void* _value = hebi_charts_GridWindow_addLineChart(getPtr<void*>(prhs[1]), row, col, rowSpan, colSpan);
            if (nlhs > 0) plhs[0] = createMexPointer(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_GridWindow_addScope") */ 1120848738: {
            static auto hebi_charts_GridWindow_addScope = libLoader.getFunc<void*(*)(void*, int, int, int, int)>("hebi_charts_GridWindow_addScope");
            int row = getScalar<int>(prhs[2]);
            int col = getScalar<int>(prhs[3]);
            int rowSpan = getScalar<int>(prhs[4]);
            int colSpan = getScalar<int>(prhs[5]);
            void* _value = hebi_charts_GridWindow_addScope(getPtr<void*>(prhs[1]), row, col, rowSpan, colSpan);
            if (nlhs > 0) plhs[0] = createMexPointer(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_GridWindow_addLatencyChart") */ 2808728484: {
            static auto hebi_charts_GridWindow_addLatencyChart = libLoader.getFunc<void*(*)(void*, int, int, int, int)>("hebi_charts_GridWindow_addLatencyChart");
            int row = getScalar<int>(prhs[2]);
            int col = getScalar<int>(prhs[3]);
            int rowSpan = getScalar<int>(prhs[4]);
            int colSpan = getScalar<int>(prhs[5]);
            void* _value = hebi_charts_GridWindow_addLatencyChart(getPtr<void*>(prhs[1]), row, col, rowSpan, colSpan);
            if (nlhs > 0) plhs[0] = createMexPointer(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_GridWindow_addScene3d") */ 994315921: {
            static auto hebi_charts_GridWindow_addScene3d = libLoader.getFunc<void*(*)(void*, int, int, int, int)>("hebi_charts_GridWindow_addScene3d");
            int row = getScalar<int>(prhs[2]);
            int col = getScalar<int>(prhs[3]);
            int rowSpan = getScalar<int>(prhs[4]);
            int colSpan = getScalar<int>(prhs[5]);
            void* _value = hebi_charts_GridWindow_addScene3d(getPtr<void*>(prhs[1]), row, col, rowSpan, colSpan);
            if (nlhs > 0) plhs[0] = createMexPointer(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_GridWindow_addStreamView") */ 2740676711: {
            static auto hebi_charts_GridWindow_addStreamView = libLoader.getFunc<void*(*)(void*, const char*, int, int, int, int)>("hebi_charts_GridWindow_addStreamView");
            MatlabString file(prhs[2]);
            int row = getScalar<int>(prhs[3]);
            int col = getScalar<int>(prhs[4]);
            int rowSpan = getScalar<int>(prhs[5]);
            int colSpan = getScalar<int>(prhs[6]);
            void* _value = hebi_charts_GridWindow_addStreamView(getPtr<void*>(prhs[1]), file, row, col, rowSpan, colSpan);
            if (nlhs > 0) plhs[0] = createMexPointer(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_GridWindow_addFxmlView") */ 494287860: {
            static auto hebi_charts_GridWindow_addFxmlView = libLoader.getFunc<void*(*)(void*, int, int, int, int)>("hebi_charts_GridWindow_addFxmlView");
            int row = getScalar<int>(prhs[2]);
            int col = getScalar<int>(prhs[3]);
            int rowSpan = getScalar<int>(prhs[4]);
            int colSpan = getScalar<int>(prhs[5]);
            void* _value = hebi_charts_GridWindow_addFxmlView(getPtr<void*>(prhs[1]), row, col, rowSpan, colSpan);
            if (nlhs > 0) plhs[0] = createMexPointer(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_GridWindow_addStylesheet") */ 2208938158: {
            static auto hebi_charts_GridWindow_addStylesheet = libLoader.getFunc<int(*)(void*, const char*, int)>("hebi_charts_GridWindow_addStylesheet");
            MatlabString pathOrUrl(prhs[2]);
            int autoReload = getScalar<int>(prhs[3]);
            int _value = hebi_charts_GridWindow_addStylesheet(getPtr<void*>(prhs[1]), pathOrUrl, autoReload);
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_GridWindow_show") */ 571378966: {
            static auto hebi_charts_GridWindow_show = libLoader.getFunc<void(*)(void*)>("hebi_charts_GridWindow_show");
            hebi_charts_GridWindow_show(getPtr<void*>(prhs[1]));
            break;
        }
        case /* hash32_fnv1a("hebi_charts_GridWindow_showOffScreen") */ 1460317711: {
            static auto hebi_charts_GridWindow_showOffScreen = libLoader.getFunc<int(*)(void*)>("hebi_charts_GridWindow_showOffScreen");
            int _value = hebi_charts_GridWindow_showOffScreen(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_GridWindow_hide") */ 3322641491: {
            static auto hebi_charts_GridWindow_hide = libLoader.getFunc<void(*)(void*)>("hebi_charts_GridWindow_hide");
            hebi_charts_GridWindow_hide(getPtr<void*>(prhs[1]));
            break;
        }
        case /* hash32_fnv1a("hebi_charts_GridWindow_isShowing") */ 220171030: {
            static auto hebi_charts_GridWindow_isShowing = libLoader.getFunc<int(*)(void*)>("hebi_charts_GridWindow_isShowing");
            int _value = hebi_charts_GridWindow_isShowing(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_GridWindow_waitUntilClosed") */ 1606158494: {
            static auto hebi_charts_GridWindow_waitUntilClosed = libLoader.getFunc<int(*)(void*)>("hebi_charts_GridWindow_waitUntilClosed");
            int _value = hebi_charts_GridWindow_waitUntilClosed(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_GridWindow_getControlPanel") */ 2796496580: {
            static auto hebi_charts_GridWindow_getControlPanel = libLoader.getFunc<void*(*)(void*)>("hebi_charts_GridWindow_getControlPanel");
            void* _value = hebi_charts_GridWindow_getControlPanel(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexPointer(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_GridWindow_createImageStream") */ 2319112984: {
            static auto hebi_charts_GridWindow_createImageStream = libLoader.getFunc<void*(*)(void*)>("hebi_charts_GridWindow_createImageStream");
            void* _value = hebi_charts_GridWindow_createImageStream(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexPointer(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_GridWindow_dispatchMouseEvent") */ 493138856: {
            static auto hebi_charts_GridWindow_dispatchMouseEvent = libLoader.getFunc<void(*)(void*, int, int, int, int, double, double)>("hebi_charts_GridWindow_dispatchMouseEvent");
            int action = getScalar<int>(prhs[2]);
            int button = getScalar<int>(prhs[3]);
            int downMask = getScalar<int>(prhs[4]);
            int modifiers = getScalar<int>(prhs[5]);
            double x = getScalar<double>(prhs[6]);
            double y = getScalar<double>(prhs[7]);
            hebi_charts_GridWindow_dispatchMouseEvent(getPtr<void*>(prhs[1]), action, button, downMask, modifiers, x, y);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_GridWindow_dispatchScrollEvent") */ 3322726912: {
            static auto hebi_charts_GridWindow_dispatchScrollEvent = libLoader.getFunc<void(*)(void*, double, double, double, double, int)>("hebi_charts_GridWindow_dispatchScrollEvent");
            double x = getScalar<double>(prhs[2]);
            double y = getScalar<double>(prhs[3]);
            double delta_x = getScalar<double>(prhs[4]);
            double delta_y = getScalar<double>(prhs[5]);
            int modifiers = getScalar<int>(prhs[6]);
            hebi_charts_GridWindow_dispatchScrollEvent(getPtr<void*>(prhs[1]), x, y, delta_x, delta_y, modifiers);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_GridWindow_release") */ 288831216: {
            static auto hebi_charts_GridWindow_release = libLoader.getFunc<void(*)(void*)>("hebi_charts_GridWindow_release");
            hebi_charts_GridWindow_release(getPtr<void*>(prhs[1]));
            break;
        }

        // HdrHistogramRecorder
        case /* hash32_fnv1a("hebi_charts_HdrHistogramRecorder_create") */ 513492597: {
            static auto hebi_charts_HdrHistogramRecorder_create = libLoader.getFunc<void*(*)()>("hebi_charts_HdrHistogramRecorder_create");
            void* _value = hebi_charts_HdrHistogramRecorder_create();
            if (nlhs > 0) plhs[0] = createMexPointer(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_HdrHistogramRecorder_getFrequency") */ 3235251207: {
            static auto hebi_charts_HdrHistogramRecorder_getFrequency = libLoader.getFunc<double(*)(void*)>("hebi_charts_HdrHistogramRecorder_getFrequency");
            double _value = hebi_charts_HdrHistogramRecorder_getFrequency(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_HdrHistogramRecorder_setFrequency") */ 126668259: {
            static auto hebi_charts_HdrHistogramRecorder_setFrequency = libLoader.getFunc<int(*)(void*, double)>("hebi_charts_HdrHistogramRecorder_setFrequency");
            double frequency = getScalar<double>(prhs[2]);
            int _value = hebi_charts_HdrHistogramRecorder_setFrequency(getPtr<void*>(prhs[1]), frequency);
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_HdrHistogramRecorder_getMax") */ 1760728599: {
            static auto hebi_charts_HdrHistogramRecorder_getMax = libLoader.getFunc<double(*)(void*)>("hebi_charts_HdrHistogramRecorder_getMax");
            double _value = hebi_charts_HdrHistogramRecorder_getMax(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_HdrHistogramRecorder_setMax") */ 1697553923: {
            static auto hebi_charts_HdrHistogramRecorder_setMax = libLoader.getFunc<int(*)(void*, double)>("hebi_charts_HdrHistogramRecorder_setMax");
            double value = getScalar<double>(prhs[2]);
            int _value = hebi_charts_HdrHistogramRecorder_setMax(getPtr<void*>(prhs[1]), value);
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_HdrHistogramRecorder_getMin") */ 1460011385: {
            static auto hebi_charts_HdrHistogramRecorder_getMin = libLoader.getFunc<double(*)(void*)>("hebi_charts_HdrHistogramRecorder_getMin");
            double _value = hebi_charts_HdrHistogramRecorder_getMin(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_HdrHistogramRecorder_setMin") */ 1531057661: {
            static auto hebi_charts_HdrHistogramRecorder_setMin = libLoader.getFunc<int(*)(void*, double)>("hebi_charts_HdrHistogramRecorder_setMin");
            double value = getScalar<double>(prhs[2]);
            int _value = hebi_charts_HdrHistogramRecorder_setMin(getPtr<void*>(prhs[1]), value);
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_HdrHistogramRecorder_getSignificantDigits") */ 2205589608: {
            static auto hebi_charts_HdrHistogramRecorder_getSignificantDigits = libLoader.getFunc<int(*)(void*)>("hebi_charts_HdrHistogramRecorder_getSignificantDigits");
            int _value = hebi_charts_HdrHistogramRecorder_getSignificantDigits(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_HdrHistogramRecorder_setSignificantDigits") */ 4180067068: {
            static auto hebi_charts_HdrHistogramRecorder_setSignificantDigits = libLoader.getFunc<int(*)(void*, int)>("hebi_charts_HdrHistogramRecorder_setSignificantDigits");
            int significantDigits = getScalar<int>(prhs[2]);
            int _value = hebi_charts_HdrHistogramRecorder_setSignificantDigits(getPtr<void*>(prhs[1]), significantDigits);
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_HdrHistogramRecorder_addTrace") */ 3486503389: {
            static auto hebi_charts_HdrHistogramRecorder_addTrace = libLoader.getFunc<void*(*)(void*, const char*)>("hebi_charts_HdrHistogramRecorder_addTrace");
            MatlabString tag(prhs[2]);
            void* _value = hebi_charts_HdrHistogramRecorder_addTrace(getPtr<void*>(prhs[1]), tag);
            if (nlhs > 0) plhs[0] = createMexPointer(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_HdrHistogramRecorder_startRecording") */ 3232561954: {
            static auto hebi_charts_HdrHistogramRecorder_startRecording = libLoader.getFunc<const char*(*)(void*, const char*)>("hebi_charts_HdrHistogramRecorder_startRecording");
            MatlabString logFile(prhs[2]);
            const char* _value = hebi_charts_HdrHistogramRecorder_startRecording(getPtr<void*>(prhs[1]), logFile);
            if (nlhs > 0) plhs[0] = createMexString(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_HdrHistogramRecorder_stopRecording") */ 4037906548: {
            static auto hebi_charts_HdrHistogramRecorder_stopRecording = libLoader.getFunc<void(*)(void*)>("hebi_charts_HdrHistogramRecorder_stopRecording");
            hebi_charts_HdrHistogramRecorder_stopRecording(getPtr<void*>(prhs[1]));
            break;
        }
        case /* hash32_fnv1a("hebi_charts_HdrHistogramRecorder_release") */ 1289531094: {
            static auto hebi_charts_HdrHistogramRecorder_release = libLoader.getFunc<void(*)(void*)>("hebi_charts_HdrHistogramRecorder_release");
            hebi_charts_HdrHistogramRecorder_release(getPtr<void*>(prhs[1]));
            break;
        }

        // HdrHistogramTrace
        case /* hash32_fnv1a("hebi_charts_HdrHistogramTrace_getMax") */ 3723481894: {
            static auto hebi_charts_HdrHistogramTrace_getMax = libLoader.getFunc<double(*)(void*)>("hebi_charts_HdrHistogramTrace_getMax");
            double _value = hebi_charts_HdrHistogramTrace_getMax(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_HdrHistogramTrace_getMean") */ 409566793: {
            static auto hebi_charts_HdrHistogramTrace_getMean = libLoader.getFunc<double(*)(void*)>("hebi_charts_HdrHistogramTrace_getMean");
            double _value = hebi_charts_HdrHistogramTrace_getMean(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_HdrHistogramTrace_getMin") */ 3487315300: {
            static auto hebi_charts_HdrHistogramTrace_getMin = libLoader.getFunc<double(*)(void*)>("hebi_charts_HdrHistogramTrace_getMin");
            double _value = hebi_charts_HdrHistogramTrace_getMin(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_HdrHistogramTrace_getStdDeviation") */ 859032600: {
            static auto hebi_charts_HdrHistogramTrace_getStdDeviation = libLoader.getFunc<double(*)(void*)>("hebi_charts_HdrHistogramTrace_getStdDeviation");
            double _value = hebi_charts_HdrHistogramTrace_getStdDeviation(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_HdrHistogramTrace_getTag") */ 2691889208: {
            static auto hebi_charts_HdrHistogramTrace_getTag = libLoader.getFunc<const char*(*)(void*)>("hebi_charts_HdrHistogramTrace_getTag");
            const char* _value = hebi_charts_HdrHistogramTrace_getTag(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexString(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_HdrHistogramTrace_getTotalCount") */ 1620338425: {
            static auto hebi_charts_HdrHistogramTrace_getTotalCount = libLoader.getFunc<size_t(*)(void*)>("hebi_charts_HdrHistogramTrace_getTotalCount");
            size_t _value = hebi_charts_HdrHistogramTrace_getTotalCount(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_HdrHistogramTrace_createLocal") */ 1127795715: {
            static auto hebi_charts_HdrHistogramTrace_createLocal = libLoader.getFunc<void*(*)(int, double, double)>("hebi_charts_HdrHistogramTrace_createLocal");
            int numberOfSignificantDigits = getScalar<int>(prhs[1]);
            double minSeconds = getScalar<double>(prhs[2]);
            double maxSeconds = getScalar<double>(prhs[3]);
            void* _value = hebi_charts_HdrHistogramTrace_createLocal(numberOfSignificantDigits, minSeconds, maxSeconds);
            if (nlhs > 0) plhs[0] = createMexPointer(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_HdrHistogramTrace_getValueAtPercentile") */ 2933860591: {
            static auto hebi_charts_HdrHistogramTrace_getValueAtPercentile = libLoader.getFunc<double(*)(void*, double)>("hebi_charts_HdrHistogramTrace_getValueAtPercentile");
            double percentile = getScalar<double>(prhs[2]);
            double _value = hebi_charts_HdrHistogramTrace_getValueAtPercentile(getPtr<void*>(prhs[1]), percentile);
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_HdrHistogramTrace_tic") */ 3683884192: {
            static auto hebi_charts_HdrHistogramTrace_tic = libLoader.getFunc<void(*)(void*)>("hebi_charts_HdrHistogramTrace_tic");
            hebi_charts_HdrHistogramTrace_tic(getPtr<void*>(prhs[1]));
            break;
        }
        case /* hash32_fnv1a("hebi_charts_HdrHistogramTrace_toc") */ 60904226: {
            static auto hebi_charts_HdrHistogramTrace_toc = libLoader.getFunc<double(*)(void*)>("hebi_charts_HdrHistogramTrace_toc");
            double _value = hebi_charts_HdrHistogramTrace_toc(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_HdrHistogramTrace_ticToc") */ 4060989918: {
            static auto hebi_charts_HdrHistogramTrace_ticToc = libLoader.getFunc<double(*)(void*)>("hebi_charts_HdrHistogramTrace_ticToc");
            double _value = hebi_charts_HdrHistogramTrace_ticToc(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_HdrHistogramTrace_recordValue") */ 961953446: {
            static auto hebi_charts_HdrHistogramTrace_recordValue = libLoader.getFunc<void(*)(void*, double)>("hebi_charts_HdrHistogramTrace_recordValue");
            double value = getScalar<double>(prhs[2]);
            hebi_charts_HdrHistogramTrace_recordValue(getPtr<void*>(prhs[1]), value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_HdrHistogramTrace_recordValueWithCount") */ 1955216297: {
            static auto hebi_charts_HdrHistogramTrace_recordValueWithCount = libLoader.getFunc<void(*)(void*, double, size_t)>("hebi_charts_HdrHistogramTrace_recordValueWithCount");
            double value = getScalar<double>(prhs[2]);
            size_t count = getScalar<size_t>(prhs[3]);
            hebi_charts_HdrHistogramTrace_recordValueWithCount(getPtr<void*>(prhs[1]), value, count);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_HdrHistogramTrace_recordValueWithExpectedInterval") */ 595474035: {
            static auto hebi_charts_HdrHistogramTrace_recordValueWithExpectedInterval = libLoader.getFunc<void(*)(void*, double, double)>("hebi_charts_HdrHistogramTrace_recordValueWithExpectedInterval");
            double value = getScalar<double>(prhs[2]);
            double expectedIntervalBetweenSamples = getScalar<double>(prhs[3]);
            hebi_charts_HdrHistogramTrace_recordValueWithExpectedInterval(getPtr<void*>(prhs[1]), value, expectedIntervalBetweenSamples);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_HdrHistogramTrace_reset") */ 778242123: {
            static auto hebi_charts_HdrHistogramTrace_reset = libLoader.getFunc<void(*)(void*)>("hebi_charts_HdrHistogramTrace_reset");
            hebi_charts_HdrHistogramTrace_reset(getPtr<void*>(prhs[1]));
            break;
        }
        case /* hash32_fnv1a("hebi_charts_HdrHistogramTrace_toHgrmString") */ 1378221180: {
            static auto hebi_charts_HdrHistogramTrace_toHgrmString = libLoader.getFunc<const char*(*)(void*, double)>("hebi_charts_HdrHistogramTrace_toHgrmString");
            double outputUnitsPerSecond = getScalar<double>(prhs[2]);
            const char* _value = hebi_charts_HdrHistogramTrace_toHgrmString(getPtr<void*>(prhs[1]), outputUnitsPerSecond);
            if (nlhs > 0) plhs[0] = createMexString(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_HdrHistogramTrace_saveAsHgrm") */ 1639013757: {
            static auto hebi_charts_HdrHistogramTrace_saveAsHgrm = libLoader.getFunc<const char*(*)(void*, const char*, double)>("hebi_charts_HdrHistogramTrace_saveAsHgrm");
            MatlabString fileName(prhs[2]);
            double outputUnitsPerSecond = getScalar<double>(prhs[3]);
            const char* _value = hebi_charts_HdrHistogramTrace_saveAsHgrm(getPtr<void*>(prhs[1]), fileName, outputUnitsPerSecond);
            if (nlhs > 0) plhs[0] = createMexString(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_HdrHistogramTrace_release") */ 2275312785: {
            static auto hebi_charts_HdrHistogramTrace_release = libLoader.getFunc<void(*)(void*)>("hebi_charts_HdrHistogramTrace_release");
            hebi_charts_HdrHistogramTrace_release(getPtr<void*>(prhs[1]));
            break;
        }

        // ImageStream
        case /* hash32_fnv1a("hebi_charts_ImageStream_getBuffer") */ 2670059014: {
            static auto hebi_charts_ImageStream_getBuffer = libLoader.getFunc<void*(*)(void*)>("hebi_charts_ImageStream_getBuffer");
            void* _value = hebi_charts_ImageStream_getBuffer(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexPointer(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_ImageStream_getBufferSize") */ 4286272295: {
            static auto hebi_charts_ImageStream_getBufferSize = libLoader.getFunc<size_t(*)(void*)>("hebi_charts_ImageStream_getBufferSize");
            size_t _value = hebi_charts_ImageStream_getBufferSize(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_ImageStream_getChannels") */ 2905113076: {
            static auto hebi_charts_ImageStream_getChannels = libLoader.getFunc<int(*)(void*)>("hebi_charts_ImageStream_getChannels");
            int _value = hebi_charts_ImageStream_getChannels(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_ImageStream_getHeight") */ 449632559: {
            static auto hebi_charts_ImageStream_getHeight = libLoader.getFunc<int(*)(void*)>("hebi_charts_ImageStream_getHeight");
            int _value = hebi_charts_ImageStream_getHeight(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_ImageStream_getPixelFormat") */ 1231628591: {
            static auto hebi_charts_ImageStream_getPixelFormat = libLoader.getFunc<int(*)(void*)>("hebi_charts_ImageStream_getPixelFormat");
            int _value = hebi_charts_ImageStream_getPixelFormat(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_ImageStream_setPixelFormat") */ 3498113259: {
            static auto hebi_charts_ImageStream_setPixelFormat = libLoader.getFunc<void(*)(void*, int)>("hebi_charts_ImageStream_setPixelFormat");
            int pixelFormat = getScalar<int>(prhs[2]);
            hebi_charts_ImageStream_setPixelFormat(getPtr<void*>(prhs[1]), pixelFormat);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_ImageStream_getRateLimit") */ 3218724877: {
            static auto hebi_charts_ImageStream_getRateLimit = libLoader.getFunc<double(*)(void*)>("hebi_charts_ImageStream_getRateLimit");
            double _value = hebi_charts_ImageStream_getRateLimit(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_ImageStream_setRateLimit") */ 2797088897: {
            static auto hebi_charts_ImageStream_setRateLimit = libLoader.getFunc<int(*)(void*, double)>("hebi_charts_ImageStream_setRateLimit");
            double maxFramesPerSecond = getScalar<double>(prhs[2]);
            int _value = hebi_charts_ImageStream_setRateLimit(getPtr<void*>(prhs[1]), maxFramesPerSecond);
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_ImageStream_getRecorderThreads") */ 4046330431: {
            static auto hebi_charts_ImageStream_getRecorderThreads = libLoader.getFunc<size_t(*)(void*)>("hebi_charts_ImageStream_getRecorderThreads");
            size_t _value = hebi_charts_ImageStream_getRecorderThreads(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_ImageStream_setRecorderThreads") */ 1737193747: {
            static auto hebi_charts_ImageStream_setRecorderThreads = libLoader.getFunc<int(*)(void*, size_t)>("hebi_charts_ImageStream_setRecorderThreads");
            size_t numThreads = getScalar<size_t>(prhs[2]);
            int _value = hebi_charts_ImageStream_setRecorderThreads(getPtr<void*>(prhs[1]), numThreads);
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_ImageStream_isRecording") */ 1217846067: {
            static auto hebi_charts_ImageStream_isRecording = libLoader.getFunc<int(*)(void*)>("hebi_charts_ImageStream_isRecording");
            int _value = hebi_charts_ImageStream_isRecording(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_ImageStream_getRenderScale") */ 1687894102: {
            static auto hebi_charts_ImageStream_getRenderScale = libLoader.getFunc<double(*)(void*)>("hebi_charts_ImageStream_getRenderScale");
            double _value = hebi_charts_ImageStream_getRenderScale(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_ImageStream_getRenderScaleX") */ 1851342346: {
            static auto hebi_charts_ImageStream_getRenderScaleX = libLoader.getFunc<double(*)(void*)>("hebi_charts_ImageStream_getRenderScaleX");
            double _value = hebi_charts_ImageStream_getRenderScaleX(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_ImageStream_getRenderScaleY") */ 1868119965: {
            static auto hebi_charts_ImageStream_getRenderScaleY = libLoader.getFunc<double(*)(void*)>("hebi_charts_ImageStream_getRenderScaleY");
            double _value = hebi_charts_ImageStream_getRenderScaleY(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_ImageStream_getSequence") */ 3666491377: {
            static auto hebi_charts_ImageStream_getSequence = libLoader.getFunc<size_t(*)(void*)>("hebi_charts_ImageStream_getSequence");
            size_t _value = hebi_charts_ImageStream_getSequence(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_ImageStream_getStride") */ 2883829305: {
            static auto hebi_charts_ImageStream_getStride = libLoader.getFunc<int(*)(void*)>("hebi_charts_ImageStream_getStride");
            int _value = hebi_charts_ImageStream_getStride(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_ImageStream_getTimestamp") */ 4221629260: {
            static auto hebi_charts_ImageStream_getTimestamp = libLoader.getFunc<double(*)(void*)>("hebi_charts_ImageStream_getTimestamp");
            double _value = hebi_charts_ImageStream_getTimestamp(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_ImageStream_getWidth") */ 2250410136: {
            static auto hebi_charts_ImageStream_getWidth = libLoader.getFunc<int(*)(void*)>("hebi_charts_ImageStream_getWidth");
            int _value = hebi_charts_ImageStream_getWidth(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_ImageStream_setResolution") */ 1812315458: {
            static auto hebi_charts_ImageStream_setResolution = libLoader.getFunc<int(*)(void*, int, int)>("hebi_charts_ImageStream_setResolution");
            int width = getScalar<int>(prhs[2]);
            int height = getScalar<int>(prhs[3]);
            int _value = hebi_charts_ImageStream_setResolution(getPtr<void*>(prhs[1]), width, height);
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_ImageStream_waitForNext") */ 996531661: {
            static auto hebi_charts_ImageStream_waitForNext = libLoader.getFunc<int(*)(void*, size_t)>("hebi_charts_ImageStream_waitForNext");
            size_t maxTimeoutMillis = getScalar<size_t>(prhs[2]);
            int _value = hebi_charts_ImageStream_waitForNext(getPtr<void*>(prhs[1]), maxTimeoutMillis);
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_ImageStream_tryGetNext") */ 1851634994: {
            static auto hebi_charts_ImageStream_tryGetNext = libLoader.getFunc<int(*)(void*)>("hebi_charts_ImageStream_tryGetNext");
            int _value = hebi_charts_ImageStream_tryGetNext(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_ImageStream_startRecording") */ 2959520791: {
            static auto hebi_charts_ImageStream_startRecording = libLoader.getFunc<int(*)(void*, const char*, int)>("hebi_charts_ImageStream_startRecording");
            MatlabString baseName(prhs[2]);
            int overwrite = getScalar<int>(prhs[3]);
            int _value = hebi_charts_ImageStream_startRecording(getPtr<void*>(prhs[1]), baseName, overwrite);
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_ImageStream_stopRecording") */ 451110275: {
            static auto hebi_charts_ImageStream_stopRecording = libLoader.getFunc<void*(*)(void*)>("hebi_charts_ImageStream_stopRecording");
            void* _value = hebi_charts_ImageStream_stopRecording(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexPointer(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_ImageStream_saveToFile") */ 3426388426: {
            static auto hebi_charts_ImageStream_saveToFile = libLoader.getFunc<int(*)(void*, const char*)>("hebi_charts_ImageStream_saveToFile");
            MatlabString fileName(prhs[2]);
            int _value = hebi_charts_ImageStream_saveToFile(getPtr<void*>(prhs[1]), fileName);
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_ImageStream_release") */ 2907127757: {
            static auto hebi_charts_ImageStream_release = libLoader.getFunc<void(*)(void*)>("hebi_charts_ImageStream_release");
            hebi_charts_ImageStream_release(getPtr<void*>(prhs[1]));
            break;
        }

        // LoopTimer
        case /* hash32_fnv1a("hebi_charts_LoopTimer_create") */ 3391059830: {
            static auto hebi_charts_LoopTimer_create = libLoader.getFunc<void*(*)()>("hebi_charts_LoopTimer_create");
            void* _value = hebi_charts_LoopTimer_create();
            if (nlhs > 0) plhs[0] = createMexPointer(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_LoopTimer_getElapsedTime") */ 1842144231: {
            static auto hebi_charts_LoopTimer_getElapsedTime = libLoader.getFunc<double(*)(void*)>("hebi_charts_LoopTimer_getElapsedTime");
            double _value = hebi_charts_LoopTimer_getElapsedTime(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_LoopTimer_getFrequency") */ 740806432: {
            static auto hebi_charts_LoopTimer_getFrequency = libLoader.getFunc<double(*)(void*)>("hebi_charts_LoopTimer_getFrequency");
            double _value = hebi_charts_LoopTimer_getFrequency(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_LoopTimer_setFrequency") */ 2423821956: {
            static auto hebi_charts_LoopTimer_setFrequency = libLoader.getFunc<void(*)(void*, double)>("hebi_charts_LoopTimer_setFrequency");
            double frequency = getScalar<double>(prhs[2]);
            hebi_charts_LoopTimer_setFrequency(getPtr<void*>(prhs[1]), frequency);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_LoopTimer_getPeriod") */ 2990288999: {
            static auto hebi_charts_LoopTimer_getPeriod = libLoader.getFunc<double(*)(void*)>("hebi_charts_LoopTimer_getPeriod");
            double _value = hebi_charts_LoopTimer_getPeriod(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_LoopTimer_setPeriod") */ 1951692307: {
            static auto hebi_charts_LoopTimer_setPeriod = libLoader.getFunc<void(*)(void*, double)>("hebi_charts_LoopTimer_setPeriod");
            double seconds = getScalar<double>(prhs[2]);
            hebi_charts_LoopTimer_setPeriod(getPtr<void*>(prhs[1]), seconds);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_LoopTimer_restart") */ 1382219461: {
            static auto hebi_charts_LoopTimer_restart = libLoader.getFunc<void(*)(void*)>("hebi_charts_LoopTimer_restart");
            hebi_charts_LoopTimer_restart(getPtr<void*>(prhs[1]));
            break;
        }
        case /* hash32_fnv1a("hebi_charts_LoopTimer_tic") */ 4074232662: {
            static auto hebi_charts_LoopTimer_tic = libLoader.getFunc<void(*)(void*)>("hebi_charts_LoopTimer_tic");
            hebi_charts_LoopTimer_tic(getPtr<void*>(prhs[1]));
            break;
        }
        case /* hash32_fnv1a("hebi_charts_LoopTimer_toc") */ 4208850972: {
            static auto hebi_charts_LoopTimer_toc = libLoader.getFunc<double(*)(void*)>("hebi_charts_LoopTimer_toc");
            double _value = hebi_charts_LoopTimer_toc(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_LoopTimer_ticToc") */ 578924568: {
            static auto hebi_charts_LoopTimer_ticToc = libLoader.getFunc<double(*)(void*)>("hebi_charts_LoopTimer_ticToc");
            double _value = hebi_charts_LoopTimer_ticToc(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_LoopTimer_waitForNextTick") */ 2999084238: {
            static auto hebi_charts_LoopTimer_waitForNextTick = libLoader.getFunc<void(*)(void*)>("hebi_charts_LoopTimer_waitForNextTick");
            hebi_charts_LoopTimer_waitForNextTick(getPtr<void*>(prhs[1]));
            break;
        }
        case /* hash32_fnv1a("hebi_charts_LoopTimer_getSecondsToNextTick") */ 420776018: {
            static auto hebi_charts_LoopTimer_getSecondsToNextTick = libLoader.getFunc<double(*)(void*)>("hebi_charts_LoopTimer_getSecondsToNextTick");
            double _value = hebi_charts_LoopTimer_getSecondsToNextTick(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_LoopTimer_getNanosToNextTick") */ 3013832472: {
            static auto hebi_charts_LoopTimer_getNanosToNextTick = libLoader.getFunc<size_t(*)(void*)>("hebi_charts_LoopTimer_getNanosToNextTick");
            size_t _value = hebi_charts_LoopTimer_getNanosToNextTick(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_LoopTimer_timeSeconds") */ 2932397192: {
            static auto hebi_charts_LoopTimer_timeSeconds = libLoader.getFunc<double(*)()>("hebi_charts_LoopTimer_timeSeconds");
            double _value = hebi_charts_LoopTimer_timeSeconds();
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_LoopTimer_timeNanos") */ 1429901862: {
            static auto hebi_charts_LoopTimer_timeNanos = libLoader.getFunc<size_t(*)()>("hebi_charts_LoopTimer_timeNanos");
            size_t _value = hebi_charts_LoopTimer_timeNanos();
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_LoopTimer_sleepNanos") */ 2868392724: {
            static auto hebi_charts_LoopTimer_sleepNanos = libLoader.getFunc<void(*)(size_t)>("hebi_charts_LoopTimer_sleepNanos");
            size_t nanos = getScalar<size_t>(prhs[1]);
            hebi_charts_LoopTimer_sleepNanos(nanos);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_LoopTimer_parkNanos") */ 3410159229: {
            static auto hebi_charts_LoopTimer_parkNanos = libLoader.getFunc<void(*)(size_t)>("hebi_charts_LoopTimer_parkNanos");
            size_t nanos = getScalar<size_t>(prhs[1]);
            hebi_charts_LoopTimer_parkNanos(nanos);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_LoopTimer_waitNanos") */ 3978608554: {
            static auto hebi_charts_LoopTimer_waitNanos = libLoader.getFunc<void(*)(size_t)>("hebi_charts_LoopTimer_waitNanos");
            size_t nanos = getScalar<size_t>(prhs[1]);
            hebi_charts_LoopTimer_waitNanos(nanos);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_LoopTimer_spinNanos") */ 3435998397: {
            static auto hebi_charts_LoopTimer_spinNanos = libLoader.getFunc<void(*)(size_t)>("hebi_charts_LoopTimer_spinNanos");
            size_t nanos = getScalar<size_t>(prhs[1]);
            hebi_charts_LoopTimer_spinNanos(nanos);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_LoopTimer_release") */ 2762685731: {
            static auto hebi_charts_LoopTimer_release = libLoader.getFunc<void(*)(void*)>("hebi_charts_LoopTimer_release");
            hebi_charts_LoopTimer_release(getPtr<void*>(prhs[1]));
            break;
        }

        // Object3d
        case /* hash32_fnv1a("hebi_charts_Object3d_isVisible") */ 4037753891: {
            static auto hebi_charts_Object3d_isVisible = libLoader.getFunc<int(*)(void*)>("hebi_charts_Object3d_isVisible");
            int _value = hebi_charts_Object3d_isVisible(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Object3d_setVisible") */ 3013872277: {
            static auto hebi_charts_Object3d_setVisible = libLoader.getFunc<void(*)(void*, int)>("hebi_charts_Object3d_setVisible");
            int visible = getScalar<int>(prhs[2]);
            hebi_charts_Object3d_setVisible(getPtr<void*>(prhs[1]), visible);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Object3d_setOrientation") */ 3092801013: {
            static auto hebi_charts_Object3d_setOrientation = libLoader.getFunc<int(*)(void*, double, double, double, double)>("hebi_charts_Object3d_setOrientation");
            double qx = getScalar<double>(prhs[2]);
            double qy = getScalar<double>(prhs[3]);
            double qz = getScalar<double>(prhs[4]);
            double qw = getScalar<double>(prhs[5]);
            int _value = hebi_charts_Object3d_setOrientation(getPtr<void*>(prhs[1]), qx, qy, qz, qw);
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Object3d_setOrientationRPY") */ 1032601474: {
            static auto hebi_charts_Object3d_setOrientationRPY = libLoader.getFunc<int(*)(void*, double, double, double)>("hebi_charts_Object3d_setOrientationRPY");
            double roll = getScalar<double>(prhs[2]);
            double pitch = getScalar<double>(prhs[3]);
            double yaw = getScalar<double>(prhs[4]);
            int _value = hebi_charts_Object3d_setOrientationRPY(getPtr<void*>(prhs[1]), roll, pitch, yaw);
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Object3d_setTranslation") */ 4253214088: {
            static auto hebi_charts_Object3d_setTranslation = libLoader.getFunc<int(*)(void*, double, double, double)>("hebi_charts_Object3d_setTranslation");
            double x = getScalar<double>(prhs[2]);
            double y = getScalar<double>(prhs[3]);
            double z = getScalar<double>(prhs[4]);
            int _value = hebi_charts_Object3d_setTranslation(getPtr<void*>(prhs[1]), x, y, z);
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Object3d_setPose") */ 1159682568: {
            static auto hebi_charts_Object3d_setPose = libLoader.getFunc<int(*)(void*, double, double, double, double, double, double, double)>("hebi_charts_Object3d_setPose");
            double x = getScalar<double>(prhs[2]);
            double y = getScalar<double>(prhs[3]);
            double z = getScalar<double>(prhs[4]);
            double qx = getScalar<double>(prhs[5]);
            double qy = getScalar<double>(prhs[6]);
            double qz = getScalar<double>(prhs[7]);
            double qw = getScalar<double>(prhs[8]);
            int _value = hebi_charts_Object3d_setPose(getPtr<void*>(prhs[1]), x, y, z, qx, qy, qz, qw);
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Object3d_setTransform4x4") */ 3412822525: {
            static auto hebi_charts_Object3d_setTransform4x4 = libLoader.getFunc<int(*)(void*, const double*, int)>("hebi_charts_Object3d_setTransform4x4");
            MatlabArray<double, mxDOUBLE_CLASS> matrix(prhs[2]);
            int ordering = getScalar<int>(prhs[3]);
            int _value = hebi_charts_Object3d_setTransform4x4(getPtr<void*>(prhs[1]), matrix, ordering);
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Object3d_release") */ 424151368: {
            static auto hebi_charts_Object3d_release = libLoader.getFunc<void(*)(void*)>("hebi_charts_Object3d_release");
            hebi_charts_Object3d_release(getPtr<void*>(prhs[1]));
            break;
        }

        // Frame
        case /* hash32_fnv1a("hebi_charts_Frame_to_Object3d") */ 3969566570: {
            static auto hebi_charts_Frame_to_Object3d = libLoader.getFunc<void*(*)(void*)>("hebi_charts_Frame_to_Object3d");
            if (nlhs > 0) plhs[0] = createMexPointer(hebi_charts_Frame_to_Object3d(getPtr<void*>(prhs[1])));
            break;
        }

        // Mesh
        case /* hash32_fnv1a("hebi_charts_Mesh_isCentered") */ 822326588: {
            static auto hebi_charts_Mesh_isCentered = libLoader.getFunc<int(*)(void*)>("hebi_charts_Mesh_isCentered");
            int _value = hebi_charts_Mesh_isCentered(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Mesh_setCentered") */ 485890492: {
            static auto hebi_charts_Mesh_setCentered = libLoader.getFunc<void(*)(void*, int)>("hebi_charts_Mesh_setCentered");
            int centered = getScalar<int>(prhs[2]);
            hebi_charts_Mesh_setCentered(getPtr<void*>(prhs[1]), centered);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Mesh_getScale") */ 3920716242: {
            static auto hebi_charts_Mesh_getScale = libLoader.getFunc<double(*)(void*)>("hebi_charts_Mesh_getScale");
            double _value = hebi_charts_Mesh_getScale(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Mesh_setScale") */ 262304758: {
            static auto hebi_charts_Mesh_setScale = libLoader.getFunc<void(*)(void*, double)>("hebi_charts_Mesh_setScale");
            double scaleUnitsToMillimeters = getScalar<double>(prhs[2]);
            hebi_charts_Mesh_setScale(getPtr<void*>(prhs[1]), scaleUnitsToMillimeters);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Mesh_getDisplayStyle") */ 975132563: {
            static auto hebi_charts_Mesh_getDisplayStyle = libLoader.getFunc<int(*)(void*)>("hebi_charts_Mesh_getDisplayStyle");
            int _value = hebi_charts_Mesh_getDisplayStyle(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Mesh_setDisplayStyle") */ 3775830343: {
            static auto hebi_charts_Mesh_setDisplayStyle = libLoader.getFunc<void(*)(void*, int)>("hebi_charts_Mesh_setDisplayStyle");
            int style = getScalar<int>(prhs[2]);
            hebi_charts_Mesh_setDisplayStyle(getPtr<void*>(prhs[1]), style);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Mesh_to_Object3d") */ 398257298: {
            static auto hebi_charts_Mesh_to_Object3d = libLoader.getFunc<void*(*)(void*)>("hebi_charts_Mesh_to_Object3d");
            if (nlhs > 0) plhs[0] = createMexPointer(hebi_charts_Mesh_to_Object3d(getPtr<void*>(prhs[1])));
            break;
        }

        // Robot
        case /* hash32_fnv1a("hebi_charts_Robot_getDisplayStyle") */ 163598878: {
            static auto hebi_charts_Robot_getDisplayStyle = libLoader.getFunc<int(*)(void*)>("hebi_charts_Robot_getDisplayStyle");
            int _value = hebi_charts_Robot_getDisplayStyle(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Robot_setDisplayStyle") */ 3200811722: {
            static auto hebi_charts_Robot_setDisplayStyle = libLoader.getFunc<void(*)(void*, int)>("hebi_charts_Robot_setDisplayStyle");
            int style = getScalar<int>(prhs[2]);
            hebi_charts_Robot_setDisplayStyle(getPtr<void*>(prhs[1]), style);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Robot_getDof") */ 3894024574: {
            static auto hebi_charts_Robot_getDof = libLoader.getFunc<size_t(*)(void*)>("hebi_charts_Robot_getDof");
            size_t _value = hebi_charts_Robot_getDof(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Robot_setPositions") */ 4060801135: {
            static auto hebi_charts_Robot_setPositions = libLoader.getFunc<int(*)(void*, const double*, size_t)>("hebi_charts_Robot_setPositions");
            MatlabArray<double, mxDOUBLE_CLASS> positions(prhs[2]);
            size_t length = getScalar<size_t>(prhs[3]);
            int _value = hebi_charts_Robot_setPositions(getPtr<void*>(prhs[1]), positions, length);
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Robot_to_Object3d") */ 1269492559: {
            static auto hebi_charts_Robot_to_Object3d = libLoader.getFunc<void*(*)(void*)>("hebi_charts_Robot_to_Object3d");
            if (nlhs > 0) plhs[0] = createMexPointer(hebi_charts_Robot_to_Object3d(getPtr<void*>(prhs[1])));
            break;
        }

        // Series3d
        case /* hash32_fnv1a("hebi_charts_Series3d_getColor") */ 1267755258: {
            static auto hebi_charts_Series3d_getColor = libLoader.getFunc<int(*)(void*)>("hebi_charts_Series3d_getColor");
            int _value = hebi_charts_Series3d_getColor(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Series3d_setColor") */ 677907614: {
            static auto hebi_charts_Series3d_setColor = libLoader.getFunc<void(*)(void*, int)>("hebi_charts_Series3d_setColor");
            int color = getScalar<int>(prhs[2]);
            hebi_charts_Series3d_setColor(getPtr<void*>(prhs[1]), color);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Series3d_to_Object3d") */ 2939041005: {
            static auto hebi_charts_Series3d_to_Object3d = libLoader.getFunc<void*(*)(void*)>("hebi_charts_Series3d_to_Object3d");
            if (nlhs > 0) plhs[0] = createMexPointer(hebi_charts_Series3d_to_Object3d(getPtr<void*>(prhs[1])));
            break;
        }

        // Line3d
        case /* hash32_fnv1a("hebi_charts_Line3d_setMaxPointCount") */ 4037341455: {
            static auto hebi_charts_Line3d_setMaxPointCount = libLoader.getFunc<void(*)(void*, size_t)>("hebi_charts_Line3d_setMaxPointCount");
            size_t count = getScalar<size_t>(prhs[2]);
            hebi_charts_Line3d_setMaxPointCount(getPtr<void*>(prhs[1]), count);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Line3d_clear") */ 815267325: {
            static auto hebi_charts_Line3d_clear = libLoader.getFunc<void(*)(void*)>("hebi_charts_Line3d_clear");
            hebi_charts_Line3d_clear(getPtr<void*>(prhs[1]));
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Line3d_setData") */ 3313487720: {
            static auto hebi_charts_Line3d_setData = libLoader.getFunc<void(*)(void*, const double*, const double*, const double*, size_t)>("hebi_charts_Line3d_setData");
            MatlabArray<double, mxDOUBLE_CLASS> x(prhs[2]);
            MatlabArray<double, mxDOUBLE_CLASS> y(prhs[3]);
            MatlabArray<double, mxDOUBLE_CLASS> z(prhs[4]);
            size_t length = getScalar<size_t>(prhs[5]);
            hebi_charts_Line3d_setData(getPtr<void*>(prhs[1]), x, y, z, length);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Line3d_addPoints") */ 592149656: {
            static auto hebi_charts_Line3d_addPoints = libLoader.getFunc<void(*)(void*, const double*, const double*, const double*, size_t)>("hebi_charts_Line3d_addPoints");
            MatlabArray<double, mxDOUBLE_CLASS> x(prhs[2]);
            MatlabArray<double, mxDOUBLE_CLASS> y(prhs[3]);
            MatlabArray<double, mxDOUBLE_CLASS> z(prhs[4]);
            size_t length = getScalar<size_t>(prhs[5]);
            hebi_charts_Line3d_addPoints(getPtr<void*>(prhs[1]), x, y, z, length);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Line3d_addPoint") */ 3496792187: {
            static auto hebi_charts_Line3d_addPoint = libLoader.getFunc<void(*)(void*, double, double, double)>("hebi_charts_Line3d_addPoint");
            double x = getScalar<double>(prhs[2]);
            double y = getScalar<double>(prhs[3]);
            double z = getScalar<double>(prhs[4]);
            hebi_charts_Line3d_addPoint(getPtr<void*>(prhs[1]), x, y, z);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Line3d_to_Series3d") */ 1298098948: {
            static auto hebi_charts_Line3d_to_Series3d = libLoader.getFunc<void*(*)(void*)>("hebi_charts_Line3d_to_Series3d");
            if (nlhs > 0) plhs[0] = createMexPointer(hebi_charts_Line3d_to_Series3d(getPtr<void*>(prhs[1])));
            break;
        }

        // Points3d
        case /* hash32_fnv1a("hebi_charts_Points3d_getMarkerShape") */ 3408937892: {
            static auto hebi_charts_Points3d_getMarkerShape = libLoader.getFunc<int(*)(void*)>("hebi_charts_Points3d_getMarkerShape");
            int _value = hebi_charts_Points3d_getMarkerShape(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Points3d_setMarkerShape") */ 3521551648: {
            static auto hebi_charts_Points3d_setMarkerShape = libLoader.getFunc<void(*)(void*, int)>("hebi_charts_Points3d_setMarkerShape");
            int shape = getScalar<int>(prhs[2]);
            hebi_charts_Points3d_setMarkerShape(getPtr<void*>(prhs[1]), shape);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Points3d_getMarkerSize") */ 2476486298: {
            static auto hebi_charts_Points3d_getMarkerSize = libLoader.getFunc<double(*)(void*)>("hebi_charts_Points3d_getMarkerSize");
            double _value = hebi_charts_Points3d_getMarkerSize(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Points3d_setMarkerSize") */ 1065476686: {
            static auto hebi_charts_Points3d_setMarkerSize = libLoader.getFunc<void(*)(void*, double)>("hebi_charts_Points3d_setMarkerSize");
            double value = getScalar<double>(prhs[2]);
            hebi_charts_Points3d_setMarkerSize(getPtr<void*>(prhs[1]), value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Points3d_isSelfIllumination") */ 210770462: {
            static auto hebi_charts_Points3d_isSelfIllumination = libLoader.getFunc<int(*)(void*)>("hebi_charts_Points3d_isSelfIllumination");
            int _value = hebi_charts_Points3d_isSelfIllumination(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Points3d_setSelfIllumination") */ 512389092: {
            static auto hebi_charts_Points3d_setSelfIllumination = libLoader.getFunc<void(*)(void*, int)>("hebi_charts_Points3d_setSelfIllumination");
            int value = getScalar<int>(prhs[2]);
            hebi_charts_Points3d_setSelfIllumination(getPtr<void*>(prhs[1]), value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Points3d_isVertexSharing") */ 1374619971: {
            static auto hebi_charts_Points3d_isVertexSharing = libLoader.getFunc<int(*)(void*)>("hebi_charts_Points3d_isVertexSharing");
            int _value = hebi_charts_Points3d_isVertexSharing(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Points3d_setVertexSharing") */ 1791901321: {
            static auto hebi_charts_Points3d_setVertexSharing = libLoader.getFunc<void(*)(void*, int)>("hebi_charts_Points3d_setVertexSharing");
            int value = getScalar<int>(prhs[2]);
            hebi_charts_Points3d_setVertexSharing(getPtr<void*>(prhs[1]), value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Points3d_setMaxPointCount") */ 247628292: {
            static auto hebi_charts_Points3d_setMaxPointCount = libLoader.getFunc<void(*)(void*, size_t)>("hebi_charts_Points3d_setMaxPointCount");
            size_t count = getScalar<size_t>(prhs[2]);
            hebi_charts_Points3d_setMaxPointCount(getPtr<void*>(prhs[1]), count);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Points3d_clear") */ 1098647088: {
            static auto hebi_charts_Points3d_clear = libLoader.getFunc<void(*)(void*)>("hebi_charts_Points3d_clear");
            hebi_charts_Points3d_clear(getPtr<void*>(prhs[1]));
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Points3d_setData") */ 2851140133: {
            static auto hebi_charts_Points3d_setData = libLoader.getFunc<void(*)(void*, const double*, const double*, const double*, size_t)>("hebi_charts_Points3d_setData");
            MatlabArray<double, mxDOUBLE_CLASS> x(prhs[2]);
            MatlabArray<double, mxDOUBLE_CLASS> y(prhs[3]);
            MatlabArray<double, mxDOUBLE_CLASS> z(prhs[4]);
            size_t length = getScalar<size_t>(prhs[5]);
            hebi_charts_Points3d_setData(getPtr<void*>(prhs[1]), x, y, z, length);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Points3d_addPoints") */ 1010583341: {
            static auto hebi_charts_Points3d_addPoints = libLoader.getFunc<void(*)(void*, const double*, const double*, const double*, size_t)>("hebi_charts_Points3d_addPoints");
            MatlabArray<double, mxDOUBLE_CLASS> x(prhs[2]);
            MatlabArray<double, mxDOUBLE_CLASS> y(prhs[3]);
            MatlabArray<double, mxDOUBLE_CLASS> z(prhs[4]);
            size_t length = getScalar<size_t>(prhs[5]);
            hebi_charts_Points3d_addPoints(getPtr<void*>(prhs[1]), x, y, z, length);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Points3d_addPoint") */ 735251532: {
            static auto hebi_charts_Points3d_addPoint = libLoader.getFunc<void(*)(void*, double, double, double)>("hebi_charts_Points3d_addPoint");
            double x = getScalar<double>(prhs[2]);
            double y = getScalar<double>(prhs[3]);
            double z = getScalar<double>(prhs[4]);
            hebi_charts_Points3d_addPoint(getPtr<void*>(prhs[1]), x, y, z);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Points3d_to_Series3d") */ 2229257233: {
            static auto hebi_charts_Points3d_to_Series3d = libLoader.getFunc<void*(*)(void*)>("hebi_charts_Points3d_to_Series3d");
            if (nlhs > 0) plhs[0] = createMexPointer(hebi_charts_Points3d_to_Series3d(getPtr<void*>(prhs[1])));
            break;
        }

        // RecordingResult
        case /* hash32_fnv1a("hebi_charts_RecordingResult_getDirectory") */ 1213405822: {
            static auto hebi_charts_RecordingResult_getDirectory = libLoader.getFunc<const char*(*)(void*)>("hebi_charts_RecordingResult_getDirectory");
            const char* _value = hebi_charts_RecordingResult_getDirectory(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexString(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_RecordingResult_getDroppedCount") */ 128995370: {
            static auto hebi_charts_RecordingResult_getDroppedCount = libLoader.getFunc<size_t(*)(void*)>("hebi_charts_RecordingResult_getDroppedCount");
            size_t _value = hebi_charts_RecordingResult_getDroppedCount(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_RecordingResult_getDuration") */ 877501107: {
            static auto hebi_charts_RecordingResult_getDuration = libLoader.getFunc<double(*)(void*)>("hebi_charts_RecordingResult_getDuration");
            double _value = hebi_charts_RecordingResult_getDuration(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_RecordingResult_getAverageFps") */ 1394133149: {
            static auto hebi_charts_RecordingResult_getAverageFps = libLoader.getFunc<double(*)(void*)>("hebi_charts_RecordingResult_getAverageFps");
            double _value = hebi_charts_RecordingResult_getAverageFps(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_RecordingResult_getManifest") */ 2666175586: {
            static auto hebi_charts_RecordingResult_getManifest = libLoader.getFunc<const char*(*)(void*)>("hebi_charts_RecordingResult_getManifest");
            const char* _value = hebi_charts_RecordingResult_getManifest(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexString(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_RecordingResult_getRecordedCount") */ 1112605622: {
            static auto hebi_charts_RecordingResult_getRecordedCount = libLoader.getFunc<size_t(*)(void*)>("hebi_charts_RecordingResult_getRecordedCount");
            size_t _value = hebi_charts_RecordingResult_getRecordedCount(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_RecordingResult_getSkippedCount") */ 2669863444: {
            static auto hebi_charts_RecordingResult_getSkippedCount = libLoader.getFunc<size_t(*)(void*)>("hebi_charts_RecordingResult_getSkippedCount");
            size_t _value = hebi_charts_RecordingResult_getSkippedCount(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_RecordingResult_getTotalFrames") */ 1060893205: {
            static auto hebi_charts_RecordingResult_getTotalFrames = libLoader.getFunc<size_t(*)(void*)>("hebi_charts_RecordingResult_getTotalFrames");
            size_t _value = hebi_charts_RecordingResult_getTotalFrames(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_RecordingResult_getFfmpegCommand") */ 595525159: {
            static auto hebi_charts_RecordingResult_getFfmpegCommand = libLoader.getFunc<const char*(*)(void*, int, int)>("hebi_charts_RecordingResult_getFfmpegCommand");
            int outputFormat = getScalar<int>(prhs[2]);
            int deleteDirectory = getScalar<int>(prhs[3]);
            const char* _value = hebi_charts_RecordingResult_getFfmpegCommand(getPtr<void*>(prhs[1]), outputFormat, deleteDirectory);
            if (nlhs > 0) plhs[0] = createMexString(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_RecordingResult_runFfmpeg") */ 259333519: {
            static auto hebi_charts_RecordingResult_runFfmpeg = libLoader.getFunc<const char*(*)(void*, int, int)>("hebi_charts_RecordingResult_runFfmpeg");
            int outputFormat = getScalar<int>(prhs[2]);
            int deleteDirectory = getScalar<int>(prhs[3]);
            const char* _value = hebi_charts_RecordingResult_runFfmpeg(getPtr<void*>(prhs[1]), outputFormat, deleteDirectory);
            if (nlhs > 0) plhs[0] = createMexString(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_RecordingResult_release") */ 3748485838: {
            static auto hebi_charts_RecordingResult_release = libLoader.getFunc<void(*)(void*)>("hebi_charts_RecordingResult_release");
            hebi_charts_RecordingResult_release(getPtr<void*>(prhs[1]));
            break;
        }

        // Scene3d
        case /* hash32_fnv1a("hebi_charts_Scene3d_setGridBounds") */ 4249784247: {
            static auto hebi_charts_Scene3d_setGridBounds = libLoader.getFunc<void(*)(void*, double, double, double, double, double, double)>("hebi_charts_Scene3d_setGridBounds");
            double minX = getScalar<double>(prhs[2]);
            double maxX = getScalar<double>(prhs[3]);
            double minY = getScalar<double>(prhs[4]);
            double maxY = getScalar<double>(prhs[5]);
            double minZ = getScalar<double>(prhs[6]);
            double maxZ = getScalar<double>(prhs[7]);
            hebi_charts_Scene3d_setGridBounds(getPtr<void*>(prhs[1]), minX, maxX, minY, maxY, minZ, maxZ);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Scene3d_getMaxX") */ 646308900: {
            static auto hebi_charts_Scene3d_getMaxX = libLoader.getFunc<double(*)(void*)>("hebi_charts_Scene3d_getMaxX");
            double _value = hebi_charts_Scene3d_getMaxX(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Scene3d_setMaxX") */ 3323998328: {
            static auto hebi_charts_Scene3d_setMaxX = libLoader.getFunc<void(*)(void*, double)>("hebi_charts_Scene3d_setMaxX");
            double val = getScalar<double>(prhs[2]);
            hebi_charts_Scene3d_setMaxX(getPtr<void*>(prhs[1]), val);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Scene3d_getMaxY") */ 663086519: {
            static auto hebi_charts_Scene3d_getMaxY = libLoader.getFunc<double(*)(void*)>("hebi_charts_Scene3d_getMaxY");
            double _value = hebi_charts_Scene3d_getMaxY(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Scene3d_setMaxY") */ 3340775947: {
            static auto hebi_charts_Scene3d_setMaxY = libLoader.getFunc<void(*)(void*, double)>("hebi_charts_Scene3d_setMaxY");
            double val = getScalar<double>(prhs[2]);
            hebi_charts_Scene3d_setMaxY(getPtr<void*>(prhs[1]), val);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Scene3d_getMaxZ") */ 679864138: {
            static auto hebi_charts_Scene3d_getMaxZ = libLoader.getFunc<double(*)(void*)>("hebi_charts_Scene3d_getMaxZ");
            double _value = hebi_charts_Scene3d_getMaxZ(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Scene3d_setMaxZ") */ 3357553566: {
            static auto hebi_charts_Scene3d_setMaxZ = libLoader.getFunc<void(*)(void*, double)>("hebi_charts_Scene3d_setMaxZ");
            double val = getScalar<double>(prhs[2]);
            hebi_charts_Scene3d_setMaxZ(getPtr<void*>(prhs[1]), val);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Scene3d_getMinX") */ 3281704058: {
            static auto hebi_charts_Scene3d_getMinX = libLoader.getFunc<double(*)(void*)>("hebi_charts_Scene3d_getMinX");
            double _value = hebi_charts_Scene3d_getMinX(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Scene3d_setMinX") */ 597161382: {
            static auto hebi_charts_Scene3d_setMinX = libLoader.getFunc<void(*)(void*, double)>("hebi_charts_Scene3d_setMinX");
            double val = getScalar<double>(prhs[2]);
            hebi_charts_Scene3d_setMinX(getPtr<void*>(prhs[1]), val);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Scene3d_getMinY") */ 3298481677: {
            static auto hebi_charts_Scene3d_getMinY = libLoader.getFunc<double(*)(void*)>("hebi_charts_Scene3d_getMinY");
            double _value = hebi_charts_Scene3d_getMinY(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Scene3d_setMinY") */ 613939001: {
            static auto hebi_charts_Scene3d_setMinY = libLoader.getFunc<void(*)(void*, double)>("hebi_charts_Scene3d_setMinY");
            double val = getScalar<double>(prhs[2]);
            hebi_charts_Scene3d_setMinY(getPtr<void*>(prhs[1]), val);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Scene3d_getMinZ") */ 3248148820: {
            static auto hebi_charts_Scene3d_getMinZ = libLoader.getFunc<double(*)(void*)>("hebi_charts_Scene3d_getMinZ");
            double _value = hebi_charts_Scene3d_getMinZ(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Scene3d_setMinZ") */ 563606144: {
            static auto hebi_charts_Scene3d_setMinZ = libLoader.getFunc<void(*)(void*, double)>("hebi_charts_Scene3d_setMinZ");
            double val = getScalar<double>(prhs[2]);
            hebi_charts_Scene3d_setMinZ(getPtr<void*>(prhs[1]), val);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Scene3d_getGridStep") */ 1482289480: {
            static auto hebi_charts_Scene3d_getGridStep = libLoader.getFunc<double(*)(void*)>("hebi_charts_Scene3d_getGridStep");
            double _value = hebi_charts_Scene3d_getGridStep(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Scene3d_setGridStep") */ 1127495300: {
            static auto hebi_charts_Scene3d_setGridStep = libLoader.getFunc<void(*)(void*, double)>("hebi_charts_Scene3d_setGridStep");
            double gridStep = getScalar<double>(prhs[2]);
            hebi_charts_Scene3d_setGridStep(getPtr<void*>(prhs[1]), gridStep);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Scene3d_getCamera") */ 4036660369: {
            static auto hebi_charts_Scene3d_getCamera = libLoader.getFunc<void*(*)(void*)>("hebi_charts_Scene3d_getCamera");
            void* _value = hebi_charts_Scene3d_getCamera(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexPointer(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Scene3d_addRobot") */ 1709942591: {
            static auto hebi_charts_Scene3d_addRobot = libLoader.getFunc<void*(*)(void*, const char*)>("hebi_charts_Scene3d_addRobot");
            MatlabString pathOrUrl(prhs[2]);
            void* _value = hebi_charts_Scene3d_addRobot(getPtr<void*>(prhs[1]), pathOrUrl);
            if (nlhs > 0) plhs[0] = createMexPointer(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Scene3d_addMesh") */ 3080073628: {
            static auto hebi_charts_Scene3d_addMesh = libLoader.getFunc<void*(*)(void*, const char*)>("hebi_charts_Scene3d_addMesh");
            MatlabString pathOrUrl(prhs[2]);
            void* _value = hebi_charts_Scene3d_addMesh(getPtr<void*>(prhs[1]), pathOrUrl);
            if (nlhs > 0) plhs[0] = createMexPointer(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Scene3d_addFrame") */ 1393531910: {
            static auto hebi_charts_Scene3d_addFrame = libLoader.getFunc<void*(*)(void*, double)>("hebi_charts_Scene3d_addFrame");
            double lengthInMeters = getScalar<double>(prhs[2]);
            void* _value = hebi_charts_Scene3d_addFrame(getPtr<void*>(prhs[1]), lengthInMeters);
            if (nlhs > 0) plhs[0] = createMexPointer(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Scene3d_addLine") */ 1203584519: {
            static auto hebi_charts_Scene3d_addLine = libLoader.getFunc<void*(*)(void*)>("hebi_charts_Scene3d_addLine");
            void* _value = hebi_charts_Scene3d_addLine(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexPointer(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Scene3d_addPoints") */ 4072579238: {
            static auto hebi_charts_Scene3d_addPoints = libLoader.getFunc<void*(*)(void*)>("hebi_charts_Scene3d_addPoints");
            void* _value = hebi_charts_Scene3d_addPoints(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexPointer(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Scene3d_release") */ 1418222175: {
            static auto hebi_charts_Scene3d_release = libLoader.getFunc<void(*)(void*)>("hebi_charts_Scene3d_release");
            hebi_charts_Scene3d_release(getPtr<void*>(prhs[1]));
            break;
        }

        // StreamView
        case /* hash32_fnv1a("hebi_charts_StreamView_release") */ 2205044721: {
            static auto hebi_charts_StreamView_release = libLoader.getFunc<void(*)(void*)>("hebi_charts_StreamView_release");
            hebi_charts_StreamView_release(getPtr<void*>(prhs[1]));
            break;
        }

        // XYChart
        case /* hash32_fnv1a("hebi_charts_XYChart_getTitle") */ 446598326: {
            static auto hebi_charts_XYChart_getTitle = libLoader.getFunc<const char*(*)(void*)>("hebi_charts_XYChart_getTitle");
            const char* _value = hebi_charts_XYChart_getTitle(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexString(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYChart_setTitle") */ 3453143722: {
            static auto hebi_charts_XYChart_setTitle = libLoader.getFunc<void(*)(void*, const char*)>("hebi_charts_XYChart_setTitle");
            MatlabString title(prhs[2]);
            hebi_charts_XYChart_setTitle(getPtr<void*>(prhs[1]), title);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYChart_getXAssumeSorted") */ 3851556259: {
            static auto hebi_charts_XYChart_getXAssumeSorted = libLoader.getFunc<int(*)(void*)>("hebi_charts_XYChart_getXAssumeSorted");
            int _value = hebi_charts_XYChart_getXAssumeSorted(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYChart_setXAssumeSorted") */ 3077688367: {
            static auto hebi_charts_XYChart_setXAssumeSorted = libLoader.getFunc<void(*)(void*, int)>("hebi_charts_XYChart_setXAssumeSorted");
            int xAssumeSorted = getScalar<int>(prhs[2]);
            hebi_charts_XYChart_setXAssumeSorted(getPtr<void*>(prhs[1]), xAssumeSorted);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYChart_isXAutoUnitScaling") */ 790614670: {
            static auto hebi_charts_XYChart_isXAutoUnitScaling = libLoader.getFunc<int(*)(void*)>("hebi_charts_XYChart_isXAutoUnitScaling");
            int _value = hebi_charts_XYChart_isXAutoUnitScaling(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYChart_setXAutoUnitScaling") */ 1247121678: {
            static auto hebi_charts_XYChart_setXAutoUnitScaling = libLoader.getFunc<void(*)(void*, int)>("hebi_charts_XYChart_setXAutoUnitScaling");
            int enabled = getScalar<int>(prhs[2]);
            hebi_charts_XYChart_setXAutoUnitScaling(getPtr<void*>(prhs[1]), enabled);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYChart_getXLabel") */ 1702966006: {
            static auto hebi_charts_XYChart_getXLabel = libLoader.getFunc<const char*(*)(void*)>("hebi_charts_XYChart_getXLabel");
            const char* _value = hebi_charts_XYChart_getXLabel(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexString(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYChart_setXLabel") */ 931614378: {
            static auto hebi_charts_XYChart_setXLabel = libLoader.getFunc<void(*)(void*, const char*)>("hebi_charts_XYChart_setXLabel");
            MatlabString label(prhs[2]);
            hebi_charts_XYChart_setXLabel(getPtr<void*>(prhs[1]), label);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYChart_setXLim") */ 3399138658: {
            static auto hebi_charts_XYChart_setXLim = libLoader.getFunc<int(*)(void*, double, double)>("hebi_charts_XYChart_setXLim");
            double min = getScalar<double>(prhs[2]);
            double max = getScalar<double>(prhs[3]);
            int _value = hebi_charts_XYChart_setXLim(getPtr<void*>(prhs[1]), min, max);
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYChart_getXMax") */ 1111650342: {
            static auto hebi_charts_XYChart_getXMax = libLoader.getFunc<double(*)(void*)>("hebi_charts_XYChart_getXMax");
            double _value = hebi_charts_XYChart_getXMax(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYChart_setXMax") */ 701119482: {
            static auto hebi_charts_XYChart_setXMax = libLoader.getFunc<void(*)(void*, double)>("hebi_charts_XYChart_setXMax");
            double max = getScalar<double>(prhs[2]);
            hebi_charts_XYChart_setXMax(getPtr<void*>(prhs[1]), max);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYChart_getXMin") */ 875483748: {
            static auto hebi_charts_XYChart_getXMin = libLoader.getFunc<double(*)(void*)>("hebi_charts_XYChart_getXMin");
            double _value = hebi_charts_XYChart_getXMin(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYChart_setXMin") */ 870175600: {
            static auto hebi_charts_XYChart_setXMin = libLoader.getFunc<void(*)(void*, double)>("hebi_charts_XYChart_setXMin");
            double min = getScalar<double>(prhs[2]);
            hebi_charts_XYChart_setXMin(getPtr<void*>(prhs[1]), min);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYChart_getXUnit") */ 2514604662: {
            static auto hebi_charts_XYChart_getXUnit = libLoader.getFunc<const char*(*)(void*)>("hebi_charts_XYChart_getXUnit");
            const char* _value = hebi_charts_XYChart_getXUnit(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexString(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYChart_setXUnit") */ 2396226850: {
            static auto hebi_charts_XYChart_setXUnit = libLoader.getFunc<void(*)(void*, const char*)>("hebi_charts_XYChart_setXUnit");
            MatlabString unit(prhs[2]);
            hebi_charts_XYChart_setXUnit(getPtr<void*>(prhs[1]), unit);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYChart_isYAutoUnitScaling") */ 704255987: {
            static auto hebi_charts_XYChart_isYAutoUnitScaling = libLoader.getFunc<int(*)(void*)>("hebi_charts_XYChart_isYAutoUnitScaling");
            int _value = hebi_charts_XYChart_isYAutoUnitScaling(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYChart_setYAutoUnitScaling") */ 1160762995: {
            static auto hebi_charts_XYChart_setYAutoUnitScaling = libLoader.getFunc<void(*)(void*, int)>("hebi_charts_XYChart_setYAutoUnitScaling");
            int enabled = getScalar<int>(prhs[2]);
            hebi_charts_XYChart_setYAutoUnitScaling(getPtr<void*>(prhs[1]), enabled);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYChart_getYLabel") */ 3186521171: {
            static auto hebi_charts_XYChart_getYLabel = libLoader.getFunc<const char*(*)(void*)>("hebi_charts_XYChart_getYLabel");
            const char* _value = hebi_charts_XYChart_getYLabel(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexString(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYChart_setYLabel") */ 1546141303: {
            static auto hebi_charts_XYChart_setYLabel = libLoader.getFunc<void(*)(void*, const char*)>("hebi_charts_XYChart_setYLabel");
            MatlabString label(prhs[2]);
            hebi_charts_XYChart_setYLabel(getPtr<void*>(prhs[1]), label);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYChart_setYLim") */ 232695975: {
            static auto hebi_charts_XYChart_setYLim = libLoader.getFunc<void(*)(void*, double, double)>("hebi_charts_XYChart_setYLim");
            double min = getScalar<double>(prhs[2]);
            double max = getScalar<double>(prhs[3]);
            hebi_charts_XYChart_setYLim(getPtr<void*>(prhs[1]), min, max);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYChart_getYMax") */ 558185587: {
            static auto hebi_charts_XYChart_getYMax = libLoader.getFunc<double(*)(void*)>("hebi_charts_XYChart_getYMax");
            double _value = hebi_charts_XYChart_getYMax(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYChart_setYMax") */ 2517709703: {
            static auto hebi_charts_XYChart_setYMax = libLoader.getFunc<void(*)(void*, double)>("hebi_charts_XYChart_setYMax");
            double max = getScalar<double>(prhs[2]);
            hebi_charts_XYChart_setYMax(getPtr<void*>(prhs[1]), max);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYChart_getYMin") */ 928573133: {
            static auto hebi_charts_XYChart_getYMin = libLoader.getFunc<double(*)(void*)>("hebi_charts_XYChart_getYMin");
            double _value = hebi_charts_XYChart_getYMin(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYChart_setYMin") */ 2753876297: {
            static auto hebi_charts_XYChart_setYMin = libLoader.getFunc<void(*)(void*, double)>("hebi_charts_XYChart_setYMin");
            double min = getScalar<double>(prhs[2]);
            hebi_charts_XYChart_setYMin(getPtr<void*>(prhs[1]), min);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYChart_getYUnit") */ 4268819729: {
            static auto hebi_charts_XYChart_getYUnit = libLoader.getFunc<const char*(*)(void*)>("hebi_charts_XYChart_getYUnit");
            const char* _value = hebi_charts_XYChart_getYUnit(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexString(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYChart_setYUnit") */ 3765687725: {
            static auto hebi_charts_XYChart_setYUnit = libLoader.getFunc<void(*)(void*, const char*)>("hebi_charts_XYChart_setYUnit");
            MatlabString unit(prhs[2]);
            hebi_charts_XYChart_setYUnit(getPtr<void*>(prhs[1]), unit);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYChart_addXCursor") */ 310287279: {
            static auto hebi_charts_XYChart_addXCursor = libLoader.getFunc<void*(*)(void*)>("hebi_charts_XYChart_addXCursor");
            void* _value = hebi_charts_XYChart_addXCursor(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexPointer(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYChart_addYCursor") */ 2095989168: {
            static auto hebi_charts_XYChart_addYCursor = libLoader.getFunc<void*(*)(void*)>("hebi_charts_XYChart_addYCursor");
            void* _value = hebi_charts_XYChart_addYCursor(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexPointer(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYChart_release") */ 2008978777: {
            static auto hebi_charts_XYChart_release = libLoader.getFunc<void(*)(void*)>("hebi_charts_XYChart_release");
            hebi_charts_XYChart_release(getPtr<void*>(prhs[1]));
            break;
        }

        // LatencyChart
        case /* hash32_fnv1a("hebi_charts_LatencyChart_addTrace") */ 2363952669: {
            static auto hebi_charts_LatencyChart_addTrace = libLoader.getFunc<void*(*)(void*, const char*)>("hebi_charts_LatencyChart_addTrace");
            MatlabString name(prhs[2]);
            void* _value = hebi_charts_LatencyChart_addTrace(getPtr<void*>(prhs[1]), name);
            if (nlhs > 0) plhs[0] = createMexPointer(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_LatencyChart_to_XYChart") */ 1611720866: {
            static auto hebi_charts_LatencyChart_to_XYChart = libLoader.getFunc<void*(*)(void*)>("hebi_charts_LatencyChart_to_XYChart");
            if (nlhs > 0) plhs[0] = createMexPointer(hebi_charts_LatencyChart_to_XYChart(getPtr<void*>(prhs[1])));
            break;
        }

        // LineChart
        case /* hash32_fnv1a("hebi_charts_LineChart_addLine") */ 59856830: {
            static auto hebi_charts_LineChart_addLine = libLoader.getFunc<void*(*)(void*, const char*)>("hebi_charts_LineChart_addLine");
            MatlabString label(prhs[2]);
            void* _value = hebi_charts_LineChart_addLine(getPtr<void*>(prhs[1]), label);
            if (nlhs > 0) plhs[0] = createMexPointer(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_LineChart_to_XYChart") */ 2806901250: {
            static auto hebi_charts_LineChart_to_XYChart = libLoader.getFunc<void*(*)(void*)>("hebi_charts_LineChart_to_XYChart");
            if (nlhs > 0) plhs[0] = createMexPointer(hebi_charts_LineChart_to_XYChart(getPtr<void*>(prhs[1])));
            break;
        }

        // XYSeries
        case /* hash32_fnv1a("hebi_charts_XYSeries_getColor") */ 1905988630: {
            static auto hebi_charts_XYSeries_getColor = libLoader.getFunc<int(*)(void*)>("hebi_charts_XYSeries_getColor");
            int _value = hebi_charts_XYSeries_getColor(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYSeries_setColor") */ 2118705778: {
            static auto hebi_charts_XYSeries_setColor = libLoader.getFunc<void(*)(void*, int)>("hebi_charts_XYSeries_setColor");
            int color = getScalar<int>(prhs[2]);
            hebi_charts_XYSeries_setColor(getPtr<void*>(prhs[1]), color);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYSeries_getLabel") */ 3419944895: {
            static auto hebi_charts_XYSeries_getLabel = libLoader.getFunc<const char*(*)(void*)>("hebi_charts_XYSeries_getLabel");
            const char* _value = hebi_charts_XYSeries_getLabel(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexString(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYSeries_setLabel") */ 182381595: {
            static auto hebi_charts_XYSeries_setLabel = libLoader.getFunc<void(*)(void*, const char*)>("hebi_charts_XYSeries_setLabel");
            MatlabString label(prhs[2]);
            hebi_charts_XYSeries_setLabel(getPtr<void*>(prhs[1]), label);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYSeries_getLineStyle") */ 3465012550: {
            static auto hebi_charts_XYSeries_getLineStyle = libLoader.getFunc<int(*)(void*)>("hebi_charts_XYSeries_getLineStyle");
            int _value = hebi_charts_XYSeries_getLineStyle(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYSeries_setLineStyle") */ 826964842: {
            static auto hebi_charts_XYSeries_setLineStyle = libLoader.getFunc<void(*)(void*, int)>("hebi_charts_XYSeries_setLineStyle");
            int lineStyle = getScalar<int>(prhs[2]);
            hebi_charts_XYSeries_setLineStyle(getPtr<void*>(prhs[1]), lineStyle);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYSeries_getLineWidth") */ 1097775599: {
            static auto hebi_charts_XYSeries_getLineWidth = libLoader.getFunc<double(*)(void*)>("hebi_charts_XYSeries_getLineWidth");
            double _value = hebi_charts_XYSeries_getLineWidth(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYSeries_setLineWidth") */ 1783421363: {
            static auto hebi_charts_XYSeries_setLineWidth = libLoader.getFunc<void(*)(void*, double)>("hebi_charts_XYSeries_setLineWidth");
            double lineWidth = getScalar<double>(prhs[2]);
            hebi_charts_XYSeries_setLineWidth(getPtr<void*>(prhs[1]), lineWidth);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYSeries_getMarkerSize") */ 2363327772: {
            static auto hebi_charts_XYSeries_getMarkerSize = libLoader.getFunc<double(*)(void*)>("hebi_charts_XYSeries_getMarkerSize");
            double _value = hebi_charts_XYSeries_getMarkerSize(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYSeries_setMarkerSize") */ 3401588224: {
            static auto hebi_charts_XYSeries_setMarkerSize = libLoader.getFunc<void(*)(void*, double)>("hebi_charts_XYSeries_setMarkerSize");
            double markerSize = getScalar<double>(prhs[2]);
            hebi_charts_XYSeries_setMarkerSize(getPtr<void*>(prhs[1]), markerSize);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYSeries_getMarkerType") */ 3085579277: {
            static auto hebi_charts_XYSeries_getMarkerType = libLoader.getFunc<int(*)(void*)>("hebi_charts_XYSeries_getMarkerType");
            int _value = hebi_charts_XYSeries_getMarkerType(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYSeries_setMarkerType") */ 1292126297: {
            static auto hebi_charts_XYSeries_setMarkerType = libLoader.getFunc<void(*)(void*, int)>("hebi_charts_XYSeries_setMarkerType");
            int markerType = getScalar<int>(prhs[2]);
            hebi_charts_XYSeries_setMarkerType(getPtr<void*>(prhs[1]), markerType);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYSeries_isShowInLegend") */ 2272441234: {
            static auto hebi_charts_XYSeries_isShowInLegend = libLoader.getFunc<int(*)(void*)>("hebi_charts_XYSeries_isShowInLegend");
            int _value = hebi_charts_XYSeries_isShowInLegend(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYSeries_setShowInLegend") */ 678502520: {
            static auto hebi_charts_XYSeries_setShowInLegend = libLoader.getFunc<void(*)(void*, int)>("hebi_charts_XYSeries_setShowInLegend");
            int showInLegend = getScalar<int>(prhs[2]);
            hebi_charts_XYSeries_setShowInLegend(getPtr<void*>(prhs[1]), showInLegend);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYSeries_isVisible") */ 4264825601: {
            static auto hebi_charts_XYSeries_isVisible = libLoader.getFunc<int(*)(void*)>("hebi_charts_XYSeries_isVisible");
            int _value = hebi_charts_XYSeries_isVisible(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYSeries_setVisible") */ 226469463: {
            static auto hebi_charts_XYSeries_setVisible = libLoader.getFunc<void(*)(void*, int)>("hebi_charts_XYSeries_setVisible");
            int visible = getScalar<int>(prhs[2]);
            hebi_charts_XYSeries_setVisible(getPtr<void*>(prhs[1]), visible);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_XYSeries_release") */ 3347007086: {
            static auto hebi_charts_XYSeries_release = libLoader.getFunc<void(*)(void*)>("hebi_charts_XYSeries_release");
            hebi_charts_XYSeries_release(getPtr<void*>(prhs[1]));
            break;
        }

        // LatencyTrace
        case /* hash32_fnv1a("hebi_charts_LatencyTrace_tic") */ 3312694110: {
            static auto hebi_charts_LatencyTrace_tic = libLoader.getFunc<void(*)(void*)>("hebi_charts_LatencyTrace_tic");
            hebi_charts_LatencyTrace_tic(getPtr<void*>(prhs[1]));
            break;
        }
        case /* hash32_fnv1a("hebi_charts_LatencyTrace_toc") */ 3447209252: {
            static auto hebi_charts_LatencyTrace_toc = libLoader.getFunc<double(*)(void*)>("hebi_charts_LatencyTrace_toc");
            double _value = hebi_charts_LatencyTrace_toc(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_LatencyTrace_ticToc") */ 893458976: {
            static auto hebi_charts_LatencyTrace_ticToc = libLoader.getFunc<double(*)(void*)>("hebi_charts_LatencyTrace_ticToc");
            double _value = hebi_charts_LatencyTrace_ticToc(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_LatencyTrace_record") */ 289077951: {
            static auto hebi_charts_LatencyTrace_record = libLoader.getFunc<void(*)(void*, double)>("hebi_charts_LatencyTrace_record");
            double value = getScalar<double>(prhs[2]);
            hebi_charts_LatencyTrace_record(getPtr<void*>(prhs[1]), value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_LatencyTrace_recordWithCount") */ 3013829598: {
            static auto hebi_charts_LatencyTrace_recordWithCount = libLoader.getFunc<void(*)(void*, double, size_t)>("hebi_charts_LatencyTrace_recordWithCount");
            double value = getScalar<double>(prhs[2]);
            size_t count = getScalar<size_t>(prhs[3]);
            hebi_charts_LatencyTrace_recordWithCount(getPtr<void*>(prhs[1]), value, count);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_LatencyTrace_recordCompensated") */ 2071056952: {
            static auto hebi_charts_LatencyTrace_recordCompensated = libLoader.getFunc<void(*)(void*, double, double)>("hebi_charts_LatencyTrace_recordCompensated");
            double value = getScalar<double>(prhs[2]);
            double expectedIntervalBetweenValueSamples = getScalar<double>(prhs[3]);
            hebi_charts_LatencyTrace_recordCompensated(getPtr<void*>(prhs[1]), value, expectedIntervalBetweenValueSamples);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_LatencyTrace_reset") */ 2834722861: {
            static auto hebi_charts_LatencyTrace_reset = libLoader.getFunc<void(*)(void*)>("hebi_charts_LatencyTrace_reset");
            hebi_charts_LatencyTrace_reset(getPtr<void*>(prhs[1]));
            break;
        }
        case /* hash32_fnv1a("hebi_charts_LatencyTrace_to_XYSeries") */ 2481743194: {
            static auto hebi_charts_LatencyTrace_to_XYSeries = libLoader.getFunc<void*(*)(void*)>("hebi_charts_LatencyTrace_to_XYSeries");
            if (nlhs > 0) plhs[0] = createMexPointer(hebi_charts_LatencyTrace_to_XYSeries(getPtr<void*>(prhs[1])));
            break;
        }

        // Line
        case /* hash32_fnv1a("hebi_charts_Line_getMaxPointCount") */ 1615881806: {
            static auto hebi_charts_Line_getMaxPointCount = libLoader.getFunc<size_t(*)(void*)>("hebi_charts_Line_getMaxPointCount");
            size_t _value = hebi_charts_Line_getMaxPointCount(getPtr<void*>(prhs[1]));
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Line_setMaxPointCount") */ 3014365810: {
            static auto hebi_charts_Line_setMaxPointCount = libLoader.getFunc<void(*)(void*, size_t)>("hebi_charts_Line_setMaxPointCount");
            size_t count = getScalar<size_t>(prhs[2]);
            hebi_charts_Line_setMaxPointCount(getPtr<void*>(prhs[1]), count);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Line_clear") */ 672966850: {
            static auto hebi_charts_Line_clear = libLoader.getFunc<void(*)(void*)>("hebi_charts_Line_clear");
            hebi_charts_Line_clear(getPtr<void*>(prhs[1]));
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Line_setData") */ 3640589355: {
            static auto hebi_charts_Line_setData = libLoader.getFunc<void(*)(void*, const double*, const double*, size_t)>("hebi_charts_Line_setData");
            MatlabArray<double, mxDOUBLE_CLASS> x(prhs[2]);
            MatlabArray<double, mxDOUBLE_CLASS> y(prhs[3]);
            size_t length = getScalar<size_t>(prhs[4]);
            hebi_charts_Line_setData(getPtr<void*>(prhs[1]), x, y, length);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Line_addPoints") */ 3366173531: {
            static auto hebi_charts_Line_addPoints = libLoader.getFunc<void(*)(void*, const double*, const double*, size_t)>("hebi_charts_Line_addPoints");
            MatlabArray<double, mxDOUBLE_CLASS> x(prhs[2]);
            MatlabArray<double, mxDOUBLE_CLASS> y(prhs[3]);
            size_t length = getScalar<size_t>(prhs[4]);
            hebi_charts_Line_addPoints(getPtr<void*>(prhs[1]), x, y, length);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Line_addPoint") */ 4217019498: {
            static auto hebi_charts_Line_addPoint = libLoader.getFunc<void(*)(void*, double, double)>("hebi_charts_Line_addPoint");
            double x = getScalar<double>(prhs[2]);
            double y = getScalar<double>(prhs[3]);
            hebi_charts_Line_addPoint(getPtr<void*>(prhs[1]), x, y);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Line_to_XYSeries") */ 1080509183: {
            static auto hebi_charts_Line_to_XYSeries = libLoader.getFunc<void*(*)(void*)>("hebi_charts_Line_to_XYSeries");
            if (nlhs > 0) plhs[0] = createMexPointer(hebi_charts_Line_to_XYSeries(getPtr<void*>(prhs[1])));
            break;
        }

        // Runtime
        case /* hash32_fnv1a("hebi_charts_Runtime_setOption") */ 4101297084: {
            static auto hebi_charts_Runtime_setOption = libLoader.getFunc<int(*)(int, const char*)>("hebi_charts_Runtime_setOption");
            int option = getScalar<int>(prhs[1]);
            MatlabString value(prhs[2]);
            int _value = hebi_charts_Runtime_setOption(option, value);
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Runtime_setTheme") */ 823017098: {
            static auto hebi_charts_Runtime_setTheme = libLoader.getFunc<void(*)(int)>("hebi_charts_Runtime_setTheme");
            int theme = getScalar<int>(prhs[1]);
            hebi_charts_Runtime_setTheme(theme);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Runtime_setAutoCloseWindows") */ 168480543: {
            static auto hebi_charts_Runtime_setAutoCloseWindows = libLoader.getFunc<void(*)(int)>("hebi_charts_Runtime_setAutoCloseWindows");
            int autoClose = getScalar<int>(prhs[1]);
            hebi_charts_Runtime_setAutoCloseWindows(autoClose);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Runtime_waitUntilWindowsClosed") */ 1596282055: {
            static auto hebi_charts_Runtime_waitUntilWindowsClosed = libLoader.getFunc<int(*)()>("hebi_charts_Runtime_waitUntilWindowsClosed");
            int _value = hebi_charts_Runtime_waitUntilWindowsClosed();
            if (nlhs > 0) plhs[0] = createMexScalar(_value);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Runtime_collect") */ 1426291779: {
            static auto hebi_charts_Runtime_collect = libLoader.getFunc<void(*)()>("hebi_charts_Runtime_collect");
            hebi_charts_Runtime_collect();
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Runtime_closeAll") */ 1568276924: {
            static auto hebi_charts_Runtime_closeAll = libLoader.getFunc<void(*)()>("hebi_charts_Runtime_closeAll");
            hebi_charts_Runtime_closeAll();
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Runtime_runOnUiThread") */ 791123223: {
            static auto hebi_charts_Runtime_runOnUiThread = libLoader.getFunc<void(*)(void*, void*)>("hebi_charts_Runtime_runOnUiThread");
            void* func = getPtr<void*>(prhs[1]);
            void* userData = getPtr<void*>(prhs[2]);
            hebi_charts_Runtime_runOnUiThread(func, userData);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Runtime_printLastErrorDetails") */ 4208988338: {
            static auto hebi_charts_Runtime_printLastErrorDetails = libLoader.getFunc<void(*)()>("hebi_charts_Runtime_printLastErrorDetails");
            hebi_charts_Runtime_printLastErrorDetails();
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Runtime_printThreadInfo") */ 33274370: {
            static auto hebi_charts_Runtime_printThreadInfo = libLoader.getFunc<void(*)(const char*)>("hebi_charts_Runtime_printThreadInfo");
            MatlabString name(prhs[1]);
            hebi_charts_Runtime_printThreadInfo(name);
            break;
        }
        case /* hash32_fnv1a("hebi_charts_Runtime_getLastErrorString") */ 1987067772: {
            static auto hebi_charts_Runtime_getLastErrorString = libLoader.getFunc<const char*(*)()>("hebi_charts_Runtime_getLastErrorString");
            const char* _value = hebi_charts_Runtime_getLastErrorString();
            if (nlhs > 0) plhs[0] = createMexString(_value);
            break;
        }


        default:
            mexErrMsgIdAndTxt("hebi_charts:NotFound", "Function hash %08X not found.", funcHash);
    }

}

#ifdef __cplusplus
}
#endif