all: electricity-quiry

electricity-quiry: smtp.cpp electricity-quiry.cpp main.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ -lcurl

clean:
	rm *.0
	rm electricity-quiry

