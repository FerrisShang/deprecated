%[Sa Fs] = audioread('x.wav');
[Sa Fs] = audioread('rec.wav');
Sa = Sa(:,3);

%offset = 5000;
%S = Sa(1+offset:60+offset,3);
S = Sa;
T = length(S)/Fs;       % Sampling period       
L = length(S);          % Length of signal
t = (0:L-1)*T;        % Time vector

X = S ;% + .01*randn(size(t))';

subplot(2,2,1);
plot(t(1:end),X(1:end));
subplot(2,2,2);
plot(t(1:end),S(1:end));

Y = fft(X);
P2 = abs(Y/L);
P1 = P2(1:L/2+1);
P1(2:end-1) = 2*P1(2:end-1);
f = Fs*(0:(L/2))/L;
subplot(2,2,3);
plot(f,P1) 

%filter
Z = Y;
l = 10000;
Z(length(Z)/2+1-l:length(Z)/2+1+l) = zeros(1,l*2+1);
l = 2000;
Z(2:2+l-1) = zeros(1,l);
Z(length(Z)-l+1:length(Z)) = zeros(1,l);

P =ifft(Z);
subplot(2,2,4);
plot(t(1:end), P(1:end));
%axis([1 length(y) -.2 .2]);

%test
Y = fft(P);
P2 = abs(Y/L);
P1 = P2(1:L/2+1);
P1(2:end-1) = 2*P1(2:end-1);
f = Fs*(0:(L/2))/L;
subplot(2,2,4);
plot(f,P1) 

sound(P, Fs);
