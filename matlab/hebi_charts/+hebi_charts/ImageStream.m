classdef (Sealed) ImageStream < handle & matlab.mixin.SetGet
% [EXPERIMENTAL API]
%   Represents a stream of images with an accessible pixel buffer. This class
%   is not thread-safe and should only be used from one thread. Buffers and metadata
%   for an image are only valid in between successful next() calls.
%
%   The stream reuses multiple buffers internally and provides efficient access to the raw memory.

    properties (Dependent)
        buffer
        bufferSize
        channels
        height
        pixelFormat
        rateLimit
        recorderThreads
        recording
        renderScale
        renderScaleX
        renderScaleY
        sequence
        stride
        timestamp
        width
    end

    methods

        function result = get.buffer(this)
            % @return the raw internal pixel buffer. Only valid between getNext calls.
            result = hebi_charts_native('hebi_charts_ImageStream_getBuffer', this.ptr);
        end

        function result = get.bufferSize(this)
            % @return the raw buffer size in bytes
            result = hebi_charts_native('hebi_charts_ImageStream_getBufferSize', this.ptr);
        end

        function result = get.channels(this)
            % @return the number of channels of the current frame (e.g. 4 for BGRA)
            result = hebi_charts_native('hebi_charts_ImageStream_getChannels', this.ptr);
        end

        function result = get.height(this)
            % @return the height of the current frame in pixels
            result = hebi_charts_native('hebi_charts_ImageStream_getHeight', this.ptr);
        end

        function result = get.pixelFormat(this)
            % Represents the pixel format of the current buffer. The native and most
            %   performant format is BGRA_PRE, which stores pixels in adjacent bytes
            %   with premultiplied alpha components.
            %
            %   Other convenience formats may be added in the future, but as of this point
            %   the others are all experimental.
            %
            %   [Memory Layout (BGRA_PRE)]
            %   Bytes are stored in order of increasing index: Blue, Green, Red, Alpha
            %
            %   [Compatibility]
            %   With an Alpha of 255 BGRA_PRE is identical to BGRA and is binary-compatible with the following:
            %
            %       OpenCV:    CV_8UC4
            %
            %       wxWidgets: BitmapBufferFormat_ARGB32
            %                  BitmapBufferFormat_RGB32
            %
            %       Qt:        Format_ARGB32 (on little endian)
            %                  Format_BGRA8888
            %
            %   Alpha less than 255 would show the image as darker or distorted, in which case
            %   the channels would need to be un-multiplied first.
            %
            %   [Usage]
            %   Pixels in this format can be decoded using the following sample code:
            %
            %       int i = rowstart + x * 4;
            %       int blue  = buffer[i + 0] & 0xff;
            %       int green = buffer[i + 1] & 0xff;
            %       int red   = buffer[i + 2] & 0xff;
            %       int alpha = buffer[i + 3] & 0xff;
            %
            %   @return the pixel format of the current frame
            result = hebi_charts_native('hebi_charts_ImageStream_getPixelFormat', this.ptr);
        end

        function set.pixelFormat(this, pixelFormat)
            % Sets the desired pixel format for future frames. The stream starts
            %   with the default of BGRA_PRE. Setting Unknown also reverts back to
            %   the default.
            %
            %   All other formats are considered experimental.
            %
            %   Inputs:
            %       pixelFormat
            hebi_charts_native('hebi_charts_ImageStream_setPixelFormat', this.ptr, hebi_charts.PixelFormat.toNativeValue(pixelFormat));
        end

        function result = get.rateLimit(this)
            % An artificial rate limit above which frames get ignored
            result = hebi_charts_native('hebi_charts_ImageStream_getRateLimit', this.ptr);
        end

        function set.rateLimit(this, maxFramesPerSecond)
            % Sets the artificial rate limit above which frames get ignored.
            %
            %   Inputs:
            %       maxFramesPerSecond - Rate limit in fps, e.g., 30.0
            %
            %   Throws:
            %       Error on internal errors
            status_ = hebi_charts_native('hebi_charts_ImageStream_setRateLimit', this.ptr, maxFramesPerSecond);
            if status_ ~= 0
                error(['Encountered error in ImageStream.set.rateLimit' ': ' hebi_charts_native('hebi_charts_Runtime_getLastErrorString')]);
            end
        end

        function result = get.recorderThreads(this)
            % Gets the number of threads used for encoding individual frames
            %
            %   Throws:
            %       Error on internal errors
            result = hebi_charts_native('hebi_charts_ImageStream_getRecorderThreads', this.ptr);
        end

        function set.recorderThreads(this, numThreads)
            % Sets the number of threads used for encoding individual frames
            %
            %   Inputs:
            %       numThreads
            %
            %   Throws:
            %       Error on internal errors
            status_ = hebi_charts_native('hebi_charts_ImageStream_setRecorderThreads', this.ptr, numThreads);
            if status_ ~= 0
                error(['Encountered error in ImageStream.set.recorderThreads' ': ' hebi_charts_native('hebi_charts_Runtime_getLastErrorString')]);
            end
        end

        function result = get.recording(this)
            % [EXPERIMENTAL API] Checks whether the stream is currently recording.
            %
            %   Throws:
            %       Error on internal errors
            result = hebi_charts_native('hebi_charts_ImageStream_isRecording', this.ptr);
        end

        function result = get.renderScale(this)
            % @return the logical to actual pixel scale that was applied to this snapshot
            result = hebi_charts_native('hebi_charts_ImageStream_getRenderScale', this.ptr);
        end

        function result = get.renderScaleX(this)
            % @return the logical to actual pixel scale that was applied to this snapshot in x
            result = hebi_charts_native('hebi_charts_ImageStream_getRenderScaleX', this.ptr);
        end

        function result = get.renderScaleY(this)
            % @return the logical to actual pixel scale that was applied to this snapshot in y
            result = hebi_charts_native('hebi_charts_ImageStream_getRenderScaleY', this.ptr);
        end

        function result = get.sequence(this)
            % @return the sequence number of the current frame
            result = hebi_charts_native('hebi_charts_ImageStream_getSequence', this.ptr);
        end

        function result = get.stride(this)
            % @return the number of bytes from the start of one row to the next.
            %   This includes any padding for memory alignment.
            result = hebi_charts_native('hebi_charts_ImageStream_getStride', this.ptr);
        end

        function result = get.timestamp(this)
            % @return the timestamp of the current frame in seconds
            result = hebi_charts_native('hebi_charts_ImageStream_getTimestamp', this.ptr);
        end

        function result = get.width(this)
            % @return the width of the current frame in pixels
            result = hebi_charts_native('hebi_charts_ImageStream_getWidth', this.ptr);
        end

        function setResolution(this, width, height)
            % Sets the resolution for future snapshots. Defaults to the initial resolution. Set 0 to auto-size.
            %
            %   Inputs:
            %       width
            %       height
            %
            %   Throws:
            %       Error on internal errors
            status_ = hebi_charts_native('hebi_charts_ImageStream_setResolution', this.ptr, width, height);
            if status_ ~= 0
                error(['Encountered error in ImageStream.setResolution' ': ' hebi_charts_native('hebi_charts_Runtime_getLastErrorString')]);
            end
        end

        function result = waitForNext(this, maxTimeoutMillis)
            % Waits until there is a new image, and flips internal buffers
            %   as needed. Similar in behavior, but more efficient than.
            %
            %       while (!tryGetNext() && !timeout) yield();
            %
            %   @return true if a new image is available
            %
            %   Inputs:
            %       maxTimeoutMillis - zero waits forever
            %
            %   Throws:
            %       Error on internal errors
            result = hebi_charts_native('hebi_charts_ImageStream_waitForNext', this.ptr, maxTimeoutMillis);
        end

        function result = tryGetNext(this)
            % Checks whether there is a new image, and flips
            %   internal buffers as needed. Any metadata is only
            %   valid until the next call.
            %
            %   @return true if a new image is available
            result = hebi_charts_native('hebi_charts_ImageStream_tryGetNext', this.ptr);
        end

        function startRecording(this, baseName, overwrite)
            % [EXPERIMENTAL API]
            %   Losslessly records individual frames to disk in a way that ffmpeg can convert.
            %
            %   The base name represents the file name without the extension. Images get stored
            %   in dir/<base>/*.png and the result will be in dir/<base>.<extension>.
            %
            %   Inputs:
            %       baseName - The target base name
            %       overwrite - Whether the recorder is allowed to overwrite existing files
            %
            %   Throws:
            %       Error IO errors when creating the directory
            if nargin < 3
                overwrite = false;
            end
            status_ = hebi_charts_native('hebi_charts_ImageStream_startRecording', this.ptr, baseName, overwrite);
            if status_ ~= 0
                error(['Encountered error in ImageStream.startRecording' ': ' hebi_charts_native('hebi_charts_Runtime_getLastErrorString')]);
            end
        end

        function obj = stopRecording(this)
            % [EXPERIMENTAL API] Blocking call that stops recording and returns the result.
            %
            %   Throws:
            %       Error on internal errors
            ptr_ = hebi_charts_native('hebi_charts_ImageStream_stopRecording', this.ptr);
            if isempty(ptr_)
                error(['Failed to create hebi_charts.RecordingResult in ImageStream.stopRecording' ': ' hebi_charts_native('hebi_charts_Runtime_getLastErrorString')]);
            end
            obj = hebi_charts.RecordingResult(ptr_);
        end

        function saveToFile(this, fileName)
            % Saves the image to a file
            %
            %   Inputs:
            %       fileName
            %
            %   Throws:
            %       Error on internal errors
            status_ = hebi_charts_native('hebi_charts_ImageStream_saveToFile', this.ptr, fileName);
            if status_ ~= 0
                error(['Encountered error in ImageStream.saveToFile' ': ' hebi_charts_native('hebi_charts_Runtime_getLastErrorString')]);
            end
        end

    end

    methods(Access = public, Hidden = true)
        function this = ImageStream(ptr, varargin)
            if ~isnumeric(ptr) && ~isa(ptr, 'lib.pointer')
                error('ImageStream constructor expects a C pointer type');
            end
            this.ptr = ptr;

            % Apply name-value pairs to properties, if provided
            if numel(varargin) > 1
                this.set(varargin{:});
            end
        end
    end
    properties (SetAccess = private, GetAccess = private, Hidden = true)
        ptr = [];
    end

    methods(Access = public, Hidden = true)

        function ptr = c_ptr(this)
            ptr = this.ptr;
        end

        function delete(this) % destructor
             if ~isempty(this.ptr)
                hebi_charts_native('hebi_charts_ImageStream_release', this.ptr);
                this.ptr = [];
            end
        end

        % Inherited handle methods that should be omitted from auto - complete
        function varargout = addlistener(varargin)
            varargout{:} = addlistener@handle(varargin{:});
        end
        function varargout = eq(varargin)
            varargout{:} = eq@handle(varargin{:});
        end
        function varargout = findobj(varargin)
            varargout{:} = findobj@handle(varargin{:});
        end
        function varargout = findprop(varargin)
            varargout{:} = findprop@handle(varargin{:});
        end
        function varargout = ge(varargin)
            varargout{:} = ge@handle(varargin{:});
        end
        function varargout = gt(varargin)
            varargout{:} = gt@handle(varargin{:});
        end
        function varargout = le(varargin)
            varargout{:} = le@handle(varargin{:});
        end
        function varargout = listener(varargin)
            varargout{:} = listener@handle(varargin{:});
        end
        function varargout = lt(varargin)
            varargout{:} = lt@handle(varargin{:});
        end
        function varargout = ne(varargin)
            varargout{:} = ne@handle(varargin{:});
        end
        function varargout = notify(varargin)
            varargout{:} = notify@handle(varargin{:});
        end

    end

end