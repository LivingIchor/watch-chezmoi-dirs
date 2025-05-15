target = watch-chezmoi-dirs

CC = gcc
CFLAGS = -g -Wall -Wextra

bindir = bin
objdir = obj
srcdir = src

srcs := $(wildcard $(srcdir)/*.c)
objs := $(patsubst $(srcdir)/%.c,$(objdir)/%.o,$(srcs))

.PHONY: all
all: $(target)

$(target): $(objs) | $(bindir)
	$(CC) $(CFLAGS) -o $(bindir)/$@ $^

$(bindir):
	mkdir -p $(bindir)

$(objdir)/%.o: $(srcdir)/%.c | $(objdir)
	$(CC) $(CFLAGS) -c -o $@ $^

$(objdir):
	mkdir -p $(objdir)

.PHONY: clean
clean:
	rm -rf $(objdir) $(bindir)
