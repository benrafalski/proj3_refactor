project3: *.h *.cc
	g++ -std=c++11 *.h *.cc -o project3 

clean:
	rm *.o project3