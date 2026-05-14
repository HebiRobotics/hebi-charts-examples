classdef (Sealed) Robot < hebi_charts.Object3d
% Represents robot kinematics

    properties (Dependent)
        displayStyle
        dof
    end

    methods

        function result = get.displayStyle(this)
            % Gets the current visual display style (Original, Ghosted, etc.)
            result = hebi_charts_native('hebi_charts_Robot_getDisplayStyle', this.ref_Robot);
        end

        function set.displayStyle(this, style)
            % Updates the visual representation of the robot model.
            %
            %   This is typically used to visually distinguish between multiple states of
            %   the same robot, such as overlaying a semi-transparent 'Ghosted' target
            %   pose over the 'Original' pose.
            %
            %   Inputs:
            %       style - sets the display style
            hebi_charts_native('hebi_charts_Robot_setDisplayStyle', this.ref_Robot, hebi_charts.DisplayStyle.toNativeValue(style));
        end

        function result = get.dof(this)
            % Gets the number of joints (Degrees of Freedom) of this robot
            result = hebi_charts_native('hebi_charts_Robot_getDof', this.ref_Robot);
        end

        function setPositions(this, positions, length)
            % Updates the robot model configuration (kinematics) using a vector of joint positions.
            %
            %   Units:
            %     - Revolute joints: [rad]
            %     - Prismatic joints: [m]
            %
            %   The order of the vector must match the joint definitions in the underlying model.
            %   The 'length' parameter must exactly match the number of degrees of freedom (DOF)
            %   returned by getDof().
            %
            %   Inputs:
            %       positions - pointer to an array of joint positions
            %       length - number of joints (must match getDof)
            %
            %   Throws:
            %       Error if position vector length does not match number of joints.
            if nargin < 3
                length = numel(positions);
            end
            status_ = hebi_charts_native('hebi_charts_Robot_setPositions', this.ref_Robot, positions, length);
            if status_ ~= 0
                error(['Encountered error in Robot.setPositions' ': ' hebi_charts_native('hebi_charts_Runtime_getLastErrorString')]);
            end
        end

    end

    methods(Access = public, Hidden = true)
        function this = Robot(ref_Robot, varargin)
            this@hebi_charts.Object3d(hebi_charts_native('hebi_charts_Robot_to_Object3d', ref_Robot));
            this.ref_Robot = ref_Robot;
            if numel(varargin) > 1 % Call setters from the leaf to make sure that the ref field is set
                this.set(varargin{:});
            end
        end
    end
    properties (SetAccess = private, GetAccess = private, Hidden = true)
        ref_Robot = [];
    end

    methods(Access = public, Hidden = true)

        function ptr = c_ptr(this)
            ptr = this.ref_Robot;
        end

    end

end