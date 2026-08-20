classdef (Sealed) LatencyChart < hebi_charts.XYChart
% Shows latency measurements in HdrHistogram percentile format

    methods

        function obj = addTrace(this, name, varargin)
            % Creates a new hdr histogram dataset
            %
            %   Inputs:
            %       name
            %
            %   Outputs:
            %       Represents a latency measurement that records latency values in the form of an HdrHistogram
            %
            %   Throws:
            %       Error on internal errors
            ptr_ = hebi_charts_native('hebi_charts_LatencyChart_addTrace', this.ref_LatencyChart, name);
            if isempty(ptr_)
                error('Failed to create hebi_charts.LatencyTrace in LatencyChart.addTrace');
            end
            obj = hebi_charts.LatencyTrace(ptr_, varargin{:});
        end

    end

    methods(Access = public, Hidden = true)
        function this = LatencyChart(ref_LatencyChart, varargin)
            this@hebi_charts.XYChart(hebi_charts_native('hebi_charts_LatencyChart_to_XYChart', ref_LatencyChart));
            this.ref_LatencyChart = ref_LatencyChart;
            if numel(varargin) > 1 % Call setters from the leaf to make sure that the ref field is set
                this.set(varargin{:});
            end
        end
    end
    properties (SetAccess = private, GetAccess = private, Hidden = true)
        ref_LatencyChart = [];
    end

    methods(Access = public, Hidden = true)

        function ptr = c_ptr(this)
            ptr = this.ref_LatencyChart;
        end

    end

end