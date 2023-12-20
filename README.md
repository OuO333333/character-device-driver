# Character-device-driver
How to use:  
  In driver:
  1. sudo make clean
  2. sudo make
  3. sudo insmod mychardev.ko
  4. sed -n '/^Character/, /^$/ { /^$/ !p }' /proc/devices
  5. sudo rmmod mychardev.ko

  In test:
  1. sudo make
  2. sudo make check
  3. sudo tail -n50 /var/log/kern.log

reference:  
    &nbsp;&nbsp;&nbsp;character device drivers:  
    &nbsp;&nbsp;&nbsp;https://linux-kernel-labs.github.io/refs/heads/master/labs/device_drivers.html  
    &nbsp;&nbsp;&nbsp;MKDEV(MAJOR, MINOR):  
    &nbsp;&nbsp;&nbsp;https://www.cnblogs.com/lilto/p/11878288.html  
    &nbsp;&nbsp;&nbsp;alloc_chrdev_region:  
    &nbsp;&nbsp;&nbsp;https://blog.csdn.net/kangear/article/details/13746091  
    &nbsp;&nbsp;&nbsp;gcc obj to exec:  
    &nbsp;&nbsp;&nbsp;https://stackoverflow.com/questions/1846913/how-to-make-an-executable-file-from-a-c-object-file  
    &nbsp;&nbsp;&nbsp;PHONY:  
    &nbsp;&nbsp;&nbsp;https://blog.csdn.net/lxq19980430/article/details/84800916  
    &nbsp;&nbsp;&nbsp;atomic example:  
    &nbsp;&nbsp;&nbsp;https://www.hitchhikersguidetolearning.com/2021/01/03/linux-kernel-atomic-operations-sample-code/  
    &nbsp;&nbsp;&nbsp;-Wimplicit-function-declaration:  
    &nbsp;&nbsp;&nbsp;https://hackmd.io/@unis/44455566
