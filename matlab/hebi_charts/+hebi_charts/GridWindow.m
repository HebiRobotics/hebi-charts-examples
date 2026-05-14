classdef (Sealed) GridWindow < handle & matlab.mixin.SetGet
% Represents a window containing an equally sized row/col grid

    properties (Dependent)
        fullScreen
        height
        keepOpen
        location
        size
        title
        width
        x
        y
    end

    methods

        function result = get.fullScreen(this)
            % Gets the current full screen mode
            result = hebi_charts_native('hebi_charts_GridWindow_isFullScreen', this.ptr);
        end

        function set.fullScreen(this, fullScreen)
            % Enters or exits fullscreen mode. Does not apply to off screen windows
            %
            %   Inputs:
            %       fullScreen
            hebi_charts_native('hebi_charts_GridWindow_setFullScreen', this.ptr, fullScreen);
        end

        function result = get.height(this)
            % Gets the content height in display points
            result = hebi_charts_native('hebi_charts_GridWindow_getHeight', this.ptr);
        end

        function set.height(this, height)
            % Sets the content height in display points
            %
            %   Inputs:
            %       height
            hebi_charts_native('hebi_charts_GridWindow_setHeight', this.ptr, height);
        end

        function result = get.keepOpen(this)
            % Gets the current keepOpen state
            result = hebi_charts_native('hebi_charts_GridWindow_getKeepOpen', this.ptr);
        end

        function set.keepOpen(this, keepOpen)
            % Keeps the window open after the destructor gets called
            %
            %   Inputs:
            %       keepOpen
            hebi_charts_native('hebi_charts_GridWindow_setKeepOpen', this.ptr, keepOpen);
        end

        function set.location(this, location)
            % Sets the window's screen location in pixels (x, y)
            %
            %   Inputs:
            %       xOffset
            %       yOffset
            if numel(location) ~= 2
                error('Property "location" requires 2 elements');
            end
            if iscell(location)
                [xOffset, yOffset] = location{:};
            else
                xOffset = location(1);
                yOffset = location(2);
            end
            hebi_charts_native('hebi_charts_GridWindow_setLocation', this.ptr, xOffset, yOffset);
        end

        function set.size(this, size)
            % Sets the content size in display points (width, height), excluding the title bar.
            %
            %   Inputs:
            %       width
            %       height
            if numel(size) ~= 2
                error('Property "size" requires 2 elements');
            end
            if iscell(size)
                [width, height] = size{:};
            else
                width = size(1);
                height = size(2);
            end
            hebi_charts_native('hebi_charts_GridWindow_setSize', this.ptr, width, height);
        end

        function result = get.title(this)
            % Gets the title of the window header bar
            result = hebi_charts_native('hebi_charts_GridWindow_getTitle', this.ptr);
        end

        function set.title(this, title)
            % Sets the title of the window header bar
            %
            %   Inputs:
            %       title
            hebi_charts_native('hebi_charts_GridWindow_setTitle', this.ptr, title);
        end

        function result = get.width(this)
            % Gets the content width in display points
            result = hebi_charts_native('hebi_charts_GridWindow_getWidth', this.ptr);
        end

        function set.width(this, width)
            % Sets the content width in display points
            %
            %   Inputs:
            %       width
            hebi_charts_native('hebi_charts_GridWindow_setWidth', this.ptr, width);
        end

        function result = get.x(this)
            % Gets the window's horizontal screen location in pixels
            result = hebi_charts_native('hebi_charts_GridWindow_getX', this.ptr);
        end

        function set.x(this, x)
            % Sets the window's horizontal screen location in pixels
            %
            %   Inputs:
            %       x
            hebi_charts_native('hebi_charts_GridWindow_setX', this.ptr, x);
        end

        function result = get.y(this)
            % Gets the window's vertical screen location in pixels
            result = hebi_charts_native('hebi_charts_GridWindow_getY', this.ptr);
        end

        function set.y(this, y)
            % Sets the window's vertical screen location in pixels
            %
            %   Inputs:
            %       y
            hebi_charts_native('hebi_charts_GridWindow_setY', this.ptr, y);
        end

        function obj = addLineChart(this, varargin)
            % Creates a 2d line chart with the given size
            %
            %   Inputs:
            %       row
            %       col
            %       rowSpan
            %       colSpan
            %
            %   Throws:
            %       Error on internal errors
            row = 0;
            if ~isempty(varargin) && ~ischar(varargin{1}) && ~isstring(varargin{1})
                row = varargin{1};
                varargin(1) = [];
            end
            col = 0;
            if ~isempty(varargin) && ~ischar(varargin{1}) && ~isstring(varargin{1})
                col = varargin{1};
                varargin(1) = [];
            end
            rowSpan = 1;
            if ~isempty(varargin) && ~ischar(varargin{1}) && ~isstring(varargin{1})
                rowSpan = varargin{1};
                varargin(1) = [];
            end
            colSpan = 1;
            if ~isempty(varargin) && ~ischar(varargin{1}) && ~isstring(varargin{1})
                colSpan = varargin{1};
                varargin(1) = [];
            end
            % Switch to zero-based indexing. We can't distinguish between the default and
            % a user-set zero, but that likely results in the user-intended behavior anyways.
            if row > 0, row = row - 1; else, row = 0; end
            if col > 0, col = col - 1; else, col = 0; end
            ptr_ = hebi_charts_native('hebi_charts_GridWindow_addLineChart', this.ptr, row, col, rowSpan, colSpan);
            if isempty(ptr_)
                error(['Failed to create hebi_charts.LineChart in GridWindow.addLineChart' ': ' hebi_charts_native('hebi_charts_Runtime_getLastErrorString')]);
            end
            obj = hebi_charts.LineChart(ptr_, varargin{:});
        end

        function obj = addScope(this, varargin)
            % Creates a line chart with a pre-set time axis in [s]
            %
            %   Inputs:
            %       row
            %       col
            %       rowSpan
            %       colSpan
            %
            %   Throws:
            %       Error on internal errors
            row = 0;
            if ~isempty(varargin) && ~ischar(varargin{1}) && ~isstring(varargin{1})
                row = varargin{1};
                varargin(1) = [];
            end
            col = 0;
            if ~isempty(varargin) && ~ischar(varargin{1}) && ~isstring(varargin{1})
                col = varargin{1};
                varargin(1) = [];
            end
            rowSpan = 1;
            if ~isempty(varargin) && ~ischar(varargin{1}) && ~isstring(varargin{1})
                rowSpan = varargin{1};
                varargin(1) = [];
            end
            colSpan = 1;
            if ~isempty(varargin) && ~ischar(varargin{1}) && ~isstring(varargin{1})
                colSpan = varargin{1};
                varargin(1) = [];
            end
            % Switch to zero-based indexing. We can't distinguish between the default and
            % a user-set zero, but that likely results in the user-intended behavior anyways.
            if row > 0, row = row - 1; else, row = 0; end
            if col > 0, col = col - 1; else, col = 0; end
            ptr_ = hebi_charts_native('hebi_charts_GridWindow_addScope', this.ptr, row, col, rowSpan, colSpan);
            if isempty(ptr_)
                error(['Failed to create hebi_charts.LineChart in GridWindow.addScope' ': ' hebi_charts_native('hebi_charts_Runtime_getLastErrorString')]);
            end
            obj = hebi_charts.LineChart(ptr_, varargin{:});
        end

        function obj = addPercentileHistogram(this, varargin)
            % Creates a percentile histogram with the given size
            %
            %   Inputs:
            %       row
            %       col
            %       rowSpan
            %       colSpan
            %
            %   Throws:
            %       Error on internal errors
            row = 0;
            if ~isempty(varargin) && ~ischar(varargin{1}) && ~isstring(varargin{1})
                row = varargin{1};
                varargin(1) = [];
            end
            col = 0;
            if ~isempty(varargin) && ~ischar(varargin{1}) && ~isstring(varargin{1})
                col = varargin{1};
                varargin(1) = [];
            end
            rowSpan = 1;
            if ~isempty(varargin) && ~ischar(varargin{1}) && ~isstring(varargin{1})
                rowSpan = varargin{1};
                varargin(1) = [];
            end
            colSpan = 1;
            if ~isempty(varargin) && ~ischar(varargin{1}) && ~isstring(varargin{1})
                colSpan = varargin{1};
                varargin(1) = [];
            end
            % Switch to zero-based indexing. We can't distinguish between the default and
            % a user-set zero, but that likely results in the user-intended behavior anyways.
            if row > 0, row = row - 1; else, row = 0; end
            if col > 0, col = col - 1; else, col = 0; end
            ptr_ = hebi_charts_native('hebi_charts_GridWindow_addPercentileHistogram', this.ptr, row, col, rowSpan, colSpan);
            if isempty(ptr_)
                error(['Failed to create hebi_charts.HistogramChart in GridWindow.addPercentileHistogram' ': ' hebi_charts_native('hebi_charts_Runtime_getLastErrorString')]);
            end
            obj = hebi_charts.HistogramChart(ptr_, varargin{:});
        end

        function obj = addLatencyChart(this, varargin)
            % Utility for creating a percentile histogram for measuring latency in [s]
            %
            %   Inputs:
            %       row
            %       col
            %       rowSpan
            %       colSpan
            %
            %   Throws:
            %       Error on internal errors
            row = 0;
            if ~isempty(varargin) && ~ischar(varargin{1}) && ~isstring(varargin{1})
                row = varargin{1};
                varargin(1) = [];
            end
            col = 0;
            if ~isempty(varargin) && ~ischar(varargin{1}) && ~isstring(varargin{1})
                col = varargin{1};
                varargin(1) = [];
            end
            rowSpan = 1;
            if ~isempty(varargin) && ~ischar(varargin{1}) && ~isstring(varargin{1})
                rowSpan = varargin{1};
                varargin(1) = [];
            end
            colSpan = 1;
            if ~isempty(varargin) && ~ischar(varargin{1}) && ~isstring(varargin{1})
                colSpan = varargin{1};
                varargin(1) = [];
            end
            % Switch to zero-based indexing. We can't distinguish between the default and
            % a user-set zero, but that likely results in the user-intended behavior anyways.
            if row > 0, row = row - 1; else, row = 0; end
            if col > 0, col = col - 1; else, col = 0; end
            ptr_ = hebi_charts_native('hebi_charts_GridWindow_addLatencyChart', this.ptr, row, col, rowSpan, colSpan);
            if isempty(ptr_)
                error(['Failed to create hebi_charts.HistogramChart in GridWindow.addLatencyChart' ': ' hebi_charts_native('hebi_charts_Runtime_getLastErrorString')]);
            end
            obj = hebi_charts.HistogramChart(ptr_, varargin{:});
        end

        function obj = addScene3d(this, varargin)
            % Creates a 3d chart with the given size
            %
            %   Inputs:
            %       row
            %       col
            %       rowSpan
            %       colSpan
            %
            %   Throws:
            %       Error on internal errors
            row = 0;
            if ~isempty(varargin) && ~ischar(varargin{1}) && ~isstring(varargin{1})
                row = varargin{1};
                varargin(1) = [];
            end
            col = 0;
            if ~isempty(varargin) && ~ischar(varargin{1}) && ~isstring(varargin{1})
                col = varargin{1};
                varargin(1) = [];
            end
            rowSpan = 1;
            if ~isempty(varargin) && ~ischar(varargin{1}) && ~isstring(varargin{1})
                rowSpan = varargin{1};
                varargin(1) = [];
            end
            colSpan = 1;
            if ~isempty(varargin) && ~ischar(varargin{1}) && ~isstring(varargin{1})
                colSpan = varargin{1};
                varargin(1) = [];
            end
            % Switch to zero-based indexing. We can't distinguish between the default and
            % a user-set zero, but that likely results in the user-intended behavior anyways.
            if row > 0, row = row - 1; else, row = 0; end
            if col > 0, col = col - 1; else, col = 0; end
            ptr_ = hebi_charts_native('hebi_charts_GridWindow_addScene3d', this.ptr, row, col, rowSpan, colSpan);
            if isempty(ptr_)
                error(['Failed to create hebi_charts.Scene3d in GridWindow.addScene3d' ': ' hebi_charts_native('hebi_charts_Runtime_getLastErrorString')]);
            end
            obj = hebi_charts.Scene3d(ptr_, varargin{:});
        end

        function obj = addStreamView(this, file, row, col, rowSpan, colSpan)
            % Shows a shared-memory stream generated by hebi-video tools
            %
            %   Inputs:
            %       file
            %       row
            %       col
            %       rowSpan
            %       colSpan
            %
            %   Throws:
            %       Error on internal errors
            if nargin < 3
                row = 0;
            end
            if nargin < 4
                col = 0;
            end
            if nargin < 5
                rowSpan = 1;
            end
            if nargin < 6
                colSpan = 1;
            end
            % Switch to zero-based indexing. We can't distinguish between the default and
            % a user-set zero, but that likely results in the user-intended behavior anyways.
            if row > 0, row = row - 1; else, row = 0; end
            if col > 0, col = col - 1; else, col = 0; end
            ptr_ = hebi_charts_native('hebi_charts_GridWindow_addStreamView', this.ptr, file, row, col, rowSpan, colSpan);
            if isempty(ptr_)
                error(['Failed to create hebi_charts.StreamView in GridWindow.addStreamView' ': ' hebi_charts_native('hebi_charts_Runtime_getLastErrorString')]);
            end
            obj = hebi_charts.StreamView(ptr_);
        end

        function obj = addFxmlView(this, varargin)
            % Shows a panel for interactive controls
            %
            %   Inputs:
            %       row
            %       col
            %       rowSpan
            %       colSpan
            %
            %   Throws:
            %       Error on internal errors
            row = 0;
            if ~isempty(varargin) && ~ischar(varargin{1}) && ~isstring(varargin{1})
                row = varargin{1};
                varargin(1) = [];
            end
            col = 0;
            if ~isempty(varargin) && ~ischar(varargin{1}) && ~isstring(varargin{1})
                col = varargin{1};
                varargin(1) = [];
            end
            rowSpan = 1;
            if ~isempty(varargin) && ~ischar(varargin{1}) && ~isstring(varargin{1})
                rowSpan = varargin{1};
                varargin(1) = [];
            end
            colSpan = 1;
            if ~isempty(varargin) && ~ischar(varargin{1}) && ~isstring(varargin{1})
                colSpan = varargin{1};
                varargin(1) = [];
            end
            % Switch to zero-based indexing. We can't distinguish between the default and
            % a user-set zero, but that likely results in the user-intended behavior anyways.
            if row > 0, row = row - 1; else, row = 0; end
            if col > 0, col = col - 1; else, col = 0; end
            ptr_ = hebi_charts_native('hebi_charts_GridWindow_addFxmlView', this.ptr, row, col, rowSpan, colSpan);
            if isempty(ptr_)
                error(['Failed to create hebi_charts.FxmlView in GridWindow.addFxmlView' ': ' hebi_charts_native('hebi_charts_Runtime_getLastErrorString')]);
            end
            obj = hebi_charts.FxmlView(ptr_, varargin{:});
        end

        function addStylesheet(this, pathOrUrl, autoReload)
            % Adds a CSS file to the root scene
            %
            %   Inputs:
            %       pathOrUrl - path to a CSS file
            %       autoReload - true adds a hot-reload capability on file change
            %
            %   Throws:
            %       Error on internal errors
            if nargin < 3
                autoReload = false;
            end
            status_ = hebi_charts_native('hebi_charts_GridWindow_addStylesheet', this.ptr, pathOrUrl, autoReload);
            if status_ ~= 0
                error(['Encountered error in GridWindow.addStylesheet' ': ' hebi_charts_native('hebi_charts_Runtime_getLastErrorString')]);
            end
        end

        function show(this)
            % Shows the window. May be called multiple times
            hebi_charts_native('hebi_charts_GridWindow_show', this.ptr);
        end

        function showOffScreen(this)
            % [EXPERIMENTAL] Shows the window in a hidden utility window. Use this if you encounter issues with headless rendering.
            %
            %   Throws:
            %       Error on internal errors
            status_ = hebi_charts_native('hebi_charts_GridWindow_showOffScreen', this.ptr);
            if status_ ~= 0
                error(['Encountered error in GridWindow.showOffScreen' ': ' hebi_charts_native('hebi_charts_Runtime_getLastErrorString')]);
            end
        end

        function hide(this)
            % Hides the window. May be called multiple times. Hidden windows are not destroyed
            hebi_charts_native('hebi_charts_GridWindow_hide', this.ptr);
        end

        function result = isShowing(this)
            % Returns whether the window is currently showing
            result = hebi_charts_native('hebi_charts_GridWindow_isShowing', this.ptr);
        end

        function waitUntilClosed(this)
            % Waits until this window gets closed by the user
            %
            %   Throws:
            %       Error when called from the FX thread
            status_ = hebi_charts_native('hebi_charts_GridWindow_waitUntilClosed', this.ptr);
            if status_ ~= 0
                error(['Encountered error in GridWindow.waitUntilClosed' ': ' hebi_charts_native('hebi_charts_Runtime_getLastErrorString')]);
            end
        end

        function obj = getControlPanel(this, varargin)
            % Returns a fixed-size panel on the side of the window that can be used for interactive controls
            %
            %   Throws:
            %       Error on internal errors
            ptr_ = hebi_charts_native('hebi_charts_GridWindow_getControlPanel', this.ptr);
            if isempty(ptr_)
                error(['Failed to create hebi_charts.ControlPanel in GridWindow.getControlPanel' ': ' hebi_charts_native('hebi_charts_Runtime_getLastErrorString')]);
            end
            obj = hebi_charts.ControlPanel(ptr_, varargin{:});
        end

        function obj = createImageStream(this, varargin)
            % [EXPERIMENTAL API]
            %   Creates an image stream that continuously stores the content as images.
            %
            %   Throws:
            %       Error on internal errors
            ptr_ = hebi_charts_native('hebi_charts_GridWindow_createImageStream', this.ptr);
            if isempty(ptr_)
                error(['Failed to create hebi_charts.ImageStream in GridWindow.createImageStream' ': ' hebi_charts_native('hebi_charts_Runtime_getLastErrorString')]);
            end
            obj = hebi_charts.ImageStream(ptr_, varargin{:});
        end

        function dispatchMouseEvent(this, action, button, downMask, modifiers, x, y)
            % [EXPERIMENTAL API]
            %   Injects a mouse event into this window
            %
            %   Inputs:
            %       action
            %       button
            %       downMask
            %       modifiers
            %       x
            %       y
            hebi_charts_native('hebi_charts_GridWindow_dispatchMouseEvent', this.ptr, action, button, downMask, modifiers, x, y);
        end

        function dispatchScrollEvent(this, x, y, delta_x, delta_y, modifiers)
            % [EXPERIMENTAL API]
            %   Injects a mouse event into this window
            %
            %   Inputs:
            %       x
            %       y
            %       delta_x
            %       delta_y
            %       modifiers
            hebi_charts_native('hebi_charts_GridWindow_dispatchScrollEvent', this.ptr, x, y, delta_x, delta_y, modifiers);
        end

    end

    methods(Access = public)
        function this = GridWindow(varargin)
            % Creates a grid of equally sized rows and columns
            %
            %   Inputs:
            %       rows
            %       cols
            %
            %   Throws:
            %       Error on internal errors
            rows = 1;
            if ~isempty(varargin) && ~ischar(varargin{1}) && ~isstring(varargin{1})
                rows = varargin{1};
                varargin(1) = [];
            end
            cols = 1;
            if ~isempty(varargin) && ~ischar(varargin{1}) && ~isstring(varargin{1})
                cols = varargin{1};
                varargin(1) = [];
            end
            ptr_ = hebi_charts_native('hebi_charts_GridWindow_create', rows, cols);
            if isempty(ptr_)
                error(['Failed to create hebi_charts.GridWindow in GridWindow.GridWindow' ': ' hebi_charts_native('hebi_charts_Runtime_getLastErrorString')]);
            end
            this.ptr = ptr_;
            if numel(varargin) > 0
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
                hebi_charts_native('hebi_charts_GridWindow_release', this.ptr);
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