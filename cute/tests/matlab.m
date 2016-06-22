% Example 9.5

clc
clear

% Source parameters
V0 = 230;
omegae = 120*pi;

% Motor parameters
poles = 2;
Lmain = .0806;
Rmain = 0.58;
Laux = 0.196;
Raux = 3.37;
Lr = 4.7e-6;
Rr = 37.6e-6;
Lmainr = 5.88e-4;
Lauxr = 9.09e-4;
C = 35e-6;
Xc = -1/(omegae*C);
Prot = 40;
Pcore = 105;

% Run through program twice.  If calcswitch = 1, then
% calculate at speed of 3500 r/min only.  The second time
% program will produce the plot for part (d).

for calcswitch = 1:2

if calcswitch == 1
  mmax = 1;
else
   mmax = 101;
end

for m = 1:mmax

if calcswitch == 1
   speed(m) = 3500;
else
   speed(m) = 3599*(m-1)/100;
end

% Calculate the slip
ns = (2/poles)*3600;
s = (ns-speed(m))/ns;

% part (a)
% Calculate the various complex constants
Kplus = s*omegae/(2*(Rr + j*s*omegae*Lr));
Kminus = (2-s)*omegae/(2*(Rr + j*(2-s)*omegae*Lr));
A1 = Lmain - j*Lmainr^2*(Kplus+Kminus);
A2 = Lmainr*Lauxr*(Kplus-Kminus);
A3 = Laux - j*Lauxr^2*(Kplus+Kminus);

% Set up the matrix
M(1,1) = Rmain + j*omegae*A1; 
M(1,2) = j*omegae*A2;
M(2,1) = -j*omegae*A2;
M(2,2) = Raux + j*Xc+ j*omegae*A3; 

% Here is the voltage vector
V = [V0 ; -V0];

% Now find the current matrix

I = M\V;

Imain = I(1);
Iaux = I(2);
Is = Imain-Iaux;

magImain = abs(Imain);
angleImain = angle(Imain)*180/pi;
magIaux = abs(Iaux);
angleIaux = angle(Iaux)*180/pi;
magIs = abs(Is);
angleIs = angle(Is)*180/pi;

%Capacitor voltage
Vcap = Iaux*Xc;
magVcap = abs(Vcap);

% part (b)
Tmech1 = conj(Kplus-Kminus);
Tmech1 = Tmech1*(Lmainr^2*Imain*conj(Imain)+Lauxr^2*Iaux*conj(Iaux));
Tmech2 = j*Lmainr*Lauxr*conj(Kplus+Kminus);
Tmech2 = Tmech2*(conj(Imain)*Iaux-Imain*conj(Iaux));
Tmech(m) = (poles/2)*real(Tmech1+Tmech2);
Pshaft = (2/poles)*(1-s)*omegae*Tmech(m)-Prot;


%part (c)
Pmain = real(V0*conj(Imain));
Paux = real(-V0*conj(Iaux));
Pin = Pmain+Paux+Pcore;
eta = Pshaft/Pin;

if calcswitch == 1
   fprintf('part (a):')
   fprintf('\n   Imain = %g A at angle %g degrees',magImain,angleImain)
   fprintf('\n   Iaux = %g A at angle %g degrees',magIaux,angleIaux)
   fprintf('\n   Is = %g A at angle %g degrees',magIs,angleIs)
   fprintf('\n   Vcap = %g V\n',magVcap)
   fprintf('\npart (b):')
   fprintf('\n   Tmech = %g N-m',Tmech)
   fprintf('\n   Pshaft = %g W\n',Pshaft)
   fprintf('\npart (c):')
   fprintf('\n   Pmain = %g W',Pmain)
   fprintf('\n   Paux = %g W',Paux)
   fprintf('\n   Pin = %g W',Pin)
   fprintf('\n   eta = %g percent\n\n',100*eta)
else
   plot(speed,Tmech)
   xlabel('speed [r/min]')
   ylabel('<Tmech> [N-m]')

end

end %end of for m loop 
end %end of for calcswitch loop
