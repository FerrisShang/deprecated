[Sa Fs] = audioread('x.wav');

sample_ms = 50;
overlap_ms = 20;

L = sample_ms/1000 * Fs;   % Length of signal
T = 1/Fs;             % Sampling period       
t = (0:L-1)*T;        % Time vector
S = Sa;
SND = zeros(size(S));

for cnt = 1:L-overlap_ms/1000*Fs:length(S)-L
	 X = S(cnt:cnt+L-1) ;% + .01*randn(size(t))';
	 %X = X.*hann(L);
	 subplot(2,2,1);
	 plot(t,X);
	 Y = fft(X);
	 P2 = abs(Y/L);
	 P1 = P2(1:L/2+1);
	 P1(2:end-1) = 2*P1(2:end-1);
	 f = Fs*(0:(L/2))/L;
	 subplot(2,2,2);
	 plot(f,P1) 

	 %filter
	 Z = Y;
	 P =ifft(Z);
	 subplot(2,2,3);
	 plot(t, P);
	 drawnow;
	 SND(cnt:cnt+L-1) = SND(cnt:cnt+L-1) + X;
	 band_ms = 100;
	 sound(S(cnt:cnt+L-1+band_ms/1000*Fs), Fs);
	 pause(0.5);
end;
