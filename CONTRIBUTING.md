Please feel free to fork and submit pull requests. I don't have
the time or patience to maintain this actively, but I'll try to
take improvements wherever I can.

In particular I would appreciate:

- Checking for memory leaks in the way things are implemented
- Functional and stability testing
- Adding common, useful sqlite3 functions (see [the full function
  listing](https://sqlite.org/c3ref/funclist.html) for ideas)
- Finding out what the `const char * what` argument to
  `scm_gc_malloc_pointerless` is for, and whether it needs to be
  set to any particular value
- Making a call on how Makefile should get the installation
  directory from the environment -- right now it is hard-coded to
  `/usr/local/lib`, which may not apply to all environments
