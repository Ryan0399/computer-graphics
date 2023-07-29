function [Q, R, T] = ARAP_pretreatment(x, y, t)
    % 对网格x,y的Jacobi矩阵进行预处理
    % 输出每一个三角面片对应的旋转矩阵R、伸缩矩阵Q和相对坐标T
    n = length(t);
    Q = zeros(2 * n, 2); R = zeros(1, n); T = zeros(2 * n, 2);
    x2d = x(:, 1:2); y2d = y(:, 1:2);

    for i = 1:n
        T(2 * i - 1, :) = x2d(t(i, 1), :) - x2d(t(i, 2), :);
        T(2 * i, :) = x2d(t(i, 2), :) - x2d(t(i, 3), :);
        relaA = [T(2 * i - 1, :); T(2 * i, :)].';
        tempY1 = y2d(t(i, 1), :) - y2d(t(i, 2), :);
        tempY2 = y2d(t(i, 2), :) - y2d(t(i, 3), :);
        relaB = [tempY1; tempY2].';
        A = relaB / relaA;
        [U, S, V] = svd(A);
        tempR = U * V.'; Q(2 * i - 1:2 * i, :) = V * S * V.'; % 修改R，值传递修改为旋转角theta
        R(i) = atan2(tempR(1, 2), tempR(1, 1));

    end

    % 解决旋转一致性问题
    R = rotation_consistency(R, t, x2d, y2d); % 自己的方法


end

function R = rotation_consistency(R, t, x, y)
    areaX = zeros(length(t), 1); areaY = zeros(length(t), 1);

    for i = 1:length(t)
        areaX(i) = [area(x(t(i, 1), :), x(t(i, 2), :), x(t(i, 3), :))];
        areaY(i) = [area(y(t(i, 1), :), y(t(i, 2), :), y(t(i, 3), :))];
    end

    areaW = (areaX + areaY) / 2;
    areaW = areaW / sum(areaW);
    ang = R * areaW;
    Rstd = abs(R - ang) * areaW;

    for i = 1:length(t)

        if R(i) * ang < 0 && abs(R(i) - ang) > Rstd
            R(i) = R(i) + sign(ang) * 2 * pi;
        end

    end

end

function s = area(x1, x2, x3)
    a = norm(x1 - x2); b = norm(x2 - x3); c = norm(x3 - x1);
    p = (a + b + c) / 2;
    s = sqrt(p * (p - a) * (p - b) * (p - c));
end



