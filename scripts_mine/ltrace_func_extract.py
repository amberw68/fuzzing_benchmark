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
		if "(" in i :
			func_name = i[0:i.index("(")]
			print (func_name)
	#	else :
	#		print (i)
if __name__ == '__main__' :
	main(sys.argv[1:])
