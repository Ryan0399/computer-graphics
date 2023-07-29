function z = ARAP_interp(T, w)
    global R Q H M
    n = length(R); % 网格面的个数
    m = size(M, 2); % 顶点的个数

    for i = 1:n
        localR = [cos(w * R(i)) sin(w * R(i)); -sin(w * R(i)) cos(w * R(i))];
        A = localR * ((1 - w) * eye(2) + w * Q(2 * i - 1:2 * i, :));
        T(2 * i - 1:2 * i, :) = T(2 * i - 1:2 * i, :) * A.';
    end

    z = M \ H.' * T;

    % global L
    %     z=linsolve(L,T);
    z = z - z(1, :);
    z = [z zeros(m, 1)];
end
