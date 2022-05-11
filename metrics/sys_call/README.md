## system calls

- obtain system call list 
``` shell
$PROC='objdump -d small.elf'
strace -o sys_call.txt -c $PROC
```
- Obtain system call number 
``` shell
wc -l sys_call.txt
```
