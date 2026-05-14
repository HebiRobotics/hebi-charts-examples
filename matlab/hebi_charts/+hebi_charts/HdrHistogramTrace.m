classdef (Sealed) HdrHistogramTrace < handle & matlab.mixin.SetGet
% A wait-free single-writer HdrHistogram record

    methods(Static)

        function obj = createLocal(numberOfSignificantDigits, minSeconds, maxSeconds)
            % Creates an unattached and untagged histogram. The local version removes
            %   all synchronization overhead, but is not thread-safe.
            %
            %   Inputs:
            %       numberOfSignificantDigits
            %       minSeconds
            %       maxSeconds
            if nargin < 1
                numberOfSignificantDigits = 2;
            end
            if nargin < 2
                minSeconds = 1e-9;
            end
            if nargin < 3
                maxSeconds = 3600;
            end
            ptr_ = hebi_charts_native('hebi_charts_HdrHistogramTrace_createLocal', numberOfSignificantDigits, minSeconds, maxSeconds);
            if isempty(ptr_)
                error(['Failed to create hebi_charts.HdrHistogramTrace in HdrHistogramTrace.createLocal']);
            end
            obj = hebi_charts.HdrHistogramTrace(ptr_);
        end

    end

    properties (Dependent)
        max
        mean
        min
        stdDeviation
        tag
        totalCount
    end

    methods

        function result = get.max(this)
            % Maximum recorded value in [s].
            result = hebi_charts_native('hebi_charts_HdrHistogramTrace_getMax', this.ptr);
        end

        function result = get.mean(this)
            % Arithmetic mean of recorded values in [s].
            result = hebi_charts_native('hebi_charts_HdrHistogramTrace_getMean', this.ptr);
        end

        function result = get.min(this)
            % Minimum recorded value in [s].
            result = hebi_charts_native('hebi_charts_HdrHistogramTrace_getMin', this.ptr);
        end

        function result = get.stdDeviation(this)
            % Standard deviation of recorded values in [s].
            result = hebi_charts_native('hebi_charts_HdrHistogramTrace_getStdDeviation', this.ptr);
        end

        function result = get.tag(this)
            % The internal tag
            result = hebi_charts_native('hebi_charts_HdrHistogramTrace_getTag', this.ptr);
        end

        function result = get.totalCount(this)
            % The total number of samples recorded.
            result = hebi_charts_native('hebi_charts_HdrHistogramTrace_getTotalCount', this.ptr);
        end

        function result = getValueAtPercentile(this, percentile)
            % Returns the value at a specific percentile (0-100) in [s].
            %
            %   Inputs:
            %       percentile
            result = hebi_charts_native('hebi_charts_HdrHistogramTrace_getValueAtPercentile', this.ptr, percentile);
        end

        function tic(this)
            % Sets a timestamp for subsequent toc calls
            hebi_charts_native('hebi_charts_HdrHistogramTrace_tic', this.ptr);
        end

        function result = toc(this)
            % Records the elapsed time since the last tic call. Returns the recorded value in [s]
            result = hebi_charts_native('hebi_charts_HdrHistogramTrace_toc', this.ptr);
        end

        function result = ticToc(this)
            % Calls tic and toc in one call.  Returns the recorded value in [s]
            result = hebi_charts_native('hebi_charts_HdrHistogramTrace_ticToc', this.ptr);
        end

        function recordValue(this, value)
            % Records a single latency value in seconds. Values outside the
            %   min/max range are clamped. Returns the recorded value in [s]
            %
            %   Inputs:
            %       value - latency in [s]
            hebi_charts_native('hebi_charts_HdrHistogramTrace_recordValue', this.ptr, value);
        end

        function recordValueWithCount(this, value, count)
            % Records a latency value in seconds with a specific occurrence count.
            %
            %   Inputs:
            %       value - latency in [s]
            %       count - occurrence count
            hebi_charts_native('hebi_charts_HdrHistogramTrace_recordValueWithCount', this.ptr, value, count);
        end

        function recordValueWithExpectedInterval(this, value, expectedIntervalBetweenSamples)
            % Records a value in seconds with Coordinated Omission compensation.
            %   If the value is larger than the expected interval, additional samples
            %   are auto-generated to fill the gap.
            %
            %   Inputs:
            %       value - latency in [s]
            %       expectedIntervalBetweenSamples - expected interval in [s]
            hebi_charts_native('hebi_charts_HdrHistogramTrace_recordValueWithExpectedInterval', this.ptr, value, expectedIntervalBetweenSamples);
        end

        function reset(this)
            % Reset the contents and stats of this histogram
            hebi_charts_native('hebi_charts_HdrHistogramTrace_reset', this.ptr);
        end

        function result = toHgrmString(this, outputUnitsPerSecond)
            % A string of the percentile distribution for plotting .hgrm files
            %
            %   Inputs:
            %       outputUnitsPerSecond - output scale (ms=1e3, us=1e6, ns=1e9
            if nargin < 2
                outputUnitsPerSecond = 1e6;
            end
            result = hebi_charts_native('hebi_charts_HdrHistogramTrace_toHgrmString', this.ptr, outputUnitsPerSecond);
        end

        function result = saveAsHgrm(this, fileName, outputUnitsPerSecond)
            % Saves the percentile distribution as an .hgrm file in the desired output units. This
            %   can be loaded into standard hgrm plotting tools. Returns the absolute path to the output.
            %
            %   Inputs:
            %       fileName
            %       outputUnitsPerSecond - output scale (ms=1e3, us=1e6, ns=1e9
            %
            %   Throws:
            %       Error on internal errors
            if nargin < 3
                outputUnitsPerSecond = 1e6;
            end
            result = hebi_charts_native('hebi_charts_HdrHistogramTrace_saveAsHgrm', this.ptr, fileName, outputUnitsPerSecond);
        end

    end

    methods(Access = public, Hidden = true)
        function this = HdrHistogramTrace(ptr, varargin)
            if ~isnumeric(ptr) && ~isa(ptr, 'lib.pointer')
                error('HdrHistogramTrace constructor expects a C pointer type');
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
                hebi_charts_native('hebi_charts_HdrHistogramTrace_release', this.ptr);
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