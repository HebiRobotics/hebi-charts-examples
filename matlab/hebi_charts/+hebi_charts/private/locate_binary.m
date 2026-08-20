function info = locate_binary()
%LOCATE_BINARY finds the binary path. Downloads if needed.

persistent cachedInfo
if isempty(cachedInfo)

    libName = 'hebi_charts';
    version = '0.9.5';
    buildNumber = '130';

    % Determine appropriate OS parameters
    switch computer('arch')
        case 'win64'
            platform = 'win_amd64';
            libFile = [libName '.dll'];
            cacheHome = getenv('LOCALAPPDATA');
        case 'glnxa64'
            platform = 'linux_amd64';
            libFile = ['lib' libName '.so'];
            cacheHome = getenv('XDG_CACHE_HOME');
            if isempty(cacheHome)
                cacheHome = fullfile(getenv('HOME'), '.cache');
            end
        case 'maci64' % Intel
            platform = 'osx_amd64';
            libFile = ['lib' libName '.dylib'];
            cacheHome = fullfile(getenv('HOME'), 'Library', 'Caches');
        case 'maca64' % Apple Silicon
            platform = 'osx_arm64';
            libFile = ['lib' libName '.dylib'];
            cacheHome = fullfile(getenv('HOME'), 'Library', 'Caches');
        otherwise
            error('Unsupported architecture.')
    end

    % Prioritize locally bundled lib binaries (dev / offline)
    thisFile = mfilename('fullpath');
    thisDir = fileparts(thisFile);
    rootDir = fullfile(thisDir, '..', '..');
    binDir = fullfile(rootDir, 'lib', platform);
    binPath = fullfile(binDir, libFile);

    if ~exist(binPath, 'file')

        % Fall back to shared downloaded version in local cache. We also
        % use the bundled include files, so we can bundle the language
        % specific files without any headers.
        downloadDir = fullfile(cacheHome, libName, [version '-build' buildNumber]);
        rootDir = fullfile(downloadDir, libName);
        binDir = fullfile(rootDir, 'lib', platform);
        binPath = fullfile(binDir, libFile);

        % Otherwise download it into a shared dir. We use the OS cache
        % so that the binaries can be shared and don't pollute the root dir.
        if ~exist(binPath, 'file')
            download_binary(downloadDir, [libName '-' version '-' platform '.zip']);
        end

    end

    % Sanity Check
    if ~exist(binPath, 'file')
        error('Required binary not found: %s', libPath);
    end

    info = struct();
    info.binDir = binDir;
    info.binPath = binPath;
    info.includeDir = fullfile(rootDir, 'include');
    info.headerFile = fullfile(info.includeDir, [libName '_c.h']);
    info.matlabHeader = fullfile(thisDir, [libName '_matlab.h']);
    cachedInfo = info;

end

info = cachedInfo;
end
