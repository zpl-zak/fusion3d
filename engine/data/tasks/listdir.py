import sys

mypath = sys.argv[1]

from os import listdir
from os.path import isfile, join
onlyfiles = [f for f in listdir(mypath) if isfile(join(mypath, f))]

if len(sys.argv) >= 3:
	newl = []
	for x in onlyfiles:
		if sys.argv[2] == x[x.index("."):]:
			if len(sys.argv) == 4:
				newl.append(x[:x.index(".")])
			else:
				newl.append(x)
	onlyfiles = newl

sys.stdout.write("@".join(onlyfiles))