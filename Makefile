NAME		= webserv

CC			= clang++
RM			= rm -f

_SRCS		= main.cpp \
			  references/Methods.cpp \
			  references/Statuses.cpp \
			  references/MimeTypes.cpp \
			  references/ServerConfigKeys.cpp \
			  references/LocationConfigKeys.cpp \
			  config/configParser.cpp \
			  config/ServerParser.cpp \
			  config/LocationParser.cpp \
			  Server.cpp \
			  Connections.cpp \
			  RequestParser.cpp \
			  Response.cpp \
			  CgiResponse.cpp \
			  responses/utils.cpp \
			  responses/generateErrorResponse.cpp \
			  responses/generateStaticResponse.cpp \
			  responses/generatePutResponse.cpp \
			  responses/generateRedirectResponse.cpp \
			  responses/generateCgiResponse.cpp \
			  pageGenerators.cpp \
			  utils.cpp

SRCS        = $(addprefix srcs/, $(_SRCS))
OBJS		= $(SRCS:.cpp=.o)
DEPS		= $(SRCS:.cpp=.d)

CFLAGS		= -Wall -Wextra -Werror -std=c++98

all:		$(NAME)

%.o : %.cpp
			$(CC) $(CFLAGS) -Isrcs -c $< -o $@

%.d: %.cpp
			@set -e; $(RM) $@; \
			$(CC) -Isrcs -M $(CPPFLAGS) $< > $@.$$$$; \
			sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
			$(RM) $@.$$$$

$(NAME):	$(OBJS)
			$(CC) $(OBJS) -o $(NAME)

clean:
			$(RM) $(OBJS) $(DEPS)

fclean:		clean
			$(RM) $(NAME)

re:			fclean all

-include $(DEPS)

.PHONY: clean fclean re all test
