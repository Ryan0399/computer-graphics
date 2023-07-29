function p = bezier_curve(p, t, h)
    % 输入：控制点p、取样参数t、bezier曲线
    % 输出：取样点信息
    B = @(n, i, t) nchoosek(n, i) * (1 - t) .^ (n - i) .* t .^ i;
    N = length(p) - 1;
    x = zeros(1, length(t)); y = zeros(1, length(t));

    for i = 0:N
        x = x + B(N, i, t) * p(i + 1, 1);
        y = y + B(N, i, t) * p(i + 1, 2);
    end

    p = [x.' y.'];
    p = p * [1; 1i];
    if nargin > 2, set(h, 'xdata', real(p), 'ydata', imag(p)); end

end
