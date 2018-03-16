CXX = g++
FLAGS = -std=c++14 -o3 -pthread

TARGET = bin
OBJ_DIR = obj
PROGRAM_NAME = $(TARGET)/main

SETTINGS_DIR = settings
OPTIONS_DIR = options
INTEGRAL_DIR = integral


OBJECTS = $(addprefix $(OBJ_DIR)/, main.o)

OBJECTS += $(addprefix $(OBJ_DIR)/,\
settings.o\
settings_tokens.o\
settings_retriever.o\
settings_holder.o)

OBJECTS += $(addprefix $(OBJ_DIR)/,\
options.o\
options_tokens.o)

OBJECTS += $(addprefix $(OBJ_DIR)/,\
point.o\
integral.o\
integrator.o\
estimator.o\
calculator.o\
result.o)


all: init $(PROGRAM_NAME)

init:
	mkdir -p $(OBJ_DIR)
	mkdir -p $(TARGET)

$(PROGRAM_NAME): $(OBJECTS)
	$(CXX) $(FLAGS) $^ -o $@

$(OBJ_DIR)/%.o: %.cpp
	$(CXX) $(FLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SETTINGS_DIR)/%.cpp $(SETTINGS_DIR)/*.h
	$(CXX) $(FLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(OPTIONS_DIR)/%.cpp $(OPTIONS_DIR)/*.h
	$(CXX) $(FLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(INTEGRAL_DIR)/%.cpp $(INTEGRAL_DIR)/*.h
	$(CXX) $(FLAGS) -c $< -o $@


.PHONY: clean
clean:
	rm -rf $(TARGET)
	rm -rf $(OBJ_DIR)