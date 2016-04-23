CC = g++ -std=c++0x

CC_FLAGS = -w
LIBS = -lboost_system -lprotobuf -lcrypto -lssl -L/home/alex/Program/HTMLHandle/HTMLHandle/ -lhtml

EXEC = run
SOURCES = $(wildcard *.cpp)
PROTO_SOURCES = $(wildcard *.pb.cc)
OBJECTS = $(SOURCES:.cpp=.o)
PROTO_OBJECTS = $(PROTO_SOURCES:.pb.cc=.o)
$(EXEC): $(OBJECTS) $(PROTO_OBJECTS)
	$(CC) $(OBJECTS) $(PROTO_OBJECTS) -o $(EXEC) -D_GLIBCXX_USE_CXX11_ABI=0  $(LIBS)
%.o: %.cpp
	$(CC) -c -I/home/alex/Program/HTMLHandle/HTMLHandle/ $(CC_FLAGS) $< -o $@
%.o: %.pb.cc
	$(CC) -c $(CC_FLAGS) $< -o $@
clean:
	rm -f $(EXEC) $(OBJECTS)
