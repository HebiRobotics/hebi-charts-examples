classdef (Sealed) Points3d < hebi_charts.Series3d
% Represents points in 3d space. Each point gets rendered as
%   the specified marker shape. This is intended for markers
%   and is not appropriate for large scale lidar point clouds.

    properties (Dependent)
        markerShape
        markerSize
        selfIllumination
        vertexSharing
    end

    methods

        function result = get.markerShape(this)
            % Gets the geometry used to represent each point in the series
            result = hebi_charts_native('hebi_charts_Points3d_getMarkerShape', this.ref_Points3d);
        end

        function set.markerShape(this, shape)
            % Sets the geometry used to represent each point in the series
            %
            %   Inputs:
            %       shape
            hebi_charts_native('hebi_charts_Points3d_setMarkerShape', this.ref_Points3d, hebi_charts.MarkerShape.toNativeValue(shape));
        end

        function result = get.markerSize(this)
            % Gets the characteristic size (diameter or side length) of the markers in [m]
            result = hebi_charts_native('hebi_charts_Points3d_getMarkerSize', this.ref_Points3d);
        end

        function set.markerSize(this, value)
            % Sets the characteristic size (diameter or side length) of the markers in [m]
            %
            %   Inputs:
            %       value - [m]
            hebi_charts_native('hebi_charts_Points3d_setMarkerSize', this.ref_Points3d, value);
        end

        function result = get.selfIllumination(this)
            % Gets the current self illumination state
            result = hebi_charts_native('hebi_charts_Points3d_isSelfIllumination', this.ref_Points3d);
        end

        function set.selfIllumination(this, value)
            % Self illumination makes the shapes glow without an external light source (defaults to true)
            %
            %   Inputs:
            %       value
            hebi_charts_native('hebi_charts_Points3d_setSelfIllumination', this.ref_Points3d, value);
        end

        function result = get.vertexSharing(this)
            % Vertex sharing reduces the complexity, but can result in poor lighting
            result = hebi_charts_native('hebi_charts_Points3d_isVertexSharing', this.ref_Points3d);
        end

        function set.vertexSharing(this, value)
            % Vertex sharing reduces the complexity, but can result in poor lighting
            %
            %   Inputs:
            %       value
            hebi_charts_native('hebi_charts_Points3d_setVertexSharing', this.ref_Points3d, value);
        end

        function setMaxPointCount(this, count)
            % Sets the internal maximum point count for incrementally
            %   adding points. May clear existing data.
            %
            %   Inputs:
            %       count - maximum number of points
            hebi_charts_native('hebi_charts_Points3d_setMaxPointCount', this.ref_Points3d, count);
        end

        function clear(this)
            % Clears all existing data
            hebi_charts_native('hebi_charts_Points3d_clear', this.ref_Points3d);
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
            hebi_charts_native('hebi_charts_Points3d_setData', this.ref_Points3d, x, y, z, length);
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
            hebi_charts_native('hebi_charts_Points3d_addPoints', this.ref_Points3d, x, y, z, length);
        end

        function addPoint(this, x, y, z)
            % Adds one point to an internal rolling buffer. Once the maximum
            %   point count is reached, it will overwrite the earliest data.
            %
            %   Inputs:
            %       x
            %       y
            %       z
            hebi_charts_native('hebi_charts_Points3d_addPoint', this.ref_Points3d, x, y, z);
        end

    end

    methods(Access = public, Hidden = true)
        function this = Points3d(ref_Points3d, varargin)
            this@hebi_charts.Series3d(hebi_charts_native('hebi_charts_Points3d_to_Series3d', ref_Points3d));
            this.ref_Points3d = ref_Points3d;
            if numel(varargin) > 1 % Call setters from the leaf to make sure that the ref field is set
                this.set(varargin{:});
            end
        end
    end
    properties (SetAccess = private, GetAccess = private, Hidden = true)
        ref_Points3d = [];
    end

    methods(Access = public, Hidden = true)

        function ptr = c_ptr(this)
            ptr = this.ref_Points3d;
        end

    end

end