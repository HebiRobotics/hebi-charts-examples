classdef (Sealed) Camera < handle & matlab.mixin.SetGet
% Represents a view point looking at a 3d scene

    methods

        function setView(this, view)
            % Sets the view point to a predefined standard view
            %
            %   Inputs:
            %       view
            hebi_charts_native('hebi_charts_Camera_setView', this.ptr, hebi_charts.CameraView.toNativeValue(view));
        end

        function reset(this)
            % Resets the camera orientation, zoom, and pan to defaults
            hebi_charts_native('hebi_charts_Camera_reset', this.ptr);
        end

        function applyRotation(this, qx, qy, qz, qw)
            % Applies an incremental rotation to the current camera view using a unit quaternion (x, y, z, w).
            %   This follows the ROS/REP-103 convention where the scalar component 'w' is last.
            %
            %   This rotation is multiplied by the current camera orientation.
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
            hebi_charts_native('hebi_charts_Camera_applyRotation', this.ptr, qx, qy, qz, qw);
        end

        function setDistance(this, distanceInMeters)
            % Sets the distance (zoom) from the camera to the pivot center. Out of range values are clamped.
            %
            %   Inputs:
            %       distanceInMeters - distance [m]
            %
            %   Throws:
            %       Error on internal errors
            hebi_charts_native('hebi_charts_Camera_setDistance', this.ptr, distanceInMeters);
        end

        function setPan(this, x, y, z)
            % Sets the 3D pan offset (panning) of the camera.
            %   This slides the entire scene relative to the camera view.
            %
            %   To center the camera on a specific object, use this to offset
            %   the world origin.
            %
            %   Inputs:
            %       x - position x [m]
            %       y - position y [m]
            %       z - position z [m]
            %
            %   Throws:
            %       Error on internal errors
            hebi_charts_native('hebi_charts_Camera_setPan', this.ptr, x, y, z);
        end

        function setControlsVisible(this, value)
            % Shows or hides the on-screen navigation UI controls (buttons/overlays).
            %
            %   Inputs:
            %       value
            hebi_charts_native('hebi_charts_Camera_setControlsVisible', this.ptr, value);
        end

    end

    methods(Access = public, Hidden = true)
        function this = Camera(ptr, varargin)
            if ~isa(ptr, 'uint64') || ~isscalar(ptr)
                error('Camera instances are created by the library');
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
                hebi_charts_native('hebi_charts_Camera_release', this.ptr);
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