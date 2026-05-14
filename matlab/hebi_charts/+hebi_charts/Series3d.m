classdef Series3d < hebi_charts.Object3d
% A collection of related spatial coordinates

    properties (Dependent)
        color
    end

    methods

        function result = get.color(this)
            % Gets the current dataset color
            result = hebi_charts_native('hebi_charts_Series3d_getColor', this.ref_Series3d);
        end

        function set.color(this, color)
            % Sets the dataset color
            %
            %   Inputs:
            %       color
            hebi_charts_native('hebi_charts_Series3d_setColor', this.ref_Series3d, hebi_charts.Color.toNativeValue(color));
        end

    end

    methods(Access = public, Hidden = true)
        function this = Series3d(ref_Series3d, varargin)
            this@hebi_charts.Object3d(hebi_charts_native('hebi_charts_Series3d_to_Object3d', ref_Series3d));
            this.ref_Series3d = ref_Series3d;
            if numel(varargin) > 1 % Call setters from the leaf to make sure that the ref field is set
                this.set(varargin{:});
            end
        end
    end
    properties (SetAccess = private, GetAccess = private, Hidden = true)
        ref_Series3d = [];
    end

    methods(Access = public, Hidden = true)

        function ptr = c_ptr(this)
            ptr = this.ref_Series3d;
        end

    end

end