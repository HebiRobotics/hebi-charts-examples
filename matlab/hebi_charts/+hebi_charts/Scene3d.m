classdef (Sealed) Scene3d < handle & matlab.mixin.SetGet
% Represents a 3d scene that can render a variety of objects in 3d space

    properties (Dependent)
        gridBounds
        gridMaxX
        gridMaxY
        gridMaxZ
        gridMinX
        gridMinY
        gridMinZ
        gridStep
    end

    methods

        function set.gridBounds(this, gridBounds)
            % Sets the boundaries of the 3D grid cage in [m]
            %
            %   Inputs:
            %       minX - [m]
            %       maxX - [m]
            %       minY - [m]
            %       maxY - [m]
            %       minZ - [m]
            %       maxZ - [m]
            if numel(gridBounds) ~= 6
                error('Property "gridBounds" requires 6 elements');
            end
            if iscell(gridBounds)
                [minX, maxX, minY, maxY, minZ, maxZ] = gridBounds{:};
            else
                minX = gridBounds(1);
                maxX = gridBounds(2);
                minY = gridBounds(3);
                maxY = gridBounds(4);
                minZ = gridBounds(5);
                maxZ = gridBounds(6);
            end
            hebi_charts_native('hebi_charts_Scene3d_setGridBounds', this.ptr, minX, maxX, minY, maxY, minZ, maxZ);
        end

        function result = get.gridMaxX(this)
            % Gets the maximum X boundary of the 3D grid cage in [m]
            result = hebi_charts_native('hebi_charts_Scene3d_getMaxX', this.ptr);
        end

        function set.gridMaxX(this, val)
            % Sets the maximum X boundary of the 3D grid cage in [m]
            %
            %   Inputs:
            %       val
            hebi_charts_native('hebi_charts_Scene3d_setMaxX', this.ptr, val);
        end

        function result = get.gridMaxY(this)
            % Gets the maximum Y boundary of the 3D grid cage in [m]
            result = hebi_charts_native('hebi_charts_Scene3d_getMaxY', this.ptr);
        end

        function set.gridMaxY(this, val)
            % Sets the maximum Y boundary of the 3D grid cage in [m]
            %
            %   Inputs:
            %       val
            hebi_charts_native('hebi_charts_Scene3d_setMaxY', this.ptr, val);
        end

        function result = get.gridMaxZ(this)
            % Gets the maximum Z boundary of the 3D grid cage in [m]
            result = hebi_charts_native('hebi_charts_Scene3d_getMaxZ', this.ptr);
        end

        function set.gridMaxZ(this, val)
            % Sets the maximum Z boundary of the 3D grid cage in [m]
            %
            %   Inputs:
            %       val
            hebi_charts_native('hebi_charts_Scene3d_setMaxZ', this.ptr, val);
        end

        function result = get.gridMinX(this)
            % Gets the minimum X boundary of the 3D grid cage in [m]
            result = hebi_charts_native('hebi_charts_Scene3d_getMinX', this.ptr);
        end

        function set.gridMinX(this, val)
            % Sets the minimum X boundary of the 3D grid cage in [m]
            %
            %   Inputs:
            %       val
            hebi_charts_native('hebi_charts_Scene3d_setMinX', this.ptr, val);
        end

        function result = get.gridMinY(this)
            % Gets the minimum Y boundary of the 3D grid cage in [m]
            result = hebi_charts_native('hebi_charts_Scene3d_getMinY', this.ptr);
        end

        function set.gridMinY(this, val)
            % Sets the minimum Y boundary of the 3D grid cage in [m]
            %
            %   Inputs:
            %       val
            hebi_charts_native('hebi_charts_Scene3d_setMinY', this.ptr, val);
        end

        function result = get.gridMinZ(this)
            % Gets the minimum Z boundary of the 3D grid cage in [m]
            result = hebi_charts_native('hebi_charts_Scene3d_getMinZ', this.ptr);
        end

        function set.gridMinZ(this, val)
            % Sets the minimum Z boundary of the 3D grid cage in [m]
            %
            %   Inputs:
            %       val
            hebi_charts_native('hebi_charts_Scene3d_setMinZ', this.ptr, val);
        end

        function result = get.gridStep(this)
            % Gets the distance between grid lines in [m]
            result = hebi_charts_native('hebi_charts_Scene3d_getGridStep', this.ptr);
        end

        function set.gridStep(this, gridStep)
            % Sets the distance between grid lines in [m]
            %
            %   Inputs:
            %       gridStep - Distance in [m]
            hebi_charts_native('hebi_charts_Scene3d_setGridStep', this.ptr, gridStep);
        end

        function obj = getCamera(this)
            % Returns the camera of this 3d chart
            %
            %   Outputs:
            %       Represents a view point looking at a 3d scene
            ptr_ = hebi_charts_native('hebi_charts_Scene3d_getCamera', this.ptr);
            if isempty(ptr_)
                error('Failed to create hebi_charts.Camera in Scene3d.getCamera');
            end
            obj = hebi_charts.Camera(ptr_);
        end

        function obj = addRobot(this, pathOrUrl, varargin)
            % Adds a robot from a description file (.hrdf)
            %
            %   Inputs:
            %       pathOrUrl - file path or web-url to a description file
            %
            %   Outputs:
            %       Represents robot kinematics
            %
            %   Throws:
            %       Error on internal errors
            ptr_ = hebi_charts_native('hebi_charts_Scene3d_addRobot', this.ptr, pathOrUrl);
            if isempty(ptr_)
                error('Failed to create hebi_charts.Robot in Scene3d.addRobot');
            end
            obj = hebi_charts.Robot(ptr_, varargin{:});
        end

        function obj = addMesh(this, pathOrUrl, varargin)
            % Adds a 3d mesh from a file (.obj)
            %
            %   Inputs:
            %       pathOrUrl - file path or web-url to an .obj file
            %
            %   Outputs:
            %       Represents a static 3d mesh
            %
            %   Throws:
            %       Error on internal errors
            ptr_ = hebi_charts_native('hebi_charts_Scene3d_addMesh', this.ptr, pathOrUrl);
            if isempty(ptr_)
                error('Failed to create hebi_charts.Mesh in Scene3d.addMesh');
            end
            obj = hebi_charts.Mesh(ptr_, varargin{:});
        end

        function obj = addFrame(this, varargin)
            % Adds a triad that represents a right-handed coordinate frame
            %
            %   Inputs:
            %       lengthInMeters - length of each axis in [m]
            %
            %   Outputs:
            %       A triad that represents a frame
            %
            %   Throws:
            %       Error on internal errors
            lengthInMeters = 0.03;
            if ~isempty(varargin) && ~ischar(varargin{1}) && ~isstring(varargin{1})
                lengthInMeters = varargin{1};
                varargin(1) = [];
            end
            ptr_ = hebi_charts_native('hebi_charts_Scene3d_addFrame', this.ptr, lengthInMeters);
            if isempty(ptr_)
                error('Failed to create hebi_charts.Frame in Scene3d.addFrame');
            end
            obj = hebi_charts.Frame(ptr_, varargin{:});
        end

        function obj = addLine(this, varargin)
            % Adds a 3D data series rendered as a continuous line
            %
            %   Outputs:
            %       Represents a line in 3d space. Note that there are currently no
            %   line primitives, so the rendering is platform dependent and the
            %   performance is limited.
            %
            %
            %   Throws:
            %       Error on internal errors
            ptr_ = hebi_charts_native('hebi_charts_Scene3d_addLine', this.ptr);
            if isempty(ptr_)
                error('Failed to create hebi_charts.Line3d in Scene3d.addLine');
            end
            obj = hebi_charts.Line3d(ptr_, varargin{:});
        end

        function obj = addPoints(this, varargin)
            % Adds a 3D data series rendered as individual mesh objects
            %
            %   Outputs:
            %       Represents points in 3d space. Each point gets rendered as
            %   the specified marker shape. This is intended for markers
            %   and is not appropriate for large scale lidar point clouds.
            %
            %
            %   Throws:
            %       Error on internal errors
            ptr_ = hebi_charts_native('hebi_charts_Scene3d_addPoints', this.ptr);
            if isempty(ptr_)
                error('Failed to create hebi_charts.Points3d in Scene3d.addPoints');
            end
            obj = hebi_charts.Points3d(ptr_, varargin{:});
        end

    end

    methods(Access = public, Hidden = true)
        function this = Scene3d(ptr, varargin)
            if ~isa(ptr, 'uint64') || ~isscalar(ptr)
                error('Scene3d instances are created by the library');
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
                hebi_charts_native('hebi_charts_Scene3d_release', this.ptr);
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