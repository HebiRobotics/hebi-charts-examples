function build_native()
% BUILD_NATIVE compiles the MEX dispatcher

% Make there is a C++ compiler
if isempty(mex.getCompilerConfigurations('C++', 'Installed'))
    disp(' ');
    warning('hebi_charts requires a compatible C++ compiler.');
    disp('');
    disp('== Installation Instructions ==');
    disp('* Windows: (Option A) Install "MinGW-w64" from the MATLAB Add-Ons menu or from:');
    disp('           https://mathworks.com/matlabcentral/fileexchange/52848-matlab-support-for-mingw-w64-c-c-fortran-compiler.');
    disp('           (Option B) Install Visual Studio Community (2019 or 2022)');
    disp('           and select the "Desktop development with C++" workload.');
    disp(' ');
    disp('* macOS: Install the Xcode command-line tools. Open the Terminal and run:');
    disp('         xcode-select --install');
    disp('         (Then restart MATLAB)');
    disp(' ');
    disp('* Linux: Install GCC (and headers) via your package manager. On Ubuntu/Debian:');
    disp('         sudo apt update && sudo apt install build-essential');
    disp(' ');
    disp('After installation, you may need to run "mex -setup" in the MATLAB Command Window to configure the compiler.');
    disp('For more details see: https://mathworks.com/support/requirements/supported-compilers.html');
    mex('-setup', 'C++'); % provide a version specific error message
end

% Make sure the C++ compiler is actually selected
if isempty(mex.getCompilerConfigurations('C++', 'Selected'))
    warning('hebi_charts:CompilerNotSelected', 'C++ compiler is installed but not configured. Running "mex -setup C++" for you.');
    try
        mex('-setup', 'C++');
        if isempty(mex.getCompilerConfigurations('C++', 'Selected'))
            error('Manual setup required.');
        end
    catch
        error('hebi_charts:SetupFailed', 'Automatic compiler setup failed. Please run "mex -setup C++" manually.');
    end
end

% Determine dirs
thisDir = fileparts(mfilename('fullpath'));
funcName = 'hebi_charts_native';
mexSrc = fullfile(thisDir, [funcName '.cpp']);

% Compile
try
    % Pick the highest supported API flag
    if verLessThan('matlab', '9.4')
        apiFlag = '-largeArrayDims'; % >= R2006b
    else
        apiFlag = '-R2018a'; % Modern Interleaved API
    end

    args = {
        '-O', ... % Optimizations On
        apiFlag, ...
        mexSrc, ...
        '-outdir', thisDir ...
        };
    mex(args{:});
catch ME
    error('Failed to compile MEX: %s', ME.message);
end

% Force MATLAB to reload the function cache
clear(funcName);
rehash;

end