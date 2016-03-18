#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <cmath>
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

std::vector<double> getClusterCenter(std::vector<std::vector<double> >& data, std::vector<int>& cluster)
{
	//Initialize a zero vector
	std::vector<double> sum(data[0].size(), 0.0);
	//For each instance in the cluster
	for (int i = 0; i < cluster.size(); ++i)
	{
		//For each attribute
		for (int a = 0; a < sum.size(); ++a)
		{
			sum[a] += data[cluster[i]][a];
		}
	}
	return sum;
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
	std::vector<std::vector<int> > clusters(k);
	//Select data points to be the starting means
	for (int m = 0; m < k; ++m )
	{
		means.push_back(data[data.size() * m / k]);
	}

	//Begin iterations
	int i;
	for (i = 0; i < numIterations; ++i)
	{
		//Clear clusters
		for (int m = 0; m < k; ++m)
		{
			clusters[m].clear();
		}

		//Assign each point to a cluster
		//For each point
		for (int p = 0; p < data.size(); ++p)
		{
			//Figure out which mean it's closer to
			double minDistanceSquared = euclideanDistanceSquared(data[p], means[0]);
			int closestMean = 0;
			for (int m = 1; m < k; ++m)
			{
				double distance = euclideanDistanceSquared(data[p], means[m]);
				if (distance < minDistanceSquared)
				{
					distance = minDistanceSquared;
					closestMean = m;
				}
			}
			//Assign the point to the cluster with the closest mean
			clusters[closestMean].push_back(p);
		}

		//Compute the new cluster centers
		std::vector<std::vector<double> > newMeans;
		for (int m = 0; m < k; ++m)
		{
			newMeans.push_back(getClusterCenter(data, clusters[m]));
		}

		//Compute total cluster center movement
		double sum = 0.0;
		for (int m = 0; m < k; ++m)
		{
			sum += sqrt(euclideanDistanceSquared(means[m],newMeans[m]));
		}

		if (sum <= epsilon)
		{
			printf("Terminated due to epsilon.\n");
			break;
		}
	}
	printf("Terminated after %d iterations\n", i);

	return clusters;
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
	
	if (argc == 4)
	{
		k = strtol(argv[1], NULL, 0);
		epsilon = strtod(argv[2], NULL);
		numIterations = strtol(argv[3], NULL, 0);
	} else if ( argc == 5 )
	{
		k = strtol(argv[1], NULL, 0);
		epsilon = strtod(argv[2], NULL);
		numIterations = strtol(argv[3], NULL, 0);
		fileName = argv[4];
	} else if ( argc == 1 )
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

	//Print out the dataset for debugging
	for ( int i = 0; i < dataSet.size(); ++i )
	{
		for (int a = 0; a < dataSet[i].size(); ++a )
		{
			printf("%f, ", dataSet[i][a]);
		}
		printf("\n");
	}

	//Call k-means
	std::vector<std::vector<int> > clusters = kMeans(dataSet, k, epsilon, numIterations);

	//Report information about the clusters
	std::vector<std::vector<double> > means;
	for (int m = 0; m < k; ++m)
	{
		//Report general information about the cluster
		printf("Cluster %d: %d instances, center at: ", m, (int)clusters[m].size());
		
		//Print the cluster center
		means.push_back(getClusterCenter(dataSet, clusters[m]));
		for (int a = 0; a < dataSet[0].size(); ++a)
		{
			printf("%f, ", means[m][a]);
		}
		printf("\n");
	}

	return 0;
}