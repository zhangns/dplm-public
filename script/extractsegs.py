# python extractsegs.py [INFILE] [OUTFILE] [SEGLEN] [#SEG]...
# Extract segments of SEGLEN lines in INFILE to OUTFILE
import sys

def main(infile, outfile, seglen, seglist):
    with open(infile, 'r') as fin, open(outfile, 'w') as fout:
        currentline = 1
        def skip_1():
            # Skip 1 segment
            for i in range(seglen):
                fin.readline()
        c = 1 # cursor
        for seg in seglist:
            while c < seg:
               skip_1()
               c += 1 
            fout.write(''.join([fin.readline() for k in range(seglen)]))
            c += 1

if __name__ == '__main__':
    infile  = sys.argv[1]
    outfile = sys.argv[2]
    seglen  = int(sys.argv[3])
    seglist = list(map(int, sys.argv[4:]))
    main(infile, outfile, seglen, seglist)
