classdef (Sealed) LatencyTrace < hebi_charts.XYSeries
% Represents a latency measurement that records latency values in the form of an HdrHistogram

    methods

        function tic(this)
            % Sets a timestamp for subsequent toc calls
            hebi_charts_native('hebi_charts_LatencyTrace_tic', this.ref_LatencyTrace);
        end

        function result = toc(this)
            % Records the elapsed time since the last tic call. Returns the recorded value in [s]
            result = hebi_charts_native('hebi_charts_LatencyTrace_toc', this.ref_LatencyTrace);
        end

        function result = ticToc(this)
            % Calls tic and toc in one call. Returns the recorded value in [s]
            result = hebi_charts_native('hebi_charts_LatencyTrace_ticToc', this.ref_LatencyTrace);
        end

        function record(this, value)
            % Record a latency value in the histogram
            %
            %   Inputs:
            %       value - the value to be recorded in [s]
            hebi_charts_native('hebi_charts_LatencyTrace_record', this.ref_LatencyTrace, value);
        end

        function recordWithCount(this, value, count)
            % Record a value in the histogram (adding to the value's current count)
            %
            %   Inputs:
            %       value - the value to be recorded in [s]
            %       count - the number of occurrences of this value to record
            hebi_charts_native('hebi_charts_LatencyTrace_recordWithCount', this.ref_LatencyTrace, value, count);
        end

        function recordCompensated(this, value, expectedIntervalBetweenValueSamples)
            % [EXPERIMENTAL - specific to HdrHistogram]
            %   Record a value in the histogram.
            %   To compensate for the loss of sampled values when a recorded value is larger than the expected interval
            %   between value samples, Histogram will auto-generate an additional series of decreasingly-smaller
            %   (down to the expectedIntervalBetweenValueSamples) value records.
            %
            %   Inputs:
            %       value - the value to be recorded in [s]
            %       expectedIntervalBetweenValueSamples - If expectedIntervalBetweenValueSamples in [s] is larger than
            %   zero, an auto-generated value records as appropriate if value
            %   is larger than expectedIntervalBetweenValueSamples
            hebi_charts_native('hebi_charts_LatencyTrace_recordCompensated', this.ref_LatencyTrace, value, expectedIntervalBetweenValueSamples);
        end

        function reset(this)
            hebi_charts_native('hebi_charts_LatencyTrace_reset', this.ref_LatencyTrace);
        end

    end

    methods(Access = public, Hidden = true)
        function this = LatencyTrace(ref_LatencyTrace, varargin)
            this@hebi_charts.XYSeries(hebi_charts_native('hebi_charts_LatencyTrace_to_XYSeries', ref_LatencyTrace));
            this.ref_LatencyTrace = ref_LatencyTrace;
            if numel(varargin) > 1 % Call setters from the leaf to make sure that the ref field is set
                this.set(varargin{:});
            end
        end
    end
    properties (SetAccess = private, GetAccess = private, Hidden = true)
        ref_LatencyTrace = [];
    end

    methods(Access = public, Hidden = true)

        function ptr = c_ptr(this)
            ptr = this.ref_LatencyTrace;
        end

    end

end