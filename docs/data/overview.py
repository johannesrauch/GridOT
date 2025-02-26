import sys
import re
import math

colors = {"GridOT": "blue", "MultiScaleOT": "red"}
ordering = {
    "WhiteNoise": 0, "GRFrough": 1, "GRFmoderate": 2, "GRFsmooth": 3, "LogGRF": 4, 
    "LogitGRF": 5, "CauchyDensity": 6, "Shapes": 7, "ClassicImages": 8, "MicroscopyImages": 9
}

def write_data(data):
    data.sort(key=lambda x: ordering[x[1]])
    solver = data[0][-1]
    dim = data[0][0]

    with open(f"{solver}-{dim}.txt", "w") as file:
        file.write("num dim cls tms sol\n")
        for col in data:
            if float(col[3]) > 1:
                file.write(f"{ordering[col[1]] + 1} {col[0]} {col[1]} {col[3]} {col[4]}\n")

def write_speedup(data1, data2):
    data1.sort(key=lambda x: ordering[x[1]])
    data2.sort(key=lambda x: ordering[x[1]])
    assert(len(data1) == len(data2))
    dim = data1[0][0]

    with open(f"Speedup-{dim}.txt", "w") as file:
        file.write("num dim cls speedup\n")
        for i in range(len(data1)):
            t1 = float(data1[i][3])
            t2 = float(data2[i][3])
            assert(data1[i][0] == data2[i][0])
            assert(data1[i][1] == data2[i][1])
            if t2 > 1:
                file.write(f"{ordering[data1[i][1]] + 1} {data1[i][0]} {data1[i][1]} {t2/t1}\n")

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

            if len(col) != 5:
                continue
            
            ot = gridot if col[-1] == "GridOT" else multiscaleot
            if not col[0] in ot:
                ot[col[0]] = []
            ot[col[0]].append(col)
    
    for dim in gridot:
        write_data(gridot[dim])
        write_data(multiscaleot[dim])
        write_speedup(gridot[dim], multiscaleot[dim])
