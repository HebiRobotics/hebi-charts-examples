classdef (Sealed) Slider < hebi_charts.Control
% A slider control for numeric input

    properties (Dependent)
        limits
        max
        min
        value
    end

    methods

        function set.limits(this, limits)
            % Sets the possible range of the slider [min, max]
            %
            %   Inputs:
            %       min
            %       max
            if numel(limits) ~= 2
                error('Property "limits" requires 2 elements');
            end
            if iscell(limits)
                [min, max] = limits{:};
            else
                min = limits(1);
                max = limits(2);
            end
            hebi_charts_native('hebi_charts_Slider_setLimits', this.ref_Slider, min, max);
        end

        function result = get.max(this)
            % Gets the maximum value of the slider range
            result = hebi_charts_native('hebi_charts_Slider_getMax', this.ref_Slider);
        end

        function set.max(this, max)
            % Sets the maximum value of the slider range. Must be greater than min.
            %
            %   Inputs:
            %       max
            hebi_charts_native('hebi_charts_Slider_setMax', this.ref_Slider, max);
        end

        function result = get.min(this)
            % Gets the minimum value of the slider range
            result = hebi_charts_native('hebi_charts_Slider_getMin', this.ref_Slider);
        end

        function set.min(this, min)
            % Sets the minimum value of the slider range. Must be less than max.
            %
            %   Inputs:
            %       min
            hebi_charts_native('hebi_charts_Slider_setMin', this.ref_Slider, min);
        end

        function result = get.value(this)
            % Gets the current value of the slider
            result = hebi_charts_native('hebi_charts_Slider_getValue', this.ref_Slider);
        end

        function set.value(this, value)
            % Sets the value of this slider
            %
            %   Inputs:
            %       value
            hebi_charts_native('hebi_charts_Slider_setValue', this.ref_Slider, value);
        end

        function result = wasChanged(this)
            % Returns whether the control value has changed since the last call and
            %   resets the flag. Toggling visibility or enabled states also reset the flag.
            result = hebi_charts_native('hebi_charts_Slider_wasChanged', this.ref_Slider);
        end

    end

    methods(Access = public, Hidden = true)
        function this = Slider(ref_Slider, varargin)
            this@hebi_charts.Control(hebi_charts_native('hebi_charts_Slider_to_Control', ref_Slider));
            this.ref_Slider = ref_Slider;
            if numel(varargin) > 1 % Call setters from the leaf to make sure that the ref field is set
                this.set(varargin{:});
            end
        end
    end
    properties (SetAccess = private, GetAccess = private, Hidden = true)
        ref_Slider = [];
    end

    methods(Access = public, Hidden = true)

        function ptr = c_ptr(this)
            ptr = this.ref_Slider;
        end

    end

end