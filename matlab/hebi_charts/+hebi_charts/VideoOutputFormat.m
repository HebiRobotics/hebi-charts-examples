classdef VideoOutputFormat < int32
% auto-generated class for holding VideoOutputFormat constants

    enumeration
        H264(0)
        H265(1)
        ProRes(2)
        WebM(3)
        Gif(4)
    end

    methods (Static, Access = public)
        function out = toNativeValue(input)

            if isstring(input)
                input = char(input);
            elseif ~ischar(input)
                input = int32(input);
            end

            switch input
                case {'H264', 'hebi_charts_VideoOutputFormat_H264', 0}
                    out = 0;
                case {'H265', 'hebi_charts_VideoOutputFormat_H265', 1}
                    out = 1;
                case {'ProRes', 'hebi_charts_VideoOutputFormat_ProRes', 2}
                    out = 2;
                case {'WebM', 'hebi_charts_VideoOutputFormat_WebM', 3}
                    out = 3;
                case {'Gif', 'hebi_charts_VideoOutputFormat_Gif', 4}
                    out = 4;
                otherwise
                    error('input is not a valid VideoOutputFormat value. Use char or int constants.');
            end

            out = int32(out);
        end

    end

end
