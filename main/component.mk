# Enable use of things requiring C99 things, like std::to_string
CPPFLAGS += -D_GLIBCXX_USE_C99

# Add the include directory to include paths so that files can be included using <smooth/file.h>
COMPONENT_ADD_INCLUDEDIRS := .

COMPONENT_SRCDIRS := \
    . \
    states \
    states/events