CXX=g++ -std=c++11 -Wall -pedantic -Werror=vla

all: dancing-links hidden-singles

dancing-links: Dancing-Links.cpp
	$(CXX) -o DancingLinks Dancing-Links.cpp

hidden-singles: Hidden-Singles.cpp
	$(CXX) -o HiddenSingles Hidden-Singles.cpp

clean:
	rm DancingLinks
	rm HiddenSingles