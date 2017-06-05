Fs = 1000;            % Sampling frequency                    
T = 1/Fs;             % Sampling period       
L = 1000;             % Length of signal
t = (0:L-1)*T;        % Time vector

S = 0.7*sin(2*pi*50*t) + sin(2*pi*120*t);

X = S + 2*randn(size(t));
%X = (S + 2*randn(size(t))).*hann(size(t,2))';

subplot(2,2,1);
plot(t(400:500),X(400:500));
axis([t(400) t(500) -6 6])
title('Signal Corrupted with Zero-Mean Random Noise')
xlabel('t (milliseconds)')
ylabel('X(t)')

subplot(2,2,2);
plot(t(400:500),S(400:500));
axis([t(400) t(500) -3 3])

Y = fft(X);
P2 = abs(Y/L);
P1 = P2(1:L/2+1);
P1(2:end-1) = 2*P1(2:end-1);

f = Fs*(0:(L/2))/L;
subplot(2,2,3);
plot(f,P1) 
title('Single-Sided Amplitude Spectrum of X(t)')
xlabel('f (Hz)')
ylabel('|P1(f)|')

Z = Y .* (abs(Y/L)>0.20);

P =ifft(Z);
subplot(2,2,4);
plot(t(400:500), P(400:500));
axis([t(400) t(500) -3 3])
var(P-S)
var(P2(2:501)-flip(P2(501:end)))

