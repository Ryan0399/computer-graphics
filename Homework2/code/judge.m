function bool = judge(im, km)
    a = 0;

    for i = 1:3

        if im(i) == km(i)
            a = a + 1;
        end

    end

    if a == 3
        bool = 1;
    else
        bool = 0;
    end

end
