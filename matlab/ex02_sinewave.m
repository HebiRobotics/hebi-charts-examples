%% Sinewave
% Select a desired Theme
hebi_charts.Runtime.setTheme('NordDark');

% Create a 1x1 grid window with a single scope (time series chart) and trace
window = hebi_charts.GridWindow('title', 'Python Time Series', 'size', [800 600]);
chart = window.addScope('title', 'Scope', 'ylabel', 'sin');
sinewave = chart.addLine('sine', 'lineStyle', 'Solid');

% Add some basic controls to change the parameters
controls = window.getControlPanel();
controls.width = 300;
amplitude = controls.addSlider('label', 'Amplitude', 'limits', [0 20], 'value', 1);
frequency = controls.addSlider('label', 'Frequency', 'limits', [0.1 10], 'value', 1);

% Initialization
phase = 0;
window.show();

% Use the built-in timer utility for more accurate loop rates
loopTimer = hebi_charts.LoopTimer('frequency', 100); % initializes tic()

%% Sinewave - update
while window.isShowing()
    loopTimer.waitForNextTick();

    % accumulate phase to avoid discontinuities
    dt = loopTimer.ticToc();
    phase = phase + 2 * pi * frequency.value * dt;
    y = amplitude.value * sin(phase);

    % append one point (non-blocking)
    sinewave.addPoint(loopTimer.elapsedTime, y);
end