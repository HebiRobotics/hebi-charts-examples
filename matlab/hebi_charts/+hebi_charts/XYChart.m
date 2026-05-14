classdef XYChart < handle & matlab.mixin.SetGet
% Chart for XY series

    properties (Dependent)
        title
        xAssumeSorted
        xauto
        xlabel
        xlim
        xmax
        xmin
        xunit
        yauto
        ylabel
        ylim
        ymax
        ymin
        yunit
    end

    methods

        function result = get.title(this)
            % Gets the title shown in the chart titlebar
            result = hebi_charts_native('hebi_charts_XYChart_getTitle', this.ptr);
        end

        function set.title(this, title)
            % Sets the title shown in the chart titlebar
            %
            %   Inputs:
            %       title
            hebi_charts_native('hebi_charts_XYChart_setTitle', this.ptr, title);
        end

        function result = get.xAssumeSorted(this)
            % Whether the renderer should assume X-axis data is strictly increasing for performance
            result = hebi_charts_native('hebi_charts_XYChart_getXAssumeSorted', this.ptr);
        end

        function set.xAssumeSorted(this, xAssumeSorted)
            % Enable to speed up rendering of large datasets. Must be disabled for paths that 'wrap back' or loops.
            %
            %   Inputs:
            %       xAssumeSorted
            hebi_charts_native('hebi_charts_XYChart_setXAssumeSorted', this.ptr, xAssumeSorted);
        end

        function result = get.xauto(this)
            % Checks if auto SI-prefix scaling is enabled for the X-axis
            result = hebi_charts_native('hebi_charts_XYChart_isXAutoUnitScaling', this.ptr);
        end

        function set.xauto(this, enabled)
            % Enables/disables auto SI-prefix scaling for X-axis (e.g., 0.001s -> 1ms)
            %
            %   Inputs:
            %       enabled
            hebi_charts_native('hebi_charts_XYChart_setXAutoUnitScaling', this.ptr, enabled);
        end

        function result = get.xlabel(this)
            % Gets the X-axis label text
            result = hebi_charts_native('hebi_charts_XYChart_getXLabel', this.ptr);
        end

        function set.xlabel(this, label)
            % Sets the X-axis label text
            %
            %   Inputs:
            %       label
            hebi_charts_native('hebi_charts_XYChart_setXLabel', this.ptr, label);
        end

        function set.xlim(this, xlim)
            % Sets the X-axis limits. Set nan for auto-ranging.
            %
            %   Inputs:
            %       min
            %       max
            %
            %   Throws:
            %       Error on internal errors
            if numel(xlim) ~= 2
                error('Property "xlim" requires 2 elements');
            end
            if iscell(xlim)
                [min, max] = xlim{:};
            else
                min = xlim(1);
                max = xlim(2);
            end
            status_ = hebi_charts_native('hebi_charts_XYChart_setXLim', this.ptr, min, max);
            if status_ ~= 0
                error(['Encountered error in XYChart.set.xlim' ': ' hebi_charts_native('hebi_charts_Runtime_getLastErrorString')]);
            end
        end

        function result = get.xmax(this)
            % Gets the X-axis max limit
            result = hebi_charts_native('hebi_charts_XYChart_getXMax', this.ptr);
        end

        function set.xmax(this, max)
            % Sets the X-axis maximum limit
            %
            %   Inputs:
            %       max
            hebi_charts_native('hebi_charts_XYChart_setXMax', this.ptr, max);
        end

        function result = get.xmin(this)
            % Gets the X-axis min limit
            result = hebi_charts_native('hebi_charts_XYChart_getXMin', this.ptr);
        end

        function set.xmin(this, min)
            % Sets the X-axis minimum limit
            %
            %   Inputs:
            %       min
            hebi_charts_native('hebi_charts_XYChart_setXMin', this.ptr, min);
        end

        function result = get.xunit(this)
            % Gets the X-axis unit (e.g., 's')
            result = hebi_charts_native('hebi_charts_XYChart_getXUnit', this.ptr);
        end

        function set.xunit(this, unit)
            % Sets the X-axis unit (e.g., 's')
            %
            %   Inputs:
            %       unit
            hebi_charts_native('hebi_charts_XYChart_setXUnit', this.ptr, unit);
        end

        function result = get.yauto(this)
            % Checks if auto SI-prefix scaling is enabled for the Y-axis
            result = hebi_charts_native('hebi_charts_XYChart_isYAutoUnitScaling', this.ptr);
        end

        function set.yauto(this, enabled)
            % Enables/disables auto SI-prefix scaling for Y-axis
            %
            %   Inputs:
            %       enabled
            hebi_charts_native('hebi_charts_XYChart_setYAutoUnitScaling', this.ptr, enabled);
        end

        function result = get.ylabel(this)
            % Gets the Y-axis label text
            result = hebi_charts_native('hebi_charts_XYChart_getYLabel', this.ptr);
        end

        function set.ylabel(this, label)
            % Sets the Y-axis label text
            %
            %   Inputs:
            %       label
            hebi_charts_native('hebi_charts_XYChart_setYLabel', this.ptr, label);
        end

        function set.ylim(this, ylim)
            % Sets the Y-axis limits. Set nan for auto-ranging.
            %
            %   Inputs:
            %       min
            %       max
            if numel(ylim) ~= 2
                error('Property "ylim" requires 2 elements');
            end
            if iscell(ylim)
                [min, max] = ylim{:};
            else
                min = ylim(1);
                max = ylim(2);
            end
            hebi_charts_native('hebi_charts_XYChart_setYLim', this.ptr, min, max);
        end

        function result = get.ymax(this)
            % Gets the Y-axis max limit
            result = hebi_charts_native('hebi_charts_XYChart_getYMax', this.ptr);
        end

        function set.ymax(this, max)
            % Sets the Y-axis maximum limit
            %
            %   Inputs:
            %       max
            hebi_charts_native('hebi_charts_XYChart_setYMax', this.ptr, max);
        end

        function result = get.ymin(this)
            % Gets the Y-axis min limit
            result = hebi_charts_native('hebi_charts_XYChart_getYMin', this.ptr);
        end

        function set.ymin(this, min)
            % Sets the Y-axis minimum limit
            %
            %   Inputs:
            %       min
            hebi_charts_native('hebi_charts_XYChart_setYMin', this.ptr, min);
        end

        function result = get.yunit(this)
            % Gets the Y-axis unit (e.g., 'V')
            result = hebi_charts_native('hebi_charts_XYChart_getYUnit', this.ptr);
        end

        function set.yunit(this, unit)
            % Sets the Y-axis unit (e.g., 'V')
            %
            %   Inputs:
            %       unit
            hebi_charts_native('hebi_charts_XYChart_setYUnit', this.ptr, unit);
        end

        function obj = addXCursor(this, varargin)
            % Adds a draggable cursor to the X-axis
            ptr_ = hebi_charts_native('hebi_charts_XYChart_addXCursor', this.ptr);
            if isempty(ptr_)
                error(['Failed to create hebi_charts.Cursor in XYChart.addXCursor' ': ' hebi_charts_native('hebi_charts_Runtime_getLastErrorString')]);
            end
            obj = hebi_charts.Cursor(ptr_, varargin{:});
        end

        function obj = addYCursor(this, varargin)
            % Adds a draggable cursor to the Y-axis
            ptr_ = hebi_charts_native('hebi_charts_XYChart_addYCursor', this.ptr);
            if isempty(ptr_)
                error(['Failed to create hebi_charts.Cursor in XYChart.addYCursor' ': ' hebi_charts_native('hebi_charts_Runtime_getLastErrorString')]);
            end
            obj = hebi_charts.Cursor(ptr_, varargin{:});
        end

    end

    methods(Access = public, Hidden = true)
        function this = XYChart(ptr, varargin)
            if ~isnumeric(ptr) && ~isa(ptr, 'lib.pointer')
                error('XYChart constructor expects a C pointer type');
            end
            this.ptr = ptr;

            % Apply name-value pairs to properties, if provided
            if numel(varargin) > 1
                this.set(varargin{:});
            end
        end
    end
    properties (SetAccess = private, GetAccess = private, Hidden = true)
        ptr = [];
    end

    methods(Access = public, Hidden = true)

        function ptr = c_ptr(this)
            ptr = this.ptr;
        end

        function delete(this) % destructor
             if ~isempty(this.ptr)
                hebi_charts_native('hebi_charts_XYChart_release', this.ptr);
                this.ptr = [];
            end
        end

        % Inherited handle methods that should be omitted from auto - complete
        function varargout = addlistener(varargin)
            varargout{:} = addlistener@handle(varargin{:});
        end
        function varargout = eq(varargin)
            varargout{:} = eq@handle(varargin{:});
        end
        function varargout = findobj(varargin)
            varargout{:} = findobj@handle(varargin{:});
        end
        function varargout = findprop(varargin)
            varargout{:} = findprop@handle(varargin{:});
        end
        function varargout = ge(varargin)
            varargout{:} = ge@handle(varargin{:});
        end
        function varargout = gt(varargin)
            varargout{:} = gt@handle(varargin{:});
        end
        function varargout = le(varargin)
            varargout{:} = le@handle(varargin{:});
        end
        function varargout = listener(varargin)
            varargout{:} = listener@handle(varargin{:});
        end
        function varargout = lt(varargin)
            varargout{:} = lt@handle(varargin{:});
        end
        function varargout = ne(varargin)
            varargout{:} = ne@handle(varargin{:});
        end
        function varargout = notify(varargin)
            varargout{:} = notify@handle(varargin{:});
        end

    end

end