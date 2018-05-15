all: detection lilypond interface 

detection:
	$(MAKE) -C Detection/ spectre

lilypond:
	$(MAKE) -C Lilypond/code/ createSheet

interface: parser
	javac ParserMidi/Swing.java

parser:
	javac ParserMidi/ParserMidi.java

clean: cdetection clilypond cinterface

cdetection:
	$(MAKE) -C Detection/ clean

clilypond:
	$(MAKE) -C Lilypond/code/ clean

cinterface:
	rm ParserMidi/*.class
