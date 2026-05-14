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