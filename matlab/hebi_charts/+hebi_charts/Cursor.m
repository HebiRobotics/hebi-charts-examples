classdef (Sealed) Cursor < handle & matlab.mixin.SetGet
% A vertical or horizontal cursor to measure or mark an axis value. Can be draggable.

    properties (Dependent)
        editable
        label
        value
        visible
    end

    methods

        function result = get.editable(this)
            % Gets the editable state of the indicator
            result = hebi_charts_native('hebi_charts_Cursor_isEditable', this.ptr);
        end

        function set.editable(this, editable)
            % Editable indicators can be dragged around by users
            %
            %   Inputs:
            %       editable
            hebi_charts_native('hebi_charts_Cursor_setEditable', this.ptr, editable);
        end

        function result = get.label(this)
            % Gets the indicator label
            result = hebi_charts_native('hebi_charts_Cursor_getLabel', this.ptr);
        end

        function set.label(this, label)
            % Sets the indicator label. Empty or null hides the label.
            %
            %   Inputs:
            %       label
            hebi_charts_native('hebi_charts_Cursor_setLabel', this.ptr, label);
        end

        function result = get.value(this)
            % Gets the current value
            result = hebi_charts_native('hebi_charts_Cursor_getValue', this.ptr);
        end

        function set.value(this, value)
            % Sets the indicated value. NaN hides the indicator.
            %
            %   Inputs:
            %       value
            hebi_charts_native('hebi_charts_Cursor_setValue', this.ptr, value);
        end

        function result = get.visible(this)
            % Gets the visibility in the chart
            result = hebi_charts_native('hebi_charts_Cursor_isVisible', this.ptr);
        end

        function set.visible(this, visible)
            % Sets the visibility in the chart
            %
            %   Inputs:
            %       visible
            hebi_charts_native('hebi_charts_Cursor_setVisible', this.ptr, visible);
        end

    end

    methods(Access = public, Hidden = true)
        function this = Cursor(ptr, varargin)
            if ~isa(ptr, 'uint64') || ~isscalar(ptr)
                error('Cursor instances are created by the library');
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
                hebi_charts_native('hebi_charts_Cursor_release', this.ptr);
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