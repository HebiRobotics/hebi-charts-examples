classdef CameraView < int32
% auto-generated class for holding CameraView constants

    enumeration
        ISOMETRIC(0)
        FRONT(1)
        BACK(2)
        LEFT(3)
        RIGHT(4)
        TOP(5)
        BOTTOM(6)
    end

    methods (Static, Access = public)
        function out = toNativeValue(input)

            if isstring(input)
                input = char(input);
            elseif ~ischar(input)
                input = int32(input);
            end

            switch input
                case {'ISOMETRIC', 'hebi_charts_CameraView_ISOMETRIC', 0}
                    out = 0;
                case {'FRONT', 'hebi_charts_CameraView_FRONT', 1}
                    out = 1;
                case {'BACK', 'hebi_charts_CameraView_BACK', 2}
                    out = 2;
                case {'LEFT', 'hebi_charts_CameraView_LEFT', 3}
                    out = 3;
                case {'RIGHT', 'hebi_charts_CameraView_RIGHT', 4}
                    out = 4;
                case {'TOP', 'hebi_charts_CameraView_TOP', 5}
                    out = 5;
                case {'BOTTOM', 'hebi_charts_CameraView_BOTTOM', 6}
                    out = 6;
                otherwise
                    error('input is not a valid CameraView value. Use char or int constants.');
            end

            out = int32(out);
        end

    end

end
