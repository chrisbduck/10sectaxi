#!/usr/bin/python
#
# For some reason, Emscripten is not noticing the functions defined in a JavaScript library and generating error
# functions that get in their way.  This lovely bit of hackery comments out the error functions

import os
import sys

file_name = sys.argv[1]
functions = sys.argv[2:]

all_lines = open(file_name).readlines()

func_start = 'function '
flen = len(func_start)

for index in xrange(len(all_lines)):
	orig_line = all_lines[index]
	line = orig_line.strip()
	if line.startswith(func_start):
		line = line[flen:]
		pos = line.find('(')
		if pos > 0:
			func_name = line[:pos]
			if func_name in functions:
				# Found a function to comment out
				all_lines[index] = '/*' + orig_line
				index += 2
				all_lines[index] += '*/'

open(file_name, 'wt').writelines(all_lines)

