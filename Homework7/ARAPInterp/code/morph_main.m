clear
clc

[x, t] = readObj('elephant_s');
y = readObj('elephant_t');
figure('position', [10 40 1210, 740]); subplot(131); drawmesh(t, x);
subplot(133); drawmesh(t, y);
subplot(132); h = drawmesh(t, x);
% 预处理
global Q R L
[Q, R, T] = ARAP_pretreatment(x, y, t);
L = zeros(2 * length(t), length(x));

for i = 1:length(t)
    L(2 * i - 1, t(i, 1)) = 1; L(2 * i - 1, t(i, 2)) = -1;
    L(2 * i, t(i, 2)) = 1; L(2 * i, t(i, 3)) = -1;
end

global H M
[H, M] = qr(L, "econ");

for w = 0:0.01:1
    %% placeholder: linear interpolating the vertex positions
%     z = (1 - w) * x + w * y;

    %% TODO: finish the ARAP interpolation function
    z = ARAP_interp(T, w);
    %% draw the result
    set(h, 'vertices', z);
    drawnow; % pause(0.01);
end

function h = drawmesh(t, x)
    h = trimesh(t, x(:, 1), x(:, 2), x(:, 1), 'facecolor', 'interp', 'edgecolor', 'k');
    axis equal; axis off; view(2);
end
