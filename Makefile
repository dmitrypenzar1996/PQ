SOURCE_DIR = source
LINK_DIR = link

.PHONY: all clean

all : PQ

clean:
	rm -rf $(LINK_DIR) PQ.exe

$(SOURCE_DIR) :
	if ! [ -d $(SOURCE_DIR) ]; then echo "Source dir doesn't exist"; exit 1; fi

$(LINK_DIR) :
	if ! [ -d $(LINK_DIR) ]; then mkdir $(LINK_DIR); fi

add.o : $(SOURCE_DIR) $(LINK_DIR) $(SOURCE_DIR)/add.c
	gcc -O2 -c $(SOURCE_DIR)/add.c -o $(LINK_DIR)/add.o

Record.o : $(SOURCE_DIR) $(LINK_DIR) $(SOURCE_DIR)/Record.c
	gcc -O2 -c $(SOURCE_DIR)/Record.c -o $(LINK_DIR)/Record.o

RecordList.o : $(SOURCE_DIR) $(LINK_DIR) $(SOURCE_DIR)/RecordList.c
	gcc -O2 -c $(SOURCE_DIR)/RecordList.c -o $(LINK_DIR)/RecordList.o

HashAlignment.o : $(SOURCE_DIR) $(LINK_DIR) $(SOURCE_DIR)/HashAlignment.c
	gcc -O2 -c $(SOURCE_DIR)/HashAlignment.c -o $(LINK_DIR)/HashAlignment.o

PWM.o : $(SOURCE_DIR) $(LINK_DIR) $(SOURCE_DIR)/PWM.c
	gcc -O2 -c $(SOURCE_DIR)/PWM.c -o $(LINK_DIR)/PWM.o

Tree.o : $(SOURCE_DIR) $(LINK_DIR) $(SOURCE_DIR)/Tree.c
	gcc -O2 -g -c $(SOURCE_DIR)/Tree.c -o $(LINK_DIR)/Tree.o

RMQ.o : $(SOURCE_DIR) $(LINK_DIR) $(SOURCE_DIR)/RMQ.c
	gcc -O2 -c $(SOURCE_DIR)/RMQ.c -o $(LINK_DIR)/RMQ.o

countScore.o : $(SOURCE_DIR) $(LINK_DIR) $(SOURCE_DIR)/countScore.c
	gcc -O2 -c $(SOURCE_DIR)/countScore.c -o $(LINK_DIR)/countScore.o

countScoreHash.o : $(SOURCE_DIR) $(LINK_DIR) $(SOURCE_DIR)/countScoreHash.c
	gcc -O2 -c $(SOURCE_DIR)/countScoreHash.c -o $(LINK_DIR)/countScoreHash.o

TreeWS.o : $(SOURCE_DIR) $(LINK_DIR) $(SOURCE_DIR)/TreeWS.c
	gcc -O2 -c $(SOURCE_DIR)/TreeWS.c -o $(LINK_DIR)/TreeWS.o

growTree.o : $(SOURCE_DIR) $(LINK_DIR) $(SOURCE_DIR)/growTree.c
	gcc -O2 -c $(SOURCE_DIR)/growTree.c -o $(LINK_DIR)/growTree.o

nniTree.o : $(SOURCE_DIR) $(LINK_DIR) $(SOURCE_DIR)/nniTree.c
	gcc -O2 -c $(SOURCE_DIR)/nniTree.c -o $(LINK_DIR)/nniTree.o

sprTree.o : $(SOURCE_DIR) $(LINK_DIR) $(SOURCE_DIR)/sprTree.c
	gcc -O2 -c $(SOURCE_DIR)/sprTree.c -o\
	    $(LINK_DIR)/sprTree.o

estimate.o : $(SOURCE_DIR) $(LINK_DIR) $(SOURCE_DIR)/estimate.c
	gcc -O2 -c $(SOURCE_DIR)/estimate.c -o $(LINK_DIR)/estimate.o

consensus.o : $(SOURCE_DIR) $(LINK_DIR) $(SOURCE_DIR)/consensus.c
	gcc -O2 -c $(SOURCE_DIR)/consensus.c -o $(LINK_DIR)/consensus.o

bootstrap.o : $(SOURCE_DIR) $(LINK_DIR) $(SOURCE_DIR)/bootstrap.c
	gcc -O2 -c $(SOURCE_DIR)/bootstrap.c -o $(LINK_DIR)/bootstrap.o

maxAgreement.o : $(SOURCE_DIR) $(LINK_DIR) $(SOURCE_DIR)/maxAgreement.c
	gcc -O2 -c $(SOURCE_DIR)/maxAgreement.c -o $(LINK_DIR)/maxAgreement.o

genitor.o : $(SOURCE_DIR) $(LINK_DIR) $(SOURCE_DIR)/genitor.c
	gcc -O2 -c $(SOURCE_DIR)/genitor.c -o $(LINK_DIR)/genitor.o

PQ.o : $(SOURCE_DIR) $(LINK_DIR) $(SOURCE_DIR)/PQ.c
	gcc -O2 -c $(SOURCE_DIR)/PQ.c -o $(LINK_DIR)/PQ.o

PQ : PQ.o add.o Record.o \
    RecordList.o HashAlignment.o PWM.o countScore.o countScoreHash.o\
    Tree.o RMQ.o growTree.o TreeWS.o nniTree.o sprTree.o estimate.o\
    consensus.o bootstrap.o maxAgreement.o genitor.o
	gcc $(LINK_DIR)/PQ.o $(LINK_DIR)/add.o\
	    $(LINK_DIR)/Record.o $(LINK_DIR)/RecordList.o\
	    $(LINK_DIR)/HashAlignment.o $(LINK_DIR)/PWM.o\
	    $(LINK_DIR)/countScore.o\
	    $(LINK_DIR)/countScoreHash.o $(LINK_DIR)/Tree.o\
	    $(LINK_DIR)/RMQ.o $(LINK_DIR)/growTree.o $(LINK_DIR)/TreeWS.o\
	    $(LINK_DIR)/nniTree.o $(LINK_DIR)/sprTree.o\
	    $(LINK_DIR)/estimate.o $(LINK_DIR)/consensus.o\
	    $(LINK_DIR)/bootstrap.o $(LINK_DIR)/maxAgreement.o $(LINK_DIR)/genitor.o -lm -o PQ.exe
