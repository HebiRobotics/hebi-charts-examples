classdef (Sealed) Mesh < hebi_charts.Object3d
% Represents a static 3d mesh

    properties (Dependent)
        centered
        scale
        style
    end

    methods

        function result = get.centered(this)
            % Returns whether the origin is moved to the center of the mesh
            result = hebi_charts_native('hebi_charts_Mesh_isCentered', this.ref_Mesh);
        end

        function set.centered(this, centered)
            % Moves the origin to the center of the mesh.
            %
            %   Inputs:
            %       centered
            hebi_charts_native('hebi_charts_Mesh_setCentered', this.ref_Mesh, centered);
        end

        function result = get.scale(this)
            % Gets the current scaling factor
            result = hebi_charts_native('hebi_charts_Mesh_getScale', this.ref_Mesh);
        end

        function set.scale(this, scaleUnitsToMillimeters)
            % Sets the scaling factor applied to the mesh. The internal
            %   units are mm, so a mesh in meters would need to be scaled
            %   by 1e-3 to render correctly.
            %
            %   Inputs:
            %       scaleUnitsToMillimeters
            hebi_charts_native('hebi_charts_Mesh_setScale', this.ref_Mesh, scaleUnitsToMillimeters);
        end

        function result = get.style(this)
            % Gets the current display style (Original, Ghosted, etc.)
            result = hebi_charts_native('hebi_charts_Mesh_getDisplayStyle', this.ref_Mesh);
        end

        function set.style(this, style)
            % Updates the visual representation of the mesh model.
            %
            %   This is typically used to visually distinguish between multiple states of
            %   the same mesh, such as overlaying a semi-transparent 'Ghosted' target
            %   pose over the 'Original' pose.
            %
            %   Inputs:
            %       style - sets the display style
            hebi_charts_native('hebi_charts_Mesh_setDisplayStyle', this.ref_Mesh, hebi_charts.DisplayStyle.toNativeValue(style));
        end

        function setMeshTransform4x4(this, matrix)
            % Sets a fixed mesh-to-object pre-transform as a 4x4 matrix of the form
            %
            %       R R R x
            %       R R R y
            %       R R R z
            %       0 0 0 1
            %
            %   The composition order is
            %
            %       rendered = objectPose * meshTransform * (centered and scaled mesh)
            %
            %   so the pre-transform is meant to be set once after loading to correct
            %   for the frame the mesh was exported in (e.g. rotating a Y-up mesh to
            %   Z-up), while the pose methods keep animating on top of it.
            %
            %   The transform needs to reference 16 elements and include the
            %   bottom row. The translation units are in meters.
            %   The input is not verified.
            %
            %   Inputs:
            %       matrix - 4x4 transform matrix
            %
            %   Throws:
            %       Error on internal errors
            hebi_charts_native('hebi_charts_Mesh_setMeshTransform4x4', this.ref_Mesh, matrix);
        end

        function setMeshPoseRPY(this, x, y, z, roll, pitch, yaw)
            % Sets the fixed mesh-to-object pre-transform using a translation and
            %   Roll, Pitch, and Yaw (radians). Follows the ROS/REP-103 convention
            %   (Extrinsic / Fixed-Axis XYZ):
            %
            %       orientation = Rz(yaw)*Ry(pitch)*Rx(roll)
            %
            %   This is a convenience for the common case of correcting the frame the
            %   mesh was exported in (e.g. a Y-up mesh needs a roll of pi/2), while
            %   the pose methods keep animating on top of it. The input is not verified!
            %
            %   Inputs:
            %       x - position x [m]
            %       y - position y [m]
            %       z - position z [m]
            %       roll - angle in [rad]
            %       pitch - angle in [rad]
            %       yaw - angle in [rad]
            %
            %   Throws:
            %       Error on internal errors
            hebi_charts_native('hebi_charts_Mesh_setMeshPoseRPY', this.ref_Mesh, x, y, z, roll, pitch, yaw);
        end

    end

    methods(Access = public, Hidden = true)
        function this = Mesh(ref_Mesh, varargin)
            this@hebi_charts.Object3d(hebi_charts_native('hebi_charts_Mesh_to_Object3d', ref_Mesh));
            this.ref_Mesh = ref_Mesh;
            if numel(varargin) > 1 % Call setters from the leaf to make sure that the ref field is set
                this.set(varargin{:});
            end
        end
    end
    properties (SetAccess = private, GetAccess = private, Hidden = true)
        ref_Mesh = [];
    end

    methods(Access = public, Hidden = true)

        function ptr = c_ptr(this)
            ptr = this.ref_Mesh;
        end

    end

end