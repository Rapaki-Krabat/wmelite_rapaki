#!/bin/bash

emcc -O2 --js-opts 0 wmelite_sdl_mixer.bc ../dependencies/freetype-2.6/objs/.libs/libfreetype.so.6.12.0 \
-o test.html  --preload-file data_amulets.dcp --preload-file settings.xml \
-s ASSERTIONS=2 -s ALLOW_MEMORY_GROWTH=1 -s ERROR_ON_UNDEFINED_SYMBOLS=0 -s EXTRA_EXPORTED_RUNTIME_METHODS=['Pointer_stringify']
