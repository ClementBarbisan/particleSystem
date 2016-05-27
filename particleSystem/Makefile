NAME = particleSystem

SRC = main.cpp \
	  	CL.cpp \
		Renderer.cpp \
		utils.cpp

OBJ = $(SRC:.cpp=.o)

FRAMEWORK = -framework OpenGL -framework OpenCL

LIB = -L$(HOME)/.brew/lib/ -lglfw3

INC = -I$(HOME)/.brew/include/ -I./

FLAG = -Wall -Wextra -std=c++11

all: $(NAME)

$(NAME): $(OBJ)
	clang++ -o $@ $^ $(FRAMEWORK) $(LIB)

%.o: %.cpp
	clang++ -o $@ -c $^ $(FLAG) $(INC)

clean:
	/bin/rm -f $(OBJ)

fclean: clean
	/bin/rm -f $(NAME)

re: fclean all
