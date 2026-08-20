classdef (Sealed) Line < hebi_charts.XYSeries
% Represents a high-performance 2D line series optimized for real-time
%   telemetry.
%   Uses a double-buffered architecture with bounded rolling buffers to
%   decouple high-frequency data ingestion from the UI rendering pulse.

    properties (Dependent)
        maxPointCount
    end

    methods

        function result = get.maxPointCount(this)
            % Returns the maximum number of points this line can hold before rolling
            result = hebi_charts_native('hebi_charts_Line_getMaxPointCount', this.ref_Line);
        end

        function set.maxPointCount(this, count)
            % Sets the maximum number of points the rolling buffer can hold.
            %   Note: Changing the capacity clears all existing data. If the
            %   new capacity matches the current value, this operation is a
            %   no-op and data is preserved.
            %
            %   Inputs:
            %       count - maximum number of points
            hebi_charts_native('hebi_charts_Line_setMaxPointCount', this.ref_Line, count);
        end

        function clear(this)
            % Clears all existing data
            hebi_charts_native('hebi_charts_Line_clear', this.ref_Line);
        end

        function setData(this, x, y)
            % Replaces the entire dataset with the provided X and Y content. This
            %   operation copies the input data, so the caller retains ownership of
            %   the memory. Sets the buffer capacity to match the input length and
            %   clears any previous rolling history. Mismatched input lengths get
            %   truncated to the shorter one.
            %
            %   Inputs:
            %       x - points
            %       y - points
            hebi_charts_native('hebi_charts_Line_setData', this.ref_Line, x, y);
        end

        function addPoints(this, x, y)
            % Appends multiple data points to the end of the internal rolling buffer.
            %   This operation copies the input data, so the caller retains ownership
            %   of the memory. If the total number of points exceeds the current capacity,
            %   the oldest points are overwritten. Mismatched input lengths get
            %   truncated to the shorter one.
            %
            %   Inputs:
            %       x - points
            %       y - points
            hebi_charts_native('hebi_charts_Line_addPoints', this.ref_Line, x, y);
        end

        function addPoint(this, x, y)
            % Adds one point to an internal rolling buffer. Once the maximum
            %   point count is reached, it will overwrite the earliest data.
            %
            %   Inputs:
            %       x
            %       y
            hebi_charts_native('hebi_charts_Line_addPoint', this.ref_Line, x, y);
        end

    end

    methods(Access = public, Hidden = true)
        function this = Line(ref_Line, varargin)
            this@hebi_charts.XYSeries(hebi_charts_native('hebi_charts_Line_to_XYSeries', ref_Line));
            this.ref_Line = ref_Line;
            if numel(varargin) > 1 % Call setters from the leaf to make sure that the ref field is set
                this.set(varargin{:});
            end
        end
    end
    properties (SetAccess = private, GetAccess = private, Hidden = true)
        ref_Line = [];
    end

    methods(Access = public, Hidden = true)

        function ptr = c_ptr(this)
            ptr = this.ref_Line;
        end

    end

end