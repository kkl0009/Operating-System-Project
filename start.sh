source cs450-setup;
make;
qemu-system-i386 -nographic -kernel kernel.bin -s;
