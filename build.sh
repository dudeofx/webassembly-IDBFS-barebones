emcc IDBFS_test.c -s WASM=1 -s EXTRA_EXPORTED_RUNTIME_METHODS='["cwrap", "ccall"]' -o IDBFS_utils.js

