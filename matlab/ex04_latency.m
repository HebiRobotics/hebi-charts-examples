%% ex04_latency.m
%% Setup
% Select a desired Theme
hebi_charts.Runtime.setTheme('NordDark');

% 2x1 Grid: Top = Scope (Time series), Bottom = LatencyChart (Aggregate distribution)
window = hebi_charts.GridWindow(2, 'title', 'Python Latency Profiler', 'size', [1000 800]);
scope = window.addScope(1, 'title', 'Measured Sleep Duration vs Target', 'ylabel', 'Latency', 'yunit', 's', 'yauto', true);
histogram = window.addLatencyChart(2, 'title', 'Aggregate HdrHistogram');

% Add Traces
sleepScope = scope.addLine('MATLAB', 'lineStyle', 'Points', 'showInLegend', false);
sleepHistogram = histogram.addTrace('MATLAB');
timerScope = scope.addLine('LoopTimer', 'lineStyle', 'Points', 'showInLegend', false);
timerHistogram = histogram.addTrace('LoopTimer');

% Interactive Target Cursor (defaults to 1ms)
target = histogram.addYCursor('label', 'Target', 'editable', true, 'value', 1e-3);

%% Main loop
loopTimer = hebi_charts.LoopTimer(); % <-- manages Windows interrupt freq (see timeBeginPeriod)
window.show();
while window.isShowing()
    % Guarantee floor of 1ns to avoid negative intervals
    seconds = max(target.value, 1e-9);
    nanos = seconds * 1e9;

    % Measure MATLAB's native sleep
    loopTimer.tic();
    pause(seconds);
    dtSleep = loopTimer.ticToc();

    % Measure LoopTimer's tiered spin
    hebi_charts.LoopTimer.spinNanos(nanos);
    dtTimer = loopTimer.toc();

    % Update traces
    t = loopTimer.elapsedTime;
    sleepScope.addPoint(t, dtSleep);
    sleepHistogram.record(dtSleep);
    timerScope.addPoint(t, dtTimer);
    timerHistogram.record(dtTimer);

    % Sync visibility so a legend click hides both traces
    sleepScope.visible = sleepHistogram.visible;
    timerScope.visible = timerHistogram.visible;
end
