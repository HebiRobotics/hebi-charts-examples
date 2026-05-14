classdef RuntimeOption < int32
% auto-generated class for holding RuntimeOption constants

    enumeration
        DpiScale(0)
        GraphicsPipeline(1)
        TextAntialiasing(2)
        VSync(3)
        VerboseGraphics(4)
        VerbosePulse(5)
        VerboseFonts(6)
        VerboseWindowing(7)
    end

    methods (Static, Access = public)
        function out = toNativeValue(input)

            if isstring(input)
                input = char(input);
            elseif ~ischar(input)
                input = int32(input);
            end

            switch input
                case {'DpiScale', 'hebi_charts_RuntimeOption_DpiScale', 0}
                    out = 0;
                case {'GraphicsPipeline', 'hebi_charts_RuntimeOption_GraphicsPipeline', 1}
                    out = 1;
                case {'TextAntialiasing', 'hebi_charts_RuntimeOption_TextAntialiasing', 2}
                    out = 2;
                case {'VSync', 'hebi_charts_RuntimeOption_VSync', 3}
                    out = 3;
                case {'VerboseGraphics', 'hebi_charts_RuntimeOption_VerboseGraphics', 4}
                    out = 4;
                case {'VerbosePulse', 'hebi_charts_RuntimeOption_VerbosePulse', 5}
                    out = 5;
                case {'VerboseFonts', 'hebi_charts_RuntimeOption_VerboseFonts', 6}
                    out = 6;
                case {'VerboseWindowing', 'hebi_charts_RuntimeOption_VerboseWindowing', 7}
                    out = 7;
                otherwise
                    error('input is not a valid RuntimeOption value. Use char or int constants.');
            end

            out = int32(out);
        end

    end

end
