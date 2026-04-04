CXX = g++
CXXFLAGS = -std=c++17 -O2 -g -Wall $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs) -lSDL2_image -lSDL2_mixer -lGL -lGLU -ltinyxml2

SRC = main.cpp \
      MainUnit.cpp \
      ScreenUnit.cpp \
      TextureUnit.cpp \
      FontUnit.cpp \
      SoundUnit.cpp \
      FrustumUnit.cpp \
      StateUnit.cpp \
      MainStatusUnit.cpp \
      IntroStatusUnit.cpp \
      LevelStatusUnit.cpp \
      GameOverStatusUnit.cpp \
      GameUnit.cpp \
      MsxMath.cpp \
      DrawModelUnit.cpp \
      Mesh.cpp \
      XmlParser.cpp \
      ModelUnit.cpp \
      KeyboardUnit.cpp \
      HighscoreUnit.cpp \
      Utils.cpp \
      MediaManagerUnit.cpp \
      Logger.cpp

OBJ = $(SRC:.cpp=.o)
TARGET = thesheepkiller

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
