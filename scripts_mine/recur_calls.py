import sys
import os

def search_recur(data) :
	for i in data.items():
		func_name = str()
		for j in i[0].split() :
			if "(" in j :
				func_name = j.split('(')[0]
		for l in i[1][1:] :
			if func_name+"(" in l : 
				print ("this is FUnc :", func_name)

def read_in_code(c_file) : 
	func_dict = dict()
	func_ls = list()
	for i in iter(c_file) : 
		if i[0] == '}' :
			func_dict[func_ls[0]] = func_ls
			func_ls = list()
		if i[0] == '{' :
			func_ls = [func_ls[-1]]
			func_ls.append(i.strip())
		else :
			if i != ' ' or i != '' : 
				func_ls.append(i.strip())
	return func_dict

def main(argv) :
	c_file = open(argv[0], 'r+')
	data = read_in_code(c_file)
	search_recur(data)

if __name__ == '__main__' :
	main(sys.argv[1:])


