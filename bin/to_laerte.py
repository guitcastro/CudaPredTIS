#!/usr/bin/env python

#This script add a space char for each 64 char in line

import sys

def convert_to_laerte(filename):

    input = open(filename, 'r')
    output = open(filename + '.dat', 'w')
    for line in input:
    #	print line
        output.write(line[:64] + line[65:129] + line[130:162])
        property1 = line[165:166]
        property2 = line[169:170]
        property3 = line[173:174]
        output.write(property1 + property2 + property3 + "\n")
    input.close
    output.close;
	


BASES = ["../input_mpi/gallus/", 
         "../input_mpi/Rattusnovergicus/",
         "../input_mpi/Rattusnovergicus/", 
         "../input_mpi/Musmusculus/", 
         "../input_mpi/celegans/", 
         "../input_mpi/athaliana/", 
         "../input_mpi/Drosophila/", 
         "../input_mpi/HomoSapiens/"]
		 
		 
for base in BASES:
    print "Converting base: " + base
    for i in range (1,11):
        convert_to_laerte(base + "cluster" + str(i))

