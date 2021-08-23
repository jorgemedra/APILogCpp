


#TARGET		= APILogCpp.lib
TARGET_LIB	= libAPILogCpp.so
TARGET_SMPL	= sample


LIB_SRC		= ./apilog
SMP_SRC		= ./sample
OBJ_DIR		= ./obj
BIN_DIR		= ./bin
DEBUG_DIR	= ./debug
LIB_DIR		= ./lib
LOG_DIR		= ./logs
INC_DIR		= ./include
HRD_LIB		= $(INC_DIR)/APILog.h

CC			= g++
CPPFLAGS	= -c -fPIC -std=c++17 -Wall -MD
LNK_LIBF	= -shared -lpthread -lstdc++
LNK_SMPF	= -lstdc++ -L$(LIB_DIR) -lpthread -lAPILogCpp


API_OBJS	= $(OBJ_DIR)/LogGate.o \
			  $(OBJ_DIR)/LogManager.o

SMP_OBJS	= $(OBJ_DIR)/LogTest.o


all: apilog sample

apilog: $(OBJ_DIR) $(LIB_DIR) $(API_OBJS)
	$(CC) $(LNK_LIBF) $(API_OBJS) -o $(LIB_DIR)/$(TARGET_LIB)

sample: $(OBJ_DIR) $(BIN_DIR) $(SMP_OBJS)
	$(CC) -o $(BIN_DIR)/$(TARGET_SMPL) $(SMP_OBJS) $(LNK_SMPF)



$(OBJ_DIR)/%.o: $(LIB_SRC)/%.cpp
	$(CC) $(CPPFLAGS) $< -o $@

$(OBJ_DIR)/%.o: $(SMP_SRC)/%.cpp
	$(CC) $(CPPFLAGS) -I$(INC_DIR) $< -o $@


run: $(LOG_DIR)
	$(BIN_DIR)/$(TARGET_SMPL)


install: $(LIB_DIR)/$(TARGET_LIB)
	@echo ***************************************
	@echo Installing /usr/lib/$(TARGET_LIB)
	
	cp -f $(LIB_DIR)/$(TARGET_LIB) /usr/lib
	chmod 0755 /usr/lib/$(TARGET_LIB)
	ldconfig
	@echo checking instalation
	ldconfig -p | grep $(TARGET_LIB)

uninstall: /usr/lib/$(TARGET_LIB)
	@echo ***************************************
	@echo Uninstalling /usr/lib/$(TARGET_LIB)
	rm -f /usr/lib/$(TARGET_LIB)
	@ldconfig

installdbg: $(DEBUG_DIR)/$(TARGET_LIB)
	@echo ***************************************
	@echo Installing /usr/lib/$(TARGET_LIB)
	
	cp -f $(DEBUG_DIR)/$(TARGET_LIB) /usr/lib
	chmod 0755 /usr/lib/$(TARGET_LIB)
	ldconfig
	@echo checking instalation
	ldconfig -p | grep $(TARGET_LIB)



$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

$(LIB_DIR):
	@mkdir -p $(LIB_DIR)

$(LOG_DIR):
	@mkdir -p $(LOG_DIR)

clean:
	@echo Cleaning
	@rm -rf $(BIN_DIR)
	@rm -rf $(OBJ_DIR)
	@rm -rf $(LIB_DIR)
	@rm -rf $(LOG_DIR)
	@echo Cleaned