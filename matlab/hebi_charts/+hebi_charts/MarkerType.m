classdef MarkerType < int32
% auto-generated class for holding MarkerType constants

    enumeration
        Default(0)
        Rectangle(1)
        Rectangle1(2)
        Rectangle2(3)
        Circle(4)
        Circle1(5)
        Circle2(6)
        Plus(7)
        Cross(8)
        Diamond(9)
        Diamond1(10)
        Diamond2(11)
    end

    methods (Static, Access = public)
        function out = toNativeValue(input)

            if isstring(input)
                input = char(input);
            elseif ~ischar(input)
                input = int32(input);
            end

            switch input
                case {'Default', 'hebi_charts_MarkerType_Default', 0}
                    out = 0;
                case {'Rectangle', 'hebi_charts_MarkerType_Rectangle', 1}
                    out = 1;
                case {'Rectangle1', 'hebi_charts_MarkerType_Rectangle1', 2}
                    out = 2;
                case {'Rectangle2', 'hebi_charts_MarkerType_Rectangle2', 3}
                    out = 3;
                case {'Circle', 'hebi_charts_MarkerType_Circle', 4}
                    out = 4;
                case {'Circle1', 'hebi_charts_MarkerType_Circle1', 5}
                    out = 5;
                case {'Circle2', 'hebi_charts_MarkerType_Circle2', 6}
                    out = 6;
                case {'Plus', 'hebi_charts_MarkerType_Plus', 7}
                    out = 7;
                case {'Cross', 'hebi_charts_MarkerType_Cross', 8}
                    out = 8;
                case {'Diamond', 'hebi_charts_MarkerType_Diamond', 9}
                    out = 9;
                case {'Diamond1', 'hebi_charts_MarkerType_Diamond1', 10}
                    out = 10;
                case {'Diamond2', 'hebi_charts_MarkerType_Diamond2', 11}
                    out = 11;
                otherwise
                    error('input is not a valid MarkerType value. Use char or int constants.');
            end

            out = int32(out);
        end

    end

end
