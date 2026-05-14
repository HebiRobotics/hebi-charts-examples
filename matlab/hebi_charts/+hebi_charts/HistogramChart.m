classdef (Sealed) HistogramChart < hebi_charts.XYChart
% Shows HdrHistogram percentile data for latency measurements

    methods

        function obj = addTrace(this, name, varargin)
            % Creates a new hdr histogram dataset
            %
            %   Inputs:
            %       name
            %
            %   Throws:
            %       Error on internal errors
            ptr_ = hebi_charts_native('hebi_charts_HistogramChart_addTrace', this.ref_HistogramChart, name);
            if isempty(ptr_)
                error(['Failed to create hebi_charts.HistogramTrace in HistogramChart.addTrace' ': ' hebi_charts_native('hebi_charts_Runtime_getLastErrorString')]);
            end
            obj = hebi_charts.HistogramTrace(ptr_, varargin{:});
        end

    end

    methods(Access = public, Hidden = true)
        function this = HistogramChart(ref_HistogramChart, varargin)
            this@hebi_charts.XYChart(hebi_charts_native('hebi_charts_HistogramChart_to_XYChart', ref_HistogramChart));
            this.ref_HistogramChart = ref_HistogramChart;
            if numel(varargin) > 1 % Call setters from the leaf to make sure that the ref field is set
                this.set(varargin{:});
            end
        end
    end
    properties (SetAccess = private, GetAccess = private, Hidden = true)
        ref_HistogramChart = [];
    end

    methods(Access = public, Hidden = true)

        function ptr = c_ptr(this)
            ptr = this.ref_HistogramChart;
        end

    end

end