classdef Theme < int32
% auto-generated class for holding Theme constants

    enumeration
        PrimerLight(0)
        PrimerDark(1)
        NordLight(2)
        NordDark(3)
        CupertinoLight(4)
        CupertinoDark(5)
        Dracula(6)
    end

    methods (Static, Access = public)
        function out = toNativeValue(input)

            if isstring(input)
                input = char(input);
            elseif ~ischar(input)
                input = int32(input);
            end

            switch input
                case {'PrimerLight', 'hebi_charts_Theme_PrimerLight', 0}
                    out = 0;
                case {'PrimerDark', 'hebi_charts_Theme_PrimerDark', 1}
                    out = 1;
                case {'NordLight', 'hebi_charts_Theme_NordLight', 2}
                    out = 2;
                case {'NordDark', 'hebi_charts_Theme_NordDark', 3}
                    out = 3;
                case {'CupertinoLight', 'hebi_charts_Theme_CupertinoLight', 4}
                    out = 4;
                case {'CupertinoDark', 'hebi_charts_Theme_CupertinoDark', 5}
                    out = 5;
                case {'Dracula', 'hebi_charts_Theme_Dracula', 6}
                    out = 6;
                otherwise
                    error('input is not a valid Theme value. Use char or int constants.');
            end

            out = int32(out);
        end

    end

end
