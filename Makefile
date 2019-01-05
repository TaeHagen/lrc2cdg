#This file is part of lrc2cdg.
#
#lrc2cdg is free software: you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation, either version 3 of the License, or
#(at your option) any later version.
#
#lrc2cdg is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#
#You should have received a copy of the GNU General Public License
#along with lrc2cdg.  If not, see <https://www.gnu.org/licenses/>.

# Declaration of variables
CC = g++
CC_FLAGS = -w

# File names
EXEC = lrc2cdg
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

# Main target
$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC)

# To obtain object files
%.o: %.cpp
	$(CC) -c $(CC_FLAGS) $< -o $@

# To remove generated files
clean:
	rm -f $(EXEC) $(OBJECTS)

install:
	cp $(EXEC) /usr/local/bin

emscripten:
	emcc $(SOURCES) -o $(EXEC).js -s EXPORTED_FUNCTIONS="['_convlrc2cdgjs']" -s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall"]' -s ALLOW_MEMORY_GROWTH=1
