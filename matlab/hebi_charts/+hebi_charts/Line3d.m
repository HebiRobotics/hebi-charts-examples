classdef (Sealed) Line3d < hebi_charts.Series3d
% Represents a line in 3d space. Note that there are currently no
%   line primitives, so the rendering is platform dependent and the
%   performance is limited.

    methods

        function setMaxPointCount(this, count)
            % Sets the internal maximum point count for incrementally
            %   adding points. May clear existing data.
            %
            %   Inputs:
            %       count - maximum number of points
            hebi_charts_native('hebi_charts_Line3d_setMaxPointCount', this.ref_Line3d, count);
        end

        function clear(this)
            % Clears all existing data
            hebi_charts_native('hebi_charts_Line3d_clear', this.ref_Line3d);
        end

        function setData(this, x, y, z, length)
            % Replaces the entire dataset with the provided X/Y/Z content. This
            %   operation copies the input data, so the caller retains ownership of
            %   the memory. Sets the buffer capacity to match the input length and
            %   clears any previous rolling history.
            %
            %   Inputs:
            %       x - points
            %       y - points
            %       z - points
            %       length - number of x/y/z points
            if nargin < 5
                length = min(numel(x),numel(y),numel(z));
            end
            hebi_charts_native('hebi_charts_Line3d_setData', this.ref_Line3d, x, y, z, length);
        end

        function addPoints(this, x, y, z, length)
            % Appends multiple data points to the end of the internal rolling buffer.
            %   This operation copies the input data, so the caller retains ownership
            %   of the memory. If the total number of points exceeds the current capacity,
            %   the oldest points are overwritten.
            %
            %   Inputs:
            %       x - points
            %       y - points
            %       z - points
            %       length - number of x/y/z points
            if nargin < 5
                length = min(numel(x),numel(y),numel(z));
            end
            hebi_charts_native('hebi_charts_Line3d_addPoints', this.ref_Line3d, x, y, z, length);
        end

        function addPoint(this, x, y, z)
            % Adds one point to an internal rolling buffer. Once the maximum
            %   point count is reached, it will overwrite the earliest data.
            %
            %   Inputs:
            %       x
            %       y
            %       z
            hebi_charts_native('hebi_charts_Line3d_addPoint', this.ref_Line3d, x, y, z);
        end

    end

    methods(Access = public, Hidden = true)
        function this = Line3d(ref_Line3d, varargin)
            this@hebi_charts.Series3d(hebi_charts_native('hebi_charts_Line3d_to_Series3d', ref_Line3d));
            this.ref_Line3d = ref_Line3d;
            if numel(varargin) > 1 % Call setters from the leaf to make sure that the ref field is set
                this.set(varargin{:});
            end
        end
    end
    properties (SetAccess = private, GetAccess = private, Hidden = true)
        ref_Line3d = [];
    end

    methods(Access = public, Hidden = true)

        function ptr = c_ptr(this)
            ptr = this.ref_Line3d;
        end

    end

end