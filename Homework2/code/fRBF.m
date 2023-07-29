function A = fRBF(p, x)
    n = size(p, 1);
    m = size(x, 1);
    A = zeros(m, n);
    d = 1000;
    %%A=(b_i(x_j))_{n*n}
    for i = 1:m

        for j = 1:n
            A(i, j) = 1 / (norm(p(j, :) - x(i, :)) ^ 2 + d);
        end

    end

end
