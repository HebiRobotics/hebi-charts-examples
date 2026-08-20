classdef (Sealed) LineChart < hebi_charts.XYChart
% Represents an XY line chart

    methods

        function obj = addLine(this, label, varargin)
            % Creates a new line series
            %
            %   Inputs:
            %       label
            %
            %   Outputs:
            %       Represents a high-performance 2D line series optimized for real-time
            %   telemetry.
            %   Uses a double-buffered architecture with bounded rolling buffers to
            %   decouple high-frequency data ingestion from the UI rendering pulse.
            %
            %
            %   Throws:
            %       Error on internal errors
            ptr_ = hebi_charts_native('hebi_charts_LineChart_addLine', this.ref_LineChart, label);
            if isempty(ptr_)
                error('Failed to create hebi_charts.Line in LineChart.addLine');
            end
            obj = hebi_charts.Line(ptr_, varargin{:});
        end

    end

    methods(Access = public, Hidden = true)
        function this = LineChart(ref_LineChart, varargin)
            this@hebi_charts.XYChart(hebi_charts_native('hebi_charts_LineChart_to_XYChart', ref_LineChart));
            this.ref_LineChart = ref_LineChart;
            if numel(varargin) > 1 % Call setters from the leaf to make sure that the ref field is set
                this.set(varargin{:});
            end
        end
    end
    properties (SetAccess = private, GetAccess = private, Hidden = true)
        ref_LineChart = [];
    end

    methods(Access = public, Hidden = true)

        function ptr = c_ptr(this)
            ptr = this.ref_LineChart;
        end

    end

end