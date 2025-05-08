target = watch-chezmoi-dirs

CC = gcc
CFLAGS = -g -Wall -Wextra

blddir = bld
objdir = obj
srcdir = src

srcs := $(wildcard $(srcdir)/*.c)
objs := $(patsubst $(srcdir)/%.c,$(objdir)/%.o,$(srcs))

.PHONY: all
all: $(target)

$(target): $(objs) | $(blddir)
	$(CC) $(CFLAGS) -o $(blddir)/$@ $^

$(blddir):
	mkdir -p $(blddir)

$(objdir)/%.o: $(srcdir)/%.c | $(objdir)
	$(CC) $(CFLAGS) -c -o $@ $^

$(objdir):
	mkdir -p $(objdir)

.PHONY: clean
clean:
	rm -rf $(objdir) $(blddir)
