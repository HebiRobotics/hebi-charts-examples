classdef (Sealed) RecordingResult < handle & matlab.mixin.SetGet
% [EXPERIMENTAL API]
%   Represents the result of a recording. Can be used to
%   get various statistics and/or trigger FFMpeg.

    properties (Dependent)
        directory
        dropped
        duration
        fps
        manifest
        recorded
        skipped
        total_frames
    end

    methods

        function result = get.directory(this)
            % The absolute path to the directory containing the PNG frames and manifest.
            %
            %   Throws:
            %       Error on internal errors
            result = hebi_charts_native('hebi_charts_RecordingResult_getDirectory', this.ptr);
        end

        function result = get.dropped(this)
            % The number of frames dropped because the hardware (CPU/Disk) couldn't keep up.
            result = hebi_charts_native('hebi_charts_RecordingResult_getDroppedCount', this.ptr);
        end

        function result = get.duration(this)
            % The total duration in seconds.
            result = hebi_charts_native('hebi_charts_RecordingResult_getDuration', this.ptr);
        end

        function result = get.fps(this)
            % The actual average frames per second of the recorded sequence.
            result = hebi_charts_native('hebi_charts_RecordingResult_getAverageFps', this.ptr);
        end

        function result = get.manifest(this)
            % The absolute path to the manifest containing timing metadata.
            %
            %   Throws:
            %       Error on internal errors
            result = hebi_charts_native('hebi_charts_RecordingResult_getManifest', this.ptr);
        end

        function result = get.recorded(this)
            % The number of frames successfully written to disk.
            result = hebi_charts_native('hebi_charts_RecordingResult_getRecordedCount', this.ptr);
        end

        function result = get.skipped(this)
            % The number of frames intentionally skipped due to the rate limit.
            result = hebi_charts_native('hebi_charts_RecordingResult_getSkippedCount', this.ptr);
        end

        function result = get.total_frames(this)
            % The total number of hardware pulses/frames encountered during the session.
            result = hebi_charts_native('hebi_charts_RecordingResult_getTotalFrames', this.ptr);
        end

        function result = getFfmpegCommand(this, outputFormat, deleteDirectory)
            % Generates an FFmpeg command that converts the stored PNG files into the specified video format.
            %   The file name is the directory name w/ extension one level up. For example, an h264 format would
            %   map as follows:
            %
            %       input: experiments/test17/*.png
            %       output: experiments/test17.mp4
            %
            %   The delete directory flag appends a command that delete the input directory after a successful conversion.
            %
            %   Inputs:
            %       outputFormat - target format
            %       deleteDirectory - cleanup after conversion
            %
            %   Throws:
            %       Error on internal errors
            if nargin < 3
                deleteDirectory = false;
            end
            result = hebi_charts_native('hebi_charts_RecordingResult_getFfmpegCommand', this.ptr, hebi_charts.VideoOutputFormat.toNativeValue(outputFormat), deleteDirectory);
        end

        function result = runFfmpeg(this, outputFormat, deleteDirectory)
            % Runs an FFmpeg command that converts the stored PNG files into the specified video format.
            %   The file name is the directory name w/ extension one level up. For example, an h264 format would
            %   map as follows:
            %
            %       input: experiments/test17/*.png
            %       output: experiments/test17.mp4
            %
            %   The delete directory flag appends a command that delete the input directory after a successful conversion.
            %
            %   Inputs:
            %       outputFormat - target format
            %       deleteDirectory - cleanup after conversion
            %
            %   Throws:
            %       Error on internal errors
            if nargin < 3
                deleteDirectory = false;
            end
            result = hebi_charts_native('hebi_charts_RecordingResult_runFfmpeg', this.ptr, hebi_charts.VideoOutputFormat.toNativeValue(outputFormat), deleteDirectory);
        end

    end

    methods(Access = public, Hidden = true)
        function this = RecordingResult(ptr, varargin)
            if ~isnumeric(ptr) && ~isa(ptr, 'lib.pointer')
                error('RecordingResult constructor expects a C pointer type');
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
                hebi_charts_native('hebi_charts_RecordingResult_release', this.ptr);
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