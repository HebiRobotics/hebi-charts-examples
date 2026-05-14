classdef MatrixOrdering < int32
% auto-generated class for holding MatrixOrdering constants

    enumeration
        RowMajor(0)
        ColumnMajor(1)
    end

    methods (Static, Access = public)
        function out = toNativeValue(input)

            if isstring(input)
                input = char(input);
            elseif ~ischar(input)
                input = int32(input);
            end

            switch input
                case {'RowMajor', 'hebi_charts_MatrixOrdering_RowMajor', 0}
                    out = 0;
                case {'ColumnMajor', 'hebi_charts_MatrixOrdering_ColumnMajor', 1}
                    out = 1;
                otherwise
                    error('input is not a valid MatrixOrdering value. Use char or int constants.');
            end

            out = int32(out);
        end

    end

end
