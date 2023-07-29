function p = B_spline(p, t, d, h)
    % 输入：控制点p、取样参数t、bezier曲线、阶数d
    % 输出：取样点信息
    n = size(p, 1);
    m = n + d;
    u = zeros(1, m);
    q = zeros(length(t), 2);

    for i = d + 1:n + 1
        u(i) = u(i - 1) + 1 / (n - d + 1);
    end

    for i = n + 1:n + d
        u(i) = 1;
    end

    for i = 1:length(t)

        for r = d:n + 1

            if t(i) >= u(r) && t(i) <= u(r + 1)
                break
            end

        end

        f = zeros(d, r, 2);

        for k = r - d + 1:r
            f(1, k, :) = p(k, :);
        end

        for j = 2:d

            for k = r - d + j:r
                f(j, k, :) = (1 - (t(i) - u(k)) / (u(k + d - j + 1) - u(k))) * f(j - 1, k - 1, :) + (t(i) - u(k)) / (u(k + d - j + 1) - u(k)) * f(j - 1, k, :);
            end

        end

        q(i, :) = f(d, r, :);
    end

    p = q;
    p = p * [1; 1i];
    if nargin > 3, set(h, 'xdata', real(p), 'ydata', imag(p)); end
end
