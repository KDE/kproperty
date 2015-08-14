# This file defines the Feature Logging macros.
#
# MACRO_LOG_FEATURE(VAR FEATURE DESCRIPTION URL [REQUIRED [MIN_VERSION [COMMENTS]]])
#   Logs the information so that it can be displayed at the end
#   of the configure run
#   VAR : TRUE or FALSE, indicating whether the feature is supported
#   FEATURE: name of the feature, e.g. "libjpeg"
#   DESCRIPTION: description what this feature provides
#   URL: home page
#   REQUIRED: TRUE or FALSE, indicating whether the feature is required
#   MIN_VERSION: minimum version number. empty string if unneeded
#   COMMENTS: More info you may want to provide.  empty string if unnecessary
#
# MACRO_DISPLAY_FEATURE_LOG()
#   Call this to display the collected results.
#   Exits CMake with a FATAL error message if a required feature is missing
#
# Example:
#
# INCLUDE(MacroLogFeature)
#
# FIND_PACKAGE(JPEG)
# MACRO_LOG_FEATURE(JPEG_FOUND "libjpeg" "Support JPEG images" "http://www.ijg.org" TRUE "3.2a" "")
# ...
# MACRO_DISPLAY_FEATURE_LOG()

# Copyright (c) 2006, Alexander Neundorf, <neundorf@kde.org>
# Copyright (c) 2006, Allen Winter, <winter@kde.org>
# Copyright (c) 2009, Sebastian Trueg, <trueg@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if (NOT _macroLogFeatureAlreadyIncluded)
   set(_file ${CMAKE_BINARY_DIR}/MissingRequirements.txt)
   if (EXISTS ${_file})
      file(REMOVE ${_file})
   endif ()

   set(_file ${CMAKE_BINARY_DIR}/EnabledFeatures.txt)
   if (EXISTS ${_file})
      file(REMOVE ${_file})
   endif ()

   set(_file ${CMAKE_BINARY_DIR}/DisabledFeatures.txt)
   if (EXISTS ${_file})
      file(REMOVE ${_file})
  endif ()

  set(_macroLogFeatureAlreadyIncluded TRUE)
endif ()


macro(MACRO_LOG_FEATURE _var _package _description _url ) # _required _minvers _comments)

   string(TOUPPER "${ARGV4}" _required)
   set(_minvers "${ARGV5}")
   set(_comments "${ARGV6}")

   if (${_var})
     if (NOT("${_required}" STREQUAL "TRUE"))
       set(_LOGFILENAME ${CMAKE_BINARY_DIR}/EnabledFeatures.txt)
     endif ()
   else ()
     if ("${_required}" STREQUAL "TRUE")
       set(_LOGFILENAME ${CMAKE_BINARY_DIR}/MissingRequirements.txt)
     else ()
       set(_LOGFILENAME ${CMAKE_BINARY_DIR}/DisabledFeatures.txt)
     endif ()
   endif ()

   set(_logtext "   * ${_package}")

   if (NOT ${_var})
      if (${_minvers} MATCHES ".*")
        set(_logtext "${_logtext} (${_minvers} or higher)")
      endif ()
      set(_logtext "${_logtext}  <${_url}>\n     ")
   else ()
     set(_logtext "${_logtext} - ")
   endif ()

   set(_logtext "${_logtext}${_description}")

   if (NOT ${_var})
      if (${_comments} MATCHES ".*")
        set(_logtext "${_logtext}\n     ${_comments}")
      endif ()
#      SET(_logtext "${_logtext}\n") #double-space missing features?
   endif ()

   if (${_LOGFILENAME})
      file(APPEND "${_LOGFILENAME}" "${_logtext}\n")
   endif ()

endmacro(MACRO_LOG_FEATURE)


macro(MACRO_DISPLAY_FEATURE_LOG)

   set(_missingFile ${CMAKE_BINARY_DIR}/MissingRequirements.txt)
   set(_enabledFile ${CMAKE_BINARY_DIR}/EnabledFeatures.txt)
   set(_disabledFile ${CMAKE_BINARY_DIR}/DisabledFeatures.txt)

   if (EXISTS ${_missingFile} OR EXISTS ${_enabledFile} OR EXISTS ${_disabledFile})
     set(_printSummary TRUE)
   endif ()

   if(_printSummary)
     set(_missingDeps 0)
     if (EXISTS ${_enabledFile})
       file(READ ${_enabledFile} _enabled)
       file(REMOVE ${_enabledFile})
       set(_summary "${_summary}\n-----------------------------------------------------------------------------\n-- The following external packages were located on your system.\n-- This installation will have the extra features provided by these packages.\n-----------------------------------------------------------------------------\n${_enabled}")
     endif ()


     if (EXISTS ${_disabledFile})
       set(_missingDeps 1)
       file(READ ${_disabledFile} _disabled)
       file(REMOVE ${_disabledFile})
       set(_summary "${_summary}\n-----------------------------------------------------------------------------\n-- The following OPTIONAL packages could NOT be located on your system.\n-- Consider installing them to enable more features from this software.\n-----------------------------------------------------------------------------\n${_disabled}")
     endif ()


     if (EXISTS ${_missingFile})
       set(_missingDeps 1)
       file(READ ${_missingFile} _requirements)
       set(_summary "${_summary}\n-----------------------------------------------------------------------------\n-- The following REQUIRED packages could NOT be located on your system.\n-- You must install these packages before continuing.\n-----------------------------------------------------------------------------\n${_requirements}")
       file(REMOVE ${_missingFile})
       set(_haveMissingReq 1)
     endif ()


     if (NOT ${_missingDeps})
       set(_summary "${_summary}\n-----------------------------------------------------------------------------\n-- Congratulations! All external packages have been found.")
     endif ()


     message(${_summary})
     message("-----------------------------------------------------------------------------\n")


     if(_haveMissingReq)
       message(FATAL_ERROR "Exiting: Missing Requirements")
     endif()

   endif()

endmacro(MACRO_DISPLAY_FEATURE_LOG)
