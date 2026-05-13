%% ex03_robot_3d.m
%% Setup
% Select a desired Theme
hebi_charts.Runtime.setTheme('NordDark');

% 3x1 Grid: Top 2/3 = 3D robot view, Bottom 1/3 = Scope
window = hebi_charts.GridWindow(3, 'title', 'Python Robot', 'size', [1000 600]);

% 3D Robot
scene = window.addScene3d(1,1,2,1, 'gridMinZ', 0); % row,col,rowSpan,colSpan
robot = scene.addRobot('https://raw.githubusercontent.com/HebiRobotics/hebi-hrdf/main/A-2240-06G.hrdf');

% 2D Scope
chart = window.addScope(3, 'title', 'Position', 'yunit', 'rad');
jointTraces = cell(1, robot.dof);
for i = 1:robot.dof
    jointTraces{i} = chart.addLine(['Joint ' num2str(i)], 'lineStyle', 'Solid');
end

%% Basic controls
controls = window.getControlPanel();
controls.addSection('Styling');

% Enum choices
[~, themes] = enumeration('hebi_charts.Theme');
theme = controls.addDropdown('options', themes, 'label', 'Theme', 'selectedIndex', 4);

[~, displayStyles] = enumeration('hebi_charts.DisplayStyle');
displayStyle = controls.addDropdown('label', 'Style', 'options', displayStyles, 'selectedIndex', 1);

% Joint sliders
controls.addSection('Positions [rad]');
jointSliders = cell(1, robot.dof);
for i = 1:robot.dof
    jointSliders{i} = controls.addSlider('label', ['Joint ' num2str(i)], 'limits', [-pi pi], 'value', 0);
end
btnZero = controls.addStartButton('text', 'Zero');
btnRand = controls.addStopButton('text', 'Random');

%% Main loop
window.show();
loopTimer = hebi_charts.LoopTimer('frequency', 100);
while window.isShowing()
    loopTimer.waitForNextTick();

    % Poll for user input
    if theme.wasChanged()
        hebi_charts.Runtime.setTheme(themes{theme.selectedIndex + 1});
    end

    if displayStyle.wasChanged()
        robot.displayStyle = displayStyles{displayStyle.selectedIndex + 1};
    end

    if btnZero.wasPressed()
        for i = 1:numel(jointSliders)
            jointSliders{i}.value = 0.0;
        end
    end

    if btnRand.wasPressed()
        for i = 1:numel(jointSliders)
            jointSliders{i}.value = rand() * (2 * pi) - pi;
        end
    end

    % Update the 3D positions
    positions = zeros(1, robot.dof);
    for i = 1:robot.dof
        positions(i) = jointSliders{i}.value;
    end
    robot.setPositions(positions);

    % Update the 2D scope traces
    t = loopTimer.elapsedTime;
    for i = 1:numel(positions)
        jointTraces{i}.addPoint(t, positions(i));
    end
end
