function varargout = hebi_charts_native(funcName, varargin)
% hebi_charts_native MEX setup call that gets shadowed once the MEX is available

%% Getting here means that the mex file is not on the path or not compatible
mexName = ['hebi_charts_native.' mexext];
thisDir = fileparts(mfilename('fullpath'));
localMex = fullfile(thisDir, mexName);

%% We first try to get it from the downloaded distribution
info = locate_binary();
shippedMex = fullfile(info.binDir, mexName);
if exist(shippedMex, 'file')
    try

        % Copy and load mex file
        fprintf('Installing pre-compiled MEX from: %s\n', mexName);
        copyfile(shippedMex, localMex);
        clear('hebi_charts_native');
        rehash;

        % Try using it
        if exist('hebi_charts_native', 'file') == 3 % MEX_FILE
            [varargout{1:nargout}] = hebi_charts_native(funcName, varargin{:});
            return;
        end

    catch
        % Not compatible -> proceed with building from scratch
        if exist(localMex, 'file')
            fprintf('Removing incompatible MEX file: %s\n', localMex);
            delete(localMex);
        end
    end
end

%% Compile for the target platform
try
    build_native();
catch ME
    error('hebi_charts:BuildError', 'Could not build MEX: %s', ME.message);
end

% Verify that MATLAB finds the new MEX function
if exist('hebi_charts_native', 'file') ~= 3 % MEX_FILE
    error('Newly compiled MEX file not found');
end

% Delegate to the newly compiled method
try
    [varargout{1:nargout}] = hebi_charts_native(funcName, varargin{:});
catch ME
    error('MEX compiled but failed to execute: %s', ME.message);
end

end