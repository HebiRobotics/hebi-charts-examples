classdef (Sealed) Dropdown < hebi_charts.Control
% A dropdown selection control for switching between discrete modes

    properties (Dependent)
        options
        selectedIndex
    end

    methods

        function set.options(this, options)
            % Sets the list of available options
            %
            %   Inputs:
            %       options
            %       count
            if ~iscell(options) && ~isstring(options)
                error('Expected cell array of strings or string array for "options"');
            end
            count = numel(options);
            hebi_charts_native('hebi_charts_Dropdown_setOptions', this.ref_Dropdown, options, count);
        end

        function result = get.selectedIndex(this)
            % Returns the currently selected index
            result = hebi_charts_native('hebi_charts_Dropdown_getSelectedIndex', this.ref_Dropdown);
            result = result + 1;
        end

        function set.selectedIndex(this, index)
            % Sets the selected index
            %
            %   Inputs:
            %       index
            index = index - 1;
            hebi_charts_native('hebi_charts_Dropdown_setSelectedIndex', this.ref_Dropdown, index);
        end

        function addOption(this, option)
            % Adds an option to the list
            %
            %   Inputs:
            %       option
            hebi_charts_native('hebi_charts_Dropdown_addOption', this.ref_Dropdown, option);
        end

        function result = wasChanged(this)
            % Returns whether the control value has changed since the last call and
            %   resets the flag. Toggling visibility or enabled states also reset the flag.
            result = hebi_charts_native('hebi_charts_Dropdown_wasChanged', this.ref_Dropdown);
        end

    end

    methods(Access = public, Hidden = true)
        function this = Dropdown(ref_Dropdown, varargin)
            this@hebi_charts.Control(hebi_charts_native('hebi_charts_Dropdown_to_Control', ref_Dropdown));
            this.ref_Dropdown = ref_Dropdown;
            if numel(varargin) > 1 % Call setters from the leaf to make sure that the ref field is set
                this.set(varargin{:});
            end
        end
    end
    properties (SetAccess = private, GetAccess = private, Hidden = true)
        ref_Dropdown = [];
    end

    methods(Access = public, Hidden = true)

        function ptr = c_ptr(this)
            ptr = this.ref_Dropdown;
        end

    end

end