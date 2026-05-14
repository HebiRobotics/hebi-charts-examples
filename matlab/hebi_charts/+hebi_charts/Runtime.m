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
            status_ = hebi_charts_native('hebi_charts_Runtime_setOption', hebi_charts.RuntimeOption.toNativeValue(option), value);
            if status_ ~= 0
                error(['Encountered error in Runtime.setOption' ': ' hebi_charts_native('hebi_charts_Runtime_getLastErrorString')]);
            end
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
            status_ = hebi_charts_native('hebi_charts_Runtime_waitUntilWindowsClosed');
            if status_ ~= 0
                error(['Encountered error in Runtime.waitUntilWindowsClosed' ': ' hebi_charts_native('hebi_charts_Runtime_getLastErrorString')]);
            end
        end

        function collect()
            % Reclaims unused memory and prepares the heap for real-time execution
            hebi_charts_native('hebi_charts_Runtime_collect');
        end

        function closeAll()
            % Hides all active windows and dashboards
            hebi_charts_native('hebi_charts_Runtime_closeAll');
        end

        function userData = runOnUiThread(func, userData)
            % Debug method to run code on the internal UI thread
            %
            %   Inputs:
            %       func
            %       userData
            userData = hebi_charts_native('hebi_charts_Runtime_runOnUiThread', func, userData);
        end

        function printLastErrorDetails()
            % Debug method that prints the last exception encountered on the current thread
            hebi_charts_native('hebi_charts_Runtime_printLastErrorDetails');
        end

        function printThreadInfo(name)
            % Debug method to print internal thread information. May be removed in the future.
            %
            %   Inputs:
            %       name
            hebi_charts_native('hebi_charts_Runtime_printThreadInfo', name);
        end

        function result = getLastErrorString()
            % Returns an address to a c string that contains the last error message.
            %   This address is only valid until the next call to this method from the
            %   same thread. Never returns nullptr. Do not free the address!
            result = hebi_charts_native('hebi_charts_Runtime_getLastErrorString');
        end

    end

end