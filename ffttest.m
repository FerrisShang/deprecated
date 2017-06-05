Fs = 16000;          % Sampling frequency                    
T = 1/Fs;            % Sampling period       
L = 8192;            % Length of signal
t = (0:L-1)*T;       % Time vector

S = sin(2*pi*20*t);

N=1024;
subplot(2,2,1);
plot(S(1:100));
Y = fft(S, N);
P2 = abs(Y/N);
P1 = P2(1:N/2+1);
P1(2:end-1) = 2*P1(2:end-1);
f = Fs*(0:(N/2))/N;
subplot(2,2,2);
plot(f,P1) 


P =ifft(Y);
subplot(2,2,4);
plot(P);
