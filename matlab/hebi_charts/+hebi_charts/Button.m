classdef (Sealed) Button < hebi_charts.Control
% A button control

    properties (Dependent)
        text
    end

    methods

        function result = get.text(this)
            % Gets the text of this button
            result = hebi_charts_native('hebi_charts_Button_getText', this.ref_Button);
        end

        function set.text(this, text)
            % Sets the text of this button
            %
            %   Inputs:
            %       text
            hebi_charts_native('hebi_charts_Button_setText', this.ref_Button, text);
        end

        function result = isPressed(this)
            % Returns the current pressed state
            result = hebi_charts_native('hebi_charts_Button_isPressed', this.ref_Button);
        end

        function result = wasPressed(this)
            % Returns whether the button was pressed at least once since the last call and
            %   resets the flag. Toggling visibility or enabled states also reset the flag.
            result = hebi_charts_native('hebi_charts_Button_wasPressed', this.ref_Button);
        end

    end

    methods(Access = public, Hidden = true)
        function this = Button(ref_Button, varargin)
            this@hebi_charts.Control(hebi_charts_native('hebi_charts_Button_to_Control', ref_Button));
            this.ref_Button = ref_Button;
            if numel(varargin) > 1 % Call setters from the leaf to make sure that the ref field is set
                this.set(varargin{:});
            end
        end
    end
    properties (SetAccess = private, GetAccess = private, Hidden = true)
        ref_Button = [];
    end

    methods(Access = public, Hidden = true)

        function ptr = c_ptr(this)
            ptr = this.ref_Button;
        end

    end

end