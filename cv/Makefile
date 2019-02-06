.PHONY: clean

all: png

png: 2019-02-05-200014.webm
	mkdir -p $@
	avconv -i $^ -f image2 $@/%04d.png

clean:
	rm -rf png

