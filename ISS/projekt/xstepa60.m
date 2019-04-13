pkg load signal;

function ukazmito(b,a,Fs)
% UPRAVENA FUNKCE ZE STUDIJNI ETAPY

% function ukazmito(b,a)
%
% - ukaze kmit charku pro zadanou vzorkovaci frekvenci
% - ukaze poly a nuly a zjisti stabilitu 

N = 32; n = 0:N-1; imp = [1 zeros(1,N-1)]; 
H = freqz(b,a,256); f=(0:255) / 256 * Fs / 2;

fig = figure('Visible', 'off'); 
plot (f,abs(H)); grid; xlabel('f'); ylabel('|H(f)|');
print(fig, '-dpng', '4.png');
close(fig);

fig = figure('Visible', 'off'); 
zplane (b,a);
print(fig, '-dpng', '3.png');
close(fig);
 
p = roots(a); 
if (isempty(p) | abs(p) < 1) 
  disp('Filtr je stabilni!');
else
  disp('Filtr je nestabilni!');
end

endfunction


%%%%%%%%%%%%%%%%%%%%%%% UKOL1 - OK
% Nactete vas signal, napiste vzorkovaci frekvenci signalu, delku ve vzorcich,
% v sekundach a pocet reprezentovanych binarnich symbolu
fprintf("PRIKLAD 1:\n");

% nacteni wav souboru
[sig, fs] = audioread('xstepa60.wav');
% ziskani radkoveho vektoru
sig = sig';
% delka signalu 
sig_len = length(sig);

fprintf("Vzorkovaci frekvence: %f [Hz].\n", fs);
fprintf("Pocet vzorku: %f\n", sig_len);
fprintf("Pocet sekund: %f [s]\n", sig_len/fs);
fprintf("Pocet binarnich symbolu %f\n", sig_len / 16);

%%%%%%%%%%%%%%%%%%%%%%%% UKOL2 - OK
% Provedte dekodovani s[n] do binarnich symbolu.
% Vezmete kazdy osmi vzorek za segmentu 16ti vzorku a pokud je >0 je vystup 1,
% pokud je < 0, je vystupem 0.
% Do protokolu dejte obrazek prvnich 20 ms audio signalu s vyznacenymi 
% dekodovanymi symboly. 
fprintf("PRIKLAD 2:\n");

decoded_sig = [];

index = 1;
for j = sig(8:16:sig_len) % zacneme na vzorku c.8 a posouvame se po 16    
    if j > 0
        decoded_sig(index) = 1;  % Jestli je hodnota puvodniho signalu vetsi nez 0, budeme dekodovat jako '1' jinak dekodujeme jako '0'.
    else
        decoded_sig(index) = 0;
    end
    index = index + 1;
end


save decoded.txt decoded_sig; % ulozeni vysledku do souboru, muzeme porovnat s puvodnim signalem ulozenym v souboru xstepa60.txt .. podle diff je obsah stejny

% 320 = pocet vzorku za dvacet milisekund v signalu ... fs*20/1000
% 20 = pocet vzorku binarniho signalu za dvacet milisekund ... 320/16
one_binary_sample_duration = 0.020 / 20;
one_sample_signal_duration = 0.020 / 320;

time_binary_sig = one_binary_sample_duration/2:one_binary_sample_duration:0.020;
time_sig = one_sample_signal_duration:one_sample_signal_duration:0.020;

f = figure('Visible', 'off');
stem(time_binary_sig, decoded_sig(1:20)', 'r');
xlabel('t')
ylabel('s[n]')
hold on;
plot(time_sig, sig(1:320)');
hold off;
print(f, '-dpng', '2.png');
close(f);

%%%%%%%%%%%%%%%%%%%%%%%% UKOL3 a UKOL4 - OK
% Mate zadany filtr s prenosovou funkci, do protokolu vlozte obrazek s nulovymi 
% body a poly prenosove funkce tohoto filtru a uvedte, zda je filtr stabilni
fprintf("PRIKLAD 3 a 4:\n");


B = [0.0192 -0.0185 -0.0185 0.0192];
A = [1.0000 -2.8870 2.7997  -0.9113];

ukazmito(B,A, fs);
% STABILNI, DOLNI PROPUST, 500 Hz


%%%%%%%%%%%%%%%%%%%% UKOL5 - OK
% Filtrujte nacteny signal s[n] timto filtrem. Vysledek bude ss[n]. 
% Signaly s[n] a ss[n] zobrazte do jednoho obrazku. 
% Najdete jaky je treba posun ss[n], aby se oba obrazky vizualne podobaly.
fprintf("PRIKLAD 5:\n");

f = figure('Visible', 'off');
hold on;
s = sig(1:320); % 320 vzorku se vejde do 20ms
t = (0:(length(s)-1)) / fs; 
plot(t,s); % plotime puvodni signal (20ms signalu ~ 320 vzorkum)

ss = filter(B,A,sig); % ss je signal projety filterem
ss_orig = ss; % do promenne ukladam filtrovany signal s puvodni delkou
ss = ss(1:320); % 320 vzorku se vejde do 20ms 
t = (0:(length(ss)-1)) / fs;
plot(t,ss); % plotime filtrovany signal (20ms signalu ~ 320 vzorkum)
hold off;
print(f, '-dpng', '5.png');
close(f);


%%%%%%%%%%%%%%%%%%%% UKOL6 - OK
% Signal ss[n] skutecne posunte. Vysledek se bude oznacovat jako ss_shifted[n].
% Provedte dekodovani ss_shifted[n] do binarnich symbolu.
% Do protokolu dejte obr. prvnich 20ms signalu s[n], ss[n], ss_shifted[n] a binarni symboly
% dekodovane z ss_shifted[n].
fprintf("PRIKLAD 6:\n");

% posun o N vzorku
ss_shifted = shift(ss, -16); % posunuti zkracenho ss[n] signalu
ss_shifted_orig = shift(ss_orig, -16); % posunuti celeho ss[n] signalu

decoded_sig_shifted = []; % do pole ukladame binarni symboly

index = 1;
for k = ss_shifted_orig(8:16:sig_len); % zacneme na vzorku c.8 a posouvame se po 16    
    if k > 0
        decoded_sig_shifted(index) = 1;  % Jestli je hodnota puvodniho signalu vetsi nez 0, budeme dekodovat jako '1' jinak dekodujeme jako '0'.
    else
        decoded_sig_shifted(index) = 0;
    end
    index = index + 1;
end


f = figure('Visible', 'off');
hold on;
plot(t,s); % plotime puvodni s[n] signal
plot(t,ss); % plotime filtrovany ss[n] signal
plot(t,ss_shifted); % plotime filtrovany posunuty ss_shifted[n] signal
stem(time_binary_sig, decoded_sig_shifted(1:20)', 'r'); % plotime binarni signal ze shiftnuteho
hold off;
print(f, '-dpng', '6.png');
close(f);


%%%%%%%%%%%%%%%%%%%% UKOL7
% Vyhodnotte jakou maji symboly dekodovane z ss_shifted[n] oproti symbolum dekodovanym z s[n] chybovost
% (pocet spatnych bitu/poctem vsech bitu). 
fprintf("PRIKLAD 7:\n");

% oba binarni signaly prozeneme xorem, rozdilne (chybne) hodnoty budou ve vyslednem vektoru jednicky
xored = xor(decoded_sig, decoded_sig_shifted);

% zjistim, kolik mame jednicek (chyb)
bad_vals = sum(xored(:) == 1);

% vypis poctu chyb a chybovosti v %
fprintf("Pocet chyb je: %f\n", bad_vals);
fprintf("Chybovost je: %f%%\n", bad_vals/length(xored)*100);


%%%%%%%%%%%%%%%%%%%% UKOL8 - OK
% Ze signalu s[n] a ss[n] vypocitejte spektra pomoci diskretni Fourierovy transformace.
% Do protokolu vlozte obrazek modulu obou spekter v zavislosti na frekvenci.
% Frekvencni osa musi byt v Hz a pouze od 0 do poloviny vzorkovaci frekvence.
fprintf("PRIKLAD 8:\n");


% Vypocet frekvence
freq = (0 : sig_len / 2 - 1) / sig_len * fs;

% Vypocet spektra pro s[n]
fft_s = abs(fft(sig));
fft_s = fft_s(1 : sig_len / 2);

% Vypocet spektra pro ss[n]
fft_ss = abs(fft(ss_orig));
fft_ss = fft_ss(1 : sig_len / 2);

f = figure('Visible', 'off');
hold on;
plot(freq, fft_s); % plotime spektrum s[n]
plot(freq, fft_ss); % plotime spektrum ss[n]
hold off;
xlabel('f [Hz]');
grid;
print(f, '-dpng', '8.png');
close(f);


%%%%%%%%%%%%%%%%%%%% UKOL9 - OK
% V tomto a dalsich cviceni budete pracovat s puvodnim signalem s[n]
% Odhadnete funkci hustoty rozdeleni pravdepodobnosti p(x) signalu s[n].
fprintf("PRIKLAD 9:\n");

f = figure('Visible', 'off');
probab_func = hist(sig, 50)/sig_len;
plot(probab_func);
print(f,'-dpng', '9.png');
close(f);



%%%%%%%%%%%%%%%%%%%% UKOL10 - OK
% Spocitejte a do protokolu vlozte obrazek korelacnich koeficientu R[k] pro 
% k nalezi -50 .. 50. Pouzite vychyleny odhad koeficienty podle vztahu
% Matlab (xcorr) / Octave (biased)
fprintf("PRIKLAD 10:\n");


[R, lag] = xcorr(sig, 50, 'biased');
f = figure('Visible', 'off');
plot(lag, R);
xlabel('k');
title('Korelacni koeficienty');
grid;
print(f, '-dpng', '10.png');
close(f);

%%%%%%%%%%%%%%%%%%%%% UKOL11 - OK
% Napiste hodnotu koeficientu R[0], R[1], a R[16]. Pokud pouzijete xcorr, 
% zamyslete se, kde je najdete, vetsinou to neni R(0) - to ani nejde, R(1), R(16)
fprintf("PRIKLAD 11:\n");

% Jsou posunute o 50
fprintf('Hodnota koef.  R[0] je %f.\n', R(51));
fprintf('Hodnota koef.  R[1] je %f.\n', R(52));
fprintf('Hodnota koef.  R[16] je %f.\n', R(67));


%%%%%%%%%%%%%%%%%%%%% UKOL12 - OK
% Provedte casovy odhad sdruzene funkce hustoty rozdeleni pravdepodobnosti p(x1,x2,1)
% mezi vzorky n a n+1. Do protokolu vlozte 3D obrazek techto hodnot.
fprintf("PRIKLAD 12:\n");

% Vykuchana funkce hist2opt

N = length(sig);
L = 50;
x = linspace(min(sig), max(sig), L);
h = zeros(L, L);
[dummy, ind1] = min(abs(repmat(sig(:)', L, 1) - repmat(x(:), 1, N)));
ind2 = ind1(1 + 1 : N);

for i = 1 : N - 1,
    d1 = ind1(i);
    d2 = ind2(i);
    h(d1, d2) = h(d1, d2) + 1;
end
surf = (x(2) - x(1)) ^ 2;
p = h / N / surf;
f = figure('Visible', 'off');
imagesc(x, x, p);
axis xy;
ylabel('x1');
xlabel('x2');
print(f, '-dpng', '12.png');
close(f);

%%%%%%%%%%%%%%%%%%%%% UKOL13 - OK
fprintf("PRIKLAD 13:\n");

% Vykuchana funkce hist2opt

check = sum(sum (p)) * surf; 

fprintf("Kontrola, vysledek by mel byt 1 a je: %f\n", check);

%%%%%%%%%%%%%%%%%%%%% UKOL14 - OK
fprintf("PRIKLAD 14:\n");

% Vykuchana funkce hist2opt

% make col vector out of x and clone it L times. 
x = x(:); X1 = repmat(x,1,L);
% make row vector out of x and clone it L times. 
x = x'; X2 = repmat(x,L,1); 
% now put it together, don't forget to multipl by tile surface
r = sum(sum (X1 .* X2 .* p)) * surf;

fprintf("Korelacni koeficient R[1] je: %f\n", r);
