qemu-system-x86_64 -bios /usr/share/ovmf/OVMF.fd \
  -drive if=ide,file=fat:rw:image,format=raw,index=0,media=disk \
  -m 2048 -smp 1 \
  -serial mon:stdio \
  -vga std \
  -nographic
