SOURCES = ChowMatrixManual.tex manual.cls
TEX_FLAGS = 

PDFS = $(patsubst %.tex, %.pdf, $(SOURCES))

.PHONY: clean

# Run xelatex twice to generate references
%.pdf: %.tex
	xelatex $(TEX_FLAGS) $<
	xelatex $(TEX_FLAGS) $<

all: $(PDFS)

clean:
	rm -f *.{log,pdf,out,aux,fls,fdb*,sync*}

deploy: $(PDFS)
	scp ChowMatrixManual.pdf jatin@ccrma-gate.stanford.edu:~/Library/Web/chowdsp/Products
