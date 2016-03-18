#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <vector>
#include <string>
#include "file_parsing.h"
#include "getCPUTime.h"

/**
 * returns the squared euclidean distance between two vectors of the same dimension
 */
double euclideanDistanceSquared(std::vector<double>& a, std::vector<double>& b)
{
	if (a.size() != b.size())
	{
		printf("Taking distance between two vectors of different sizes.\n");
		return 0;
	} else {
		double sum = 0;
		for (int i = 0; i < a.size(); ++i )
		{
			sum += a[i] * b[i];
		}
		return sum;
	}
}

/**
 * K-Means algorithm
 *
 * input: data, k, epsilon, numIterations
 * returns: an array of clusters. Each cluster is an array of indicies into the dataset.
 */

std::vector<std::vector<int> > kMeans(std::vector<std::vector<double> >& data, int k, double epsilon, int numIterations)
{
	std::vector<std::vector<double> > means;
	std::vector<std::vector<int> > clusters;
	//Select data points to be the starting means
	for (int m = 0; m < k; ++m )
	{
		means.push_back(data[data.size() * m / k]);
	}

	//Begin iterations
	for (int i = 0; i < numIterations; ++i)
	{
		//Assign each point to a cluster

	}
}

/**
 * Main Function
 * Parse input, runs K-Means, and analyzes output and timing
 */
int main( int argc, char* argv[] )
{
	//Parse arguments and inputs
	char defaultFileName[] = "../iris.csv";
	char* fileName = defaultFileName;
	int k = 3;
	double epsilon = 0.0;
	int numIterations = 10;
	
	if (argc == 3)
	{
		k = strtol(argv[1], NULL, 0);
		epsilon = strtod(argv[2], NULL);
		numIterations = strtol(argv[3], NULL, 0);
	} else if ( argc == 4 )
	{
		k = strtol(argv[1], NULL, 0);
		epsilon = strtod(argv[2], NULL);
		numIterations = strtol(argv[3], NULL, 0);
		fileName = argv[4];
	} else if ( argc == 0 )
	{
		printf("Arguments: k, epsilon, number of iterations, and fileName (optional).\n");
		printf("Using defaults: %d, %f, %d, %s\n", k, epsilon, numIterations, fileName);	
	}
	else
	{
		printf("Arguments: k, epsilon, number of iterations, and fileName (optional).\n");
		return -1;
	}
	

	FILE* csvFile = fopen(fileName,"r");
	if (csvFile == NULL)
	{
		printf("Unable to open \"%s\": %s\n", fileName, strerror(errno));
		return -2;
	}

	//Parse the file
	std::vector<std::vector<double> > dataSet;

	int lineLength;
	char* line = NULL;
	char* token = NULL;
	unsigned int attributeNum;

	//Read the attribute names
	std::vector< std::string > attributeNames;
	line = readLine(csvFile, &lineLength);
	token = tokenize(line, ',');
	int numAttributes = 0;
	while (token != NULL)
	{
		//Add this attribute name to the attributeNames list
		attributeNames.push_back( std::string(token) );
		//Get the next name
		token = tokenize(NULL, ',');
		numAttributes++;
	}


	//Read the attribute values
	while ((line = readLine(csvFile, &lineLength)) != NULL)
	{
		std::vector< double > instance;
		attributeNum = 0;
		token = tokenize(line, ',');
		//Don't read the class attribute
		while (token != NULL && (attributeNum < numAttributes - 1))
		{
			//Add this attribute's value to the instance
			instance.push_back(strtod(token, NULL));
			//Get the next attribute's entry
			token = tokenize(NULL, ',');
			++attributeNum;
		}
		//Add the instance to the database
		dataSet.push_back(instance);
	}

	//Call k-means
	std::vector<std::vector<int> > clusters = kMeans(dataSet, k, epsilon, numIterations);

	return 0;
}