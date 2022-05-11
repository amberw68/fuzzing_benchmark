## dynamic_libraries

- obtain library names example 
```shell
$PROC='objdump -d small.elf'
# collect information of calls
ltrace -o calls.txt -l '*' $PROG &> /dev/null
# analyze collected data
cat calls.txt | sed -ne '/->/{ s/^\(.*\)->.*/\1/; p }' | sort -u

 
docker run -ti -v /location/of/your/target:/src aflplusplus/aflplusplus
```

#### inside AFL++ docker needs :

- clang/clang++
- python2.7
- python3.5+
- pip3
- pip2
- PyPDF2
- poppler

```shell
curl https://bootstrap.pypa.io/pip/2.7/get-pip.py --output get-pip.py
python2 get-pip.py
```

