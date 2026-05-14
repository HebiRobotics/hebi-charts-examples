classdef (Sealed) HdrHistogramRecorder < handle & matlab.mixin.SetGet
% Manages background logging of high-dynamic-range histograms to disk.

    properties (Dependent)
        frequency
        max
        min
        significantDigits
    end

    methods

        function result = get.frequency(this)
            result = hebi_charts_native('hebi_charts_HdrHistogramRecorder_getFrequency', this.ptr);
        end

        function set.frequency(this, frequency)
            % Sets the logging frequency (Hz) for background file updates.
            %
            %   Inputs:
            %       frequency - hz
            %
            %   Throws:
            %       Error on internal errors
            hebi_charts_native('hebi_charts_HdrHistogramRecorder_setFrequency', this.ptr, frequency);
            if status_ ~= 0
                error(['Encountered error in HdrHistogramRecorder.set.frequency']);
            end
        end

        function result = get.max(this)
            result = hebi_charts_native('hebi_charts_HdrHistogramRecorder_getMax', this.ptr);
        end

        function set.max(this, value)
            % Sets the highest trackable value [s]. Values above this will throw an error.
            %
            %   Inputs:
            %       value - seconds
            %
            %   Throws:
            %       Error on internal errors
            hebi_charts_native('hebi_charts_HdrHistogramRecorder_setMax', this.ptr, value);
            if status_ ~= 0
                error(['Encountered error in HdrHistogramRecorder.set.max']);
            end
        end

        function result = get.min(this)
            result = hebi_charts_native('hebi_charts_HdrHistogramRecorder_getMin', this.ptr);
        end

        function set.min(this, value)
            % Sets the lowest trackable value [s]. Values below this will be rounded up.
            %
            %   Inputs:
            %       value - seconds
            %
            %   Throws:
            %       Error on internal errors
            hebi_charts_native('hebi_charts_HdrHistogramRecorder_setMin', this.ptr, value);
            if status_ ~= 0
                error(['Encountered error in HdrHistogramRecorder.set.min']);
            end
        end

        function result = get.significantDigits(this)
            result = hebi_charts_native('hebi_charts_HdrHistogramRecorder_getSignificantDigits', this.ptr);
        end

        function set.significantDigits(this, significantDigits)
            % Sets the number of significant decimal digits to maintain (1-5).
            %
            %   Inputs:
            %       significantDigits
            %
            %   Throws:
            %       Error on internal errors
            hebi_charts_native('hebi_charts_HdrHistogramRecorder_setSignificantDigits', this.ptr, significantDigits);
            if status_ ~= 0
                error(['Encountered error in HdrHistogramRecorder.set.significantDigits']);
            end
        end

        function obj = addTrace(this, tag)
            % Creates a new single-writer trace that gets recorded in intervals.
            %
            %   Inputs:
            %       tag
            ptr_ = hebi_charts_native('hebi_charts_HdrHistogramRecorder_addTrace', this.ptr, tag);
            if isempty(ptr_)
                error(['Failed to create hebi_charts.HdrHistogramTrace in HdrHistogramRecorder.addTrace']);
            end
            obj = hebi_charts.HdrHistogramTrace(ptr_);
        end

        function result = startRecording(this, logFile)
            % Starts the background recording thread. Returns the absolute normalized path.
            %
            %   Inputs:
            %       logFile - Path to the .hlog file
            %
            %   Throws:
            %       Error on internal errors
            result = hebi_charts_native('hebi_charts_HdrHistogramRecorder_startRecording', this.ptr, logFile);
        end

        function stopRecording(this)
            % Stops the recording thread.
            hebi_charts_native('hebi_charts_HdrHistogramRecorder_stopRecording', this.ptr);
        end

    end

    methods(Access = public)
        function this = HdrHistogramRecorder(varargin)
            ptr_ = hebi_charts_native('hebi_charts_HdrHistogramRecorder_create');
            if isempty(ptr_)
                error(['Failed to create hebi_charts.HdrHistogramRecorder in HdrHistogramRecorder.HdrHistogramRecorder']);
            end
            this.ptr = ptr_;
            if numel(varargin) > 0
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
                hebi_charts_native('hebi_charts_HdrHistogramRecorder_release', this.ptr);
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