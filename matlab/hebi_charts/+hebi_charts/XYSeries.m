classdef XYSeries < handle & matlab.mixin.SetGet
% A collection of related coordinates inside an XYChart

    properties (Dependent)
        color
        label
        lineStyle
        lineWidth
        markerSize
        markerType
        showInLegend
        visible
    end

    methods

        function result = get.color(this)
            % Gets the rendering color
            result = hebi_charts_native('hebi_charts_XYSeries_getColor', this.ptr);
        end

        function set.color(this, color)
            % Sets the rendering color
            %
            %   Inputs:
            %       color
            hebi_charts_native('hebi_charts_XYSeries_setColor', this.ptr, hebi_charts.Color.toNativeValue(color));
        end

        function result = get.label(this)
            % Gets the label shown in the chart legend
            result = hebi_charts_native('hebi_charts_XYSeries_getLabel', this.ptr);
        end

        function set.label(this, label)
            % Sets the label shown in the chart legend
            %
            %   Inputs:
            %       label
            hebi_charts_native('hebi_charts_XYSeries_setLabel', this.ptr, label);
        end

        function result = get.lineStyle(this)
            % Gets the rendering style
            result = hebi_charts_native('hebi_charts_XYSeries_getLineStyle', this.ptr);
        end

        function set.lineStyle(this, lineStyle)
            % Sets the rendering style
            %
            %   Inputs:
            %       lineStyle
            hebi_charts_native('hebi_charts_XYSeries_setLineStyle', this.ptr, hebi_charts.LineStyle.toNativeValue(lineStyle));
        end

        function result = get.lineWidth(this)
            % Gets the line width in display pixels
            result = hebi_charts_native('hebi_charts_XYSeries_getLineWidth', this.ptr);
        end

        function set.lineWidth(this, lineWidth)
            % Sets the line width
            %
            %   Inputs:
            %       lineWidth - width in [px]
            hebi_charts_native('hebi_charts_XYSeries_setLineWidth', this.ptr, lineWidth);
        end

        function result = get.markerSize(this)
            % Gets the marker size
            result = hebi_charts_native('hebi_charts_XYSeries_getMarkerSize', this.ptr);
        end

        function set.markerSize(this, markerSize)
            % Sets the marker size
            %
            %   Inputs:
            %       markerSize
            hebi_charts_native('hebi_charts_XYSeries_setMarkerSize', this.ptr, markerSize);
        end

        function result = get.markerType(this)
            % Gets the marker type
            result = hebi_charts_native('hebi_charts_XYSeries_getMarkerType', this.ptr);
        end

        function set.markerType(this, markerType)
            % Sets the marker type
            %
            %   Inputs:
            %       markerType
            hebi_charts_native('hebi_charts_XYSeries_setMarkerType', this.ptr, hebi_charts.MarkerType.toNativeValue(markerType));
        end

        function result = get.showInLegend(this)
            % Checks if this data set is shown in the legend
            result = hebi_charts_native('hebi_charts_XYSeries_isShowInLegend', this.ptr);
        end

        function set.showInLegend(this, showInLegend)
            % Shows or hides this data set from the legend
            %
            %   Inputs:
            %       showInLegend
            hebi_charts_native('hebi_charts_XYSeries_setShowInLegend', this.ptr, showInLegend);
        end

        function result = get.visible(this)
            % Gets the visibility in the chart
            result = hebi_charts_native('hebi_charts_XYSeries_isVisible', this.ptr);
        end

        function set.visible(this, visible)
            % Sets the visibility in the chart
            %
            %   Inputs:
            %       visible
            hebi_charts_native('hebi_charts_XYSeries_setVisible', this.ptr, visible);
        end

    end

    methods(Access = public, Hidden = true)
        function this = XYSeries(ptr, varargin)
            if ~isnumeric(ptr) && ~isa(ptr, 'lib.pointer')
                error('XYSeries constructor expects a C pointer type');
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
                hebi_charts_native('hebi_charts_XYSeries_release', this.ptr);
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