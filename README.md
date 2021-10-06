# copyDriver
Simple character device driver which copies given data

## Compiling and loading of the driver
1. make
2. make install
3. make load (if fails, disable Secure Boot and repeat)

## Disabling Secure Boot in UEFI (BIOS)
1. sudo apt install mokutil
2. sudo mokutil --disable-validation
3. create password
4. reboot
5. confirm to change security settings. Choose "Yes"
6. enter the previously created full password or requested characters of it
7. choose Change Secure Boot state
8. reboot 
