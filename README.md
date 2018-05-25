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

## Version 1 Quick Start
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

## objdump before/after
Before:
```
% objdump -dj .crypted example 

example:     file format elf64-x86-64


Disassembly of section .crypted:

0000000000401022 <crypted_main>:
  401022:	55                   	push   %rbp
  401023:	48 89 e5             	mov    %rsp,%rbp
  401026:	48 83 ec 10          	sub    $0x10,%rsp
  40102a:	89 7d fc             	mov    %edi,-0x4(%rbp)
  40102d:	48 89 75 f0          	mov    %rsi,-0x10(%rbp)
  401031:	bf ec 10 40 00       	mov    $0x4010ec,%edi
  401036:	e8 b5 f7 ff ff       	callq  4007f0 <puts@plt>
  40103b:	b8 64 00 00 00       	mov    $0x64,%eax
  401040:	c9                   	leaveq 
  401041:	c3                   	retq   
```

After:
```
% objdump -dj .crypted crypted

crypted:     file format elf64-x86-64


Disassembly of section .crypted:

0000000000401022 <crypted_main>:
  401022:	68 ac 6c f3 e5       	pushq  $0xffffffffe5f36cac
  401027:	6d                   	insl   (%dx),%es:(%rdi)
  401028:	91                   	xchg   %eax,%ecx
  401029:	59                   	pop    %rcx
  40102a:	d2 7b 05             	sarb   %cl,0x5(%rbx)
  40102d:	6e                   	outsb  %ds:(%rsi),(%dx)
  40102e:	20 3c 38             	and    %bh,(%rax,%rdi,1)
  401031:	74 05                	je     401038 <crypted_main+0x16>
  401033:	54                   	push   %rsp
  401034:	13 d6                	adc    %esi,%edx
  401036:	2c 31                	sub    $0x31,%al
  401038:	18 99 4c 46 5f 38    	sbb    %bl,0x385f464c(%rcx)
  40103e:	ad                   	lods   %ds:(%rsi),%eax
  40103f:	e3 bb                	jrcxz  400ffc <__libc_csu_init+0x4c>
	...

```


As you can see, the second binary contains a bunch of nonsensical rubbish
instead of readable assembler in the .crypted section.


## Quickstart Version 2
```
 ~/ELFcrypt % make
gcc -Wall -o ELFcrypt ELFcrypt.c
gcc -Wall -o ELFcrypt2 ELFcrypt2.c
gcc -Wall -o ELFcrypt2-stub ELFcrypt2-stub.c
gcc -Wall -o example example.c
 ~/ELFcrypt % ./ELFcrypt2 /bin/ls out
ELFcrypt2 by @dmfroberson
Enter passphrase: danger
Confirm passphrase: danger
 ~/ELFcrypt % cat ELFcrypt2-stub out >crypted
 ~/ELFcrypt % chmod +x crypted
 ~/ELFcrypt % ./crypted 
Enter passphrase: danger
Confirm passphrase: danger
crypted    ELFcrypt2.c	     ELFcrypt.c  example.c   out
ELFcrypt   ELFcrypt2-stub    ELFcrypt.h  LICENSE.md  README.md
ELFcrypt2  ELFcrypt2-stub.c  example	 Makefile
```

ELFcrypt2 creates a stub program that reads whatever data resides beyond
the end of its own valid ELF, decrypts this data memory, and finally
executes it in memory by means of fexecve()

To use on different programs, run them through ELFcrypt2, then use cat
as outlined above to create the binary.

This might not work right on older Linux systems that do not have the
memfd_create() function. This can be worked around by modifying the
code to create temporary files rather than utilizing this function.
Maybe one day I will care enough to fix this.

Also, the contents of your crypted executable are vulnerable to memory
dumps while it is running. This simply provides a layer of protection
for your stuff while it is relaxing on a hostile disk drive.

