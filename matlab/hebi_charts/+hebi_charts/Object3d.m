classdef Object3d < handle & matlab.mixin.SetGet
% Represents a generic 3d object that can be rotated and translated

    properties (Dependent)
        visible
    end

    methods

        function result = get.visible(this)
            % Gets the current visibility state of this node
            result = hebi_charts_native('hebi_charts_Object3d_isVisible', this.ptr);
        end

        function set.visible(this, visible)
            % Sets visibility for this object. Hidden objects are not removed from the SceneGraph
            %
            %   Inputs:
            %       visible
            hebi_charts_native('hebi_charts_Object3d_setVisible', this.ptr, visible);
        end

        function setOrientation(this, qx, qy, qz, qw)
            % Sets the orientation of the object using a unit quaternion (x, y, z, w).
            %   This follows the ROS/REP-103 convention where the scalar component 'w' is last.
            %   The input is not verified!
            %
            %   Inputs:
            %       qx - quaternion x (vector part)
            %       qy - quaternion y (vector part)
            %       qz - quaternion z (vector part)
            %       qw - quaternion w (scalar part)
            %
            %   Throws:
            %       Error on internal errors
            status_ = hebi_charts_native('hebi_charts_Object3d_setOrientation', this.ptr, qx, qy, qz, qw);
            if status_ ~= 0
                error(['Encountered error in Object3d.setOrientation' ': ' hebi_charts_native('hebi_charts_Runtime_getLastErrorString')]);
            end
        end

        function setOrientationRPY(this, roll, pitch, yaw)
            % Sets the orientation of the object using Roll, Pitch, and Yaw (radians).
            %   Follows the ROS/REP-103 convention (Extrinsic / Fixed-Axis XYZ):
            %
            %       orientation = Rz(yaw)*Ry(pitch)*Rx(roll)
            %
            %   This method preserves the current translation. The input is not verified!
            %
            %   Inputs:
            %       roll - angle in [rad]
            %       pitch - angle in [rad]
            %       yaw - angle in [rad]
            %
            %   Throws:
            %       Error on internal errors
            status_ = hebi_charts_native('hebi_charts_Object3d_setOrientationRPY', this.ptr, roll, pitch, yaw);
            if status_ ~= 0
                error(['Encountered error in Object3d.setOrientationRPY' ': ' hebi_charts_native('hebi_charts_Runtime_getLastErrorString')]);
            end
        end

        function setTranslation(this, x, y, z)
            % Translates the object. The input is not verified!
            %
            %   Inputs:
            %       x - position x [m]
            %       y - position y [m]
            %       z - position z [m]
            %
            %   Throws:
            %       Error on internal errors
            status_ = hebi_charts_native('hebi_charts_Object3d_setTranslation', this.ptr, x, y, z);
            if status_ ~= 0
                error(['Encountered error in Object3d.setTranslation' ': ' hebi_charts_native('hebi_charts_Runtime_getLastErrorString')]);
            end
        end

        function setPose(this, x, y, z, qx, qy, qz, qw)
            % Sets the full pose (position and orientation) of the object in a
            %   single atomic update. This follows the ROS/REP-103 convention
            %   (Position + Quaternion).
            %
            %   Units: Translation in [m], Quaternion (x, y, z, w).
            %   The input is not verified!
            %
            %   Inputs:
            %       x - position x [m]
            %       y - position y [m]
            %       z - position z [m]
            %       qx - quaternion x (vector part)
            %       qy - quaternion y (vector part)
            %       qz - quaternion z (vector part)
            %       qw - quaternion w (scalar part)
            %
            %   Throws:
            %       Error on internal errors
            status_ = hebi_charts_native('hebi_charts_Object3d_setPose', this.ptr, x, y, z, qx, qy, qz, qw);
            if status_ ~= 0
                error(['Encountered error in Object3d.setPose' ': ' hebi_charts_native('hebi_charts_Runtime_getLastErrorString')]);
            end
        end

        function setTransform4x4(this, matrix, ordering)
            % Sets a 4x4 transform matrix of the form
            %
            %       R R R x
            %       R R R y
            %       R R R z
            %       0 0 0 1
            %
            %   The transform needs to be of size=16 and include the
            %   bottom row. The translation units are in meters.
            %   The input is not verified.
            %
            %   Inputs:
            %       matrix - pointer to 16 double elements
            %       ordering - corresponding memory layout of the 4x4 matrix
            %
            %   Throws:
            %       Error on internal errors
            if ~isequal(size(matrix), [4 4])
                error('expected 4x4 matrix');
            end
            if nargin < 3
                ordering = 'ColumnMajor';
            end
            status_ = hebi_charts_native('hebi_charts_Object3d_setTransform4x4', this.ptr, matrix, hebi_charts.MatrixOrdering.toNativeValue(ordering));
            if status_ ~= 0
                error(['Encountered error in Object3d.setTransform4x4' ': ' hebi_charts_native('hebi_charts_Runtime_getLastErrorString')]);
            end
        end

    end

    methods(Access = public, Hidden = true)
        function this = Object3d(ptr, varargin)
            if ~isnumeric(ptr) && ~isa(ptr, 'lib.pointer')
                error('Object3d constructor expects a C pointer type');
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
                hebi_charts_native('hebi_charts_Object3d_release', this.ptr);
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