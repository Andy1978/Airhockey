.PHONY: clean

CXX		= g++
OPTIM = -Os
CXXFLAGS = -Wall -Wextra $(OPTIM) `fltk-config --use-gl --use-glut --cxxflags`
LDFLAGS	 = `fltk-config --use-gl --use-glut --ldflags`

render_table: render_table.cxx
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f render_table
