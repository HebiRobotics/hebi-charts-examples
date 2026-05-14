classdef LineStyle < int32
% auto-generated class for holding LineStyle constants

    enumeration
        Default(0)
        Solid(1)
        Dashed(2)
        Points(3)
    end

    methods (Static, Access = public)
        function out = toNativeValue(input)

            if isstring(input)
                input = char(input);
            elseif ~ischar(input)
                input = int32(input);
            end

            switch input
                case {'Default', 'hebi_charts_LineStyle_Default', 0}
                    out = 0;
                case {'Solid', 'hebi_charts_LineStyle_Solid', 1}
                    out = 1;
                case {'Dashed', 'hebi_charts_LineStyle_Dashed', 2}
                    out = 2;
                case {'Points', 'hebi_charts_LineStyle_Points', 3}
                    out = 3;
                otherwise
                    error('input is not a valid LineStyle value. Use char or int constants.');
            end

            out = int32(out);
        end

    end

end
