# - Try to find GStreamer_Plugins
# Once done this will define
#
#  GSTREAMERPLUGINSBASE_FOUND - system has GStreamer_Plugins
#  GSTREAMERPLUGINSBASE_INCLUDE_DIR - the GStreamer_Plugins include directory
#  GSTREAMERPLUGINSBASE_LIBRARIES - the libraries needed to use GStreamer_Plugins
#  GSTREAMERPLUGINSBASE_DEFINITIONS - Compiler switches required for using GStreamer_Plugins
#
#  (c)2007, Trolltech ASA

FIND_PACKAGE(PkgConfig REQUIRED)

IF (NOT WIN32)
   PKG_CHECK_MODULES( PKG_GSTREAMER REQUIRED gstreamer-plugins-base-0.10 )
ENDIF (NOT WIN32)

#
# Base plugins:
#  audio
#  cdda
#  netbuffer
#  pbutils
#  riff
#  rtp
#  rtsp
#  sdp
#  tag
#  video
#
# The gstinterfaces-0.10 library is found by FindGStreamer.cmake
#

FIND_LIBRARY(GSTREAMER_PLUGIN_AUDIO_LIBRARIES NAMES gstaudio-0.10
   PATHS
   ${PKG_GSTREAMER_LIBRARY_DIRS}
   )
FIND_LIBRARY(GSTREAMER_PLUGIN_CDDA_LIBRARIES NAMES gstcdda-0.10
   PATHS
   ${PKG_GSTREAMER_LIBRARY_DIRS}
   )
FIND_LIBRARY(GSTREAMER_PLUGIN_NETBUFFER_LIBRARIES NAMES gstnetbuffer-0.10
   PATHS
   ${PKG_GSTREAMER_LIBRARY_DIRS}
   )
FIND_LIBRARY(GSTREAMER_PLUGIN_PBUTILS_LIBRARIES NAMES gstpbutils-0.10
   PATHS
   ${PKG_GSTREAMER_LIBRARY_DIRS}
   )
FIND_LIBRARY(GSTREAMER_PLUGIN_RIFF_LIBRARIES NAMES gstriff-0.10
   PATHS
   ${PKG_GSTREAMER_LIBRARY_DIRS}
   )
FIND_LIBRARY(GSTREAMER_PLUGIN_RTP_LIBRARIES NAMES gstrtp-0.10
   PATHS
   ${PKG_GSTREAMER_LIBRARY_DIRS}
   )
FIND_LIBRARY(GSTREAMER_PLUGIN_RTSP_LIBRARIES NAMES gstrtsp-0.10
   PATHS
   ${PKG_GSTREAMER_LIBRARY_DIRS}
   )
FIND_LIBRARY(GSTREAMER_PLUGIN_SDP_LIBRARIES NAMES gstsdp-0.10
   PATHS
   ${PKG_GSTREAMER_LIBRARY_DIRS}
   )
FIND_LIBRARY(GSTREAMER_PLUGIN_TAG_LIBRARIES NAMES gsttag-0.10
   PATHS
   ${PKG_GSTREAMER_LIBRARY_DIRS}
   )
FIND_LIBRARY(GSTREAMER_PLUGIN_VIDEO_LIBRARIES NAMES gstvideo-0.10
   PATHS
   ${PKG_GSTREAMER_LIBRARY_DIRS}
   )

IF (GSTREAMER_PLUGIN_AUDIO_LIBRARIES AND
    GSTREAMER_PLUGIN_CDDA_LIBRARIES AND
    GSTREAMER_PLUGIN_NETBUFFER_LIBRARIES AND
    GSTREAMER_PLUGIN_PBUTILS_LIBRARIES AND
    GSTREAMER_PLUGIN_RIFF_LIBRARIES AND
    GSTREAMER_PLUGIN_RTP_LIBRARIES AND
    GSTREAMER_PLUGIN_RTSP_LIBRARIES AND
    GSTREAMER_PLUGIN_SDP_LIBRARIES AND
    GSTREAMER_PLUGIN_TAG_LIBRARIES AND
    GSTREAMER_PLUGIN_VIDEO_LIBRARIES)
   SET(GSTREAMERPLUGINSBASE_FOUND TRUE)
ELSE (GSTREAMER_PLUGIN_AUDIO_LIBRARIES AND
    GSTREAMER_PLUGIN_CDDA_LIBRARIES AND
    GSTREAMER_PLUGIN_NETBUFFER_LIBRARIES AND
    GSTREAMER_PLUGIN_PBUTILS_LIBRARIES AND
    GSTREAMER_PLUGIN_RIFF_LIBRARIES AND
    GSTREAMER_PLUGIN_RTP_LIBRARIES AND
    GSTREAMER_PLUGIN_RTSP_LIBRARIES AND
    GSTREAMER_PLUGIN_SDP_LIBRARIES AND
    GSTREAMER_PLUGIN_TAG_LIBRARIES AND
    GSTREAMER_PLUGIN_VIDEO_LIBRARIES)
   SET(GSTREAMERPLUGINSBASE_FOUND FALSE)
ENDIF (GSTREAMER_PLUGIN_AUDIO_LIBRARIES AND
    GSTREAMER_PLUGIN_CDDA_LIBRARIES AND
    GSTREAMER_PLUGIN_NETBUFFER_LIBRARIES AND
    GSTREAMER_PLUGIN_PBUTILS_LIBRARIES AND
    GSTREAMER_PLUGIN_RIFF_LIBRARIES AND
    GSTREAMER_PLUGIN_RTP_LIBRARIES AND
    GSTREAMER_PLUGIN_RTSP_LIBRARIES AND
    GSTREAMER_PLUGIN_SDP_LIBRARIES AND
    GSTREAMER_PLUGIN_TAG_LIBRARIES AND
    GSTREAMER_PLUGIN_VIDEO_LIBRARIES)

IF (GSTREAMERPLUGINSBASE_FOUND)
   SET(GSTREAMERPLUGINS_FOUND TRUE)
ELSE (GSTREAMERPLUGINSBASE_FOUND)
   SET(GSTREAMERPLUGINS_FOUND FALSE)
ENDIF (GSTREAMERPLUGINSBASE_FOUND)

IF (GSTREAMERPLUGINS_FOUND)
   IF (NOT GStreamer_Plugins_FIND_QUIETLY)
      MESSAGE(STATUS "Found GStreamer Plugins:
    ${GSTREAMER_PLUGIN_AUDIO_LIBRARIES}
    ${GSTREAMER_PLUGIN_CDDA_LIBRARIES}
    ${GSTREAMER_PLUGIN_NETBUFFER_LIBRARIES}
    ${GSTREAMER_PLUGIN_PBUTILS_LIBRARIES}
    ${GSTREAMER_PLUGIN_RIFF_LIBRARIES}
    ${GSTREAMER_PLUGIN_RTP_LIBRARIES}
    ${GSTREAMER_PLUGIN_RTSP_LIBRARIES}
    ${GSTREAMER_PLUGIN_SDP_LIBRARIES}
    ${GSTREAMER_PLUGIN_TAG_LIBRARIES}
    ${GSTREAMER_PLUGIN_VIDEO_LIBRARIES}")
   ENDIF (NOT GStreamer_Plugins_FIND_QUIETLY)
ELSE (GSTREAMERPLUGINS_FOUND)
   IF (GStreamer_Plugins_FIND_REQUIRED)
      MESSAGE(SEND_ERROR "Could NOT find GStreamer Plugins")
   ENDIF (GStreamer_Plugins_FIND_REQUIRED)
ENDIF (GSTREAMERPLUGINS_FOUND)

MARK_AS_ADVANCED(GSTREAMERPLUGINS_DEFINITIONS
    GSTREAMER_PLUGIN_AUDIO_LIBRARIES
    GSTREAMER_PLUGIN_CDDA_LIBRARIES
    GSTREAMER_PLUGIN_NETBUFFER_LIBRARIES
    GSTREAMER_PLUGIN_PBUTILS_LIBRARIES
    GSTREAMER_PLUGIN_RIFF_LIBRARIES
    GSTREAMER_PLUGIN_RTP_LIBRARIES
    GSTREAMER_PLUGIN_RTSP_LIBRARIES
    GSTREAMER_PLUGIN_SDP_LIBRARIES
    GSTREAMER_PLUGIN_TAG_LIBRARIES
    GSTREAMER_PLUGIN_VIDEO_LIBRARIES)
