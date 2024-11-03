## References:
* `https://blog.cambridgespark.com/50-free-machine-learning-datasets-image-datasets-241852b03b49`

## How to build
Use the included `Makefile` to build the code
* Run `make` to build in the release mode with all the optimizations enabled
* Run `make debug` to build in the debug mode
* Run `make clean` to clean up all the compiled code

## Debugging / disabling checks
- `DEBUG_MAIN_TRACE` print out what the main code (part 6) is doing
- `DEBUG_NO_KEY_MANGLE` code 5 will print `Mangling Key!!` instead of mangling the key
- `DEBUG_FORCE_TIME` always decrypt key as if it is the correct time
- `DEBUG_NO_SYSTEM_CHECK` ignore system checks like time and kernel version
