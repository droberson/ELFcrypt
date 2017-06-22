# ELFcrypt

```
MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
MMMMMMMMMMMMMMMMMMMMNNNNNNNNNNNNNNNNNNMMMMMMMMMMMMMMMMMMMMMMMMMMMM
MMMMMMMMMMMMMMNNmdhso///+o++osyyhdmNNNNNNNNNNMMMMMMMMMMMMMMMMMMMMM
MMMMMMMMMNmmhs/:...`````..........:ohmNNNNNNNNNNNNMMMMMMMMMMMMMMMM
MMMMMMMNmds:..````````...............:sdmmNNNNNNNNNNNNMMMMMMMMMMMM
MMMMNNNNm+.`.....````.............------odmmmmNNNNNNNNNNNMMMMMMMMM
MNNNNNNdo......`````..........-------:---:ymmmmmmmNNNNNNNNNMMMMMMM
NNNNNNdy.......```.....-------::::::::::--:odmmmmmmmmNNNNNNNNMMMMM
NNNNNmh:...........---::::::::::::::::::----odmmmmmmmmmmNNNNNNNMMM
NNNmmdo......-...---:::::::----::::::------.-odddmmmmmmmmNNNNNNNNM
Nmmmms.....----.------:::::::::::::----::--.-:odddddmmmmmmmNNNNNNN
mmmmd:....---------:-..-------:::--..-oso/::///hdddddmmmmmmmmNNNNN
mmmmy-..---::---.-++++/:----://++///+oo+oooyhs/sdddddddmmmmmmmNNNN
mmmdy-.---:///--.:++osyhsosyhyo/::oyhso//ooyhyo+dddddddddmmmmmmmNN
mmddy.-syysoo+-..:++//+osdhys+:/oooyys+/++::sso+hddddddddddmmmmmmN
mddds.:ohydhhs:..-/+/:/oyy+/::/dddy/oyss+/+so//ohddddddddddmmmmmmm
ddddy.-:syddhso--::/+osss////+hdyydhossyhddysoo+hddddddddddddmmmmm
ddddh/--/hhsyyhyo/+shyssys+:o+hho+hyosdmmmdysssohdddddddddddddmmmm
dddddh:-:+sssosyyydmmmmdho::+/+/+sso+oshmmmhysooddddddddddddddmmmm
dddddh/--/oysyhdhyyhmmdhs+:::---:+--:++shddhyo+sdddddddddddddddmmm
dddddho/::/oyhdddssyhhhoso::-://+ooshhyoyhhsosyhdddddddddddddddmmm
ddddddyo/::+syyhdsssssysddyssydhhhdmNmdooyysyhddddddddddddddddddmm
ddddddho+///oosyhssssossoooohhsyydhsyy+/ohhyhddddddddddddddddddmmm
dddddddy+/:/+o+syhhyysoosso+++::+ssosssssyyydddddddddddddddddddmmm
dddddddh//::/o+o+ydddhssooooos+oyhhhhddh+sshddddddddddddddddddddmm
ddddddhho::::/++/-ydddhyyso++//oyyssyhysoosddddddddddddddddddddmmm
ddddddhhs----://:-+yhhyhddyo+/+ssyydmmddhsdddddddddddddddddddddmmm
dddddhhy+---.-:-:os/shyydmmmdyhddmmmdhmmssdddddddddddddddddddddmmm
dddddddh/---.--ohyhy:-/oyhhdmddmmmd+-.sNdosyhhddddddddddddddddmmmm
dddddddy:+----:hs+hdh+.`..-/shdmmy:..-dNNhhsoooosyhdddddddddddmmmm
dddddddy:+/---:ss:/yddh+-.---/yds.```./hNNdmmdddyyyhdmNNNmmmmmmmmm
mdddddds//:-:-:+ys:-+hmdhyso/--+-.--::/omNmmdmNNNdyyyhmmmhhmNNNNNN
mmddddy+//::+///ohh/.-+omNNNmdsyddmmmmmmNNNNNmdmmdhhddydmyyhNdNNNN
mmmdyo+++o+++///oymmo..oNNNNNNNmdyysyyhhhNNdhhyssydhddsmsddmdyNNNN
dhyssyyyhhss+/++symNNy-+mNmNNmds:-----::+mdyyyyyssymmdmNhdNNsyNNNN
yyyyysyydhhd++osydNNNNd/smmdy/--......--:hmhhyhhysydNNdymNmyyNNNNN
yyhddhhmNNmhsssyhNNNNNNms//:-..........--oNdhhddddmNNNmdymhhhNNNNN
hshdNdmNmNhsyohmNNNNNNNNNy...........`..-:mmmmddNNhNmmNNddmNmNNNNm
ssmNNNsNyNsdshmNNNNNNNNNNNo..........``.--yNNNmdNNoNmNNNNNsmNNNNNm
shydmm+NomymyNddNNNNNNNNNNNo.......-....--sNNNNdNmyhdmdNNNsNNNNNNN
hhh+mhhyssNNddhsdhNNNNNNNNNNy............-sNNmNmmNNNdshyNNmmddmNNN
+yhyNdyhyhNNmNyhydmNNNNNNNNNNy-.......`..-/mNNmNdNyNsydmNNNmmmNNNN
hsNddNdMMNMNmhdmmNdNNNNNNNNNNNs-......``..-yNNNNdmmhoNNNNNmmNNNNNN
```

## How to Use

Quickstart:

```
% make
gcc -Wall -o ELFcrypt ELFcrypt.c
gcc -Wall -o example example.c
% ./ELFcrypt example
ELFcrypt by @dmfroberson

Crypting .crypted section of example, outputting to crypted

Enter passphrase: harharhar
Confirm passphrase: harharhar
% ./crypted
Enter passphrase: harharhar
Confirm passphrase: harharhar
This function was crypted
```

To use this in future projects, include ELFcrypt.h, add a call to
ELFdecrypt() to main(), and prefix your functions that you'd like to
protect with CRYPTED. After this make-believe C program is compiled,
run ELFcrypt against it with whatever password you desire and it will
be encrypted with RC4. See example.c for more details.

This also will attempt to read the ELFCRYPT environment variable as
input for the password:

```
% ELFCRYPT="harharhar" ./crypted
This function was crypted
```
