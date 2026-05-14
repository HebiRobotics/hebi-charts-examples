classdef (Sealed) Label < hebi_charts.Control
% A control that displays some text or value

    properties (Dependent)
        text
        value
    end

    methods

        function result = get.text(this)
            % Gets the text of this status label
            result = hebi_charts_native('hebi_charts_Label_getText', this.ref_Label);
        end

        function set.text(this, text)
            % Sets the text of this status label
            %
            %   Inputs:
            %       text
            hebi_charts_native('hebi_charts_Label_setText', this.ref_Label, text);
        end

        function result = get.value(this)
            % Gets the current numeric value (may be NaN if text was set last)
            result = hebi_charts_native('hebi_charts_Label_getValue', this.ref_Label);
        end

        function set.value(this, value)
            % Sets the numeric value of this label using a standard engineering format
            %
            %   Inputs:
            %       value
            hebi_charts_native('hebi_charts_Label_setValue', this.ref_Label, value);
        end

    end

    methods(Access = public, Hidden = true)
        function this = Label(ref_Label, varargin)
            this@hebi_charts.Control(hebi_charts_native('hebi_charts_Label_to_Control', ref_Label));
            this.ref_Label = ref_Label;
            if numel(varargin) > 1 % Call setters from the leaf to make sure that the ref field is set
                this.set(varargin{:});
            end
        end
    end
    properties (SetAccess = private, GetAccess = private, Hidden = true)
        ref_Label = [];
    end

    methods(Access = public, Hidden = true)

        function ptr = c_ptr(this)
            ptr = this.ref_Label;
        end

    end

end