

# Standard behavior must be included here
INCLUDE_DIRS += $(TARGET_FILE_NAME)/$(USRSRC)  # add user sources to include path
CPPSRC += $(call target_files,$(USRSRC_SLASH),*.cpp)
CSRC += $(call target_files,$(USRSRC_SLASH),*.c)

APPSOURCES=$(call target_files,$(USRSRC_SLASH),*.cpp)
APPSOURCES+=$(call target_files,$(USRSRC_SLASH),*.c)

# Custom stuff can be added here
INCLUDE_DIRS += $(TARGET_FILE_NAME)/inc