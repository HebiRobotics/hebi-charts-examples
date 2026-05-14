classdef (Sealed) Frame < hebi_charts.Object3d
% A triad that represents a frame

    methods(Access = public, Hidden = true)
        function this = Frame(ref_Frame, varargin)
            this@hebi_charts.Object3d(hebi_charts_native('hebi_charts_Frame_to_Object3d', ref_Frame));
            this.ref_Frame = ref_Frame;
            if numel(varargin) > 1 % Call setters from the leaf to make sure that the ref field is set
                this.set(varargin{:});
            end
        end
    end
    properties (SetAccess = private, GetAccess = private, Hidden = true)
        ref_Frame = [];
    end

    methods(Access = public, Hidden = true)

        function ptr = c_ptr(this)
            ptr = this.ref_Frame;
        end

    end

end