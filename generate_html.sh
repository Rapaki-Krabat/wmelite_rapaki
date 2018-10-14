#!/bin/bash

emcc -O2 --js-opts 0 wmelite_sdl_mixer.bc \
-o test.html  \
-s ALLOW_MEMORY_GROWTH=1 -s EXTRA_EXPORTED_RUNTIME_METHODS=['Pointer_stringify'] \
--preload-file data_demo.dcp --preload-file settings.xml
# --preload-file data_amulets.dcp --preload-file settings.xml
# --preload-file pakcik0.dcp --preload-file pakcik1.dcp --preload-file touch2.dcp --preload-file settings.xml --preload-file zhromadne/wsitke/fonts/KABANAN.TTF --preload-file zhromadne/wsitke/fonts/KABANA_N.TTF

# -s ASSERTIONS=2 
# -s EMULATE_FUNCTION_POINTER_CASTS=1 