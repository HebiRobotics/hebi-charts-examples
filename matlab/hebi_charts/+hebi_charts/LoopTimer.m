classdef (Sealed) LoopTimer < handle & matlab.mixin.SetGet
% Provides various time related functionality for timing, benchmarking,
%   and scheduling loops. On Windows, using any method will request an
%   interrupt timer of 1ms - beginTimePeriod(1).

    methods(Static)

        function result = timeSeconds()
            % Seconds since some arbitrary fixed point. Monotonic high-resolution timer for measuring elapsed time deltas.
            result = hebi_charts_native('hebi_charts_LoopTimer_timeSeconds');
        end

        function result = timeNanos()
            % Nanoseconds since some arbitrary fixed point. Monotonic high-resolution timer for measuring elapsed time deltas.
            result = hebi_charts_native('hebi_charts_LoopTimer_timeNanos');
        end

        function sleepNanos(nanos)
            % Experimental (may be removed in the future):
            %   Attempts to sleep for the given number of nanoseconds using Thread::sleep. Results are
            %   best effort and depend on the platform. Threads might spuriously wake up early or be late.
            %
            %   Inputs:
            %       nanos - Positive number of nanoseconds. Zero returns immediately.
            hebi_charts_native('hebi_charts_LoopTimer_sleepNanos', nanos);
        end

        function parkNanos(nanos)
            % Experimental (may be removed in the future):
            %   Attempts to sleep for the given number of nanoseconds using LockSupport::park. Results are
            %   best effort and depend on the platform. Threads might spuriously wake up early or be late.
            %
            %   Inputs:
            %       nanos - Positive number of nanoseconds. Zero returns immediately.
            hebi_charts_native('hebi_charts_LoopTimer_parkNanos', nanos);
        end

        function waitNanos(nanos)
            % Experimental (may be removed in the future):
            %   Attempts to sleep for the given number of nanoseconds using Object::wait. Results are
            %   best effort and depend on the platform. Threads might spuriously wake up early or be late.
            %
            %   Inputs:
            %       nanos - Positive number of nanoseconds. Zero returns immediately.
            hebi_charts_native('hebi_charts_LoopTimer_waitNanos', nanos);
        end

        function spinNanos(nanos)
            % Experimental (may be removed in the future):
            %   Attempts to "sleep" for the given number of nanoseconds using tiered spin methods for a more
            %   accurate result in exchange for higher CPU cost. Results are best effort and depend on the
            %   platform. Even busy spins may be late depending on the OS scheduler.
            %
            %   Inputs:
            %       nanos - Positive number of nanoseconds. Zero returns immediately.
            hebi_charts_native('hebi_charts_LoopTimer_spinNanos', nanos);
        end

    end

    properties (Dependent)
        elapsedTime
        frequency
        period
    end

    methods

        function result = get.elapsedTime(this)
            % Returns the elapsed time in seconds since the starting point.
            result = hebi_charts_native('hebi_charts_LoopTimer_getElapsedTime', this.ptr);
        end

        function result = get.frequency(this)
            result = hebi_charts_native('hebi_charts_LoopTimer_getFrequency', this.ptr);
        end

        function set.frequency(this, frequency)
            % Sets the desired frequency for waitForNextTick
            %
            %   Inputs:
            %       frequency - hz
            hebi_charts_native('hebi_charts_LoopTimer_setFrequency', this.ptr, frequency);
        end

        function result = get.period(this)
            result = hebi_charts_native('hebi_charts_LoopTimer_getPeriod', this.ptr);
        end

        function set.period(this, seconds)
            % Sets the desired period of waitForNextTick
            %
            %   Inputs:
            %       seconds - seconds
            hebi_charts_native('hebi_charts_LoopTimer_setPeriod', this.ptr, seconds);
        end

        function restart(this)
            % Resets the internal start time offset to now. This gets
            %   used to determine the elapsed time and the starting point
            %   for periodic ticks.
            hebi_charts_native('hebi_charts_LoopTimer_restart', this.ptr);
        end

        function tic(this)
            % Sets a timestamp for subsequent toc calls
            hebi_charts_native('hebi_charts_LoopTimer_tic', this.ptr);
        end

        function result = toc(this)
            % Returns the elapsed time in seconds since the last tic call
            result = hebi_charts_native('hebi_charts_LoopTimer_toc', this.ptr);
        end

        function result = ticToc(this)
            % Calls tic and toc in one call
            result = hebi_charts_native('hebi_charts_LoopTimer_ticToc', this.ptr);
        end

        function waitForNextTick(this)
            % Waits until the next periodic tick counting from the starting point. For
            %   example, if the start time is 12 with a period of 5, this call will wait
            %   until the next tick (17, 22, 27, 32, ...) that occurs after the current
            %   time.
            %   Note that this is a best effort, and results depends on the OS scheduler
            %   and load. Missed ticks will not be repeated.
            hebi_charts_native('hebi_charts_LoopTimer_waitForNextTick', this.ptr);
        end

        function result = getSecondsToNextTick(this)
            % Returns the remaining time in seconds until the next periodic tick counting
            %   from the starting point. For example, if the start time is 12 with a period
            %   of 5, this call returns the time to the next tick (17, 22, 27, 32, ...) that
            %   occurs after the current time. Missed ticks will not be repeated. This method
            %   can never return zero, so  users do not need to special case.
            result = hebi_charts_native('hebi_charts_LoopTimer_getSecondsToNextTick', this.ptr);
        end

        function result = getNanosToNextTick(this)
            % Returns the remaining time in nanoseconds until the next periodic tick counting
            %   from the starting point. For example, if the start time is 12 with a period
            %   of 5, this call returns the time to the next tick (17, 22, 27, 32, ...) that
            %   occurs after the current time. Missed ticks will not be repeated. This method
            %   can never return zero, so  users do not need to special case.
            result = hebi_charts_native('hebi_charts_LoopTimer_getNanosToNextTick', this.ptr);
        end

    end

    methods(Access = public)
        function this = LoopTimer(varargin)
            % A uint64 scalar is a handle the library created
            if nargin >= 1 && isa(varargin{1}, 'uint64') && isscalar(varargin{1})
                ptr_ = varargin{1};
                varargin(1) = [];
            else
                ptr_ = hebi_charts_native('hebi_charts_LoopTimer_create');
                if isempty(ptr_)
                    error('Failed to create hebi_charts.LoopTimer in LoopTimer.LoopTimer');
                end
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
                hebi_charts_native('hebi_charts_LoopTimer_release', this.ptr);
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