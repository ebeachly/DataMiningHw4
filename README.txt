This is the code and datasets used by Evan Beachly and Robin Vo for Assignment 4 in CSCE 474/874

This directory contains our datasets. The source directory contains our source code.
file_parsing.h contains utility functions for parsing csv files.
getCPUTime.h is a cross-platform function for measuring cpu time. Taken from:
	http://nadeausoftware.com/articles/2012/03/c_c_tip_how_measure_cpu_time_benchmarking
main.cpp is our C++ program that implements the k-means clustering algorithm. Once compiled, it accepts 3-4 arguments:
	k 						:Positive Integer, The number of clusters to use
	epsilon 				:Positive Float, Terminates the algorithm when the total cluster movement goes below this threshold
	number of iterations	:Positive Integer, The maximum number of times to iterate
	filename				:String, The path to the .csv file to use

Our algorithm expects the csv files to follow this format:
	The first line contains the names of the attributes
	All other lines contain instance data
	No empty lines at the end of the file
	No missing values
	At least one instance
	The last attribute is a class attribute, and is ignored
	All other attributes are numeric