FILES = index.md

all: lua-idn2.html lua-idn2.pdf lua-idn2.3

lua-idn2.html: template.html site.css metadata.yaml $(FILES)
	pandoc -o $@ -t html5 -s --toc --template=template.html --section-divs --self-contained -c site.css metadata.yaml $(FILES)

lua-idn2.pdf: metadata.yaml $(FILES)
	pandoc -o $@ -t latex -s --toc --toc-depth=2 -V documentclass=article -V classoption=oneside -V links-as-notes -V geometry=a4paper,includeheadfoot,margin=2.54cm metadata.yaml $(FILES)

lua-idn2.3: metadata.yaml $(FILES)
	pandoc -o $@ -t man -s metadata.yaml $(FILES)

man: lua-idn2.3
	man -l $^

clean:
	rm -f lua-idn2.html lua-idn2.pdf lua-idn2.3

.PHONY: all man install clean
