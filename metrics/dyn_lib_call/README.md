## dynamic_libraries

- obtain library names example 
```shell
$PROC='objdump -d small.elf'
# collect information of calls
ltrace -o calls.txt -l '*' $PROG &> /dev/null
# analyze collected data
cat calls.txt | sed -ne '/->/{ s/^\(.*\)->.*/\1/; p }' | sort -u > lib_ls
```

- obtain library number
```shell
wc -l lib_ls
```


