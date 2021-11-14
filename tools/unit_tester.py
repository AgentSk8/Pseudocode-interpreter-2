import subprocess, os
red = ' \u001b[31m'
green = '\u001b[32m'
reset = '\u001b[0m'
amber = '\u001b[33m'
files = os.listdir(os.path.dirname(__file__)+"/../test_scripts")
print(f"{amber}TESTING {len(files)} FILES.\n(If stalling, probably wating for input){reset}")
for file in files:
	if file.split(".")[-1] == 'pscd':
		print(f"{amber}RUNNING {file}...{reset}")
		res = subprocess.run(os.path.dirname(__file__)+"/../builds/main test_scripts/"+file, shell=True, capture_output=True)
		if res.returncode != 0:
			print(f"{red}[X] {file}\tFAILED:\n\t{reset}{res.stderr}\n")
		else:
			print(f"{green}[V] {file}\tPASSED!{reset}")