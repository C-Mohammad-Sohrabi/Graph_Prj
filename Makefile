# =============================================================================
# Simple Makefile - Graph Theory Project
#
# This Makefile targets Unix-like shells: Linux, macOS, WSL, or MSYS2/Git-Bash
# on Windows (i.e. anywhere a POSIX `mkdir -p` is available). If you're on
# plain Windows without one of those (e.g. plain cmd.exe + MSVC), use the
# cross-platform CMakeLists.txt instead - see README.md.
# =============================================================================
CC = gcc
CFLAGS = -g -Wall -Wextra -std=c11
LDLIBS = -lm
SRCDIR = src
INCDIR = include
BUILDDIR = build
OBJDIR = $(BUILDDIR)/obj
EXEC = $(BUILDDIR)/graph_program
INCLUDES = -I$(INCDIR)

# =============================================================================
# Source Files
# =============================================================================
SOURCES = $(SRCDIR)/main.c \
          $(SRCDIR)/clique.c \
          $(SRCDIR)/connectivity.c \
          $(SRCDIR)/euler_path.c \
          $(SRCDIR)/havel_hakimi.c \
          $(SRCDIR)/independent_set.c \
          $(SRCDIR)/line_graph.c \
          $(SRCDIR)/set_utils.c \
          $(SRCDIR)/vertex_cover.c \
          $(SRCDIR)/connectivity_number.c

OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))

# =============================================================================
# Build Rules
# =============================================================================
.PHONY: all clean

all: $(EXEC)

$(EXEC): $(OBJECTS)
	@echo "==> Linking $@..."
	$(CC) $(OBJECTS) -o $@ $(CFLAGS) $(LDLIBS)
	@echo "==> Build complete: $@"

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	@echo "==> Compiling $<..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR):
	@mkdir -p $(OBJDIR)

clean:
	@echo "==> Cleaning up..."
	@rm -rf $(BUILDDIR)
	@rm -rf dot_file image
	@echo "==> Clean complete"