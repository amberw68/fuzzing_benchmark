import sys
import os

def main(argv) :
	src = str()
	tar = str()
	thickness = list()
	i_cnt = list()
	o_cnt = list()
	in_f = open(argv[0], 'r+')	
	for i in iter(in_f) :
#		if 'thickness' in i : 
#			thickness.append(int(i.split()[1]))
		if 'icnt' in i :
			tmp = i[i.index('icnt'):] 
			icnt = int(tmp[tmp.index(':')+1:tmp.index('ocnt')].strip())
			i_cnt.append(icnt)
	print (max(i_cnt)) 
#		if 'sourcename' in i : 
#			src = i.split()[1]
#		if 'targetname' in i : 
#			tar = i.split()[1]
#			if src == tar : 
#				print (i)
#			src = str()
#			tar = str()
if __name__ == '__main__' :
	main(sys.argv[1:])
