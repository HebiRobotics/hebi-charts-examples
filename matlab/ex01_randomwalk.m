%% Random Walk

% Select a desired Theme
hebi_charts.Runtime.setTheme('NordDark');

% Create a 1x1 grid window with a single chart element
window = hebi_charts.GridWindow('title', 'Python Random Walk', 'size', [800 600]);

% Add a line chart element to the window
chart = window.addLineChart( ...
    'title', 'Random Walk', ...
    'xlabel', 'sample', ...
    'ylabel', 'rnd(x)');

% Add a trace with solid line style and max point limit
line = chart.addLine('walk', ...
    'lineStyle', 'Solid', ...
    'maxPointCount', 10000);

%% Update loop
x = 0;
y = 0;
window.show();
while window.isShowing()
    % Reduce the rate (optional)
    pause(0.001);

    % Append one point (non-blocking)
    x = x + 1;
    y = y + (rand() * 2 - 1);
    line.addPoint(x, y);
end