classdef Control < handle & matlab.mixin.SetGet
% Shared control functionality

    properties (Dependent)
        enabled
        label
        tooltip
        visible
    end

    methods

        function result = get.enabled(this)
            % Gets the current disabled state of this node
            result = hebi_charts_native('hebi_charts_Control_isEnabled', this.ptr);
        end

        function set.enabled(this, enabled)
            % Sets the disabled state of this node
            %
            %   Inputs:
            %       enabled
            hebi_charts_native('hebi_charts_Control_setEnabled', this.ptr, enabled);
        end

        function result = get.label(this)
            % Gets the name or descriptor of the control in the left column
            result = hebi_charts_native('hebi_charts_Control_getLabel', this.ptr);
        end

        function set.label(this, name)
            % Sets the name or descriptor of the control in the left column
            %
            %   Inputs:
            %       name
            hebi_charts_native('hebi_charts_Control_setLabel', this.ptr, name);
        end

        function result = get.tooltip(this)
            % Gets the tooltip
            result = hebi_charts_native('hebi_charts_Control_getTooltip', this.ptr);
        end

        function set.tooltip(this, tooltip)
            % Sets the tooltip
            %
            %   Inputs:
            %       tooltip
            hebi_charts_native('hebi_charts_Control_setTooltip', this.ptr, tooltip);
        end

        function result = get.visible(this)
            % Gets the current visibility state of this node
            result = hebi_charts_native('hebi_charts_Control_isVisible', this.ptr);
        end

        function set.visible(this, visible)
            % Sets the visibility of this node
            %
            %   Inputs:
            %       visible
            hebi_charts_native('hebi_charts_Control_setVisible', this.ptr, visible);
        end

    end

    methods(Access = public, Hidden = true)
        function this = Control(ptr, varargin)
            if ~isa(ptr, 'uint64') || ~isscalar(ptr)
                error('Control instances are created by the library');
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
                hebi_charts_native('hebi_charts_Control_release', this.ptr);
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