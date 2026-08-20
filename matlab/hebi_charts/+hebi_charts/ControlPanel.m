classdef (Sealed) ControlPanel < handle & matlab.mixin.SetGet
% A panel containing a list of interactive controls like buttons, sliders, and labels

    properties (Dependent)
        title
        width
    end

    methods

        function result = get.title(this)
            % Gets the title
            result = hebi_charts_native('hebi_charts_ControlPanel_getTitle', this.ptr);
        end

        function set.title(this, title)
            % Sets the title
            %
            %   Inputs:
            %       title
            hebi_charts_native('hebi_charts_ControlPanel_setTitle', this.ptr, title);
        end

        function result = get.width(this)
            % Gets the current width
            result = hebi_charts_native('hebi_charts_ControlPanel_getWidth', this.ptr);
        end

        function set.width(this, width)
            % Sets the desired width
            %
            %   Inputs:
            %       width
            hebi_charts_native('hebi_charts_ControlPanel_setWidth', this.ptr, width);
        end

        function addSection(this, title)
            % Starts a new section with the given header
            %
            %   Inputs:
            %       title
            %
            %   Throws:
            %       Error on internal errors
            hebi_charts_native('hebi_charts_ControlPanel_addSection', this.ptr, title);
        end

        function obj = addLabel(this, varargin)
            % Adds a label for displaying text to the control panel
            %
            %   Outputs:
            %       A control that displays some text or value
            %
            %   Throws:
            %       Error on internal errors
            ptr_ = hebi_charts_native('hebi_charts_ControlPanel_addLabel', this.ptr);
            if isempty(ptr_)
                error('Failed to create hebi_charts.Label in ControlPanel.addLabel');
            end
            obj = hebi_charts.Label(ptr_, varargin{:});
        end

        function obj = addButton(this, varargin)
            % Adds a button to the control panel
            %
            %   Outputs:
            %       A button control
            %
            %   Throws:
            %       Error on internal errors
            ptr_ = hebi_charts_native('hebi_charts_ControlPanel_addButton', this.ptr);
            if isempty(ptr_)
                error('Failed to create hebi_charts.Button in ControlPanel.addButton');
            end
            obj = hebi_charts.Button(ptr_, varargin{:});
        end

        function obj = addStartButton(this, varargin)
            % Adds a 'start'-style button to the control panel
            %
            %   Outputs:
            %       A button control
            %
            %   Throws:
            %       Error on internal errors
            ptr_ = hebi_charts_native('hebi_charts_ControlPanel_addStartButton', this.ptr);
            if isempty(ptr_)
                error('Failed to create hebi_charts.Button in ControlPanel.addStartButton');
            end
            obj = hebi_charts.Button(ptr_, varargin{:});
        end

        function obj = addStopButton(this, varargin)
            % Adds a 'stop'-style (red) button to the control panel
            %
            %   Outputs:
            %       A button control
            %
            %   Throws:
            %       Error on internal errors
            ptr_ = hebi_charts_native('hebi_charts_ControlPanel_addStopButton', this.ptr);
            if isempty(ptr_)
                error('Failed to create hebi_charts.Button in ControlPanel.addStopButton');
            end
            obj = hebi_charts.Button(ptr_, varargin{:});
        end

        function obj = addSlider(this, varargin)
            % Adds a slider to the control panel
            %
            %   Outputs:
            %       A slider control for numeric input
            %
            %   Throws:
            %       Error on internal errors
            ptr_ = hebi_charts_native('hebi_charts_ControlPanel_addSlider', this.ptr);
            if isempty(ptr_)
                error('Failed to create hebi_charts.Slider in ControlPanel.addSlider');
            end
            obj = hebi_charts.Slider(ptr_, varargin{:});
        end

        function obj = addToggle(this, varargin)
            % Adds a toggle to the control panel
            %
            %   Outputs:
            %       A boolean toggle switch for on/off states
            %
            %   Throws:
            %       Error on internal errors
            ptr_ = hebi_charts_native('hebi_charts_ControlPanel_addToggle', this.ptr);
            if isempty(ptr_)
                error('Failed to create hebi_charts.Toggle in ControlPanel.addToggle');
            end
            obj = hebi_charts.Toggle(ptr_, varargin{:});
        end

        function obj = addDropdown(this, varargin)
            % Adds a dropdown choice selector to the control panel
            %
            %   Outputs:
            %       A dropdown selection control for switching between discrete modes
            %
            %   Throws:
            %       Error on internal errors
            ptr_ = hebi_charts_native('hebi_charts_ControlPanel_addDropdown', this.ptr);
            if isempty(ptr_)
                error('Failed to create hebi_charts.Dropdown in ControlPanel.addDropdown');
            end
            obj = hebi_charts.Dropdown(ptr_, varargin{:});
        end

    end

    methods(Access = public, Hidden = true)
        function this = ControlPanel(ptr, varargin)
            if ~isa(ptr, 'uint64') || ~isscalar(ptr)
                error('ControlPanel instances are created by the library');
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
                hebi_charts_native('hebi_charts_ControlPanel_release', this.ptr);
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