CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

SOURCES = main.cpp utils.cpp signals.cpp cd.cpp echo.cpp pwd.cpp ls.cpp pinfo.cpp background.cpp search.cpp io_redirection.cpp pipeline.cpp history.cpp autocomplete.cpp

OBJECTS = $(SOURCES:.cpp=.o)

TARGET = a.out

LIBS = -lreadline -lhistory   

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

rebuild: clean all
