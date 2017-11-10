rem gmake clean
rem gmake
rem path=F:\tools\gcc\bin;%path%
rm *.o
rm main.bin

arm-elf-gcc -fno-inline -Os -fno-strict-aliasing -march=armv5t  -mthumb-interwork -Wall -Wno-unused -Wno-format -W--use-blx -nostdinc -c -o entry.o entry.S
arm-elf-gcc -fno-inline -Os -fno-strict-aliasing -march=armv5t  -mthumb-interwork -Wall -Wno-unused -Wno-format -W--use-blx -nostdinc -c -o main.o main.c
arm-elf-gcc -fno-inline -Os -fno-strict-aliasing -march=armv5t  -mthumb-interwork -Wall -Wno-unused -Wno-format -W--use-blx -o main.elf entry.o main.o     -nostdlib -Wl,--allow-shlib-undefined -Wl,--no-define-common,-EL,-T,link-boot.ld -Wl,-N,-Ttext,0x001FA6C0
arm-elf-nm main.elf | grep ' U ' > main.elf.syms
arm-elf-objdump -z -d main.elf > main.dump
arm-elf-objcopy -O binary main.elf main.bin

dir main.bin

Debug\a5108-mk.exe


