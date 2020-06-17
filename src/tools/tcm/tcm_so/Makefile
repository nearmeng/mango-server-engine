SHARED = -shared -z defs



CC=g++

CFLAGS=-I deps/include/ -I../xreplacer/ -g -O2 -Wall -Wextra -fPIC -pipe 



TCM_SO=../../../../bin/tools/deploy/tcm/tcm_so.so



DEP = tcm_so.d



.PHONY: all clean



all: $(TCM_SO) 



%.o: %.c

	$(CC) -c $(CFLAGS) $< -o $@



%.d: %.c

	$(CC) -MM $(CFLAGS) $< -o $@



$(TCM_SO): tcm_so.o $(DEP)

	$(CC) -o $(TCM_SO) $(SHARED) tcm_so.o deps/lib/libtcmapi.a ../xreplacer/libxreplacer.a



clean:

	$(RM) *.o *.d $(TCM_SO)



ifneq ($(MAKECMDGOALS),clean)

    -include $(DEP)

endif

