classdef PixelFormat < int32
% auto-generated class for holding PixelFormat constants

    enumeration
        Unknown(0)
        BGRA_PRE(1)
        BGRA(2)
        ARGB(3)
        RGBA(4)
        RGB(5)
        BGR(6)
        GRAY(7)
    end

    methods (Static, Access = public)
        function out = toNativeValue(input)

            if isstring(input)
                input = char(input);
            elseif ~ischar(input)
                input = int32(input);
            end

            switch input
                case {'Unknown', 'hebi_charts_PixelFormat_Unknown', 0}
                    out = 0;
                case {'BGRA_PRE', 'hebi_charts_PixelFormat_BGRA_PRE', 1}
                    out = 1;
                case {'BGRA', 'hebi_charts_PixelFormat_BGRA', 2}
                    out = 2;
                case {'ARGB', 'hebi_charts_PixelFormat_ARGB', 3}
                    out = 3;
                case {'RGBA', 'hebi_charts_PixelFormat_RGBA', 4}
                    out = 4;
                case {'RGB', 'hebi_charts_PixelFormat_RGB', 5}
                    out = 5;
                case {'BGR', 'hebi_charts_PixelFormat_BGR', 6}
                    out = 6;
                case {'GRAY', 'hebi_charts_PixelFormat_GRAY', 7}
                    out = 7;
                otherwise
                    error('input is not a valid PixelFormat value. Use char or int constants.');
            end

            out = int32(out);
        end

    end

end
