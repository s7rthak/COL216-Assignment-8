pipeline: pipeline.cpp
	g++ -o pipeline pipeline.cpp pipeline_main.cpp

test: pipeline.cpp pipeline_test.cpp
	g++ -o pipeline_test pipeline_test.cpp pipeline.cpp -lgtest -lpthread
	./pipeline_test
	rm pipeline_test

clean:
	rm -f pipeline 
