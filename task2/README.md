# Задача 2
Функция-обработчик прерывания проверяет, нажата ли клавиша и только тогда увеличивает счётчик. В осатльном реализация вполне тривиальна.

Для более удобного тестирования стоит воспользоваться командой `qemu-system-x86_64 -kernel ./boot/vmlinuz-6.7.4 -initrd ./initramfs --enable-kvm -cpu host -nographic -append "console=ttyS0" -monitor unix:qemu-monitor-socket,server,nowait`, что откроет сокет, позволяющий эмулировать ввод с PS/2 клавиатуры при помощи `echo "sendkey a" | socat - unix-connect:qemu-monitor-socket && echo`