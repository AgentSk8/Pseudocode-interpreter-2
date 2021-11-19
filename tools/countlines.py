import os
files = 0
lines = 0
characters = 0
for file in os.listdir('./src'):
	ext = file.split('.')[-1]
	if  ext == "cpp" or ext == "h":
		f = open('./src/'+file,'r').read()
		lines += len(f.split('\n'))
		characters += len(f)
		files += 1
print("CPP or H files\n"+"="*(12+len(str(characters)))+"\nFiles: %i\nLines: %i\nCharacters: %i\n" % (files, lines, characters))