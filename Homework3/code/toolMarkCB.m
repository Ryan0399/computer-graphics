function toolMarkCB(h, varargin)

evalin('base', 'delete(hpolys);');

set(h, 'Enable', 'off');

hp1 = drawpolygon(subplot(121));
hp1.set('InteractionsAllowed', 'translate');


hp2 = drawpolygon(subplot(122), 'position', hp1.Position);
hp2.set('InteractionsAllowed', 'translate');

assignin('base', 'hpolys', [hp1; hp2]);

set(h, 'Enable', 'on');

% 实时返回泊松融合的结果
toolPasteCB;
addlistener(hp1, 'MovingROI', @toolPasteCB);
addlistener(hp2, 'MovingROI', @toolPasteCB);