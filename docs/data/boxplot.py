import sys
import re
import math

colors = {"GridOT": "blue", "MultiScaleOT": "red"}

def write_boxplot(file, solver, dim, times):
    times.sort()
    l = len(times)
    pos = math.log2(float(dim)) + (-0.125 if solver == "GridOT" else 0.125)
    file.write(f"\\addplot+ [boxplot prepared={{\n"
        f"lower whisker={max(times[0],1)},\n"
        f"lower quartile={times[l // 4]},\n"
        f"median={times[l // 2]},\n"
        f"upper quartile={times[3 * l // 4]},\n"
        f"upper whisker={times[-1]},\n"
        f"draw position={pos},\n"
        f"}}, color={colors[solver]}, solid] coordinates {{}};")

def write_data(data):
    solver = data[0][-1]
    kind = data[0][1]
    dim = data[0][0]
    times = [float(col[-2]) for col in data]
    with open(f"{solver}-{kind}-{dim}.txt", "w") as file:
        write_boxplot(file, solver, dim, times)
        
if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python split.py <filename>")
        exit()
    
    gridot = {}
    multiscaleot = {}
    filename = sys.argv[1]
    with open(filename, "r") as file:
        file.readline()
        file.readline()
        file.readline()
        for line in file:
            line = line[:-1] # Remove \n
            line = re.sub(" +", " ", line) # Remove duplicate whitespaces
            col = line.split(" ")[1:]

            if len(col) != 8:
                continue
            if col[2] == col[3]: # i == j
                continue
            #if col[4] == "0": # Not optimal
                #continue
            if int(col[0]) > 128:
                continue
            
            ot = gridot if col[-1] == "GridOT" else multiscaleot
            if not col[1] in ot:
                ot[col[1]] = {}
            if not col[0] in ot[col[1]]:
                ot[col[1]][col[0]] = []
            ot[col[1]][col[0]].append(col)
    
    for kind in gridot:
        for dim in gridot[kind]:
            write_data(gridot[kind][dim])
    
    for kind in multiscaleot:
        for dim in multiscaleot[kind]:
            write_data(multiscaleot[kind][dim])
