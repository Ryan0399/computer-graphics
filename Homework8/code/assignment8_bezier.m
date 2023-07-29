clear
clc

figure;
t = 0:0.001:1;

%%
h = drawpolyline;
hold on;
hcurve1 = plot(bezier_curve(h.Position, t), 'g', 'linewidth', 2); axis([0 1 0 1])
h.addlistener('MovingROI', @(h, evt) bezier_curve(evt.CurrentPosition, t, hcurve1));

% 准均匀三阶b样条(看错任务要求的副产物)
% hcurve2 = plot(B_spline(h.Position, t, 3), 'r', 'linewidth', 2);axis([0 1 0 1])
% h.addlistener('MovingROI', @(h, evt) B_spline(evt.CurrentPosition, t, 3, hcurve2));

hcurve3 = plot(bezier_spline(h.Position, t), 'y', 'linewidth', 2); axis([0 1 0 1])
h.addlistener('MovingROI', @(h, evt) bezier_spline(evt.CurrentPosition, t, hcurve3));

legend([hcurve1 hcurve3], 'bezier\_curve', 'bezier\_spline');
%%
% function p = bezier(p, t, h)
%     p = p*[1;1i];
%     if nargin>2,set(h, 'xdata', real(p), 'ydata', imag(p)); end
% end
