classdef DisplayStyle < int32
% auto-generated class for holding DisplayStyle constants

    enumeration
        Original(0)
        Translucent(1)
        Ghosted(2)
    end

    methods (Static, Access = public)
        function out = toNativeValue(input)

            if isstring(input)
                input = char(input);
            elseif ~ischar(input)
                input = int32(input);
            end

            switch input
                case {'Original', 'hebi_charts_DisplayStyle_Original', 0}
                    out = 0;
                case {'Translucent', 'hebi_charts_DisplayStyle_Translucent', 1}
                    out = 1;
                case {'Ghosted', 'hebi_charts_DisplayStyle_Ghosted', 2}
                    out = 2;
                otherwise
                    error('input is not a valid DisplayStyle value. Use char or int constants.');
            end

            out = int32(out);
        end

    end

end
