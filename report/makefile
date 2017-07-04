Filename=Report
TexFilename=$(Filename).tex
PDFFilename=$(Filename).pdf
PDFLaTeXFlags=-interaction=batchmode

all:
	pdflatex $(PDFLaTeXFlags) $(TexFilename)
	bibtex $(Filename)
	pdflatex $(PDFLaTeXFlags) $(TexFilename)
	pdflatex $(PDFLaTeXFlags) $(TexFilename)

clean:
	rm -rf *.log  *.tui *.synctex.gz *.aux *.idx *.blg *.mp *.out *~ *.lof *.toc
