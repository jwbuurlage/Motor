#makefile structure:
#when make wants to compile xxxx then it will look for a line starting with   xxxx:
#after the colon there is a list of dependencies: it will check if these files exist or changed
#if they did not change, it will not recompile xxxx
#if they did change, it will execute the command on the next line that MUST HAVE A SINGLE TAB in front
#by default make will compile the first target: it finds
#macro $@ gives target (left side of :) and @^ gives dependencies (right side of :) and @< gives first dependency

LIBS = -lSDLmain -lSDL -lGLEW -lGL

IDIR = ./include
OBJDIR = ./obj
_OBJ := MotorRenderer.o MotorRoot.o MotorTimer.o MotorWorld.o MotorLogger.o
OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ))
_DEPS = MotorRenderer.h MotorRoot.h MotorTimer.h MotorWorld.h MotorLogger.h Motor.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))
MOTOR_TARGET = ./bin/libMotor.a

DEMOIDIR = ./samples/Demo/include
DEMO_OBJDIR = ./obj/demo
_DEMO_OBJ := DemoGame.o DemoMain.o
DEMO_OBJ = $(patsubst %,$(DEMO_OBJDIR)/%,$(_DEMO_OBJ))
_DEMO_DEPS = DemoGame.h
DEMO_DEPS = $(patsubst %,$(DEMOIDIR)/%,$(_DEMO_DEPS))
DEMO_TARGET = ./bin/Demo

all: motor demo

motor: $(OBJ)
	@echo 'Creating libMotor'
	@ar rcs $(MOTOR_TARGET) $(OBJ)

demo: motor $(DEMO_OBJ)
	@echo 'Creating Demo'
	@$(CXX) -Wall -o $(DEMO_TARGET) $(DEMO_OBJ) -L ./bin -lMotor $(LIBS)

clean:
	rm -f $(OBJ) $(DEMO_OBJ) $(MOTOR_TARGET) $(DEMO_TARGET)
	rmdir $(DEMO_OBJDIR) $(OBJDIR)

$(DEMO_OBJDIR)/%.o: ./samples/Demo/src/%.cpp $(DEMO_DEPS) $(DEPS)
	@mkdir -p $(DEMO_OBJDIR)
	@echo 'Building $@ from file: $<'
	@$(CXX) -O2 -Wall -c -I$(IDIR) -I$(DEMOIDIR) -o"$@" "$<"

$(OBJDIR)/%.o: ./src/%.cpp $(DEPS)
	@mkdir -p $(OBJDIR)
	@echo 'Building $@ from file: $<'
	@$(CXX) -O2 -Wall -c -I$(IDIR) -o"$@" "$<"


.PHONY: all clean


