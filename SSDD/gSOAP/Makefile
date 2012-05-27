# Parameters

CLIENT = client
SERVER = server

OUTPUT_DIR = project

SOURCES_CLNT.c = $(CLIENT).c
SOURCES_SVC.c = $(SERVER).c 
SOURCES.h = text.h

TARGETS_SVC.c = $(OUTPUT_DIR)/soapServer.c
TARGETS_CLNT.c = $(OUTPUT_DIR)/soapClient.c
TARGETS_COMMON.c = $(OUTPUT_DIR)/soapC.c
TARGETS = $(TARGETS_COMMON.c) $(TARGETS_SVC.c) $(TARGETS_CLNT.c)

# Compiler flags 

SOAPCPP2=soapcpp2
CFLAGS= -Wall -O0 -g3 -ggdb3 -I$(OUTPUT_DIR) -I/opt/gsoap-linux-2.7.9/include -I/usr/local/include -DDEBUG

LDFLAGS=-L/usr/local/ssl/lib -L/opt/gsoap-linux-2.7.9/lib -L/usr/local/lib
LDLIBS=-lpthread -lgsoap

# Targets 

all: $(CLIENT) $(SERVER)

$(TARGETS): $(SOURCES.h)
	mkdir -p $(OUTPUT_DIR)
	$(SOAPCPP2) -c -d $(OUTPUT_DIR) $<

$(CLIENT): $(SOURCES_CLNT.c) $(TARGETS_COMMON.c) $(TARGETS_CLNT.c)

$(SERVER): $(SOURCES_SVC.c) $(TARGETS_COMMON.c) $(TARGETS_SVC.c)

clean:
	$(RM) -rf $(CLIENT) $(SERVER) $(OUTPUT_DIR)
