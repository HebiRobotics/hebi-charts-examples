classdef (Sealed) Runtime
% Contains utility methods for working with the runtime. Some methods
%   are experimental and may change or be removed in the future.

    methods(Static)

        function setOption(option, value)
            % Applies runtime options. Needs to be done before any other methods.
            %
            %   Inputs:
            %       option
            %       value - string value depending on the option (e.g. DpiScale='2.0', VerboseGraphics='true')
            %
            %   Throws:
            %       Error on internal errors
            hebi_charts_native('hebi_charts_Runtime_setOption', hebi_charts.RuntimeOption.toNativeValue(option), value);
        end

        function setTheme(theme)
            % Sets an AtlantaFX theme for rendering the UI
            %
            %   Inputs:
            %       theme
            hebi_charts_native('hebi_charts_Runtime_setTheme', hebi_charts.Theme.toNativeValue(theme));
        end

        function setAutoCloseWindows(autoClose)
            % Applies a global auto-close behavior, i.e., window::keepOpen
            %
            %   Inputs:
            %       autoClose
            hebi_charts_native('hebi_charts_Runtime_setAutoCloseWindows', autoClose);
        end

        function waitUntilWindowsClosed()
            % Waits until all windows were closed by the user
            %
            %   Throws:
            %       Error when called from the FX thread
            hebi_charts_native('hebi_charts_Runtime_waitUntilWindowsClosed');
        end

        function collect()
            % Reclaims unused memory and prepares the heap for real-time execution
            hebi_charts_native('hebi_charts_Runtime_collect');
        end

        function closeAll()
            % Hides all active windows and dashboards
            hebi_charts_native('hebi_charts_Runtime_closeAll');
        end

        function varargout = runOnUiThread(varargin)
            % Debug method to run code on the internal UI thread
            %
            %   Not supported from MATLAB: its callback takes an argument ('userData') that the gateway cannot convert to an mxArray.
            error('hebi_charts:NotSupported', "'runOnUiThread' is not supported from MATLAB: its callback takes an argument ('userData') that the gateway cannot convert to an mxArray.");
        end

        function printThreadInfo(name)
            % Debug method to print internal thread information. May be removed in the future.
            %
            %   Inputs:
            %       name
            hebi_charts_native('hebi_charts_Runtime_printThreadInfo', name);
        end

    end

end