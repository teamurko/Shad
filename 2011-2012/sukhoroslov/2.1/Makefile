all:
	g++ -Wall -o single_kmeans single_kmeans.cpp
	g++ -Wall -fopenmp -o multi_kmeans multi_kmeans.cpp
	g++ -Wall -I/opt/local/include -o gen_data_sample gen_data_sample.cpp
clean:
	rm single_kmeans multi_kmeans gen_data_sample
