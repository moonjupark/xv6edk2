qemu-system-x86_64 -bios /usr/share/ovmf/OVMF.fd \
	-drive if=ide,file=fat:rw:image,index=0,media=disk \
	-S -m 2048 -smp 1 \
	-gdb tcp::4321 \
	-serial mon:stdio \
	-vga std \
	-nographic
