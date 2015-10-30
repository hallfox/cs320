import os
import subprocess as subp
import filecmp

test_files = filter(lambda x: x.endswith(".txt"), os.listdir("traces"))
for t in test_files:
    basename = t[:-4]
    in_file = "./traces/" + t
    out_file = "./outputs/" + basename + "-output.txt"
    subp.run(["./predictors", in_file, out_file])
    correct_out = "./correct_outputs/" + basename + "-output.txt"
    assert filecmp.cmp(out_file, correct_out, shallow=False)
