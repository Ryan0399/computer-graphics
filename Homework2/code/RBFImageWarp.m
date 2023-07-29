function im2 = RBFImageWarp(im, psrc, pdst)

    % input: im, psrc, pdst

    %% basic image manipulations
    % get image (matrix) size：h*w*dim [h, w, dim] = size(im);
    %
    % im2 = im;

    %% use loop to negate image
    % for i=1:h
    %     for j=1:w
    %         for k=1:dim
    %             im2(i, j, k) = 255 - im(i, j, k);
    %         end
    %     end
    % end

    %% TODO: compute warpped image
    A = fRBF(psrc, psrc);

    %因为pdst=A*coef
    coef = A \ (pdst - psrc);
    [h, w, ~] = size(im);
    %由meshgrid函数生成的xpix和ypix是两个大小相同的矩阵h*w
    %xpix是由1:w复制h行得到，ypix是由1:h复制w列得到
    [xpix, ypix] = meshgrid(1:w, 1:h);
    %(:)将矩阵中的每一列合并成一个长向量，[,]将两个数组水平拼接 所以x是一个(h*w)*2的矩阵
    x = [xpix(:), ypix(:)];

    %% 每个源像素的映射位置x->f(x)
    B = fRBF(psrc, x);
    q0 = B * coef;
    q = floor(q0) + x;
    %找到所有满足坐标0<x<=w，0<y<=h并记为1，其余记为0
    pFlag = all(q > 0, 2) & (q(:, 1) <= w) & (q(:, 2) <= h);
    %sub2ind函数将下标转换为线性索引
    mapPId = zeros(h * w, 1);
    sign=zeros(h*w,1);

    for i = 1:h * w

        if pFlag(i, 1) == 1
            %注意，在h*w矩阵中q(i,2)是行下标，q(i,1)是列下标
            mapPId(i, 1) = sub2ind([h, w], q(i, 2), q(i, 1));
        end

    end

    %%
    im = reshape(im, [h * w 3]);
    

    im2 = ones(h * w, 3, 'uint8') * 255;
    km = ones(1, 3, 'uint8') * 255;
    %     for i=1:h*w
    %         im2(i,:)=im(i,:);
    %     end
    for i = 1:h * w

        if mapPId(i, 1) ~= 0
            im2(mapPId(i, 1), :) = im(i, :);
            sign(mapPId(i,1),1)=1;
        end

    end
      %% 对白色缝隙进行填补
%     k = 1001;
% 
%     while k > 1000
%         k = 0;

        for i = h + 2:h * w - h - 1

            if judge(im2(i, :), km) == 1 && sign(i,1)==0

                for j = 1:3
                    im2(i, j) = min([im2(i + 1 + h, j), im2(i - 1 - h, j)]) / 4+ ...
                        min([im2(i - h, j), im2(i + h, j)]) / 4 + min([im2(i + 1, j), ...
                        im2(i - 1, j)]) / 4 + min([im2(i + 1 - h, j), im2(i - 1 + h, j)]) / 4;
                end

%                 k = k + 1;
            end

        end

%     end

    im2 = reshape(im2, [h w 3]);
end
