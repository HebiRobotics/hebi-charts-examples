classdef MarkerShape < int32
% auto-generated class for holding MarkerShape constants

    enumeration
        Default(0)
        Tetrahedron(1)
        Cube(2)
        Octahedron(3)
        Icosahedron(4)
        Sphere(5)
    end

    methods (Static, Access = public)
        function out = toNativeValue(input)

            if isstring(input)
                input = char(input);
            elseif ~ischar(input)
                input = int32(input);
            end

            switch input
                case {'Default', 'hebi_charts_MarkerShape_Default', 0}
                    out = 0;
                case {'Tetrahedron', 'hebi_charts_MarkerShape_Tetrahedron', 1}
                    out = 1;
                case {'Cube', 'hebi_charts_MarkerShape_Cube', 2}
                    out = 2;
                case {'Octahedron', 'hebi_charts_MarkerShape_Octahedron', 3}
                    out = 3;
                case {'Icosahedron', 'hebi_charts_MarkerShape_Icosahedron', 4}
                    out = 4;
                case {'Sphere', 'hebi_charts_MarkerShape_Sphere', 5}
                    out = 5;
                otherwise
                    error('input is not a valid MarkerShape value. Use char or int constants.');
            end

            out = int32(out);
        end

    end

end
