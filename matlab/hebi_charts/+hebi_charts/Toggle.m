classdef (Sealed) Toggle < hebi_charts.Control
% A boolean toggle switch for on/off states

    properties (Dependent)
        selected
    end

    methods

        function result = get.selected(this)
            % Returns the current toggle state
            result = hebi_charts_native('hebi_charts_Toggle_isSelected', this.ref_Toggle);
        end

        function set.selected(this, selected)
            % Sets the toggle state
            %
            %   Inputs:
            %       selected
            hebi_charts_native('hebi_charts_Toggle_setSelected', this.ref_Toggle, selected);
        end

        function result = wasChanged(this)
            % Returns whether the control value has changed since the last call and
            %   resets the flag. Toggling visibility or enabled states also reset the flag.
            result = hebi_charts_native('hebi_charts_Toggle_wasChanged', this.ref_Toggle);
        end

    end

    methods(Access = public, Hidden = true)
        function this = Toggle(ref_Toggle, varargin)
            this@hebi_charts.Control(hebi_charts_native('hebi_charts_Toggle_to_Control', ref_Toggle));
            this.ref_Toggle = ref_Toggle;
            if numel(varargin) > 1 % Call setters from the leaf to make sure that the ref field is set
                this.set(varargin{:});
            end
        end
    end
    properties (SetAccess = private, GetAccess = private, Hidden = true)
        ref_Toggle = [];
    end

    methods(Access = public, Hidden = true)

        function ptr = c_ptr(this)
            ptr = this.ref_Toggle;
        end

    end

end