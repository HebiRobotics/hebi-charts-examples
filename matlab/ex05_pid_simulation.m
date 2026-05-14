%% PID Controller Simulation
% Select a desired Theme
hebi_charts.Runtime.setTheme('NordDark');

% Create a 1x1 grid window with a single scope chart
window = hebi_charts.GridWindow('title', 'PID Controller Simulation', 'size', [1200 800]);
chart = window.addScope('title', 'System Response', 'ylabel', 'Position', 'ylim', [nan, nan]);
targetLine = chart.addLine('Target Setpoint');
stateLine = chart.addLine('Actual State');

% Setup interactive tuning controls
controls = window.getControlPanel('title', 'PID Simulation');

controls.addSection('Target');
setpoint = controls.addSlider('label', 'Setpoint', 'limits', [-5 5], 'value', 1);
autoFlip = controls.addToggle('label', 'Auto Flip', 'selected', true);

controls.addSection('PID Gains');
kp = controls.addSlider('label', 'Kp (Proportional)', 'limits', [0 100], 'value', 5);
ki = controls.addSlider('label', 'Ki (Integral)', 'limits', [0 50], 'value', 0);
kd = controls.addSlider('label', 'Kd (Derivative)', 'limits', [0 10], 'value', 0);
i_clamp = controls.addToggle('label', 'i Clamp', 'selected', true, 'tooltip', 'Clamps the i windup');

controls.addSection('System');
mass = controls.addSlider('label', 'Mass', 'limits', [0.01 1], 'value', 0.1);
damping = controls.addSlider('label', 'Damping', 'limits', [0 5], 'value', 1.5);
stiffness = controls.addSlider('label', 'Stiffness', 'limits', [0 20], 'value', 0);

% State variables
state = 0.0;
velocity = 0;
integral = 0.0;
lastError = 0.0;

%% Main execution loop
flipTimer = hebi_charts.LoopTimer();
loopTimer = hebi_charts.LoopTimer('frequency', 200);
window.show();
while window.isShowing()
    loopTimer.waitForNextTick();
    dt = loopTimer.ticToc();

    %% Auto-flip target
    if autoFlip.selected && flipTimer.elapsedTime > 2
        flipTimer.restart();
        setpoint.value = -setpoint.value;
    end

    %% Update PID terms
    target = setpoint.value;
    error = target - state;
    integral = integral + error * dt;
    derivative = (error - lastError) / dt;
    lastError = error;

    % Anti-windup
    if i_clamp.selected
        integral = max(-1, min(1, integral));
    end

    %% Compute the PID controller output
    pidEffort = (kp.value * error) + (ki.value * integral) + (kd.value * derivative);

    %% Standard Textbook Mass-Spring-Damper Plant Model
    springForce = stiffness.value * state;
    acceleration = (pidEffort - (damping.value * velocity) - springForce) / mass.value;
    velocity = velocity + acceleration * dt;
    state = state + velocity * dt;
    
    %% Update chart lines
    t = loopTimer.elapsedTime;
    targetLine.addPoint(t, target);
    stateLine.addPoint(t, state);
end

