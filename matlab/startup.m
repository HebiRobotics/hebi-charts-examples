% startup.m - gets executed when matlab starts in this directory

% Absolute path to the target library
libDir = fullfile(fileparts(mfilename('fullpath')), 'hebi_charts');

% Add if it wasn't already added
if ~contains(path, libDir)
    addpath(libDir);
end