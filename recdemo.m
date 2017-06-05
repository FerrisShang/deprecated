recObj = audiorecorder;
recordblocking(recObj, 5);
S = getaudiodata(recObj);
plot(S);
axis([1 length(S) -1 1]);
