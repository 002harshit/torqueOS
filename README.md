# TORQUE OS
![torque os](https://github.com/002harshit/torqueOS/blob/main/media/output.gif)
- I'll rewrite it after I get familiar with OSDev and how things work

## Milestone
- [ ] Implement USB Driver for keyboard and mouse
- [ ] Trap/Exception Handler
- [ ] RAMFS
- [ ] Stack Smashing Protector
- [ ] Memory Management (malloc, free)
- [ ] Multithreaded kernel
- [ ] Filesystem Support

## References
- https://wiki.osdev.org/GCC_Cross-Compiler
- https://littleosbook.github.io/ 
- https://web.archive.org/web/20221126084414/http://jamesmolloy.co.uk/tutorial_html/
- https://wiki.osdev.org/Creating_an_Operating_System

## GNU compiler and utils sources
- https://gcc.gnu.org/gcc-15/
- https://www.gnu.org/software/binutils/

## Running torqueOS (hard way)
### get source of the project
```sh
git clone https://github.com/002harshit/torqueOS
```
### compile or download gcc compiler and gnu binutils for i386
```
docs: https://wiki.osdev.org/GCC_Cross-Compiler
```

### modify variable in Makefile as
```sh
TOOLCHAIN=path/to/your/toolchain/bin/i386-elf-
```

### install packages to compile and run iso file

```sh
sudo apt install build-essential yasm grub-pc-bin xorriso qemu
# packages name might differ distro to distro
```

### run makefile in the root folder (should work if everything is fine)
```sh
make qemu
```

## Running torqueOS (easy way)
- that's the neat part, there is no easy way
