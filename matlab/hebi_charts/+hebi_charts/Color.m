classdef Color < int32
% auto-generated class for holding Color constants

    enumeration
        Default(0)
        Black(1)
        White(2)
        Gray(3)
        Red(4)
        Green(5)
        Blue(6)
        Yellow(7)
        Magenta(8)
    end

    methods (Static, Access = public)
        function out = toNativeValue(input)

            if isstring(input)
                input = char(input);
            elseif ~ischar(input)
                input = int32(input);
            end

            switch input
                case {'Default', 'hebi_charts_Color_Default', 0}
                    out = 0;
                case {'Black', 'hebi_charts_Color_Black', 1}
                    out = 1;
                case {'White', 'hebi_charts_Color_White', 2}
                    out = 2;
                case {'Gray', 'hebi_charts_Color_Gray', 3}
                    out = 3;
                case {'Red', 'hebi_charts_Color_Red', 4}
                    out = 4;
                case {'Green', 'hebi_charts_Color_Green', 5}
                    out = 5;
                case {'Blue', 'hebi_charts_Color_Blue', 6}
                    out = 6;
                case {'Yellow', 'hebi_charts_Color_Yellow', 7}
                    out = 7;
                case {'Magenta', 'hebi_charts_Color_Magenta', 8}
                    out = 8;
                otherwise
                    error('input is not a valid Color value. Use char or int constants.');
            end

            out = int32(out);
        end

    end

end
