#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <cmath>
#include <vector>
#include <string>
#include "file_parsing.h"
#include "getCPUTime.h"

void printVector(std::vector<double> vec)
{
	for (int a = 0; a < vec.size(); ++a)
	{
		printf("%f", vec[a]);
		if (a < vec.size() - 1)
		{
			printf(", ");
		}
	}
	printf("\n");
}

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
			sum += (a[i] - b[i])*(a[i] - b[i]);
		}
		return sum;
	}
}


/*
 * A class to represent a cluster
 */
class Cluster {
public:
	std::vector< int >  instances;
	std::vector< double > center;

	Cluster(){};

	double updateCenter( std::vector< std::vector< double > >& data)
	{
		std::vector<double> oldCenter = center;
		double movement = 0.0;
		if (instances.size() > 0)
		{
			center = std::vector<double>(data[0].size(), 0.0);
			//For each instance in the cluster
			for (int i = 0; i < instances.size(); ++i)
			{
				//For each attribute
				for (int a = 0; a < center.size(); ++a)
				{
					center [a] += data[instances[i]][a];
				}
			}
			//Divide the sum by the number of instances
			for (int a = 0; a < center.size(); ++a)
			{
				center[a] = center[a] / instances.size();
			}
		}
		if (oldCenter.size() == 0)
		{
			return 0.0;
		}
		return sqrt(euclideanDistanceSquared(center, oldCenter));
	}

	void printClusterInfo(int clusterNum)
	{
		printf("Cluster %d: %d instances, center at: ", clusterNum, (int)instances.size());
		for (int a = 0; a < center.size(); ++a)
		{
			printf("%f", center[a]);
			if (a < center.size() - 1)
			{
				printf(", ");
			}
		}
		printf("\n");
	}

	double getWithinClusterSumOfSquaredErrors(std::vector< std::vector< double > >& data)
	{
		double sumOfSquaredErrors = 0.0;
		for (int i = 0; i < instances.size(); ++i)
		{
			sumOfSquaredErrors += euclideanDistanceSquared(center, data[instances[i]]);
		}
		return sumOfSquaredErrors;
	}
};

/**
 * K-Means algorithm
 *
 * input: data, k, epsilon, numIterations
 * returns: an array of clusters. Each cluster is an array of indicies into the dataset.
 */

std::vector< Cluster > kMeans(std::vector<std::vector<double> >& data, int k, double epsilon, int numIterations)
{
	std::vector< Cluster > clusters(k);
	//Select data points to be the starting means
	//printf("Initial conditions:\n");
	for (int m = 0; m < k; ++m )
	{
		clusters[m].instances.push_back((data.size() * m) / k);
		clusters[m].updateCenter(data);
		//clusters[m].printClusterInfo(m);
	}

	//Begin iterations
	int i;
	for (i = 0; i < numIterations; ++i)
	{
		//printf("Iteration %d:\n", i + 1);
		//Clear which instances belong to which clusters
		for (int m = 0; m < k; ++m)
		{
			clusters[m].instances.clear();
		}

		//Assign each point to a cluster
		//For each point
		for (int p = 0; p < data.size(); ++p)
		{
			//printVector(data[p]);
			//Figure out which mean it's closer to
			double minDistanceSquared = euclideanDistanceSquared(data[p], clusters[0].center);
			//printf("Distance to Cluster 0: %f, new min\n", minDistanceSquared);
			int closestMean = 0;
			for (int m = 1; m < k; ++m)
			{
				double distance = euclideanDistanceSquared(data[p], clusters[m].center);
				//printf("Distance to Cluster %d: %f", m, distance);
				if (distance < minDistanceSquared)
				{
					//printf(", new min\n");
					minDistanceSquared = distance;
					closestMean = m;
				} else {
					//printf("\n");
				}
			}
			//Assign the point to the cluster with the closest mean
			clusters[closestMean].instances.push_back(p);
		}

		//Compute the new cluster centers
		double totalClusterMovement = 0.0;
		for (int m = 0; m < k; ++m)
		{
			totalClusterMovement += clusters[m].updateCenter(data);
			//clusters[m].printClusterInfo(m);
		}
		//printf("Total movement: %f\n", totalClusterMovement);

		//Check if total cluster movement is less than epsilon
		if (totalClusterMovement <= epsilon)
		{
			//printf("Terminated due to epsilon.\n");
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
	char defaultFileName[] = "../waveform-5000-25%.csv";
	char* fileName = defaultFileName;
	int k = 3;
	double epsilon = 0.0;
	int numIterations = 1000;
	
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
	/*for ( int i = 0; i < dataSet.size(); ++i )
	{
		for (int a = 0; a < dataSet[i].size(); ++a )
		{
			printf("%f, ", dataSet[i][a]);
		}
		printf("\n");
	}*/

	//Call k-means
	double startTime = getCPUTime();
	std::vector<Cluster> clusters = kMeans(dataSet, k, epsilon, numIterations);
	double endTime = getCPUTime();

	//Get the mean of the data set
	std::vector<double> dataSetCenter;
	if (dataSet.size() > 0)
	{
		dataSetCenter = std::vector<double>(dataSet[0].size(), 0.0);
		//For each instance in the data set
		for (int i = 0; i < dataSet.size(); ++i)
		{
			//For each attribute
			for (int a = 0; a < dataSetCenter.size(); ++a)
			{
				dataSetCenter[a] += dataSet[i][a];
			}
		}
		//Divide the sum by the number of instances
		for (int a = 0; a < dataSetCenter.size(); ++a)
		{
			dataSetCenter[a] = dataSetCenter[a] / dataSet.size();
		}
	}

	//Report information about the clusters
	//std::vector<std::vector<double> > means;
	printf("Final Results: Clustered in %f seconds.\n", endTime - startTime);
	double totalWithinClusterSumOfSquaredErrors = 0.0;
	for (int m = 0; m < k; ++m)
	{
		clusters[m].printClusterInfo(m);
		totalWithinClusterSumOfSquaredErrors += clusters[m].getWithinClusterSumOfSquaredErrors(dataSet);
	}
	printf("Within Cluster Sum of Squared Errors: %f\n", totalWithinClusterSumOfSquaredErrors);

	return 0;
}