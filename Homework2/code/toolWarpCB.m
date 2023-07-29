function toolWarpCB(varargin)

    %   Copyright © 2021, Renjie Chen @ USTC
    %% evalin函数用于执行指定空间中的命令
    hlines = evalin('base', 'hToolPoint.UserData');
    im = evalin('base', 'im');
    himg = evalin('base', 'himg');

    %%numel函数返回hlines中元素的个数
    p2p = zeros(numel(hlines) * 2, 2);

    for i = 1:numel(hlines)
        p2p(i * 2 + (-1:0), :) = hlines(i).getPosition();
    end

    %%输入起始点的二维平面坐标
    im2 = RBFImageWarp(im, p2p(1:2:end, :), p2p(2:2:end, :));
    set(himg, 'CData', im2);
