function p = bezier_spline(p, t, h)
    % 输入：控制点p、取样参数t、3阶bezier样条
    % 输出：取样点信息
    B = @(n, i, t) nchoosek(n, i) * (1 - t) .^ (n - i) .* t .^ i;
    n = size(p, 1);
    m = 3 * n - 2;
    % 计算控制点
    D = zeros(m);
    D(1, 1) = 1;
    D(2, 1) = 1; D(2, 2) = -2; D(2, 3) = 1;

    for k = 1:n - 2
        D(3 * k, 3 * k - 1) = 1; D(3 * k, 3 * k) = -2; D(3 * k, 3 * k + 2) = 2; D(3 * k, 3 * k + 3) = -1;
        D(3 * k + 1, 3 * k + 1) = 1;
        D(3 * k + 2, 3 * k) = 1; D(3 * k + 2, 3 * k + 1) = -2; D(3 * k + 2, 3 * k + 2) = 1;
    end

    D(m - 1, m - 2) = 1; D(m - 1, m - 1) = -2; D(m - 1, m) = 1;
    D(m, m) = 1;
    b = zeros(m, 2);

    for k = 0:n - 1
        b(3 * k + 1, :) = p(k + 1, :);
    end

    b = D \ b;
    u = linspace(0, 1, n);
    q = zeros(length(t), 2);
    % 计算bezier样条
    for i = 1:length(t)

        for r = 1:n - 1

            if t(i) >= u(r) && t(i) <= u(r + 1)
                break
            end

        end

        x = 0; y = 0;

        for j = 0:3
            s = (t(i) - u(r)) * (n - 1);
            x = x + B(3, j, s) * b(3 * r - 2 + j, 1);
            y = y + B(3, j, s) * b(3 * r - 2 + j, 2);
        end

        q(i, :) = [x y];
    end

    p = q;
    p = p * [1; 1i];
    if nargin > 2, set(h, 'xdata', real(p), 'ydata', imag(p)); end
end
