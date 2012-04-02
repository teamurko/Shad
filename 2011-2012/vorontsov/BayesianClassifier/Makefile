all:
	g++ -Wall -I/opt/local/include naive_bayes_classifier.cpp parzen_window_classifier.cpp lda_classifier.cpp classifiers.cpp main.cpp -o main
	g++ -Wall -I/opt/local/include naive_bayes_classifier.cpp parzen_window_classifier.cpp lda_classifier.cpp classifiers.cpp test_quality.cpp -o test_quality

clean:
	rm main
	rm test_quality
