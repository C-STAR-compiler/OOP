# The following variables need to be set prior to including library.mk

# Library name
NAME = oop

# Versioning
include ver/version.mk

# If the project is separated among multiple sub-folders
PROJECT_ROOTS =

# Additionnal libraries (ex: -pthread, -lmath, etc)
LIBRARIES = -pthread

# Additionnal flags for the compiler
FLAGS =

# Include the template
include $(CUT_HOME)CUT/res/library.mk
